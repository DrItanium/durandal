#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/PassRegistry.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "pipeline/clips/CLIPSPassGenerator.h"
#include "pipeline/clips/CLIPSPass.h"
#include "pipeline/clips/CLIPSPassHeader.h"
#include "rampancy/Cortex.h"
#include "rampancy/CompilerManager.h"
#include "rampancy/CompilerRegistry.h"
#include "indirect/Indirector.h"
#include "llvm/Support/raw_ostream.h"

extern "C" {
#include "clips.h"
}

using namespace llvm;
using namespace indirect;
extern "C" indirect::IndirectPassHeader::IndirectPassType TranslateInput(char* input);
extern "C" void CLIPSOptimizeCode(void* theEnv);
extern "C" void CLIPSRegisterPass(void* theEnv);
extern "C" void CLIPSUnregisterPass(void* theEnv);
extern "C" void* CLIPSPassRegistered(void* theEnv);
#define pass_registered (char*)"pass-registered"
#define optimize (char*)"optimize"
#define unregister_pass (char*)"unregister-pass"
#define register_pass (char*)"register-pass"
#define werror (char*)"werror"
#define msg(x) (char*) x
#define copy(from, to) \
	to = CharBuffer(strlen(from)); \
sprintf(to, "%s", from) 
#define BoolCast(str, tgt) tgt = (strcmp(str, "TRUE") == 0) ? TRUE : FALSE

extern "C" void RegisterCLIPSPipelineFunctions(void* theEnv) {
	EnvDefineFunction(theEnv, optimize, 'v',
			PTIEF CLIPSOptimizeCode, "CLIPSOptimizeCode");
	EnvDefineFunction(theEnv, register_pass, 'v',
			PTIEF CLIPSRegisterPass, "CLIPSRegisterPass");
	EnvDefineFunction(theEnv, unregister_pass, 'v',
			PTIEF CLIPSUnregisterPass, "CLIPSUnregisterPass");
	EnvDefineFunction2(theEnv, pass_registered, 'w', 
			PTIEF CLIPSPassRegistered, "CLIPSPassRegistered", 
			"11k");
}

void CLIPSOptimizeCode(void* theEnv) {

}

void CLIPSRegisterPass(void* theEnv) {
	/* register-pass has the following arguments
	 * 1) name (PassArg)
	 * 2) description (PassName)
	 * 3) type (string)
	 * 4) IsAnalysis (bool)
	 * 5) isCFGPass (bool)
	 * 6) needRegions (bool)
	 * 7) needLoops (bool)
	 * 8) passes (multifield)
	 * 9) required (multifield)
	 * 10) required-transitive (multifield)
	 * 11) Preserved (multifield)
	 * 12) preservesAll (bool)
	 * 13) preservesCFG (bool);
	 */

	void* passes;
	void* required;
	void* requiredTransitive;
	void* preserved;
	DATA_OBJECT arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, 
					arg10, arg11, arg12;
	char *tmp0, *tmp1, *tmp2, *tmp3, *tmp4, *tmp5, *tmp6, *tmp7, *tmp8, *tmp9,
		  *tmp10;
	char* pArg;
	char* pName;
	bool isAnalysis, isCFG, 
		  needRegions, needLoops,
		  preservesAll, preservesCFG;
	char* type;
	long long l0, l1, l2, l3;
	std::string tmp;
	raw_string_ostream stream(tmp);
	pipeline::clips::CLIPSPassHeader* header = new pipeline::clips::CLIPSPassHeader();
	header->setTemplateSet("clips");
	if(EnvArgCountCheck(theEnv, register_pass, EXACTLY, 13) == -1) {
		return;
	}

	if(EnvArgTypeCheck(theEnv, register_pass, 1, SYMBOL_OR_STRING, &arg0) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 2, SYMBOL_OR_STRING, &arg1) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 3, SYMBOL_OR_STRING, &arg2) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 4, SYMBOL, &arg3) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 5, SYMBOL, &arg4) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 6, SYMBOL, &arg5) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 7, SYMBOL, &arg6) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 8, MULTIFIELD, &arg7) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 9, MULTIFIELD, &arg8) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 10, MULTIFIELD, &arg9) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 11, MULTIFIELD, &arg10) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 12, SYMBOL, &arg11) == FALSE) {
		return;
	}
	if(EnvArgTypeCheck(theEnv, register_pass, 13, SYMBOL, &arg12) == FALSE) {
		return;
	}
	tmp0 = DOToString(arg0);
	copy(tmp0, pArg);
	header->setPassName((const char*)pArg);
	tmp1 = DOToString(arg1);
	copy(tmp1, pName);
	header->setPassDescription((const char*)pName);
	tmp2 = DOToString(arg2);
	copy(tmp2, type);
	header->setPassType(TranslateInput(type));
	tmp3 = DOToString(arg3);
	BoolCast(tmp3, isAnalysis);
	header->setIsAnalysis(isAnalysis);
	tmp4 = DOToString(arg4);
	BoolCast(tmp4, isCFG);
	header->setIsCFGOnlyPass(isCFG);
	tmp5 = DOToString(arg5);
	BoolCast(tmp5, needRegions);
	header->setNeedsRegions(needRegions);
	tmp6 = DOToString(arg6);
	BoolCast(tmp6, needLoops);
	header->setNeedsLoops(needLoops);
	//l0 = (long long)GetDOLength(arg7);
	//l1 = (long long)GetDOLength(arg8);
	//aUsage = GetValue(arg7);



   IndirectPassRegistry& indirectRegistry = *IndirectPassRegistry::getIndirectPassRegistry();
	indirectRegistry.registerIndirectPassHeader(header);
	free(pArg);
	free(pName);
	free(type);

}

