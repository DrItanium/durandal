#include <stdio.h>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "rampancy/RampancyCompiler.h"

extern "C" {
   #include "clips.h"
}

int main(int argc, char** argv, char* const *envp) {
   CLIPSEnvironment env;
   env.batchStar((char*)"Init.clp");
   env.eval((char*)"(set-current-module MAIN)");
   rampancy::Compiler compiler(env, envp);
   llvm::Module* mod = compiler.compile(argc, (const char**) argv);
   if(mod) {
      env.eval((char*)"(save-instances \"instances\" visible)");
   } else {
      env.eval((char*)"(printout t \"ERROR: Something went wrong\" crlf)");
   }
   llvm::llvm_shutdown();
}
