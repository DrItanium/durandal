#ifndef _jsedgeremovalpass_h
#define _jsedgeremovalpass_h
#include "llvm/Pass.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CFG.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Instructions.h"
#include "llvm/Analysis/RegionIterator.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;

namespace durandal {
	namespace passes {
		struct JSEdgeRemovalPass : public FunctionPass {
			static char ID;
			JSEdgeRemovalPass() : FunctionPass(ID) { }
			using FunctionPass::getAnalysisUsage;
			using FunctionPass::runOnFunction;
			virtual void getAnalysisUsage(AnalysisUsage &Info) const;
			virtual bool runOnFunction(Function& f);
			void padExitBlocks(LoopInfo& li);
			void padExitBlocks(LoopInfo& li, Loop* loop);
			void padExitBlocks(RegionInfo& ri);
			void padExitBlocks(RegionInfo& ri, Region* target);

		};
	}
}

#endif
