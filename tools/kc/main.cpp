#include <stdio.h>

#include "ExpertSystem/CLIPSEnvironment.h"
#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "rampancy/RampancyCompiler.h"

extern "C" {
   #include "clips.h"
}

int main(int argc, char** argv, char* const *envp) {
   CLIPSEnvironment env;
   env.batchStar("Init.clp");
   env.reset();
   env.eval("(set-current-module MAIN)");
   rampancy::Compiler compiler(env, argv[0], envp);
   if(compiler.compileToKnowledge(argc, argv)) {
      env.eval("(save-instances \"instances\")");
   } else {
      env.eval("(printout t \"ERROR: Something went wrong\" crlf)");
   }
   llvm::llvm_shutdown();
}
