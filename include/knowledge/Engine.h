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
#define X(unused, type, className) \
	ElectronClassNameAssociation(type, className); \
	template<> \
	struct ExternalAddressRegistration<type> { \
		static int indirectId; \
	}; \
	void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, type* data); \
	void populateInstance(void* theEnv, type* data);
#include "knowledge/EngineNodes.def"
#undef X
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
#define DefineCustomDispatch(type) \
	void* dispatch(void* theEnv, type* nativeInstance) 
#define CondDispatch(type, env, val) \
	if (type* v = llvm::dyn_cast<type>(val)) return dispatch(env, v)
#define Otherwise(type, env, val) \
	return constructInstance<type>(env, val)

template<typename T>
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
template<typename T>
void* dispatch(void* theEnv, T* nativeInstance) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		Otherwise(T, theEnv, nativeInstance);
	}
}

template<typename T>
void* dispatch(void* theEnv, T& nativeInstance) {
	WhenInstanceDoesNotExist(theEnv, &nativeInstance) {
		Otherwise(T, theEnv, &nativeInstance);
	}
}

DefineCustomDispatch(llvm::TerminatorInst) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::BranchInst, theEnv, nativeInstance);
		CondDispatch(llvm::IndirectBrInst, theEnv, nativeInstance);
		CondDispatch(llvm::InvokeInst, theEnv, nativeInstance);
		CondDispatch(llvm::ResumeInst, theEnv, nativeInstance);
		CondDispatch(llvm::ReturnInst, theEnv, nativeInstance);
		CondDispatch(llvm::SwitchInst, theEnv, nativeInstance);
		CondDispatch(llvm::UnreachableInst, theEnv, nativeInstance);
		Otherwise(llvm::TerminatorInst, theEnv, nativeInstance);
	}
}

DefineCustomDispatch(llvm::UnaryInstruction) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::AllocaInst, theEnv, nativeInstance);
		CondDispatch(llvm::CastInst, theEnv, nativeInstance);
		CondDispatch(llvm::ExtractValueInst, theEnv, nativeInstance);
		CondDispatch(llvm::LoadInst, theEnv, nativeInstance);
		CondDispatch(llvm::VAArgInst, theEnv, nativeInstance);
		Otherwise(llvm::UnaryInstruction, theEnv, nativeInstance);
	}
}

DefineCustomDispatch(llvm::CmpInst) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::FCmpInst, theEnv, nativeInstance);
		CondDispatch(llvm::ICmpInst, theEnv, nativeInstance);
		Otherwise(llvm::CmpInst, theEnv, nativeInstance);
	}
}

DefineCustomDispatch(llvm::Instruction) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::PHINode, theEnv, nativeInstance);
		CondDispatch(llvm::StoreInst, theEnv, nativeInstance);
		CondDispatch(llvm::BinaryOperator, theEnv, nativeInstance);
		CondDispatch(llvm::CallInst, theEnv, nativeInstance);
		CondDispatch(llvm::CmpInst, theEnv, nativeInstance);
		CondDispatch(llvm::ExtractElementInst, theEnv, nativeInstance);
		CondDispatch(llvm::FenceInst, theEnv, nativeInstance);
		CondDispatch(llvm::GetElementPtrInst, theEnv, nativeInstance);
		CondDispatch(llvm::InsertElementInst, theEnv, nativeInstance);
		CondDispatch(llvm::InsertValueInst, theEnv, nativeInstance);
		CondDispatch(llvm::LandingPadInst, theEnv, nativeInstance);
		CondDispatch(llvm::SelectInst, theEnv, nativeInstance);
		CondDispatch(llvm::ShuffleVectorInst, theEnv, nativeInstance);
		CondDispatch(llvm::TerminatorInst, theEnv, nativeInstance);
		CondDispatch(llvm::UnaryInstruction, theEnv, nativeInstance);
		Otherwise(llvm::Instruction, theEnv, nativeInstance);
	}
}

DefineCustomDispatch(llvm::User) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::Instruction, theEnv, nativeInstance);
		CondDispatch(llvm::Constant, theEnv, nativeInstance);
		CondDispatch(llvm::Operator, theEnv, nativeInstance);
		Otherwise(llvm::User, theEnv, nativeInstance);
	}
}
DefineCustomDispatch(llvm::Value) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::User, theEnv, nativeInstance);
		CondDispatch(llvm::BasicBlock, theEnv, nativeInstance);
		CondDispatch(llvm::Argument, theEnv, nativeInstance);
		// CondDispatch(llvm::InlineAsm, theEnv, value);
		// CondDispatch(llvm::MDNode, theEnv, value);
		// CondDispatch(llvm::MDString, theEnv, value);
		Otherwise(llvm::Value, theEnv, nativeInstance);
	}
}
DefineCustomDispatch(llvm::CastInst) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::AddrSpaceCastInst, theEnv, nativeInstance);
		CondDispatch(llvm::BitCastInst, theEnv, nativeInstance);
		CondDispatch(llvm::FPExtInst, theEnv, nativeInstance);
		CondDispatch(llvm::FPToSIInst, theEnv, nativeInstance);
		Otherwise(llvm::CastInst, theEnv, nativeInstance);
	}
}
//TODO: Finish defining this custom dispatch at some point
//DefineCustomDispatch(llvm::IntrinsicInst) {
//	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
//		CondDispatch(llvm::DbgInfoIntrinsic, theEnv, nativeInstance);
//		Otherwise(llvm::IntrinsicInst, theEnv, nativeInstance);
//	}
//}
DefineCustomDispatch(llvm::CallInst) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::IntrinsicInst, theEnv, nativeInstance);
		Otherwise(llvm::CallInst, theEnv, nativeInstance);
	}
}



#undef CondDispatch
#undef WhenInstanceDoesNotExist
#undef DeclareEngineNode
#undef ElectronClassNameAssociation

}
#endif // _KNOWLEDGE_ENGINE_H_
