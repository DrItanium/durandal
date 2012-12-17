#ifndef _rampancy_compiler_h
#define _rampancy_compiler_h
/*
 * The rampancy library is a standard way to access compilers and interpreters
 * from within CLIPS. It assumes that you're going to generate a pass
 *
 * This class is abstract
 */
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "llvm/LLVMContext.h"
#include "llvm/ADT/StringRef.h"

namespace rampancy {
   class Compiler {
      private:
         llvm::LLVMContext* context;
         CLIPSEnvironment* env;
      public:
         Compiler(); 
         ~Compiler();

         //this one gets arguments straight from CLIPS
         virtual llvm::Module* compile() = 0;
         virtual llvm::Module* compile(int argc, char** argv) = 0;
         virtual llvm::Module* interpret() = 0;
         virtual llvm::Module* interpret(llvm::StringRef input) = 0;
         llvm::LLVMContext* getContext();
         void setContext(llvm::LLVMContext* context);
         void setEnvironment(CLIPSEnvironment* tEnv);
         CLIPSEnvironment* getEnvironment();
   };
}
#endif
