#ifndef _clips_module_builder_h
#define _clips_module_builder_h

#include "ExpertSystem/CLIPSObjectBuilder.h"

using namespace llvm;
class CLIPSModuleBuilder : public CLIPSObjectBuilder {
   public:
      CLIPSModuleBuilder(std::string nm, std::string ty = "LLVMModule");
      void build(Module* mod, KnowledgeConstructor* kc);
      void addFields(Module* mod, KnowledgeConstructor* kc);
};

#endif
