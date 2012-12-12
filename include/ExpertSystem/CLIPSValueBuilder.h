#ifndef _clips_value_builder_h 
#define _clips_value_builder_h
#include "llvm/Value.h"
#include "ExpertSystem/CLIPSObjectBuilder.h"

using namespace llvm;

class CLIPSValueBuilder : public CLIPSObjectBuilder {
   public:
      CLIPSValueBuilder(std::string nm, std::string ty, FunctionNamer& namer);
      void setType(Type* t, KnowledgeConstructor* kc);
      void addFields(Value* val, KnowledgeConstructor* kc, char* parent);
		void build(Value* val, KnowledgeConstructor* kc, char* parent);
};
#endif
