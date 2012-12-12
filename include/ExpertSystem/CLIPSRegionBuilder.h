#ifndef _clips_region_builder_h
#define _clips_region_builder_h
#include "ExpertSystem/CLIPSObjectBuilder.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/RegionIterator.h"

using namespace llvm;
class CLIPSRegionBuilder : public CLIPSObjectBuilder {
	public:
		CLIPSRegionBuilder(std::string nm, FunctionNamer& namer);
		void addFields(Region* region, KnowledgeConstructor* kc, char* parent);
		void build(Region* r, KnowledgeConstructor* kc, char* parent);
};

#endif
