#include "rampancy/LLVMIRCompiler.h"
extern "C" {
#include "clips.h"
}

namespace rampancy {
   LLVMIRCompiler::LLVMIRCompiler() {

   }

   llvm::Module* LLVMIRCompiler::compile() {
      DATA_OBJECT arg1;
      char* persist;
      //this one is a little different we need to only extract a file name out
      void* theEnv = getEnvironment()->getEnvironment();
      if((EnvArgTypeCheck(theEnv, (char*)"rampancy-compile", 2,
                  SYMBOL_OR_STRING, &arg1) == 0)) {
         EnvPrintRouter(theEnv, 
               (char*)"werror",
               (char*)"\nSecond argument to the IR parser is neither a string"
               " or symbol!\n");
         return 0;
      }
      char* tmp = DOToString(arg1);
      persist = CharBuffer(strlen(tmp) + 1);
      sprintf(persist, "%s", tmp);
      const std::string fileName (persist);
      llvm::SMDiagnostic err;
      llvm::Module* mod = compile(fileName, err);
      free(persist);
      return mod;
   }

   llvm::Module* LLVMIRCompiler::compile(int argc, char** argv) {
      const std::string filePath (argv[1]);
      llvm::SMDiagnostic err;
      return compile(filePath, err);
   }

   llvm::Module* LLVMIRCompiler::compile(const std::string &fileName, 
         llvm::SMDiagnostic &err) {
      return ParseIRFile(fileName, err, *(getContext()));
   }

   llvm::Module* LLVMIRCompiler::interpret() {
      void* theEnv = getEnvironment()->getEnvironment();
      DATA_OBJECT arg1;
      char* tmp;
      char* persist;
      if((EnvArgTypeCheck(theEnv, (char*)"rampancy-interpret", 2, 
                  STRING, &arg1) == 0)) {
         EnvPrintRouter(theEnv, (char*)"werror",
               (char*)"\nInput was not a string!\n");
         return 0;
      }
      tmp = DOToString(arg1);
      persist = CharBuffer(strlen(tmp) + 1);
      sprintf(persist, "%s", tmp);
      llvm::StringRef code(persist);
      llvm::Module* result = interpret(code);
      free(persist);
      return result;
   }

   llvm::Module* LLVMIRCompiler::interpret(llvm::StringRef input) {
      llvm::SMDiagnostic err;
      return ParseIR(llvm::MemoryBuffer::getMemBuffer(input), err,
            *(getContext()));
   }

}
