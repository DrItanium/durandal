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
	static void* constructInstance(void* env, T* inst, Pass& pass);
	static void populateInstance(void* env, T* instance, Pass& pass);
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, T* data, Pass& pass);
	static void* getInstanceName(void* theEnv, T* instance, Pass& pass);
	static void setParent(void* theEnv, T* target, Pass& pass);
};
template<typename Pass, typename T>
struct Router {
	static void* dispatch(void* env, T* inst, Pass& pass) {
		WhenInstanceDoesNotExist(env, inst) {
			return ProcessingNode<Pass,T>::constructInstance(env, inst, pass);
		}
	}
};

template<typename Pass, typename T>
void* dispatch(void* env, T* inst, Pass& pass) {
	return Router<Pass,T>::dispatch(env, inst, pass);
}
template<typename Pass, typename T>
void* dispatch(void* env, T& inst, Pass& pass) {
	return Router<Pass,T>::dispatch(env, &inst, pass);
}

template<typename Pass, typename T>
void* ProcessingNode<Pass,T>::getInstanceName(void* env, T* inst, Pass& p) {
	return EnvAddSymbol(env, EnvGetInstanceName(env, dispatch(env, inst, p)));
}

template<typename Pass, typename T>
void* getInstanceName(void* theEnv, T* instance, Pass& pass) {
	return ProcessingNode<Pass,T>::getInstanceName(theEnv, instance, pass);
}

template<typename Pass, typename T>
void setParent(void* theEnv, T* inst, Pass& pass) {
	ProcessingNode<Pass,T>::setParent(theEnv, inst, pass);
}

template<typename Pass, typename T>
void ProcessingNode<Pass,T>::setParent(void* theEnv, T* target, Pass& pass) {
	DATA_OBJECT wrapper;
	void* instanceName = knowledge::getInstanceName(theEnv, target->getParent(), pass);
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, instanceName);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
}
template<typename Pass, typename T>
void ProcessingNode<Pass,T>::populateInstance(void* env, T* instance, Pass& pass) { }

template<typename Pass, typename T>
void ProcessingNode<Pass,T>::buildInstance(llvm::raw_string_ostream& str, void* theEnv, T* data, Pass& pass) { }

template<typename Pass, typename T>
void* ProcessingNode<Pass,T>::constructInstance(void* env, T* inst, Pass& pass) {
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

}
#endif
