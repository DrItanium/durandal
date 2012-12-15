#ifndef _rampancy_cortex_h
#define _rampancy_cortex_h

#include "rampancy/CompilerManager.h"
#include "rampancy/Compiler.h"
#include "rampancy/CompilerRegistry.h"
#include "llvm/Function.h"
#include "llvm/PassSupport.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Compiler.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"

namespace rampancy {
   class Cortex {
      CLIPSEnvironment* env;
      llvm::LLVMContext* context;
      CompilerManager* manager;
      public:
         Cortex(void* theEnv = 0);
         ~Cortex();
         static Cortex* getRampantCortex();
         llvm::LLVMContext* getContext();
         void setContext(llvm::LLVMContext* llvmContext);
         CLIPSEnvironment* getEnvironment();
         void setEnvironment(CLIPSEnvironment* env);
         void compileToKnowledge(void* theEnv);
         void interpretToKnowledge(void* theEnv);
         llvm::Module* compile(llvm::StringRef logicalName, void* theEnv);
         llvm::Module* interpret(llvm::StringRef logicalName, void* theEnv);
         llvm::Module* compile(char* logicalName, int argc, char** argv);
         llvm::Module* compile(llvm::StringRef logicalName, 
               int argc, char** argv);
         llvm::Module* interpret(llvm::StringRef logicalName, 
               llvm::StringRef input);
         llvm::Module* interpret(char* logicalName, llvm::StringRef input);
         void convertToKnowledge(llvm::StringRef logicalName, 
               llvm::Module* module);
         void convertToKnowledge(llvm::StringRef logicalName, 
               llvm::Module* module, void* theEnv);
   };
}
#endif
