#ifndef _KNOWLEDGE_ENGINE_H_
#define _KNOWLEDGE_ENGINE_H_
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
extern "C" {
#include "clips.h"
}
#define ENGINE_BOOKKEEPING_DATA USER_ENVIRONMENT_DATA + 0
extern "C" void RegisterEngineBookkeeping(void* theEnv);
extern "C" void RegisterNativeInstance(void* theEnv, void* native, void* instance);
extern "C" bool ContainsNativeInstance(void* theEnv, void* key);
extern "C" void* GetNativeInstance(void* theEnv, void* key);
#define EngineBookkeepingData(theEnv) \
	((struct knowledge::EngineBookkeeping*) GetEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA))
namespace knowledge {
void* makeInstance(void* theEnv, llvm::raw_string_ostream& str);
class EngineBookkeeping {
	public:
		EngineBookkeeping();
		~EngineBookkeeping();
		void registerInstance(void* key, std::string& value);
		bool containsInstance(void* key);
		std::string& getRelatedInstance(void* key);
	private:
		// just the pointers ma'am
		llvm::DenseMap<void*, std::string> instanceMap;
};
template<typename T>
struct ElectronClassNameSelector {
	static void selectName(llvm::raw_string_ostream& str, T* value) {
		str << "";
	}
};
#define ElectronClassNameAssociation(type, className) \
	template<> \
	struct ElectronClassNameSelector<type> { \
		static void selectName(llvm::raw_string_ostream& str, type * value) { \
			str << className ; \
		} \
	}
#define DeclareEngineNode(type, className) \
	ElectronClassNameAssociation(type, className); \
	void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, type *data); \
	void populateInstance(void* theEnv, type *data)
DeclareEngineNode(llvm::Module, "llvm::module");
DeclareEngineNode(llvm::BasicBlock, "llvm::basic-block");
DeclareEngineNode(llvm::Argument, "llvm::argument");
DeclareEngineNode(llvm::Function, "llvm::function");
DeclareEngineNode(llvm::Loop, "llvm::loop");
DeclareEngineNode(llvm::Region, "llvm::region");
DeclareEngineNode(llvm::Constant, "llvm::constant");
DeclareEngineNode(llvm::Operator, "llvm::operator");
DeclareEngineNode(llvm::GlobalVariable, "llvm::global-variable");
DeclareEngineNode(llvm::GlobalAlias, "llvm::global-alias");
DeclareEngineNode(llvm::GlobalValue, "llvm::global-value");
DeclareEngineNode(llvm::AddrSpaceCastInst, "llvm::address-space-cast-instruction");
DeclareEngineNode(llvm::BitCastInst, "llvm::bit-cast-instruction");
DeclareEngineNode(llvm::FPExtInst, "llvm::fp-ext-instruction");
DeclareEngineNode(llvm::FPToSIInst, "llvm::fp-to-si-instruction");
DeclareEngineNode(llvm::AllocaInst, "llvm::alloca-instruction");
DeclareEngineNode(llvm::FCmpInst, "llvm::floating-point-compare-instruction");
DeclareEngineNode(llvm::ICmpInst, "llvm::integer-compare-instruction");
DeclareEngineNode(llvm::PHINode, "llvm::phi-node");
DeclareEngineNode(llvm::StoreInst, "llvm::store-instruction");
DeclareEngineNode(llvm::BinaryOperator, "llvm::binary-operator");
DeclareEngineNode(llvm::LoadInst, "llvm::load-instruction");
DeclareEngineNode(llvm::BranchInst, "llvm::branch-instruction");
DeclareEngineNode(llvm::IndirectBrInst, "llvm::indirect-branch-instruction");
DeclareEngineNode(llvm::ReturnInst, "llvm::return-instruction");
DeclareEngineNode(llvm::SwitchInst, "llvm::switch-instruction");
DeclareEngineNode(llvm::UnreachableInst, "llvm::unreachable-instruction");
DeclareEngineNode(llvm::VAArgInst, "llvm::vaarg-instruction");
DeclareEngineNode(llvm::Type, "llvm::type");
DeclareEngineNode(llvm::Instruction, "llvm::instruction");
DeclareEngineNode(llvm::User, "llvm::user");
DeclareEngineNode(llvm::Value, "llvm::value");
DeclareEngineNode(llvm::TerminatorInst, "llvm::terminator-instruction");
DeclareEngineNode(llvm::UnaryInstruction, "llvm::unary-instruction");
DeclareEngineNode(llvm::CastInst, "llvm::cast-instruction");
DeclareEngineNode(llvm::CallInst, "llvm::call-instruction");
DeclareEngineNode(llvm::IntrinsicInst, "llvm::intrinsic-instruction");
DeclareEngineNode(llvm::CmpInst, "llvm::compare-instruction");
// SO FUCKING BEAUTIFUL :D
#define WhenInstanceDoesNotExist(env, instance) \
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, instance); \
	if (potentiallyAlreadyExistingInstance != NULL) { \
		return potentiallyAlreadyExistingInstance; \
	} else 
