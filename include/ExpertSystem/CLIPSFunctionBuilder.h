#ifndef _clips_function_builder_h
#define _clips_function_builder_h
#include "ExpertSystem/CLIPSConstantBuilder.h"

using namespace llvm;
class CLIPSFunctionBuilder : public CLIPSGlobalValueBuilder {
   public:
      CLIPSFunctionBuilder(std::string nm, FunctionNamer& namer);
      void setCallingConvention(CallingConv::ID id);
      void setAttributes(const AttributeSet& attr);
      void addFields(Function* fn, KnowledgeConstructor* kc);
      void build(Function* fn, KnowledgeConstructor* kc);
};

extern "C" void RegisterLLVMFunctionManipulationFunctions(void* theEnv);

#endif
