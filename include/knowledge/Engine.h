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
template<typename T>
void registerExternalAddressId(void* theEnv, struct externalAddressType* ea) {
	RegisterExternalAddressId(theEnv, ExternalAddressRegistration<T>::indirectId, ea);
}
template<typename T>
bool containsExternalAddressId(void* theEnv) {
	return ContainsExternalAddressId(theEnv, ExternalAddressRegistration<T>::indirectId);
}
template<typename T>
int getExternalAddressId(void* theEnv) {
	return GetExternalAddressId(theEnv, ExternalAddressRegistration<T>::indirectId);
}
// SO FUCKING BEAUTIFUL :D
#define WhenInstanceDoesNotExist(env, instance) \
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, instance); \
if (potentiallyAlreadyExistingInstance != NULL) { \
	return potentiallyAlreadyExistingInstance; \
} else 
#define Otherwise(type, env, val) \
	return constructInstance<type>(env, val)

template<typename P, typename T>
void* constructInstance(void* theEnv, T* nativeInstance) {
	std::string tmp;
	llvm::raw_string_ostream str(tmp);
	str << "( of ";
	ElectronClassNameSelector<T>::selectName(str);
	str << " ";
	buildInstance(str, theEnv, nativeInstance);
	str << ")";
	RegisterNativeInstance(theEnv, nativeInstance, makeInstance(theEnv, tmp.c_str()));
	populateInstance(theEnv, nativeInstance);
	return GetNativeInstance(theEnv, nativeInstance);
}
template<typename P, typename T>
void* dispatch(void* theEnv, T* nativeInstance) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		Otherwise(T, theEnv, nativeInstance);
	}
}

template<typename P, typename T>
void* dispatch(void* theEnv, T& nativeInstance) {
	WhenInstanceDoesNotExist(theEnv, &nativeInstance) {
		Otherwise(T, theEnv, &nativeInstance);
	}
}




}
#endif // _KNOWLEDGE_ENGINE_H_
