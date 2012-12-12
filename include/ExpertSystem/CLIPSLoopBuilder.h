#ifndef _loop_builder_h
#define _loop_builder_h
#include "ExpertSystem/CLIPSObjectBuilder.h"
#include "llvm/Analysis/LoopInfo.h"
class CLIPSLoopBuilder : public CLIPSObjectBuilder {
   public:
      CLIPSLoopBuilder(std::string nm, FunctionNamer& namer);
      void addFields(Loop* loop, KnowlegeConstructor *kc, char* parent);
		void build(Loop* loop, KnowlegeConstructor *kc, char* parent);
};
#endif
