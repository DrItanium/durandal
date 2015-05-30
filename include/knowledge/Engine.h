#ifndef _KNOWLEDGE_ENGINE_H_
#define _KNOWLEDGE_ENGINE_H_
#include "knowledge/EngineBookkeeping.h"
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
template<typename T>
struct ElectronClassNameSelector {
	static void selectName(llvm::raw_string_ostream& str) {
		str << "";
	}
};

template<typename T>
struct ExternalAddressRegistration {
	static int indirectId;
};
#define ElectronClassNameAssociation(type, className) \
	template<> \
struct ElectronClassNameSelector<type> { \
	static void selectName(llvm::raw_string_ostream& str) { \
		str << className ; \
	} \
}
// SO FUCKING BEAUTIFUL :D
#define WhenInstanceDoesNotExist(env, instance) \
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, instance); \
if (potentiallyAlreadyExistingInstance != NULL) { \
	return potentiallyAlreadyExistingInstance; \
} else 
#define Otherwise(pass, type, env, val) \
	return ProcessingNode<pass,type>::constructInstance(env, val)

template<typename P, typename T>
void* dispatch(void* theEnv, T* instance) {
	WhenInstanceDoesNotExist(theEnv, instance) {
		Otherwise(T, theEnv, instance);
	}
}

template<typename P, typename T>
void* dispatch(void* theEnv, T& instance) {
	WhenInstanceDoesNotExist(theEnv, &instance) {
		Otherwise(T, theEnv, &instance);
	}
}




}
#endif // _KNOWLEDGE_ENGINE_H_
