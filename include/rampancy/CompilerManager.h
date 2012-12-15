/*
 * The CompilerManager is a custom pass manager that handles both dynamic
 * compilation as well as knowledge construction in addition to being a
 * standard pass manager. 
 *
 * It also provides a standard way to register compilers with CLIPS through the
 * IO router system. This makes it really easy to add new compilers without any
 * complex issues. 
 *
 */
#ifndef _rampancy_compiler_manager_h
#define _rampancy_compiler_manager_h

#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "rampancy/Compiler.h"
#include "llvm/ADT/DenseMap.h" 
#include "llvm/LLVMContext.h"
#include "llvm/ADT/StringRef.h"

namespace rampancy {
   class CompilerManager {
      private:
         llvm::LLVMContext* context;
         CLIPSEnvironment* env;
      public:
         CompilerManager();
         ~CompilerManager();
         llvm::LLVMContext* getContext();
         void setContext(llvm::LLVMContext* llvmContext);
         CLIPSEnvironment* getEnvironment();
         void setEnvironment(CLIPSEnvironment* theEnv);
         llvm::Module* compile(llvm::StringRef logicalName, void* theEnv);
         llvm::Module* compile(char* logicalName, int argc, char** argv);
         llvm::Module* compile(llvm::StringRef logicalName, int argc, char** argv);
         llvm::Module* interpret(llvm::StringRef logicalName, void* theEnv);
         llvm::Module* interpret(char* logicalName, llvm::StringRef input);
         llvm::Module* interpret(llvm::StringRef logicalName, llvm::StringRef input);
   };
}
#endif
