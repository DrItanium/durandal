#ifndef _argument_builder_h
#define _argument_builder_h
#include "ExpertSystem/CLIPSValueBuilder.h"
#include "llvm/Argument.h"

using namespace llvm;
class CLIPSArgumentBuilder : public CLIPSValueBuilder {
   public:
      CLIPSArgumentBuilder(std::string nm, FunctionNamer& namer);
      void addFields(Argument* arg, KnowledgeConstructor* kc, char* parent);
      void build(Argument* arg, KnowledgeConstructor* kc, char* parent);
};
#endif
