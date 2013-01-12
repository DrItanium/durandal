#ifndef _clips_function_builder_h
#define _clips_function_builder_h
#include "ExpertSystem/CLIPSConstantBuilder.h"

using namespace llvm;
   class CLIPSFunctionBuilder : public CLIPSGlobalValueBuilder {
      public:
         CLIPSFunctionBuilder(std::string nm, FunctionNamer& namer);
         void addFields(Function* fn, KnowledgeConstructor* kc, bool traverseBlocks = false);
         void build(Function* fn, KnowledgeConstructor* kc, bool traverseBlocks = false);
   };

#endif
