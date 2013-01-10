#include "rampancy/CLIPSFunctions.h"
#include "rampancy/Cortex.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/PassRegistry.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "ExpertSystem/Types.h"
extern "C" {
#include "clips.h"
}
#define rampancy_compile (char*)"rampancy-compile"
#define rampancy_interpret (char*)"rampancy-interpret"
#define rampancy_save_module (char*)"rampancy-save-llvm-module"
#define msg(x) (char*) x

extern "C" void CompileFileIntoKnowledge(void* env) {
	rampancy::Cortex* globalCortex = rampancy::Cortex::getRampantCortex();
	globalCortex->compileToKnowledge(env);
}

extern "C" void InterpretCodeIntoKnowledge(void* env) {
	rampancy::Cortex* globalCortex = rampancy::Cortex::getRampantCortex();
	globalCortex->interpretToKnowledge(env);
}

extern "C" void* SaveModuleToFile(void* env) {
	DATA_OBJECT addrDO,
					saveAsBitcodeDO,
					pathDO;
	char *saveAsBitcodeStr, 
		  *pathStr;
	bool saveAsBitcode;
	if(EnvArgCountCheck(env, rampancy_save_module, EXACTLY, 3) == -1) {
		return FalseSymbol();
	}

	if(EnvArgTypeCheck(env, rampancy_save_module, 1, INTEGER, &addrDO) == FALSE) {
		return FalseSymbol();
	}
	if(EnvArgTypeCheck(env, rampancy_save_module, 2, SYMBOL_OR_STRING, &saveAsBitcodeDO) == FALSE) {
		return FalseSymbol();
	}
	if(EnvArgTypeCheck(env, rampancy_save_module, 3, SYMBOL_OR_STRING, &pathDO) == FALSE) {
		return FalseSymbol();
	}
	llvm::Module* module = (llvm::Module*)(PointerAddress)DOToLong(addrDO);
	saveAsBitcodeStr = DOToString(saveAsBitcodeDO);
	if(strcmp(saveAsBitcodeStr, "TRUE") == 0) {
		saveAsBitcode = TRUE; 
	} else {
		saveAsBitcode = FALSE;
	}
	pathStr = DOToString(pathDO);
	std::string tmp(pathStr);
	llvm::PassManager pm;
	std::string ErrorInfo;
	llvm::raw_fd_ostream OS(tmp.c_str(), ErrorInfo, llvm::raw_fd_ostream::F_Binary);

	if(saveAsBitcode) {
		pm.add(llvm::createBitcodeWriterPass(OS));
	} else {
		pm.add(llvm::createPrintModulePass(&OS));
	}
	pm.run(*module);
	return TrueSymbol();
}

extern "C" void SetupRampancyExpertSystemInterfaces(void* env) {
	EnvDefineFunction(env, rampancy_compile, 'v',
			PTIEF CompileFileIntoKnowledge, 
			msg("CompileFileIntoKnowledge"));
	EnvDefineFunction(env, rampancy_interpret, 'v',
			PTIEF InterpretCodeIntoKnowledge, 
			msg("InterpretCodeIntoKnowledge"));
	EnvDefineFunction(env, rampancy_save_module, 'b', 
			PTIEF SaveModuleToFile,
			msg("SaveModuleToFile"));
}

#undef rampancy_compile
#undef rampancy_interpret
#undef rampancy_save_module
#undef msg
