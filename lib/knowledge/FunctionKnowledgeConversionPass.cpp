#include "knowledge/FunctionKnowledgeConversionPass.h"
#include "knowledge/ProcessingNode.h"


namespace knowledge {
bool FunctionKnowledgeConversionPass::runOnFunctionImpl(llvm::Function& fn) {
	knowledge::dispatch(theEnv, fn, &this);
	return false;
}
}

char knowledge::FunctionKnowledgeConversionPass::ID = 0;

static llvm::RegisterPass<knowledge::FunctionKnowledgeConversionPass> fkcp(
		"function-to-knowledge",
		"Convert a given function to knowledge");
