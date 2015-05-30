#ifndef _ENGINE_H_
#define _ENGINE_H_
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
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/SmallVector.h"
extern "C" {
#include "clips.h"
}
#define ENGINE_BOOKKEEPING_DATA USER_ENVIRONMENT_DATA + 0
extern "C" void RegisterEngineBookkeeping(void* theEnv);
extern "C" void RegisterNativeInstance(void* theEnv, void* native, void* instance);
extern "C" bool ContainsNativeInstance(void* theEnv, void* key);
extern "C" void* GetNativeInstance(void* theEnv, void* key);
extern "C" void RegisterExternalAddressId(void* theEnv, int type, struct externalAddressType* ea);
extern "C" bool ContainsExternAddressId(void* theEnv, int type);
extern "C" int GetExternalAddressId(void* theEnv, int type);
#define EngineBookkeepingData(theEnv) \
	((struct knowledge::EngineBookkeeping*) GetEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA))
namespace knowledge {
void* makeInstance(void* theEnv, const std::string& str);
inline void booleanField(llvm::raw_string_ostream& str, const std::string& name, bool value);
void directPutMultifield(void* theEnv, void* nativeInstance, const std::string& slotName, void* multifieldData, int multifieldBegin, int multifieldEnd);
template<typename T>
struct ExternalAddressRegistration {
	static int indirectId;
};

template<typename T>
struct ElectronClassNameSelector {
	static void selectName(llvm::raw_string_ostream& str) {
		str << "";
	}
};

#define ElectronClassNameAssociation(type, className) \
	template<> \
struct ElectronClassNameSelector<type> { \
	static void selectName(llvm::raw_string_ostream& str) { \
		str << className ; \
	} \
}
#define ExtAddrType(name) RegisterExternalAddressId_ ## name
enum 
{
	// according to the c++ spec these values will start from 0
#define X(a, b, c) \
	ExtAddrType(a),
#include "knowledge/EngineNodes.def"
#undef X
	RegisteredExternalAddressTypes,
};
// SO FUCKING BEAUTIFUL :D
#define WhenInstanceDoesNotExist(env, instance) \
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, instance); \
if (potentiallyAlreadyExistingInstance != NULL) { \
	return potentiallyAlreadyExistingInstance; \
} else 

class EngineBookkeeping {
	public:
		EngineBookkeeping();
		~EngineBookkeeping();
		void registerInstance(void* key, std::string& value);
		bool containsInstance(void* key);
		std::string getRelatedInstance(void* key);
		void registerExternalAddress(int type, int id);
		bool containsExternalAddress(int type);
		int getRelatedExternalAddress(int type);
	private:
		// just the pointers ma'am
		llvm::DenseMap<void*, std::string> instanceMap;
		// make it a statically sized array as registration is done at compile
		// time :D
		int externalAddrs[RegisteredExternalAddressTypes];
};
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
template<typename P, typename T>
struct ProcessingNode {
	static void* constructInstance(void* env, T* inst, P* pass) {
		std::string tmp;
		llvm::raw_string_ostream str(tmp);
		str << "( of ";
		ElectronClassNameSelector<T>::selectName(str);
		str << " ";
		ProcessingNode<P,T>::buildInstance(str, env, inst);
		str << ")";
		RegisterNativeInstance(env, inst, makeInstance(env, tmp.c_str()));
		ProcessingNode<P,T>::populateInstance(env, inst);
		return GetNativeInstance(env, inst);
	}
	static void populateInstance(void* env, T* instance, P* pass) {

	}
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, T* data, P* pass) {

	}
	static void* getInstanceName(void* theEnv, T* instance, P* pass);
	static void setParent(void* theEnv, T* target, P* pass);
};

template<typename P, typename T>
struct Router {
	static void* dispatch(void* env, T* inst, P* pass) {
		WhenInstanceDoesNotExist(env, inst) {
			return ProcessingNode<P,T>::constructInstance(env, inst, pass);
		}
	}
};

template<typename P, typename T>
void* dispatch(void* env, T* inst, P* pass) {
	return Router<P,T>::dispatch(env, inst, pass);
}
template<typename P, typename T>
void* dispatch(void* env, T& inst, P* pass) {
	return Router<P,T>::dispatch(env, &inst, pass);
}

template<typename P, typename T>
void* ProcessingNode<P,T>::getInstanceName(void* env, T* inst, P* p) {
	return EnvAddSymbol(env, EnvGetInstanceName(env, dispatch(env, inst, p)));
}

template<typename P, typename T>
void* getInstanceName(void* theEnv, T* instance, P* pass) {
	return ProcessingNode<P,T>::getInstanceName(theEnv, instance, pass);
}

template<typename P, typename T>
void setParent(void* theEnv, T* inst, P* pass) {
	ProcessingNode<P,T>::setParent(theEnv, inst, pass);
}

template<typename P, typename T>
void ProcessingNode<P,T>::setParent(void* theEnv, T* target, P* pass) {
	DATA_OBJECT wrapper;
	void* instanceName = knowledge::getInstanceName(theEnv, target->getParent(), pass);
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, instanceName);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
}

