#include "knowledge/FunctionKnowledgeConversionPass.h"
#include "knowledge/Engine.h"


namespace knowledge {
bool FunctionKnowledgeConversionPass::runOnFunctionImpl(llvm::Function& fn) {
	return knowledge::convert(theEnv, &fn, this);
}
}

char knowledge::FunctionKnowledgeConversionPass::ID = 0;

static llvm::RegisterPass<knowledge::FunctionKnowledgeConversionPass> fkcp(
		"function-to-knowledge",
		"Convert a given function to knowledge");
