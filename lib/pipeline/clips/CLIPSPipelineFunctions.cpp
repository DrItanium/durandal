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
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "pipeline/clips/CLIPSPassGenerator.h"
#include "pipeline/clips/CLIPSPass.h"
#include "pipeline/clips/CLIPSPassHeader.h"
#include "rampancy/Cortex.h"
#include "rampancy/CompilerManager.h"
#include "rampancy/CompilerRegistry.h"
#include "indirect/Indirector.h"
#include "llvm/Support/raw_ostream.h"
#include "ExpertSystem/Types.h"


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
			PTIEF CLIPSOptimizeCode, 
			msg("CLIPSOptimizeCode"));
	EnvDefineFunction(theEnv, register_pass, 'v',
			PTIEF CLIPSRegisterPass, 
			msg("CLIPSRegisterPass"));
	EnvDefineFunction(theEnv, unregister_pass, 'v',
			PTIEF CLIPSUnregisterPass, 
			msg("CLIPSUnregisterPass"));
	EnvDefineFunction2(theEnv, pass_registered, 'w', 
			PTIEF CLIPSPassRegistered, 
			msg("CLIPSPassRegistered"),
			msg("11k"));
}

void CLIPSOptimizeCode(void* theEnv) {
	/* optimize has the following arguments
	 * 1) pointer to the module that we are going to use (integer)
	 * 2) Base optimization level (0-3) (integer)
	 * 3) List of passes to add (multifield)
	 * RETURNS: Nothing
	 */
	DATA_OBJECT modulePointerDO,
					baseOptLevelDO,
					passesDO;
	void* passesMultifield;
	int passesEnd, passesLength;
	if(EnvArgCountCheck(theEnv, optimize, EXACTLY, 3) == -1) {
		return;
	}

	if(EnvArgTypeCheck(theEnv, optimize, 1, INTEGER, &modulePointerDO) == FALSE) {
		return;
	}

	if(EnvArgTypeCheck(theEnv, optimize, 2, INTEGER, &baseOptLevelDO) == FALSE) {
		return;
	}

	if(EnvArgTypeCheck(theEnv, optimize, 3, MULTIFIELD, &passesDO) == FALSE) {
		return;
	}
	//can be really unsafe :D, eventually I'll want to replace this with EXTERNAL_ADDRESSES
	llvm::Module* module = (llvm::Module*)(PointerAddress)DOToLong(modulePointerDO);
	int value = DOToInteger(baseOptLevelDO);
	//normalize the value automatically
	if(value < 0) {
		value = 0;
	} 
	if(value > 3) {
		value = 3;
	}

	passesLength = GetDOLength(passesDO);
	if(passesLength > 0) {
		llvm::PassManager tmpPassManager;
		llvm::PassManagerBuilder builder;
		//taken from opt
		llvm::TargetLibraryInfo *tli = 
			new llvm::TargetLibraryInfo(llvm::Triple(module->getTargetTriple()));
		tmpPassManager.add(tli);
		llvm::TargetData *td = 0;
		const std::string &moduleDataLayout = module->getDataLayout();
		if(!moduleDataLayout.empty()) {
			td = new llvm::TargetData(moduleDataLayout);
		}
		if(td) {
			tmpPassManager.add(td);
		}
		llvm::PassManager& PM = tmpPassManager;
		builder.OptLevel = value;
		builder.DisableSimplifyLibCalls = false;
		builder.populateModulePassManager(PM);
		//let's see if this fixes the issue
		llvm::PassRegistry& registry = *llvm::PassRegistry::getPassRegistry();
		IndirectPassRegistry& indirectRegistry = *IndirectPassRegistry::getIndirectPassRegistry();
		passesMultifield = GetValue(passesDO);
		passesEnd = GetDOEnd(passesDO);
		for(int i = GetDOBegin(passesDO); i <= passesEnd; ++i) {
			if(GetMFType(passesMultifield, i) == SYMBOL) {
				/* There are two groups of passes: Indirect and Native
				 * 
				 * Native passes are those that were defined in C++ using the
				 * standard mechanisms provided by LLVM. 
				 *
				 * Indirect passes are those that were defined using libindirect.
				 * In this context, an indirect pass is one written in CLIPS but
				 * registered in the llvm::PassManager. 
				 *
				 * There is a catch, since indirect passes do not have a default
				 * constructor it is necessary to determine if a pass is an
				 * indirect one so it can be constructed. 
				 *
				 * This is actually really easy to do because of the indirect pass
				 * registry. A native pass is a pass that is registered in the
				 * llvm::PassRegistry and not in the IndirectPassRegistry. 
				 *
				 * An indirect pass is a pass that is registered in both the
				 * llvm::PassRegistry and the IndirectPassRegistry. 
				 */
				char* tmpName = ValueToString(GetMFValue(passesMultifield, i));
				std::string c(tmpName);

				IndirectPassHeader* header = indirectRegistry.getIndirectPassHeader(c.c_str());
				if(header) { //It is an indirect header
					llvm::Pass* p = indirectRegistry.createPass(llvm::StringRef(c.c_str()));
               if(!p) {
                  EnvPrintRouter(theEnv, werror,
                        msg("The given pass "));
                  EnvPrintRouter(theEnv, werror,
                        msg(c.c_str()));
                  EnvPrintRouter(theEnv, werror, 
                        msg(" exists but for some reason wasn't created\n"));
                  return;
               } else {
					   //we know it's a CLIPS pass
                  indirect::IndirectPass* ps = (indirect::IndirectPass*)p;
                  //set the header
                  ps->setIndirectPassHeader(header);
					   pipeline::clips::CLIPSPass* tmpPass = (pipeline::clips::CLIPSPass*)p;
					   tmpPass->setEnvironment(theEnv);
					   PM.add(p);
               }
				} else { //it isn't an indirect header (native)
					const llvm::PassInfo* pass = registry.getPassInfo(llvm::StringRef(c.c_str()));
               if(pass) {
					   PM.add(pass->createPass());
               } else {
                  EnvPrintRouter(theEnv, werror, msg("The given pass "));
                  EnvPrintRouter(theEnv, werror, msg(c.c_str()));
                  EnvPrintRouter(theEnv, werror, msg(" is not a valid pass\n"));
                  return;
               }
				}
				//regardless of pass type, we should add a verifier pass after it.	
				PM.add(llvm::createVerifierPass());
			} 
		}
		PM.run(*module);
	} else {
		EnvPrintRouter(theEnv, werror,
				msg("ERROR: No passes provided to be executed!\n"));
		return;
	}
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

	DATA_OBJECT nameDO, 
					descDO, 
					typeDO, 
					isAnalysisDO, 
					isCFGDO,
					needRegionsDO,
					needLoopsDO,
					passesDO,
					requiredDO,
					requiredTransitiveDO,
					preservedDO,
					preservesAllDO,
					preservesCFGDO;
	char *nameTmp,
		  *descTmp,
		  *typeTmp,
		  *isAnalysisTmp,
		  *isCFGTmp,
		  *needRegionsTmp,
		  *needLoopsTmp,
		  *preservesAllTmp,
		  *preservesCFGTmp;
	bool isAnalysis, 
		  isCFG, 
		  needRegions, 
		  needLoops,
		  preservesAll, 
		  preservesCFG;
	int passesLength, 
		 requiredLength, 
		 requiredTransitiveLength, 
		 preservedLength,
		 passesEnd,
		 requiredEnd,
		 requiredTransitiveEnd,
		 preservedEnd;
	void *passesMultifield,
		  *requiredMultifield,
		  *requiredTransitiveMultifield,
		  *preservedMultifield;
	pipeline::clips::CLIPSPassHeader* header = new pipeline::clips::CLIPSPassHeader();
	header->setTemplateSet("clips");
	if(EnvArgCountCheck(theEnv, register_pass, EXACTLY, 13) == -1) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 1, SYMBOL_OR_STRING, &nameDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 2, SYMBOL_OR_STRING, &descDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 3, SYMBOL_OR_STRING, &typeDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 4, SYMBOL, &isAnalysisDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 5, SYMBOL, &isCFGDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 6, SYMBOL, &needRegionsDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 7, SYMBOL, &needLoopsDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 8, MULTIFIELD, &passesDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 9, MULTIFIELD, &requiredDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 10, MULTIFIELD, &requiredTransitiveDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 11, MULTIFIELD, &preservedDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 12, SYMBOL, &preservesAllDO) == FALSE) { return; }
	if(EnvArgTypeCheck(theEnv, register_pass, 13, SYMBOL, &preservesCFGDO) == FALSE) { return; }

	nameTmp = DOToString(nameDO);
	header->setPassName((const char*)nameTmp);

	descTmp = DOToString(descDO);
	header->setPassDescription((const char*)descTmp);

	typeTmp = DOToString(typeDO);
	header->setPassType(TranslateInput(typeTmp));

	isAnalysisTmp = DOToString(isAnalysisDO);
	BoolCast(isAnalysisTmp, isAnalysis);
	header->setIsAnalysis(isAnalysis);

	isCFGTmp = DOToString(isCFGDO);
	BoolCast(isCFGTmp, isCFG);
	header->setIsCFGOnlyPass(isCFG);

	needRegionsTmp = DOToString(needRegionsDO);
	BoolCast(needRegionsTmp, needRegions);
	header->setNeedsRegions(needRegions);

	needLoopsTmp = DOToString(needLoopsDO);
	BoolCast(needLoopsTmp, needLoops);
	header->setNeedsLoops(needLoops);

	preservesAllTmp = DOToString(preservesAllDO);
	BoolCast(preservesAllTmp, preservesAll);
	header->setPreservesAll(preservesAllTmp);

	preservesCFGTmp = DOToString(preservesCFGDO);
	BoolCast(preservesCFGTmp, preservesCFG);
	header->setPreservesCFG(preservesCFG);

	passesLength = GetDOLength(passesDO);
	if(passesLength > 0) {
		passesMultifield = GetValue(passesDO);
		std::string t0;
		raw_string_ostream passesStream(t0);
		passesEnd = GetDOEnd(passesDO);
		char* passesTmpPtr;
		for(int i = GetDOBegin(passesDO); i <= passesEnd; ++i) {
			if(GetMFType(passesMultifield, i) == SYMBOL) {
				passesTmpPtr = ValueToString(GetMFValue(passesMultifield, i));
				passesStream << passesTmpPtr << " ";
			} else {
				EnvPrintRouter(theEnv, werror,
						msg("ERROR: provided a pass name that wasn't a symbol!\n"));
				return;
			}
		}
		header->setPasses(passesStream.str().c_str());
	} else {
		header->setPasses((const char*)DOToString(nameDO));
	}

	requiredLength = GetDOLength(requiredDO);
	if(requiredLength > 0) {
		requiredMultifield = GetValue(requiredDO);
		requiredEnd = GetDOEnd(requiredDO);
		for(int i = GetDOBegin(requiredDO); i <= requiredEnd; ++i) {
			if(GetMFType(requiredMultifield, i) == SYMBOL) {
				char* tmpPtr = ValueToString(GetMFValue(requiredMultifield, i)); 
				std::string c(tmpPtr);
				header->addRequired(c.c_str());
			} else {
				EnvPrintRouter(theEnv, werror,
						msg("ERROR: provided a required analysis pass that wasn't a symbol!\n"));
				return;
			}
		}
	}

	requiredTransitiveLength = GetDOLength(requiredTransitiveDO);
	if(requiredTransitiveLength > 0) {
		requiredTransitiveMultifield = GetValue(requiredTransitiveDO);
		requiredTransitiveEnd = GetDOEnd(requiredTransitiveDO);
		for(int i = GetDOBegin(requiredTransitiveDO);
				i <= requiredTransitiveEnd; ++i) {
			if(GetMFType(requiredTransitiveMultifield, i) == SYMBOL) {
				char* tmpPtr = ValueToString(GetMFValue(requiredTransitiveMultifield, i));
				std::string c(tmpPtr);
				header->addRequiredTransitive(c.c_str());
			} else {
				EnvPrintRouter(theEnv, werror,
						msg("ERROR: provided a required transitive analysis pass that wasn't a symbol!\n"));
				return;
			}
		}
	}

	preservedLength = GetDOLength(preservedDO);
	if(preservedLength > 0) {
		preservedMultifield = GetValue(preservedDO);
		preservedEnd = GetDOEnd(preservedDO);
		for(int i = GetDOBegin(preservedDO); i <= preservedEnd; ++i) {
			if(GetMFType(preservedMultifield, i) == SYMBOL) {
				char* tmpPtr = ValueToString(GetMFValue(preservedMultifield, i));
				std::string c(tmpPtr);
				header->addPreserved(c.c_str());
			} else {
				EnvPrintRouter(theEnv, werror,
						msg("ERROR: provided a preserved analysis pass that wasn't a symbol!\n"));
				return;
			}
		}
	}
	//Once everything is done, register it with the indirect pass registry
	IndirectPassRegistry& indirectRegistry = *IndirectPassRegistry::getIndirectPassRegistry();
	indirectRegistry.registerIndirectPassHeader(header);
}

