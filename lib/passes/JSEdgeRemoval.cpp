#include "JSEdgeRemovalPass.h"
namespace durandal {
	namespace passes {
		void JSEdgeRemovalPass::getAnalysisUsage(AnalysisUsage &Info) const{
			//Info.addRequired<LoopInfo>();
			Info.addRequired<RegionInfo>();
		}
		bool JSEdgeRemovalPass::runOnFunction(Function& f) {
			//Split critical edges aka JSEdges
			for(Function::iterator i = f.begin(), e = f.end(); i != e; ++i) {
				BasicBlock* bb = i;
				llvm::TerminatorInst* tInst = bb->getTerminator();
				if(!IndirectBrInst::classof(tInst)) {
					for(succ_iterator PI = succ_begin(bb), E = succ_end(bb); PI != E; ++PI) {
						if(isCriticalEdge(tInst, PI.getSuccessorIndex())) {
							SplitCriticalEdge(bb->getTerminator(), PI.getSuccessorIndex(), this);
						}
					}
				}
			}
			RegionInfo &ri = getAnalysis<RegionInfo>();
			padExitBlocks(ri);
			//Pad exit blocks for regions and loops via splitting
			//         LoopInfo &li = getAnalysis<LoopInfo>();
			//        padExitBlocks(li);
			return true;
		}

		void JSEdgeRemovalPass::padExitBlocks(LoopInfo& li) {
			for(LoopInfo::iterator b = li.begin(), e = li.end(); b != e; ++b) {
				Loop* l = *b;
				padExitBlocks(li, l);
			}
		}
		void JSEdgeRemovalPass::padExitBlocks(LoopInfo& li, Loop* loop) {
			SmallVector<BasicBlock*, 128> exitBlocks;
			loop->getExitBlocks(exitBlocks);
			for(SmallVectorImpl<BasicBlock*>::iterator i = exitBlocks.begin(),
					e = exitBlocks.end(); i != e; ++i) {
				BasicBlock* bb = (*i);
				SmallVector<BasicBlock*,32> elements;
				for(pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
					BasicBlock* predbb = *PI;
					if(loop->contains(predbb)) {
						elements.push_back(predbb);
					}
				}
				SplitBlockPredecessors(bb, elements, "Pad", this);
			}
			//  li.verifyAnalysis();
			for(Loop::iterator q = loop->begin(), qu = loop->end(); q != qu; ++q) {
				Loop* subLoop = *q;
				padExitBlocks(li, subLoop);
			}
		}
		void JSEdgeRemovalPass::padExitBlocks(RegionInfo& ri) {
			Region* top = ri.getTopLevelRegion();
			if(top != 0) {
				padExitBlocks(ri, top);
			}
		}

		void JSEdgeRemovalPass::padExitBlocks(RegionInfo& ri, Region* target) {
			for(Region::element_iterator i = target->element_begin(), e = target->element_end(); i != e; ++i) {
				RegionNode* rn = (RegionNode*)(*i);
				if(rn->isSubRegion()) {
					Region* subRegion = rn->getNodeAs<Region>();
					padExitBlocks(ri, subRegion);
				}
			}
			BasicBlock* exit = target->getExit();
			if(exit != NULL) {
				//we need to split the block into two parts to give padding space
				//Keep the phi nodes and landing pad instructions in the original
				//block and push all instructions into the new block below. That
				//way we have convergence yet space to move elements up onto a
				//given path if it turns out that the given instruction can't 

				//32 is a good number
				unsigned count = 0;
				SmallVector<BasicBlock*,32> elements;
				for(pred_iterator PI = pred_begin(exit), E = pred_end(exit); PI != E; ++PI) {
					BasicBlock* bb = *PI;
					if(target->contains(bb)) {
						elements.push_back(bb);
					}
					count++;
				}
				if(elements.size() > 0 && count != elements.size()) { 
					BasicBlock* update = SplitBlockPredecessors(exit, elements, "Pad", this);
					target->replaceExit(update);
				}
				//ri.splitBlock(update, exit);
			}

		}
	}
}
char durandal::passes::JSEdgeRemovalPass::ID = 0;
static RegisterPass<durandal::passes::JSEdgeRemovalPass> Z("jsedge", "JS Edge Removal");


