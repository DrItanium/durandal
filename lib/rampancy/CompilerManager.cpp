#include "rampancy/CompilerManager.h"
#include "rampancy/CompilerRegistry.h"
#include "ExpertSystem/Types.h"

extern "C" {
   #include "clips.h"   
}

namespace rampancy {
   CompilerManager::CompilerManager() {

   }

   CompilerManager::~CompilerManager() {

   }

   llvm::LLVMContext* CompilerManager::getContext() {
      return context;
   }
   void CompilerManager::setContext(llvm::LLVMContext* llvmContext) {
      context = llvmContext;
   }
   CLIPSEnvironment* CompilerManager::getEnvironment() {
      return env;
   }
   void CompilerManager::setEnvironment(CLIPSEnvironment* theEnv) {
      env = theEnv;
   }

   llvm::Module* CompilerManager::compile(char* logicalName, int argc, char** argv) {
      llvm::StringRef name ((const char*)logicalName);
      return compile(name, argc, argv);
   }

   llvm::Module* CompilerManager::compile(llvm::StringRef logicalName, 
         int argc, char** argv) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry();
      assert( context && "LLVM Context not set in CompilerManager!");
      assert( env && "No CLIPS Provided to CompilerManager!");
      const Compiler* compiler = compilers->getCompiler(logicalName);
      assert( compiler && "No compiler bound target logical name!");
      //otherwise we are good to go man :D
      compiler->setLLVMContext(context);
      compiler->setEnvironment(env);
      return compiler->compile(argc, argv);
   }
   llvm::Module* CompilerManager::compile(llvm::StringRef logicalName, 
         void* theEnv) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry();
      assert( context && "LLVM context not set in CompilerManager!");
      const Compiler* compiler = compilers->getCompiler(logicalName);
      if(!compiler) {
         char* buf = CharBuffer(512);
         sprintf(buf, 
               "(printout t \"ERROR: Provided Compiler %s does not exist!\" crlf)",
               logicalName.data());
         EnvEval(theEnv, buf);
         free(buf);
         return 0;
      } else {
         CLIPSEnvironment* tmpEnv = new CLIPSEnvironment(theEnv);
         compiler->setEnvironment(tmpEnv);
         compiler->setContext(context);
         llvm::Module* result = compiler->compile();
         //make sure that this is deleted
         delete tmpEnv;
         //reset it to the preowned environment, even if it's null
         compiler->setEnvironment(env);
         return result;
      }
   }
   llvm::Module* CompilerManager::interpret(llvm::StringRef logicalName, 
         llvm::StringRef input) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry();
      assert( context && "LLVM Context not set in CompilerManager!");
      assert( env && "No CLIPS Provided to CompilerManager!");
      const Compiler* compiler = compilers->getCompiler(logicalName);
      assert( compiler && "No compiler bound target logical name!");
      compiler->setLLVMContext(context);
      compiler->setEnvironment(env);
      return compiler->interpret(input);
   }
   llvm::Module* CompilerManager::interpret(char* logicalName, 
         llvm::StringRef input) {
      llvm::StringRef lName ((const char*)logicalName);
      return interpret(lName, input);
   }
   
   llvm::Module* CompilerManager::interpret(llvm::StringRef, void* theEnv) {
      CompilerRegistry* compilers = CompilerRegistry::getCompilerRegistry();
      assert( context && "LLVM Context not set in CompilerManager!");
      const Compiler* compiler = compilers->getCompiler(logicalName);
      if(!compiler) {
         char* buf = CharBuffer(512);
         sprintf(buf, 
               "(printout t \"ERROR: Provided Compiler %s does not exist!\" crlf)",
               logicalName.data());
         EnvEval(theEnv, buf);
         free(buf);
         return 0;
      } else {
         CLIPSEnvironment* tmpEnv = new CLIPSEnvironment(theEnv);
         compiler->setEnvironment(tmpEnv);
         compiler->setContext(context);
         llvm::Module* result = compiler->interpret();
         //make sure that this is deleted
         delete tmpEnv;
         //reset it to the preowned environment, even if it's null
         compiler->setEnvironment(env);
         return result;
      }
   }

}