template<typename T>
void field(llvm::raw_string_ostream& str, const std::string& name, T value) {
	str << " (" << name << " " << value << ") ";
}
void field(llvm::raw_string_ostream& str, const std::string& name, const std::string& value) {
	str << " (" << name << " " << value << ") ";
}
void field(llvm::raw_string_ostream& str, const std::string& name, bool value) {
	if (value) {
		field(str, name, "TRUE");
	}
}
template<typename P>
struct ProcessingNode<P, llvm::BasicBlock> {
	static void buildInstance(llvm::raw_string_ostream& str, void* env, llvm::BasicBlock* instance, P* pass) {
		DATA_OBJECT tmp;
		field(str, "is-landing-pad", instance->isLandingPad());
		field(str, "has-address-taken", instance->hasAddressTaken());
		EnvEval(env, "(printout t partial-specialization crlf)", &tmp);
	}
	static void populateInstance(void* env, llvm::BasicBlock* blk, P* pass) {
		int index = 0;
		knowledge::setParent(env, blk, pass);
		if (blk->size() > 0) {
			void* mf = EnvCreateMultifield(env, blk->size());
			index = 1;
			for(llvm::BasicBlock::iterator it = blk->begin();
					it != blk->end();
					++it, ++index) {
				llvm::Instruction& inst = *it;
				SetMFType(mf, index, INSTANCE_NAME);
				SetMFValue(mf, index, getInstanceName(env, &inst, pass));
			}
			directPutMultifield(env, blk, "contents", mf, 1, index - 1);
		}
		llvm::SmallVector<llvm::BasicBlock*, 8> preds(pred_begin(blk), pred_end(blk));
		void *predmf = EnvCreateMultifield(env, preds.size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = preds.begin();
				it != preds.end(); ++it, ++index) {
			SetMFType(predmf, index, INSTANCE_NAME);
			SetMFValue(predmf, index, getInstanceName(env, *it, pass));
		}
		directPutMultifield(env, blk, "predecessors", predmf, 1, preds.size());

		llvm::SmallVector<llvm::BasicBlock*, 8> succs(succ_begin(blk), succ_end(blk));
		void *succmf = EnvCreateMultifield(env, succs.size());
		void *prodmf = EnvCreateMultifield(env, succs.size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = succs.begin(); 
				it != succs.end(); ++it, ++index) {
			void* result = knowledge::getInstanceName(env, *it, pass);
			SetMFType(prodmf, index, INSTANCE_NAME);
			SetMFValue(prodmf, index, result);
			SetMFType(succmf, index, INSTANCE_NAME);
			SetMFValue(succmf, index, result);
		}
		directPutMultifield(env, blk, "successors", succmf, 1, succs.size());
		directPutMultifield(env, blk, "produces", prodmf, 1, succs.size());
	}
};
template<>
struct ProcessingNode<llvm::BasicBlockPass,llvm::BasicBlock> {
	static void populateInstance(void* env, llvm::BasicBlock* blk, llvm::BasicBlockPass& pass) {
		//do nothing
	}
};

template<typename P>
struct ProcessingNode<P, llvm::Argument> {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Argument* data, P* pass) {
		field(str, "index", data->getArgNo());
	}
	static void populateInstance(void* theEnv, llvm::Argument* data, P* pass) {
		knowledge::setParent(theEnv, data, pass);
	}
};

template<typename P>
struct ProcessingNode<P, llvm::Module> {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Module * data, P* pass) {
		field(str, "triple", data->getTargetTriple());
		field(str, "data-layout", data->getDataLayoutStr());
		field(str, "module-identifier", data->getModuleIdentifier());
		field(str, "inline-asm", data->getModuleInlineAsm());
	}
	static void populateInstance(void* theEnv, llvm::Module* data, P* pass) { 

	}
};

template<typename P>
struct ProcessingNode<P, llvm::Function> {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Function* data, P* pass) {
		ProcessingNode<P, llvm::GlobalObject>::buildInstance(str, theEnv, (llvm::GlobalObject*)data, pass);
		field(str, "is-var-arg", data->isVarArg());
		field(str, "is-materializable", data->isMaterializable());
		field(str, "is-intrinsic", data->isIntrinsic());
		field(str, "has-gc", data->hasGC());
		field(str, "calls-function-that-returns-twice", data->callsFunctionThatReturnsTwice());
		field(str, "is-def-trivally-dead", data->isDefTriviallyDead());
		field(str, "has-prologue-data", data->hasPrologueData());
		field(str, "does-not-throw", data->doesNotThrow());
		field(str, "does-not-return", data->doesNotReturn());
		field(str, "does-not-access-memory", data->doesNotAccessMemory());
		field(str, "cannot-duplicate", data->cannotDuplicate());
		field(str, "has-uw-table", data->hasUWTable());
		field(str, "needs-unwind-table-entry", data->needsUnwindTableEntry());
		field(str, "gc", data->getGC());
	}
	static void populateInstance(void* theEnv, llvm::Function* data, P* pass) {
		ProcessingNode<P, llvm::GlobalObject>::populateInstance(theEnv, (llvm::GlobalObject*)data, pass);
	}
};


