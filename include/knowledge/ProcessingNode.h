#ifndef _PROCESSING_NODE_H_
#define _PROCESSING_NODE_H_
#include "knowledge/EngineBookkeeping.h"
#include "knowledge/Engine.h"
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
struct ProcessingNode {
	static void* constructInstance(void* env, T* inst) {
		std::string tmp;
		llvm::raw_string_ostream str(tmp);
		str << "( of ";
		ElectronClassNameSelector<T>::selectName(str);
		str << " ";
		ProcessingNode<Pass,T>::buildInstance(str, env, inst);
		str << ")";
		RegisterNativeInstance(env, inst, makeInstance(env, tmp.c_str()));
		ProcessingNode<Pass,T>::populateInstance(env, inst);
		return GetNativeInstance(env, inst);
	}
	static void populateInstance(void* env, T* instance) { }
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Module * data) { }
};

}
#endif
