#ifndef _ENGINE_H_
#define _ENGINE_H_
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/LoopPass.h"
extern "C" void RegisterEngineBookkeeping(void* theEnv);
namespace knowledge {
bool convert(void* env, llvm::BasicBlock* blk, llvm::BasicBlockPass* pass);
bool convert(void* env, llvm::Function* fn, llvm::FunctionPass* pass);
bool convert(void* env, llvm::Module* mod, llvm::ModulePass* pass);
bool convert(void* env, llvm::Region* region, llvm::RegionPass* pass);
bool convert(void* env, llvm::Loop* loop, llvm::LoopPass* pass);
}
#endif
