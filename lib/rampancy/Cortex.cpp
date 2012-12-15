#include "rampancy/Cortex.h"

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
      context = new LLVMContext();
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
      if((EnvArgCountCheck(theEnv, "compile", AT_LEAST, 1) == -1)) {
         EnvPrintRouter(theEnv, "werror", "\nNo compiler provided to use!\n");
         return;
      }

      if((EnvArgTypeCheck(theEnv, "compile", 1, SYMBOL, arg0) == 0)) {
         EnvPrintRouter(theEnv, "werror", "\ncompiler name must be a symbol!\n");
         return;
      }
      std::string tmp (DOToString(arg0));
      //we need some indirection to prevent the CLIPS garbage collector from
      //coming down like a fucking hammer on what we're doing here. Awesomely,
      //this will all be cleaned up at the end of the function :D.
      llvm::StringRef logicalName(tmp);
      llvm::Module* module = compile(logicalName, theEnv);
      if(!module) {
         EnvPrintRouter(theEnv, "werror", 
               "\nThe compiler failed to compile the target file\n");
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
      if((EnvArgCountCheck(theEnv, "interpret", AT_LEAST, 1) == -1)) {
         EnvPrintRouter(theEnv, "werror", "\nNo compiler provided to use!\n");
         return;
      }

      if((EnvArgTypeCheck(theEnv, "interpret", 1, SYMBOL, arg0) == 0)) {
         EnvPrintRouter(theEnv, "werror", "\nCompiler name must be a symbol!\n");
         return;
      }
      std::string tmp (DOToString(arg0));
      //we need some indirection to prevent the CLIPS garbage collector from
      //coming down like a fucking hammer on what we're doing here. Awesomely,
      //this will all be cleaned up at the end of the function :D.
      llvm::StringRef logicalName(tmp);
      llvm::Module* module = interpret(logicalName, theEnv);
      if(!module) {
         EnvPrintRouter(theEnv, "werror", 
               "\nThe compiler failed to interpret the target file\n");
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
         llvm::Module* module, void* theEnv) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry(); 
      Compiler* target = compilers->getCompiler(logicalName);
      if(!target) {
         EnvPrintRouter(theEnv, "werror", "\nProvided logical name does not exist\n");
         return;
      } else {
         llvm::PassManager tmpPassManager;
         CLIPSEnvironment* tEnv = new CLIPSEnvironment(theEnv);
         //taken from opt
         llvm::TargetLibraryInfo *tli = 
            new llvm::TargetLibraryInfo(Triple(module->getTargetTriple()));
         tmpPassManager.add(tli);
         TargetData *td = 0;
         const std::string &moduleDataLayout = module->getDataLayout();
         if(!moduleDataLayout.empty())
            td = new TargetData(ModuleDataLayout);
         if(td)
            tmpPassManager.add(td);
         target->setEnvironment(tEnv);
         target->setContext(context);
         tmpPassManager.add(target);
         tmpPassManager.run(module);
      }
   }
   void Cortex::convertToKnowledge(llvm::StringRef logicalName, 
         llvm::Module* module) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry(); 
      Compiler* target = compilers->getCompiler(logicalName);
      if(!target) {
         EnvPrintRouter(env->getEnvironment(), "werror", "\nProvided logical name does not exist\n");
         return;
      } else {
         llvm::PassManager tmpPassManager;
         //taken from opt
         llvm::TargetLibraryInfo *tli = 
            new llvm::TargetLibraryInfo(Triple(module->getTargetTriple()));
         tmpPassManager.add(tli);
         TargetData *td = 0;
         const std::string &moduleDataLayout = module->getDataLayout();
         if(!moduleDataLayout.empty())
            td = new TargetData(ModuleDataLayout);
         if(td)
            tmpPassManager.add(td);
         target->setEnvironment(env);
         target->setContext(context);
         tmpPassManager.add(target);
         tmpPassManager.run(module);
      }
   }

}
