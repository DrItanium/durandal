#ifndef _clips_basic_block_builder_h
#define _clips_basic_block_builder_h
#include "ExpertSystem/CLIPSValueBuilder.h"
#include "llvm/BasicBlock.h"

using namespace llvm;
class CLIPSBasicBlockBuilder : public CLIPSValueBuilder {
   public:
      CLIPSBasicBlockBuilder(std::string nm, FunctionNamer& namer);
      void addFields(BasicBlock* bb, KnowledgeConstructor* kc, char* parent, 
            bool constructInstructions = true);
		void build(BasicBlock* bb, KnowledgeConstructor* kc, char* parent, 
            bool constructInstructions = true);
};
#endif
