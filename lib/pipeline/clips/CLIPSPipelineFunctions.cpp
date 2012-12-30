#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/PassRegistry.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "pipeline/clips/CLIPSPassGenerator.h"
#include "rampancy/Cortex.h"
#include "rampancy/CompilerManager.h"
#include "rampancy/CompilerRegistry.h"
#include "indirect/Indirector.h"

using namespace llvm;
extern "C" void OptimizeCode(void* theEnv);
extern "C" void RegisterPass(void* theEnv);
extern "C" void UnregisterPass(void* theEnv);
	
extern "C" RegisterCLIPSPipelineFunctions(void* theEnv) {
	EnvDefineFunction(theEnv, "optimize", 
			PTIEF OptimizeCode, "OptimizeCode");
	EnvDefineFunction(theEnv, "register-pass", 
			PTIEF RegisterPass, "RegisterPass");
	EnvDefineFunction(theEnv, "unregister-pass",
			PTIEF UnregisterPass, "UnregisterPass");
}

void OptimizeCode(void* theEnv) {

}

void RegisterPass(void* theEnv) {

}

void UnregisterPass(void* theEnv) {

}
