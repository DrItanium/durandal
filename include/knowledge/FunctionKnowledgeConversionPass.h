#ifndef _function_knoledge_conversion_pass_h
#define _function_knoledge_conversion_pass_h
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"
extern "C" {
#include "clips.h"
}
namespace knowledge {
	struct FunctionKnowledgeConversionPass : public llvm::FunctionPass {
		static char ID;
		private:
		void* theEnv;
		/*
		bool skipRegions;
		bool skipLoops;
		*/
		bool runOnFunctionImpl(llvm::Function& fn);
		public:
		FunctionKnowledgeConversionPass() : FunctionPass(ID) {
			theEnv = CreateEnvironment();
		}
		~FunctionKnowledgeConversionPass() {
			DestroyEnvironment(theEnv);
			theEnv = 0;
		}
		/*
		void setSkipRegions(bool sRegions);
		void setSkipLoops(bool sLoops);
		virtual void getAnalysisUsage(AnalysisUsage &info) const {
			if(!skipRegions) {
				info.addRequired<LoopInfo>();
			}
			if(!skipLoops) {
				info.addRequired<RegionInfo>();
			}
		}
		*/
		virtual bool runOnFunction(llvm::Function& fn) {
			return runOnFunctionImpl(fn);
		}

	};
}
#endif
