#ifndef _global_value_knowledge_representation_h
#define _global_value_knowledge_representation_h
#include "knowledge/Traits.h"
#include "knowledge/Types.h"
#include "llvm/GlobalValue.h"
namespace knowledge {
   DECLARE_CLIPS_TYPE_NAME(llvm::GlobalValue, "GlobalValue");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::GlobalValue);
   DECLARE_HAS_CUSTOM_ROUTER_LOGIC(llvm::GlobalValue);
}

#endif
