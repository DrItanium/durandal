#ifndef _function_knoledge_conversion_pass_h
#define _function_knoledge_conversion_pass_h
#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/Types.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"
using namespace llvm;
namespace ExpertSystem {
	struct FunctionKnowledgeConversionPass : public FunctionPass {
		static char ID;
		private:
		CLIPSEnvironment* env;
		bool skipRegions;
		bool skipLoops;
		bool runOnFunctionImpl(Function& fn);
		public:
		FunctionKnowledgeConversionPass() : FunctionPass(ID) { }
		void setEnvironment(CLIPSEnvironment* e);
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
		virtual bool runOnFunction(Function& fn) {
			return runOnFunctionImpl(fn);
		}

	};
}
#endif
