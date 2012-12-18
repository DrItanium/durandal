#include <stdio.h>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "rampancy/Compiler.h"
#include "rampancy/LLVMIRCompiler.h"
#include "rampancy/Cortex.h"

extern "C" {
   #include "clips.h"
}

int main(int argc, char** argv, char* const *envp) {
   rampancy::ClangCompilerGlobals::argv0 = argv[0];
   rampancy::Cortex& rampantCortex = *rampancy::Cortex::getRampantCortex();
   rampancy::CompilerRegistry& compilerRegistry = 
      *rampancy::CompilerRegistry::getCompilerRegistry();
   PassRegistry &registry = *PassRegistry::getPassRegistry();
   initializeCore(registry);
   initializeScalarOpts(registry);
   initializeVectorization(registry);
   initializeIPO(registry);
   initializeAnalysis(registry);
   initializeIPA(registry);
   initializeTransformUtils(registry);
   initializeInstCombine(registry);
   initializeInstrumentation(registry);
   initializeTarget(registry);
   llvm::StringRef ref("bc");
   llvm::StringRef ref1("llvm-ir");
   llvm::StringRef ref2("bitcode");
   llvm::StringRef ref3("llvm-bitcode");
   rampancy::LLVMIRCompiler cc;
   compilerRegistry.registerCompiler(ref, &cc);
   compilerRegistry.registerCompiler(ref1, &cc);
   compilerRegistry.registerCompiler(ref2, &cc);
   compilerRegistry.registerCompiler(ref3, &cc);
   CLIPSEnvironment* theEnv = rampantCortex.getEnvironment();
   theEnv->batchStar("Init.clp");

   llvm::Module* mod = rampantCortex.compile(ref, argc, argv);
	//we need to setup 
   if(mod) {
      rampantCortex.convertToKnowledge(ref, mod);
      theEnv->eval((char*)"(save-instances \"instances\" visible)");
   } else {
      theEnv->eval((char*)"(printout t \"ERROR: Something went wrong\" crlf)");
   }
   llvm::llvm_shutdown();
}
