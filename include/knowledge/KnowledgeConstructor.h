#ifndef _knowledge_construction_pass_h
#define _knowledge_construction_pass_h
#include "llvm/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/Argument.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/User.h"
#include "llvm/Value.h"
#include "llvm/Type.h"
#include "llvm/BasicBlock.h"
#include "llvm/Analysis/RegionIterator.h"
#include "llvm/Operator.h"
#include "llvm/Metadata.h"
#include "llvm/InlineAsm.h"
#include "llvm/Argument.h"
#include "llvm/Constant.h"
#include "llvm/Constants.h"
#include "llvm/GlobalVariable.h"
#include "llvm/GlobalValue.h"
#include "llvm/GlobalAlias.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Operator.h"
#include "llvm/Function.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Value.h"
#include "ExpertSystem/Types.h"
#include "ExpertSystem/FunctionNamer.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include <string>

using namespace llvm;
namespace knowledge {
   class KnowledgeConstructor {
      llvm::DenseMap<PointerAddress, std::string>* names;
      llvm::raw_string_ostream* instanceStream; 
      std::string* tmp;
      void* theEnv;
      public:
      KnowledgeConstructor(void* theEnv);
      ~KnowledgeConstructor();
      std::string getInstancesAsString();
      void addToInstances(std::string &instance);
      void registerKnowledgeName(PointerAddress ptr, std::string &name);
      bool objectAlreadyConvertedToKnowledge(PointerAddress ptr);
      void gensym(char* buffer);
      std::string getObjectKnowledgeName(PointerAddress ptr);
   };
}
#endif
