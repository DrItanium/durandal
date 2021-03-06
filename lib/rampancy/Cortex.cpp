#include "rampancy/Cortex.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <memory>
#include <algorithm>
#include "rampancy/Compiler.h"
#include "rampancy/CompilerManager.h"
#include "rampancy/CompilerRegistry.h"
#include "ExpertSystem/FunctionKnowledgeConversionPass.h"

extern "C" {
#include "clips.h"
}
namespace rampancy {
   static llvm::ManagedStatic<Cortex> rampantCortexObject;

   Cortex* Cortex::getRampantCortex() {
      return &*rampantCortexObject;
   }
   Cortex::Cortex(void* theEnv) {
      if(theEnv) {
         env = new CLIPSEnvironment(theEnv);
      } else {
         env = new CLIPSEnvironment();
      }
      context = new llvm::LLVMContext();
      manager = new CompilerManager();
      manager->setContext(context);
      manager->setEnvironment(env);
   }
   Cortex::~Cortex() {
      delete manager;
      delete context;
      delete env;
   }

   llvm::LLVMContext* Cortex::getContext() {
      return context;
   }

   void Cortex::setContext(llvm::LLVMContext* llvmContext) {
      context = llvmContext;
      manager->setContext(context);
   }

   CLIPSEnvironment* Cortex::getEnvironment() {
      return env;
   }
   void Cortex::setEnvironment(CLIPSEnvironment* theEnv) {
      env = theEnv; 
      manager->setEnvironment(env);
   }

   void Cortex::compileToKnowledge(void* theEnv) {
      //we need to get the first argument from theEnv to find out where to 
      //route it to
      DATA_OBJECT arg0;
      char* copy;
      copy = CharBuffer(512);
      //change this so that only the first argument is handled by
      //compileToKnowledge
      if((EnvArgCountCheck(theEnv, (char*)"rampancy-compile", AT_LEAST, 1) == -1)) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\nCompile arguments are <compiler> ...\n"
               "It is up to the compiler backend to handle the rest of the arguments\n");
         return;
      }

      if((EnvArgTypeCheck(theEnv, (char*)"rampancy-compile", 1, SYMBOL, &arg0) == 0)) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\ncompiler name must be a symbol!\n");
         return;
      }
      std::string tmp (DOToString(arg0));
      //we need some indirection to prevent the CLIPS garbage collector from
      //coming down like a fucking hammer on what we're doing here. Awesomely,
      //this will all be cleaned up at the end of the function :D.
      llvm::StringRef logicalName(tmp);
      llvm::Module* module = compile(logicalName, theEnv);
      if(!module) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\nThe compiler failed to compile the target file\n");
         return;
      } 
      convertToKnowledge(logicalName, module, theEnv);
   }
   void Cortex::interpretToKnowledge(void* theEnv) {
      //we need to get the first argument from theEnv to find out where to 
      //route it to
      DATA_OBJECT arg0;
      char* copy;
      copy = CharBuffer(512);
      if((EnvArgCountCheck(theEnv, (char*)"rampancy-interpret", EXACTLY, 2) == -1)) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\nInterpret arguments are <compiler> <code>\n");
         return;
      }

      if((EnvArgTypeCheck(theEnv, (char*)"rampancy-interpret", 1, SYMBOL, &arg0) == 0)) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\nCompiler name must be a symbol!\n");
         return;
      }
      std::string tmp (DOToString(arg0));
      //we need some indirection to prevent the CLIPS garbage collector from
      //coming down like a fucking hammer on what we're doing here. Awesomely,
      //this will all be cleaned up at the end of the function :D.
      llvm::StringRef logicalName(tmp);
      llvm::Module* module = interpret(logicalName, theEnv);
      if(!module) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\nThe compiler failed to interpret the target file\n");
         return;
      } 
      convertToKnowledge(logicalName, module, theEnv);
   }
   llvm::Module* Cortex::compile(llvm::StringRef logicalName, void* theEnv) {
      return manager->compile(logicalName, theEnv);
   }
   llvm::Module* Cortex::interpret(llvm::StringRef logicalName, void* theEnv) {
      return manager->interpret(logicalName, theEnv);
   }
   llvm::Module* Cortex::compile(char* logicalName, int argc, char** argv) {
      return manager->compile(logicalName, argc, argv);
   }
   llvm::Module* Cortex::compile(llvm::StringRef logicalName, int argc, char** argv) {
      return manager->compile(logicalName, argc, argv);
   }

   llvm::Module* Cortex::interpret(llvm::StringRef logicalName, 
         llvm::StringRef input) {
      return manager->interpret(logicalName, input);
   }
   llvm::Module* Cortex::interpret(char* logicalName, llvm::StringRef input) {
      return manager->interpret(logicalName, input);
   }

   void Cortex::convertToKnowledge(llvm::StringRef logicalName,
         llvm::Module* module) {
      convertToKnowledge(logicalName, module, env->getEnvironment());
   }

   void Cortex::convertToKnowledge(llvm::StringRef logicalName, 
         llvm::Module* module, void* theEnv) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry(); 
      Compiler* target = compilers->getCompiler(logicalName);
      if(!target) {
         EnvPrintRouter(theEnv, (char*)"werror", 
               (char*)"\nProvided logical name does not exist\n");
         return;
      } else {

         CLIPSEnvironment* tEnv = new CLIPSEnvironment(theEnv);
         //we need to cheat a little bit and create the module here
         KnowledgeConstructor tmp;
         tmp.route(module);
			//we only build the target module, nothing more
         tEnv->makeInstances((char*)tmp.getInstancesAsString().c_str());
			char* definstances = CharBuffer(1024);
		 	sprintf(definstances, "(definstances MAIN::module-%lld-declaration %s)", 
					(PointerAddress)module, 
					tmp.getInstancesAsString().c_str());
			tEnv->build(definstances);
			free(definstances);
      }
   }
}