#define X(unused, type, className) \
	ElectronClassNameAssociation(type, className); \
	template<> \
	struct ExternalAddressRegistration<type> { \
		static int indirectId; \
	}; 
#include "knowledge/EngineNodes.def"
#undef X
// SO FUCKING BEAUTIFUL :D
#define Route(type, env, inst) \
	static void* dispatch(void* env, type * inst, P* pass) 
#define BeginCustomDispatch(type, env, inst) \
	template<typename P> \
	struct Router<P, type> { \
		Route(type, env, inst) { \
			WhenInstanceDoesNotExist(env, inst) {

#define EndCustomDispatch } } };



#define CondDispatch(type, env, val) \
	if (type* v = llvm::dyn_cast<type>(val)) return Router<P, type>::dispatch(env, v, pass)
#define Otherwise(type, env, val) \
	return ProcessingNode<P, type>::constructInstance(env, val, pass)


BeginCustomDispatch(llvm::TerminatorInst, env, inst)
	CondDispatch(llvm::BranchInst, env, inst);
	CondDispatch(llvm::IndirectBrInst, env, inst);
	CondDispatch(llvm::InvokeInst, env, inst);
	CondDispatch(llvm::ResumeInst, env, inst);
	CondDispatch(llvm::ReturnInst, env, inst);
	CondDispatch(llvm::SwitchInst, env, inst);
	CondDispatch(llvm::UnreachableInst, env, inst);
	Otherwise(llvm::TerminatorInst, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::UnaryInstruction, env, inst) 
	CondDispatch(llvm::AllocaInst, env, inst);
	CondDispatch(llvm::CastInst, env, inst);
	CondDispatch(llvm::ExtractValueInst, env, inst);
	CondDispatch(llvm::LoadInst, env, inst);
	CondDispatch(llvm::VAArgInst, env, inst);
	Otherwise(llvm::UnaryInstruction, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::CmpInst, env, inst)
	CondDispatch(llvm::FCmpInst, env, inst);
	CondDispatch(llvm::ICmpInst, env, inst);
	Otherwise(llvm::CmpInst, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::Instruction, env, inst)
	CondDispatch(llvm::PHINode, env, inst);
	CondDispatch(llvm::StoreInst, env, inst);
	CondDispatch(llvm::BinaryOperator, env, inst);
	CondDispatch(llvm::CallInst, env, inst);
	CondDispatch(llvm::CmpInst, env, inst);
	CondDispatch(llvm::ExtractElementInst, env, inst);
	CondDispatch(llvm::FenceInst, env, inst);
	CondDispatch(llvm::GetElementPtrInst, env, inst);
	CondDispatch(llvm::InsertElementInst, env, inst);
	CondDispatch(llvm::InsertValueInst, env, inst);
	CondDispatch(llvm::LandingPadInst, env, inst);
	CondDispatch(llvm::SelectInst, env, inst);
	CondDispatch(llvm::ShuffleVectorInst, env, inst);
	CondDispatch(llvm::TerminatorInst, env, inst);
	CondDispatch(llvm::UnaryInstruction, env, inst);
	Otherwise(llvm::Instruction, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::User, env, inst) 
	CondDispatch(llvm::Instruction, env, inst);
	CondDispatch(llvm::Constant, env, inst);
	CondDispatch(llvm::Operator, env, inst);
	Otherwise(llvm::User, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::Value, env, inst)
	CondDispatch(llvm::User, env, inst);
	CondDispatch(llvm::BasicBlock, env, inst);
	CondDispatch(llvm::Argument, env, inst);
	// CondDispatch(llvm::InlineAsm, env, value);
	// CondDispatch(llvm::MDNode, env, value);
	// CondDispatch(llvm::MDString, env, value);
	Otherwise(llvm::Value, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::CastInst, env, inst)
	CondDispatch(llvm::AddrSpaceCastInst, env, inst);
	CondDispatch(llvm::BitCastInst, env, inst);
	CondDispatch(llvm::FPExtInst, env, inst);
	CondDispatch(llvm::FPToSIInst, env, inst);
	Otherwise(llvm::CastInst, env, inst);
EndCustomDispatch

//TODO: Finish defining this custom dispatch at some point
//BeginCustomDispatch(llvm::IntrinsicInst, env, inst)
//	CondDispatch(llvm::DbgInfoIntrinsic, env, inst);
//	Otherwise(llvm::IntrinsicInst, env, inst);
//EndCustomDispatch

BeginCustomDispatch( llvm::CallInst, env, inst)
	CondDispatch(llvm::IntrinsicInst, env, inst);
	Otherwise(llvm::CallInst, env, inst);
EndCustomDispatch

}
#endif