void CLIPSUnregisterPass(void* theEnv) {

}

void* CLIPSPassRegistered(void* theEnv) {
	DATA_OBJECT arg0;
	char* a;
	char* b;
	if(EnvArgCountCheck(theEnv, pass_registered, EXACTLY, 1) == -1) {
		return FalseSymbol();
	}

	if(EnvArgTypeCheck(theEnv, pass_registered, 1, SYMBOL_OR_STRING, &arg0) == FALSE) {
		return FalseSymbol();
	}
	a = DOToString(arg0);
	copy(a, b);

	llvm::PassRegistry* registry = llvm::PassRegistry::getPassRegistry();
	const llvm::PassInfo* pi = registry->getPassInfo(llvm::StringRef(b));
	free(b);
	if(pi) {
		return TrueSymbol();
	} else {
		return FalseSymbol();
	}
}
IndirectPassHeader::IndirectPassType TranslateInput(char* input) {
	if(strcmp(input, "Module") == 0) {
		return IndirectPassHeader::Module;
	} else if(strcmp(input, "Function") == 0) {
		return IndirectPassHeader::Function;
	} else if(strcmp(input, "BasicBlock") == 0) {
		return IndirectPassHeader::BasicBlock;
	} else if(strcmp(input, "Loop") == 0) {
		return IndirectPassHeader::Loop;
	} else if(strcmp(input, "Region") == 0) {
		return IndirectPassHeader::Region;
	} else if(strcmp(input, "MachineFunction") == 0) {
		return IndirectPassHeader::MachineFunction;
	} else if(strcmp(input, "CallGraphSCC") == 0) {
		return IndirectPassHeader::CallGraphSCC;
	} else {
		return IndirectPassHeader::Unknown;
	}
}
/*
	llvm::PassManager tmpPassManager;
	llvm::PassManagerBuilder builder;
//taken from opt
llvm::TargetLibraryInfo *tli = 
new llvm::TargetLibraryInfo(llvm::Triple(module->getTargetTriple()));
tmpPassManager.add(tli);
llvm::TargetData *td = 0;
const std::string &moduleDataLayout = module->getDataLayout();
if(!moduleDataLayout.empty())
td = new llvm::TargetData(moduleDataLayout);
if(td)
tmpPassManager.add(td);
llvm::PassManager& PM = tmpPassManager;
//add em all!
builder.OptLevel = 2;
builder.DisableSimplifyLibCalls = false;
builder.populateModulePassManager(PM);
//let's see if this fixes the issue
llvm::PassRegistry* registry = llvm::PassRegistry::getPassRegistry();
const llvm::PassInfo* ci = registry->getPassInfo(
llvm::StringRef("function-to-knowledge"));
const llvm::PassInfo* ls = registry->getPassInfo(
llvm::StringRef("loop-simplify"));
const llvm::PassInfo* bce = registry->getPassInfo(
llvm::StringRef("break-crit-edges"));
ExpertSystem::FunctionKnowledgeConversionPass* copy = 
(ExpertSystem::FunctionKnowledgeConversionPass*)ci->createPass();
copy->setEnvironment(tEnv);
tmpPassManager.add(ls->createPass());
tmpPassManager.add(bce->createPass());
tmpPassManager.add(copy);
tmpPassManager.add(llvm::createVerifierPass());
tmpPassManager.run(*module);

*/
namespace pipeline {
	namespace clips {
		void initializeCLIPSIndirector() {
			IndirectPassRegistry& indirectRegistry = *IndirectPassRegistry::getIndirectPassRegistry();
			indirectRegistry.registerPassGenerator<CLIPSPassGenerator>("clips");
		}
	}
}
#undef pass_registered 
#undef optimize 
#undef unregister_pass 
#undef register_pass 
#undef werror
#undef msg
#undef copy
#undef BoolCast