void CLIPSUnregisterPass(void* theEnv) {
	/* This function takes in one argument
	 * 1) passArg (string)
	 */
	DATA_OBJECT arg0;
	char* tmp;
	if(EnvArgCountCheck(theEnv, unregister_pass, EXACTLY, 1) == -1) {
		return;
	}

	if(EnvArgTypeCheck(theEnv, unregister_pass, 1, SYMBOL_OR_STRING, &arg0) == FALSE) {
		return;
	}
	tmp = DOToString(arg0);
	std::string c(tmp);
	IndirectPassRegistry& indirectRegistry = *IndirectPassRegistry::getIndirectPassRegistry();
	IndirectPassHeader* header = indirectRegistry.getIndirectPassHeader(c.c_str());
	if(header) {
		//this will also deregister the pass from the llvm::PassRegistry as well.
		indirectRegistry.unregisterIndirectPassHeader(llvm::StringRef(c.c_str()));
	} else {
		llvm::PassRegistry& registry = *llvm::PassRegistry::getPassRegistry();
		const llvm::PassInfo* ci = registry.getPassInfo(llvm::StringRef(c.c_str()));
		if(ci) {
			registry.unregisterPass(*ci);
		} else {
			EnvPrintRouter(theEnv, werror, msg("ERROR: target pass "));
			EnvPrintRouter(theEnv, werror, (char*)c.c_str());
			EnvPrintRouter(theEnv, werror, msg(" does not exist!\n"));
		}
	}
}

void* CLIPSPassRegistered(void* theEnv) {
	DATA_OBJECT arg0;
	char *a, *b;
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