#define DefineCustomDispatch(type) \
	template<> \
	void* dispatch< type >(void* theEnv, type * nativeInstance) 
#define CondDispatch(type, env, val) \
		if (type * v = llvm::dyn_cast<type>((val))) return dispatch<type>((env), v)
#define Otherwise(type, env, val) \
		return construct<type>(env, val)
template<class T>
void* dispatch(void* theEnv, T* nativeInstance) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		return construct(theEnv, nativeInstance);
	}
}
template<class T>
void* construct(void* theEnv, T* nativeInstance) {
	std::string tmp;
	llvm::raw_string_ostream str(tmp);
	str << "( of ";
	ElectronClassNameSelector<T>::selectName(str, nativeInstance);
	str << " ";
	buildInstance(str, theEnv, nativeInstance);
	str << ")";
	RegisterInstance(theEnv, nativeInstance, makeInstance(theEnv, tmp.c_str()));
	populateInstance(theEnv, nativeInstance);
	return GetNativeInstance(theEnv, nativeInstance);
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
	WhenInstanceDoesNotExist(llvm::UnaryInstruction) {
		CondDispatch(llvm::AllocaInst, theEnv, nativeInstance);
		CondDispatch(llvm::CastInst, theEnv, nativeInstance);
		CondDispatch(llvm::ExtractValueInst, theEnv, nativeInstance);
		CondDispatch(llvm::LoadInst, theEnv, nativeInstance);
		CondDispatch(llvm::VAArgInst, theEnv, nativeInstance);
		Otherwise(llvm::UnaryInstruction, theEnv, nativeInstance);
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
DefineCustomDispatch(llvm::CallInst) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::IntrinsicInst, theEnv, nativeInstance);
		Otherwise(llvm::CallInst, theEnv, nativeInstance);
	}
}
//TODO: Finish defining this custom dispatch at some point
//DefineCustomDispatch(llvm::IntrinsicInst) {
//	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
//		CondDispatch(llvm::DbgInfoIntrinsic, theEnv, nativeInstance);
//		Otherwise(llvm::IntrinsicInst, theEnv, nativeInstance);
//	}
//}
DefineCustomDispatch(llvm::CmpInst) {
	WhenInstanceDoesNotExist(theEnv, nativeInstance) {
		CondDispatch(llvm::FCmpInst, theEnv, nativeInstance);
		CondDispatch(llvm::ICmpInst, theEnv, nativeInstance);
		Otherwise(llvm::CmpInst, theEnv, nativeInstance);
	}
}
}

#undef CondDispatch
#undef WhenInstanceDoesNotExist
#undef DeclareEngineNode
#undef ElectronClassNameAssociation

#endif // _KNOWLEDGE_ENGINE_H_
