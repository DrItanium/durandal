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
void* makeInstance(void* theEnv, const std::string& str);
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
template<typename T, typename Pass = llvm::Pass>
struct InstanceBuilderNode {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, T* data, Pass* pass) { }
};


template<typename T, typename Pass = llvm::Pass>
struct InstancePopulatorNode {
	static void populateInstance(void* env, T* instance, Pass* pass) { }
};
template<typename T, typename Pass = llvm::Pass>
struct InstanceQueryNode {
	static void* getInstanceName(void* theEnv, T* instance, Pass* pass);
};
template<typename T, typename Pass = llvm::Pass>
struct SetParentNode {
	static void setParent(void* theEnv, T* target, Pass* pass);
};
template<typename T, typename Pass = llvm::Pass>
struct ProcessingNode {
	static void* constructInstance(void* env, T* inst, Pass* pass);
};

template<typename T, typename Pass = llvm::Pass>
struct Router {
	static void* dispatch(void* env, T* inst, Pass* pass) {
		WhenInstanceDoesNotExist(env, inst) {
			return ProcessingNode<T,Pass>::constructInstance(env, inst, pass);
		}
	}
};
template<typename T, typename Pass = llvm::Pass>
void* dispatch(void* env, T* inst, Pass* pass) {
	return Router<T,Pass>::dispatch(env, inst, pass);
}
template<typename T, typename Pass = llvm::Pass>
void* dispatch(void* env, T& inst, Pass* pass) {
	return Router<T,Pass>::dispatch(env, &inst, pass);
}

template<typename T, typename Pass = llvm::Pass>
void* dispatch(void* env, T* inst, Pass& pass) {
	return Router<T,Pass>::dispatch(env, inst, &pass);
}
template<typename T, typename Pass = llvm::Pass>
void* dispatch(void* env, T& inst, Pass& pass) {
	return Router<T,Pass>::dispatch(env, &inst, &pass);
}

template<typename T, typename Pass>
void* InstanceQueryNode<T,Pass>::getInstanceName(void* env, T* inst, Pass* p) {
	return EnvAddSymbol(env, EnvGetInstanceName(env, dispatch(env, inst, p)));
}

template<typename T, typename Pass = llvm::Pass>
void* getInstanceName(void* theEnv, T* instance, Pass* pass) {
	return InstanceQueryNode<T,Pass>::getInstanceName(theEnv, instance, pass);
}


template<typename T, typename Pass>
void SetParentNode<T,Pass>::setParent(void* theEnv, T* target, Pass* pass) {
	DATA_OBJECT wrapper;
	void* instanceName = knowledge::getInstanceName(theEnv, target->getParent(), pass);
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, instanceName);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
}

template<typename T, typename Pass = llvm::Pass>
void setParent(void* theEnv, T* inst, Pass* pass) {
	SetParentNode<T,Pass>::setParent(theEnv, inst, pass);
}
template<typename T, typename Pass>
void* ProcessingNode<T,Pass>::constructInstance(void* env, T* inst, Pass* pass) {
	std::string tmp;
	llvm::raw_string_ostream str(tmp);
	str << "( of ";
	ElectronClassNameSelector<T>::selectName(str);
	str << " ";
	InstanceBuilderNode<T, Pass>::buildInstance(str, env, inst, pass);
	str << ")";
	RegisterNativeInstance(env, inst, makeInstance(env, tmp.c_str()));
	InstancePopulatorNode<T,Pass>::populateInstance(env, inst, pass);
	return GetNativeInstance(env, inst);
}

template<typename T, typename Pass = llvm::Pass>
void populateInstance(void* env, T* instance, Pass* pass) {
	InstancePopulatorNode<T, Pass>::populateInstance(env, instance, pass);
}

template<typename T, typename Pass = llvm::Pass>
void buildInstance(llvm::raw_string_ostream& str, void* env, T* instance, Pass* pass) {
	InstanceBuilderNode<T, Pass>::buildInstance(str, env, instance, pass);
}

/*
template<typename T, typename Pass = llvm::Pass>
void* constructInstance(void* env, T* inst, Pass* pass) {
	return ProcessingNode<T, Pass>::constructInstance(env, inst, pass);
}
*/



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
	static void* dispatch(void* env, type * inst, Pass* pass) 
