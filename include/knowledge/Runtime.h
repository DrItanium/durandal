#ifndef _KNOWLEDGE_RUNTIME_H_
#define _KNOWLEDGE_RUNTIME_H_
#include "knowledge/ExternalAddressRegistration.h"
#include "knowledge/ClassNameSelector.h"
#include "knowledge/EngineBookkeeping.h"
#include "knowledge/ProcessingNode.h"
#include "knowledge/EngineDecl.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constant.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Casting.h"
extern "C" {
#include "clips.h"
}
namespace knowledge {
void* makeInstance(void* theEnv, const std::string& str);
inline void booleanField(llvm::raw_string_ostream& str, const std::string& name, bool value);
void directPutMultifield(void* theEnv, void* nativeInstance, const std::string& slotName, void* multifieldData, int multifieldBegin, int multifieldEnd);
}
#endif // _KNOWLEDGE_RUNTIME_H_
