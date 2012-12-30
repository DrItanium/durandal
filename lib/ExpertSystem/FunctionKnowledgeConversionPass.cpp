#include "ExpertSystem/FunctionKnowledgeConversionPass.h"


using namespace llvm;

namespace ExpertSystem {
  void FunctionKnowledgeConversionPass::setEnvironment(CLIPSEnvironment* e) {
    env = e;
  }
  void FunctionKnowledgeConversionPass::setSkipRegions(bool sRegions) {
	  skipRegions = sRegions;
  }
  void FunctionKnowledgeConversionPass::setSkipLoops(bool sLoops) {
	  skipLoops = sLoops;
  }
  bool FunctionKnowledgeConversionPass::runOnFunctionImpl(Function& fn) {
    if(env) {
      KnowledgeConstructor kc;
		if(!skipRegions && !skipLoops) {
        RegionInfo& ri = getAnalysis<llvm::RegionInfo>();
        LoopInfo& li = getAnalysis<llvm::LoopInfo>();
        kc.route(fn, li, ri);
		} else if (!skipRegions && skipLoops) {
        RegionInfo& ri = getAnalysis<llvm::RegionInfo>();
        kc.route(fn, ri);
		} else if (!skipLoops && skipRegions) {
        LoopInfo& li = getAnalysis<llvm::LoopInfo>();
        kc.route(fn, li);
		} else {
			kc.route(fn);
		}
      env->makeInstances((char*)kc.getInstancesAsString().c_str());
    }
    return false;
  }
}

char ExpertSystem::FunctionKnowledgeConversionPass::ID = 0;

static RegisterPass<ExpertSystem::FunctionKnowledgeConversionPass> fkcp(
    "function-to-knowledge",
    "Convert a given function to knowledge");