#define BeginCustomDispatch(type, env, inst) \
	template<typename Pass> \
	struct Router<type, Pass> { \
		Route(type, env, inst) { \
			WhenInstanceDoesNotExist(env, inst) {

#define EndCustomDispatch } } };



#define CondDispatch(type, env, val) \
	if (type* v = llvm::dyn_cast<type>(val)) return Router<type, Pass>::dispatch(env, v, pass)
#define Otherwise(type, env, val) \
	return ProcessingNode<Pass, type>::constructInstance(env, val, pass)


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

BeginCustomDispatch(llvm::SequentialType, env, inst);
	CondDispatch(llvm::ArrayType, env, inst);
	CondDispatch(llvm::PointerType, env, inst);
	CondDispatch(llvm::VectorType, env, inst);
	Otherwise(llvm::SequentialType, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::CompositeType, env, inst)
	CondDispatch(llvm::StructType, env, inst);
	CondDispatch(llvm::SequentialType, env, inst);
	Otherwise(llvm::CompositeType, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::Type, env, inst)
	CondDispatch(llvm::FunctionType, env, inst);
	CondDispatch(llvm::IntegerType, env, inst);
	CondDispatch(llvm::CompositeType, env, inst);
	Otherwise(llvm::Type, env, inst);
EndCustomDispatch

// Generic Pass handling
#define NewNode_Partial(type, name) \
template<typename Pass> \
struct name <type, Pass> 

#define NewNode_Full(type, passType, name) \
template<> \
struct name <type, passType>

#define EndNode };

//Build Instance Node
#define BuildInstanceHeader_Full(type, passType, strName, envName, typeName, passName) \
	static void buildInstance(llvm::raw_string_ostream& strName, void* envName, type * typeName, passType* passName) 
#define BuildInstanceHeader_Partial(type, strName, envName, typeName, passName) \
	BuildInstanceHeader_Full(type, Pass, strName, envName, typeName, passName)

#define BeginInstanceBuilderNode_Partial(type, strName, envName, typeName, passName) \
	NewNode_Partial(type, InstanceBuilderNode) { \
		BuildInstanceHeader_Partial(type, strName, envName, typeName, passName)

#define BeginInstanceBuilderNode_Full(type, passType, strName, envName, typeName, passName) \
	NewNode_Full(type, passType, InstanceBuilderNode) { \
		BuildInstanceHeader_Full(type, passType, strName, envName, typeName, passName)


#define EndInstanceBuilderNode EndNode


// Instance Populator Node
#define PopulateInstanceHeader_Full(type, passType, envName, typeName, passName) \
	static void populateInstance(void* envName, type * typeName, passType * passName)

#define PopulateInstanceHeader_Partial(type, envName, typeName, passName) \
	PopulateInstanceHeader_Full(type, Pass, envName, typeName, passName)
#define BeginInstancePopulatorNode_Partial(type, envName, typeName, passName) \
	NewNode_Partial(type, InstancePopulatorNode) { \
		PopulateInstanceHeader_Partial(type, envName, typeName, passName)

#define BeginInstancePopulatorNode_Full(type, passType, envName, typeName, passName) \
	NewNode_Full(type, passType, InstancePopulatorNode) { \
		PopulateInstanceHeader_Full(type, passType, envName, typeName, passName)

#define EndInstancePopulatorNode EndNode
	

BeginInstanceBuilderNode_Partial(llvm::IntegerType, str, env, t, p)  {
	buildInstance(str, env, (llvm::Type*)t, p);
	field(str, "bit-width", t->getBitWidth());
	field(str, "bit-mask", t->getBitMask());
	field(str, "sign-bit", t->getSignBit());
	field(str, "is-power-of-2-byte-width", t->isPowerOf2ByteWidth());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::IntegerType, env, t, p) {
	populateInstance(env, (llvm::Type*)t, p);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::FunctionType, str, env, t, p) {
	buildInstance(str, env, (llvm::Type*)t, p);
	field(str, "is-var-arg", t->isVarArg());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::FunctionType, env, t, p) {
	populateInstance(env, (llvm::Type*)t, p);
	//TODO: add code to iterate over param types
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::CompositeType, str, env, t, p) {
	buildInstance(str, env, (llvm::Type*)t, p);
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::CompositeType, env, t, p) {
	populateInstance(env, (llvm::Type*)t, p);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::StructType, str, env, t, p) {
	buildInstance(str, env, (llvm::CompositeType*)t, p);
	field(str, "packed", t->isPacked());
	field(str, "literal", t->isLiteral());
	field(str, "opaque", t->isOpaque());
	field(str, "has-name", t->hasName());
	field(str, "struct-name", t->getName());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::StructType, env, t, p) {
	populateInstance(env, (llvm::CompositeType*)t, p);
	// TODO: populate elements
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::SequentialType, str, env, t, p) {
	buildInstance(str, env, (llvm::CompositeType*)t, p);
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::SequentialType, env, t, p) {
	populateInstance(env, (llvm::CompositeType*)t, p);
	const std::string elementType("element-type");
	DATA_OBJECT wrapper;
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, knowledge::dispatch(env, t->getElementType(), p));
	EnvDirectPutSlot(env, GetNativeInstance(env, t), elementType.c_str(), &wrapper);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::ArrayType, str, env, t, p) {
	buildInstance(str, env, (llvm::SequentialType*)t, p);
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::ArrayType, env, t, p) {
	populateInstance(env, (llvm::SequentialType*)t, p);
	//TODO: populate sub elements
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::PointerType, str, env, t, p) {
	buildInstance(str, env, (llvm::SequentialType*)t, p);
	field(str, "address-space", t->getAddressSpace());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::PointerType, env, t, p) {
	populateInstance(env, (llvm::SequentialType*)t, p);
	//TODO: populate sub elements
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::VectorType, str, env, t, p) {
	buildInstance(str, env, (llvm::SequentialType*)t, p);
	field(str, "bit-width", t->getBitWidth());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::VectorType, env, t, p) {
	populateInstance(env, (llvm::SequentialType*)t, p);
	//TODO: populate sub elements
}
EndInstancePopulatorNode




template<typename Pass>
struct InstanceBuilderNode<llvm::Type, Pass> {
	static void buildInstance(llvm::raw_string_ostream& str, void* env, llvm::Type* instance, Pass* pass) {
		field(str, "is-void-type", instance->isVoidTy());
		field(str, "is-half-type", instance->isHalfTy());
		field(str, "is-float-type", instance->isFloatTy());
		field(str, "is-double-type", instance->isDoubleTy());
		field(str, "is-x86-fp80-type", instance->isX86_FP80Ty());
		field(str, "is-x86-mmx-type", instance->isX86_MMXTy());
		//field(str, "is-label-type", instance->is
	}
};
template<typename Pass>
struct InstancePopulatorNode<llvm::Value, Pass> {
	static void populateInstance(void* env, llvm::Value* v, Pass* pass) {
		const std::string type("type");
		void* typeInstance = knowledge::dispatch(env, v->getType(), pass);
		DATA_OBJECT wrapper;
		SetType(wrapper, INSTANCE_NAME);
		SetValue(wrapper, knowledge::dispatch(env, v->getType(), pass));
		EnvDirectPutSlot(env, GetNativeInstance(env, v), type.c_str(), &wrapper);
	}
};
template<typename Pass>
struct InstanceBuilderNode<llvm::BasicBlock, Pass> {
	static void buildInstance(llvm::raw_string_ostream& str, void* env, llvm::BasicBlock* instance, Pass* pass) {
		field(str, "is-landing-pad", instance->isLandingPad());
		field(str, "has-address-taken", instance->hasAddressTaken());
	}
};
template<typename Pass>
struct InstancePopulatorNode<llvm::BasicBlock, Pass> {
	static void populateInstance(void* env, llvm::BasicBlock* blk, Pass* pass) {
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
template<typename Pass>
struct InstanceBuilderNode<llvm::Argument, Pass> {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Argument* data, Pass* pass) {
		field(str, "index", data->getArgNo());
	}
};
template<typename Pass>
struct InstancePopulatorNode<llvm::Argument, Pass> {
	static void populateInstance(void* theEnv, llvm::Argument* data, Pass* pass) {
		knowledge::setParent(theEnv, data, pass);
	}
};

template<typename Pass>
struct InstanceBuilderNode<llvm::Module, Pass> {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Module * data, Pass* pass) {
		field(str, "triple", data->getTargetTriple());
		field(str, "data-layout", data->getDataLayoutStr());
		field(str, "module-identifier", data->getModuleIdentifier());
		field(str, "inline-asm", data->getModuleInlineAsm());
	}
};

template<typename Pass>
struct InstanceBuilderNode<llvm::Function, Pass> {
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, llvm::Function* data, Pass* pass) {
		InstanceBuilderNode<llvm::GlobalObject, Pass>::buildInstance(str, theEnv, (llvm::GlobalObject*)data, pass);
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
};
template<typename Pass>
struct InstancePopulatorNode<llvm::Function, Pass> {
	static void populateInstance(void* theEnv, llvm::Function* data, Pass* pass) {
	}
};

// Basic Block Pass
template<>
struct InstancePopulatorNode<llvm::BasicBlock, llvm::BasicBlockPass> {
	static void populateInstance(void* env, llvm::BasicBlock* blk, llvm::BasicBlockPass* pass) {
		//do nothing
	}
};

}
#endif
