#ifndef _KNOWLEDGE_ENGINE_H_
#define _KNOWLEDGE_ENGINE_H_
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
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
extern "C" {
#include "clips.h"
}
#define ENGINE_BOOKKEEPING_DATA USER_ENVIRONMENT_DATA + 0
extern "C" RegisterEngineBookkeeping(void* theEnv);
#define EngineBookkeepingData(theEnv) \
	((struct knowledge::EngineBookkeeping*) GetEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA))
namespace knowledge {
struct EngineBookkeeping {
// just the pointers ma'am
	llvm::DenseMap<void*, void*> instanceMap;
};
void* convert(void* theEnv, llvm::Module* module);
void* convert(void* theEnv, llvm::BasicBlock* block, void* parent);
void* convert(void* theEnv, llvm::Instruction* inst, void* parent);
void* convert(void* theEnv, llvm::Function* func, void* parent);
void* convert(void* theEnv, llvm::Loop* loop, void* parent);
void* convert(void* theEnv, llvm::Region* region, void* parent);
void* convert(void* theEnv, llvm::Constant* constant, void* parent);
void* convert(void* theEnv, llvm::Value* value, void* parent);
void* convert(void* theEnv, llvm::User* user, void* parent);
void* convert(void* theEnv, llvm::Operator* op, void* parent);
void* convert(void* theEnv, llvm::Argument* arg, void* parent);
void* convert(void* theEnv, llvm::GlobalVariable* var);
void* convert(void* theEnv, llvm::GlobalAlias* alias);
void* convert(void* theEnv, llvm::GlobalValue* value);
void* convert(void* theEnv, llvm::Type* type);
}

#endif // _KNOWLEDGE_ENGINE_H_
