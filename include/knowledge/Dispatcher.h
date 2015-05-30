#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_
#include "knowledge/EngineBookkeeping.h"
#include "knowledge/Engine.h"
#include "knowledge/ProcessingNode.h"
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
template<typename Pass, typename T>
struct Router {
	static void* dispatch(void* env, T& inst) {
		return Router<Pass,T>::dispatch(env, &inst);
	}
	static void* dispatch(void* env, T* inst) {
		WhenInstanceDoesNotExist(env, inst) {
			return ProcessingNode<Pass,T>::constructInstance(env, inst);
		}
	}
};
}
#endif
