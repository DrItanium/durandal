#include "rampancy/Compiler.h"
#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/Types.h"
extern "C" {
#include "clips.h"
}

namespace rampancy {
   Compiler::Compiler() { }
   Compiler::~Compiler() { }

   llvm::LLVMContext* Compiler::getContext() {
      return context;
   }
   void Compiler::setContext(llvm::LLVMContext* llvmContext) {
      context = llvmContext; 
   }

   CLIPSEnvironment* Compiler::getEnvironment() {
     return env;
   }

   void Compiler::setEnvironment(CLIPSEnvironment* tEnv) {
     env = tEnv;
   }

}
