#ifndef _clips_region_builder_h
#define _clips_region_builder_h
#include "ExpertSystem/CLIPSObjectBuilder.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/RegionIterator.h"

using namespace llvm;
class CLIPSRegionBuilder : public CLIPSObjectBuilder {
	public:
		CLIPSRegionBuilder(std::string nm, FunctionNamer& namer);
		void addFields(Region* region, KnowledgeConstruction* kc, char* parent);
		void build(Region* r, KnowledgeConstruction* kc, char* parent);
};

#endif
