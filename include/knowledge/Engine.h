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
extern "C" void RegisterEngineBookkeeping(void* theEnv);
extern "C" void RegisterNativeInstance(void* theEnv, void* native, void* instance);
extern "C" bool ContainsNativeInstance(void* theEnv, void* key);
extern "C" void* GetNativeInstance(void* theEnv, void* key);
#define EngineBookkeepingData(theEnv) \
	((struct knowledge::EngineBookkeeping*) GetEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA))
namespace knowledge {
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
struct ElectronClassName {
	static const char* name = "";
};
template<typename T>
struct ElectronClassNameSelector {
	static void selectName(llvm::raw_string_ostream& str, T* value) {
		str << ElectronClassName<T>::name;
	}
}


template<class T>
void* construct(void* theEnv, T* nativeInstance) {
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(theEnv, nativeInstance); 
	if (potentiallyAlreadyExistingInstance != NULL) { 
		return potentiallyAlreadyExistingInstance; 
	} else {
		llvm::raw_string_ostream& str;

		str << "( of ";
		ElectronClassNameSelector<T>::selectName(str, nativeInstance);
		str << " ";
		buildInstance(str, theEnv, nativeInstance);
		str << ")";
		RegisterInstance(theEnv, nativeInstance, makeInstance(theEnv, str.str().c_str()));
		populateInstance(theEnv, nativeInstance);
		return GetNativeInstance(theEnv, block);
	}
}
#define ElectronClassNameAssociation(type, className) \
	template<> \
	struct ElectronClassName<type> { \
		static const char* name = className; \
	}
#define DeclareEngineNode(type, className) \
	ElectronClassNameAssociation(type, className); \
	void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, type *data); \
	void populateInstance(void* theEnv, type *data)
DeclareEngineNode(llvm::Value, "llvm::value");
template<>
struct ElectronClassNameSelector<llvm::Value> {
	static void selectName(llvm::raw_string_ostream& str, llvm::Value* value) {
#define CondDispatch(cond, type, val) \
		cond (type * v = dyn_cast<type>(val)) ElectronClassNameSelector<type>(val)
		CondDispatch(if, llvm::User, value);
		CondDispatch(else if, llvm::BasicBlock, value);
		CondDispatch(else if, llvm::Argument, value);
		// CondDispatch(else if, llvm::InlineAsm, value);
		// CondDispatch(else if, llvm::MDNode, value);
		// CondDispatch(else if, llvm::MDString, value);
		else {
			str << ElectronClassName<llvm::Value>::name;
		}
#undef CondDispatch
	}
}
DeclareEngineNode(llvm::User, "llvm::user");
template<>
struct ElectronClassNameSelector<llvm::User> {
	static void selectName(llvm::raw_string_ostream& str, llvm::User* value) {
#define CondDispatch(cond, type, val) \
		cond (type * v = dyn_cast<type>(val)) ElectronClassNameSelector<type>(val)
		CondDispatch(if, llvm::Instruction, value);
		CondDispatch(else if, llvm::Constant, value);
		CondDispatch(else if, llvm::Operator, value);
		else {
			str << ElectronClassName<llvm::User>::name;
		}
#undef CondDispatch
	}
}
DeclareEngineNode(llvm::Module, "llvm::module");
DeclareEngineNode(llvm::BasicBlock, "llvm::basic-block");
DeclareEngineNode(llvm::Argument, "llvm::argument");
DeclareEngineNode(llvm::Function, "llvm::function");
DeclareEngineNode(llvm::Loop, "llvm::loop");
DeclareEngineNode(llvm::Region, "llvm::region");
DeclareEngineNode(llvm::Constant, "llvm::constant");
DeclareEngineNode(llvm::Operator, "llvm::operator");
DeclareEngineNode(llvm::Type, "llvm::type");
DeclareEngineNode(llvm::GlobalVariable, "llvm::global-variable");
DeclareEngineNode(llvm::GlobalAlias, "llvm::global-alias");
DeclareEngineNode(llvm::GlobalValue, "llvm::global-value");
DeclareEngineNode(llvm::Instruction, "llvm::instruction");
DeclareEngineNode(llvm::PHINode, "llvm::phi-node");
DeclareEngineNode(llvm::TerminatorInstruction, "llvm::terminator-instruction");
DeclareEngineNode(llvm::StoreInst, "llvm::store-instruction");
DeclareEngineNode(llvm::BinaryOperator, "llvm::binary-operator");
DeclareEngineNode(llvm::UnaryInstruction, "llvm::unary-instruction");
DeclareEngineNode(llvm::LoadInst, "llvm::load-instruction");
DeclareEngineNode(llvm::AllocaInst, "llvm::alloca-instruction");
DeclareEngineNode(llvm::BranchInst, "llvm::branch-instruction");
DeclareEngineNode(llvm::IndirectBrInst, "llvm::indirect-branch-instruction");
DeclareEngineNode(llvm::ReturnInst, "llvm::return-instruction");
DeclareEngineNode(llvm::SwitchInst, "llvm::switch-instruction");
}

#undef DeclareEngineNode
#undef ElectronClassNameAssociation

#endif // _KNOWLEDGE_ENGINE_H_
