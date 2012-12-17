#include "ExpertSystem/FunctionKnowledgeConversionPass.h"


using namespace llvm;

namespace ExpertSystem {
  void FunctionKnowledgeConversionPass::setEnvironment(CLIPSEnvironment* e) {
    env = e;
  }
  bool FunctionKnowledgeConversionPass::runOnFunctionImpl(Function& fn) {
    if(env) {
      KnowledgeConstructor kc;
      RegionInfo& ri = getAnalysis<llvm::RegionInfo>();
      LoopInfo& li = getAnalysis<llvm::LoopInfo>();
      kc.route(fn, li, ri);
      env->makeInstances((char*)kc.getInstancesAsString().c_str());
    }
    return false;
  }
}

char ExpertSystem::FunctionKnowledgeConversionPass::ID = 0;

static RegisterPass<ExpertSystem::FunctionKnowledgeConversionPass> fkcp(
    "function-to-knowledge",
    "Convert a given function to knowledge");
