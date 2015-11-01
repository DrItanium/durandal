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
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/LoopPass.h"
extern "C" {
#include "clips.h"
}

// Extension functions to extract the type of the external address
#define EnvDOPToExternalAddressType(theEnv, target) (((struct externalAddressHashNode *) ((target)->value))->type)
#define EnvDOToExternalAddressType(theEnv, target) (((struct externalAddressHashNode *) ((target).value))->type)

#define ENGINE_BOOKKEEPING_DATA (USER_ENVIRONMENT_DATA + 0)
extern "C" void RegisterNativeInstance(void* theEnv, void* native, void* instance);
extern "C" bool ContainsNativeInstance(void* theEnv, void* key);
extern "C" void* GetNativeInstance(void* theEnv, void* key);
extern "C" void RegisterExternalAddressId(void* theEnv, int type, struct externalAddressType* ea);
extern "C" bool ContainsExternAddressId(void* theEnv, int type);
extern "C" int GetExternalAddressId(void* theEnv, int type);
#define EngineBookkeepingData(theEnv) \
	((knowledge::EngineBookkeeping*) GetEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA))
namespace knowledge {
#define ExtAddrType(name) RegisterExternalAddressId_ ## name
enum 
{
	// according to the c++ spec these values will start from 0
#define X(a, b, c, unused) \
	ExtAddrType(a),
#include "knowledge/EngineNodes.def"
#undef X
	RegisteredExternalAddressTypes,
};
class EngineBookkeeping {
	public:
		EngineBookkeeping() { }
		~EngineBookkeeping() { }
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

void* makeInstance(void* theEnv, const std::string& str) {
	void* result = EnvMakeInstance(theEnv, str.c_str());
	if (result == NULL) {
		std::string tmp;
		llvm::raw_string_ostream msg(tmp);
		msg << "Couldn't create instance from " << str;
		llvm::report_fatal_error(msg.str());
	} 
	return result;
}
void directPutMultifield(void* theEnv, void* nativeInstance,
		const std::string& slotName, void* multifieldData, int multifieldBegin,
		int multifieldEnd) {
	DATA_OBJECT wrapper;
	SetType(wrapper, MULTIFIELD);
	SetValue(wrapper, multifieldData);
	SetDOBegin(wrapper, multifieldBegin);
	SetDOEnd(wrapper, multifieldEnd);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, nativeInstance),
			slotName.c_str(), &wrapper);
}
void directPutInstanceName(void* env, void* addr, const std::string& slotName, void* iname) {
	DATA_OBJECT wrapper;
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, iname);
	EnvDirectPutSlot(env, GetNativeInstance(env, addr), slotName.c_str(), &wrapper);
}
template<typename T>
void field(llvm::raw_string_ostream& str, const std::string& name, T value) {
	str << " (" << name << " " << value << ") ";
}
void field(llvm::raw_string_ostream& str, const std::string& name, const std::string& value) {
	str << " (" << name << " " << value << ") ";
}
void field(llvm::raw_string_ostream& str, const std::string& name, uint64_t value) {
	if (value > 0) {
		str << " (" << name << " " << value << ") ";
	}
}
void field(llvm::raw_string_ostream& str, const std::string& name, unsigned value) {
	if (value > 0) {
		str << " (" << name << " " << value << ") ";
	}

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
// SO FUCKING BEAUTIFUL :D
#define WhenInstanceDoesNotExist(env, instance) \
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, instance); \
if (potentiallyAlreadyExistingInstance != NULL) { \
	return potentiallyAlreadyExistingInstance; \
} else 
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
	static void* queryInstanceName(void* theEnv, T* instance, Pass* pass);
};
template<typename T, typename Pass = llvm::Pass>
struct InstanceParentImbueNode {
	static void imbueParent(void* theEnv, T* target, Pass* pass);
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
void* InstanceQueryNode<T,Pass>::queryInstanceName(void* env, T* inst, Pass* p) {
	return EnvAddSymbol(env, EnvGetInstanceName(env, dispatch(env, inst, p)));
}

template<typename T, typename Pass = llvm::Pass>
void* getInstanceName(void* theEnv, T* instance, Pass* pass) {
	return InstanceQueryNode<T,Pass>::queryInstanceName(theEnv, instance, pass);
}


template<typename T, typename Pass>
void InstanceParentImbueNode<T,Pass>::imbueParent(void* theEnv, T* target, Pass* pass) {
	DATA_OBJECT wrapper;
	void* instanceName = getInstanceName(theEnv, target->getParent(), pass);
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, instanceName);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
}
template<typename T>
void installNativeInstance(void* env, T* target);

template<typename T, typename Pass = llvm::Pass>
void setParent(void* theEnv, T* inst, Pass* pass) {
	InstanceParentImbueNode<T,Pass>::imbueParent(theEnv, inst, pass);
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
	RegisterNativeInstance(env, inst, makeInstance(env, str.str().c_str()));
	InstancePopulatorNode<T,Pass>::populateInstance(env, inst, pass);
	// install the native instance
	installNativeInstance(env, inst);
	return GetNativeInstance(env, inst);
}

template<typename T, typename Pass = llvm::Pass>
void populate(void* env, T* instance, Pass* pass) {
	InstancePopulatorNode<T, Pass>::populateInstance(env, instance, pass);
}

template<typename T, typename Pass = llvm::Pass>
void build(llvm::raw_string_ostream& str, void* env, T* instance, Pass* pass) {
	InstanceBuilderNode<T, Pass>::buildInstance(str, env, instance, pass);
}

/*
template<typename T, typename Pass = llvm::Pass>
void* constructInstance(void* env, T* inst, Pass* pass) {
	return ProcessingNode<T, Pass>::constructInstance(env, inst, pass);
}
*/



#define X(unused, type, className, unused2) \
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
	return ProcessingNode<type, Pass>::constructInstance(env, val, pass)

// Define the routing code or where to dispatch things of different types to.
// This is done using macros and templates because writing this by hand is a
// major pain in the ass
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
	build(str, env, (llvm::Type*)t, p);
	field(str, "bit-width", t->getBitWidth());
	field(str, "bit-mask", t->getBitMask());
	field(str, "sign-bit", t->getSignBit());
	field(str, "is-power-of-2-byte-width", t->isPowerOf2ByteWidth());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::IntegerType, env, t, p) {
	populate(env, (llvm::Type*)t, p);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::FunctionType, str, env, t, p) {
	build(str, env, (llvm::Type*)t, p);
	field(str, "is-var-arg", t->isVarArg());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::FunctionType, env, t, p) {
	populate(env, (llvm::Type*)t, p);
	directPutInstanceName(env, t, "return-type", 
			getInstanceName(env, t->getReturnType(), p));
	void* argsmf = EnvCreateMultifield(env, t->getNumParams());
	int argIndex = 1;
	for (llvm::FunctionType::param_iterator it = t->param_begin();
			it != t->param_end(); ++it, ++argIndex) {
		SetMFType(argsmf, argIndex, INSTANCE_NAME);
		SetMFValue(argsmf, argIndex, getInstanceName(env, *it, p));
	}
	directPutMultifield(env, t, "params", argsmf, 1, argIndex - 1);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::CompositeType, str, env, t, p) {
	build(str, env, (llvm::Type*)t, p);
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::CompositeType, env, t, p) {
	populate(env, (llvm::Type*)t, p);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::StructType, str, env, t, p) {
	build(str, env, (llvm::CompositeType*)t, p);
	field(str, "packed", t->isPacked());
	field(str, "literal", t->isLiteral());
	field(str, "opaque", t->isOpaque());
	field(str, "has-name", t->hasName());
	if (t->hasName()) {
		field(str, "struct-name", t->getName());
	}
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::StructType, env, t, p) {
	populate(env, (llvm::CompositeType*)t, p);
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::SequentialType, str, env, t, p) {
	build(str, env, (llvm::CompositeType*)t, p);
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::SequentialType, env, t, p) {
	populate(env, (llvm::CompositeType*)t, p);
	directPutInstanceName(env, t, "element-type", 
			getInstanceName(env, t->getElementType(), p));
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::ArrayType, str, env, t, p) {
	build(str, env, (llvm::SequentialType*)t, p);
	field(str, "num-elements", t->getNumElements());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::ArrayType, env, t, p) {
	populate(env, (llvm::SequentialType*)t, p);
	//TODO: populate sub elements
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::PointerType, str, env, t, p) {
	build(str, env, (llvm::SequentialType*)t, p);
	field(str, "address-space", t->getAddressSpace());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::PointerType, env, t, p) {
	populate(env, (llvm::SequentialType*)t, p);
	//TODO: populate sub elements
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::VectorType, str, env, t, p) {
	build(str, env, (llvm::SequentialType*)t, p);
	field(str, "bit-width", t->getBitWidth());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::VectorType, env, t, p) {
	populate(env, (llvm::SequentialType*)t, p);
	//TODO: populate sub elements
}
EndInstancePopulatorNode




BeginInstanceBuilderNode_Partial(llvm::Type, str, env, t, p) {
	field(str, "is-void-type", t->isVoidTy());
	field(str, "is-half-type", t->isHalfTy());
	field(str, "is-float-type", t->isFloatTy());
	field(str, "is-double-type", t->isDoubleTy());
	field(str, "is-x86-fp80-type", t->isX86_FP80Ty());
	field(str, "is-fp128-type", t->isFP128Ty());
	field(str, "is-ppc-fp128-type", t->isPPC_FP128Ty());
	field(str, "is-floating-point", t->isFloatingPointTy());
	field(str, "is-x86-mmx-type", t->isX86_MMXTy());
	field(str, "is-fp-or-fp-vector-type", t->isFPOrFPVectorTy());
	field(str, "is-label-type", t->isLabelTy());
	field(str, "is-metadata-type", t->isMetadataTy());
	field(str, "is-int-or-int-vector-type", t->isIntOrIntVectorTy());
	field(str, "is-ptr-or-ptr-vector-type", t->isPtrOrPtrVectorTy());
	field(str, "is-empty-type", t->isEmptyTy());
	field(str, "is-first-class-type", t->isFirstClassType());
	field(str, "is-single-value-type", t->isSingleValueType());
	field(str, "is-aggregate-type", t->isAggregateType());
	field(str, "primitive-bit-size", t->getPrimitiveSizeInBits());
	field(str, "scalar-bit-size", t->getScalarSizeInBits());
	field(str, "fp-mantissa-width", t->getFPMantissaWidth());
	// omit the entries like is-integer-type since CLIPS' type system has
	// awesome reflective capabilities
	//
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::Type, env, t, p) {
	// first populate the scalar type, this can be a wierd case as it may
	// return this class! So it is something to be aware of in CLIPS itself
	directPutInstanceName(env, t, "scalar-type", 
			getInstanceName(env, t->getScalarType(), p));
	if (t->getNumContainedTypes() > 0) {
		void* elementmf = EnvCreateMultifield(env, t->getNumContainedTypes());
		int elementIndex = 1;
		for (llvm::StructType::element_iterator it = t->subtype_begin();
				it != t->subtype_end(); ++it, ++elementIndex) {
			SetMFType(elementmf, elementIndex, INSTANCE_NAME);
			SetMFValue(elementmf, elementIndex, getInstanceName(env, *it, p));
		}
		directPutMultifield(env, t, "subtypes", elementmf, 1, elementIndex - 1);
	}
}
EndInstancePopulatorNode
BeginInstanceBuilderNode_Partial(llvm::Value, str, env, v, p) {
	field(str, "value-id", v->getValueID());
	field(str, "has-value-handle", v->hasValueHandle());
	field(str, "is-used-by-metadata", v->isUsedByMetadata());
}
EndInstanceBuilderNode

BeginInstancePopulatorNode_Partial(llvm::Value, env, v, p) {
	directPutInstanceName(env, v, "native-type", 
			getInstanceName(env, v->getType(), p));
	//TODO: add support for converting user and perhaps use iterators to
	// knowledge
}
EndInstancePopulatorNode

BeginInstanceBuilderNode_Partial(llvm::Argument, str, env, t, p) {
	build(str, env, (llvm::Value*)t, p);
	field(str, "index", t->getArgNo());
	//TODO: migrate these attributes to a list to make code maintentance easier
	field(str, "non-null-attr", t->hasNonNullAttr());
	field(str, "has-by-val-attr", t->hasByValAttr());
	field(str, "has-by-val-or-in-alloca-attr", t->hasByValOrInAllocaAttr());
	if (t->getType()->isPointerTy()) {
		field(str, "param-alignment", t->getParent());
		field(str, "num-dereferenceable-bytes", t->getDereferenceableBytes());
		// this is referenced in the mainline docs but not in 3.6
		//field(str, "num-dereferenceable-or-null-bytes", t->getDereferenceableOrNullBytes());
	} 
	field(str, "has-nest-attr", t->hasNestAttr());
	field(str, "has-no-alias-attr", t->hasNoAliasAttr());
	field(str, "has-no-capture-attr", t->hasNoCaptureAttr());
	field(str, "has-struct-ret-attr", t->hasStructRetAttr());
	field(str, "has-returned-attr", t->hasReturnedAttr());
	field(str, "only-reads-memory", t->onlyReadsMemory());
	field(str, "has-in-alloca-attr", t->hasInAllocaAttr());
	field(str, "has-zext-attr", t->hasZExtAttr());
	field(str, "has-sext-attr", t->hasSExtAttr());
}
EndInstancePopulatorNode

BeginInstancePopulatorNode_Partial(llvm::Argument, env, t, p) {
	populate(env, (llvm::Value*)t, p);
	knowledge::setParent(env, t, p);
}
EndInstancePopulatorNode


BeginInstanceBuilderNode_Partial(llvm::BasicBlock, str, env, t, p) {
	build(str, env, (llvm::Value*)t, p);
	field(str, "is-landing-pad", t->isLandingPad());
	field(str, "has-address-taken", t->hasAddressTaken());
}
EndInstanceBuilderNode

void commonPopulateInstance(void* env, llvm::BasicBlock* t, llvm::Pass* p) {
	populate(env, (llvm::Value*)t, p);
	if (t->size() > 0) {
		void* mf = EnvCreateMultifield(env, t->size());
		int index = 1;
		for(llvm::BasicBlock::iterator it = t->begin();
				it != t->end();
				++it, ++index) {
			llvm::Instruction& inst = *it;
			SetMFType(mf, index, INSTANCE_NAME);
			SetMFValue(mf, index, getInstanceName(env, &inst, p));
		}
		directPutMultifield(env, t, "contents", mf, 1, index - 1);
	}
}
BeginInstancePopulatorNode_Full(llvm::BasicBlock, llvm::BasicBlockPass, env, t, p) {
	commonPopulateInstance(env, t, p);
}
EndInstancePopulatorNode

BeginInstancePopulatorNode_Partial(llvm::BasicBlock, env, blk, p) {
	commonPopulateInstance(env, blk, p);
	int index = 0;
	knowledge::setParent(env, blk, p);
	llvm::SmallVector<llvm::BasicBlock*, 8> preds(pred_begin(blk), pred_end(blk));
	void *predmf = EnvCreateMultifield(env, preds.size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = preds.begin();
			it != preds.end(); ++it, ++index) {
		SetMFType(predmf, index, INSTANCE_NAME);
		SetMFValue(predmf, index, getInstanceName(env, *it, p));
	}
	directPutMultifield(env, blk, "predecessors", predmf, 1, preds.size());

	llvm::SmallVector<llvm::BasicBlock*, 8> succs(succ_begin(blk), succ_end(blk));
	void *succmf = EnvCreateMultifield(env, succs.size());
	void *prodmf = EnvCreateMultifield(env, succs.size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = succs.begin(); 
			it != succs.end(); ++it, ++index) {
		void* result = getInstanceName(env, *it, p);
		SetMFType(prodmf, index, INSTANCE_NAME);
		SetMFValue(prodmf, index, result);
		SetMFType(succmf, index, INSTANCE_NAME);
		SetMFValue(succmf, index, result);
	}
	directPutMultifield(env, blk, "successors", succmf, 1, succs.size());
	directPutMultifield(env, blk, "produces", prodmf, 1, succs.size());
}
EndInstancePopulatorNode

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
		build(str, theEnv, (llvm::GlobalObject*)data, pass);
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
// Basic Block Pass

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
void EngineBookkeeping::registerInstance(void* nativeInstance, std::string& clipsInstance) {
	std::pair<void*, std::string> pair(nativeInstance, clipsInstance);
	instanceMap.insert(pair);
}
bool EngineBookkeeping::containsInstance(void* nativeInstance) {
	return instanceMap.find(nativeInstance) != instanceMap.end();
}
std::string EngineBookkeeping::getRelatedInstance(void* nativeInstance) {
	return (containsInstance(nativeInstance) ? instanceMap[nativeInstance] : "");
}

void EngineBookkeeping::registerExternalAddress(int type, int id) {
    if (type >= 0 && type < RegisteredExternalAddressTypes) {
        externalAddrs[type] = id;
    }
}

bool EngineBookkeeping::containsExternalAddress(int type) {
    return type >= 0 && type < RegisteredExternalAddressTypes;
}

int EngineBookkeeping::getRelatedExternalAddress(int type) {
    if (type >= 0 && type < RegisteredExternalAddressTypes) {
        return externalAddrs[type];
    } else {
        return -1;
    }
}
template<typename T>
void installNativeInstance(void* env, T* target) {
	DATA_OBJECT wrapper;
	SetType(wrapper, EXTERNAL_ADDRESS);
	SetValue(wrapper, EnvAddExternalAddress(env, (void*)target, getExternalAddressId<T>(env)));
	EnvDirectPutSlot(env, GetNativeInstance(env, target), "native", &wrapper);
}
template<typename T, typename P>
bool conv(void* env, T* t, P* p) {
	void* result = dispatch(env, t, p);
	return result != NULL;
}
#define declareConvert(type, passType) \
	bool convert(void* e, type * t, passType * p) { return conv(e, t, p); }
declareConvert(llvm::Function, llvm::FunctionPass);
declareConvert(llvm::BasicBlock, llvm::BasicBlockPass);
declareConvert(llvm::Module, llvm::ModulePass);
declareConvert(llvm::Loop, llvm::LoopPass);
declareConvert(llvm::Region, llvm::RegionPass);
#undef declareConvert

template<typename T>
struct ElectronClassInheritanceHierarchy {
	static void getSupertypes(llvm::raw_string_ostream& str) {
		str << "USER";
	}
};

#define ElectronClassInheritanceAssociation(type, inherits) \
	template<> \
struct ElectronClassInheritanceHierarchy<type> { \
	static void getSupertypes(llvm::raw_string_ostream& str) { \
		str << inherits ; \
	} \
}
#define X(unused, type, unused2, inherits) \
	ElectronClassInheritanceAssociation(type, inherits);
#include "knowledge/EngineNodes.def"
#undef X
template<typename T>
struct ElectronClassSlotBuilder {
	static void AddSlots(llvm::raw_string_ostream& str) { }
};
template<typename T>
void defslot(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(slot " << name << "\n"
		<< "(visibility public))\n";
}
template<typename T>
void defmultislot(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(multislot " << name << "\n"
		<< "(visibility public))\n";
}

#define defdefslot(type) \
	template<> \
	void defslot(llvm::raw_string_ostream& str, const std::string& name) 

template<>
void defslot<bool>(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(slot " << name << "\n"
		<< "(type SYMBOL)\n"
		<< "(visibility public)\n"
		<< "(allowed-values FALSE\nTRUE))\n";
}
#define defstring_defslot(type) \
template<> \
void defslot<type>(llvm::raw_string_ostream& str, const std::string& name) { \
	str << "(slot " << name << "\n" \
		<< "(type LEXEME)\n"; \
		<< "(visibility public))\n" \
}

defstring_defslot(std::string)
defstring_defslot(llvm::StringRef)

template<> 
void defslot<unsigned>(llvm::raw_string_ostream& str, const std::string& name) { 
	str << "(slot " << name << "\n" 
		<< "(type NUMBER)\n" 
		<< "(visibility public)\n" 
		<< "(range 0 ?VARIABLE))\n"; 
}
template<>
void defslot<int>(llvm::raw_string_ostream& str, const std::string& name) {
	str << "(slot " << name << "\n" 
		<< "(type NUMBER)\n" 
		<< "(visibility public))\n";
}

#define slot(type, name) \
		defslot<type>(str, name)
#define multislot(type, name) \
		defmultislot<type>(str, name)
#define bool_slot(name) \
		slot(bool, name)
template<typename T>
struct DefClassBuilderNode {
	static void buildDefClass(llvm::raw_string_ostream& str) { }
};
//Build DefClass Node
#define BeginDefClassBuilderNode(type) \
	template<> \
	struct DefClassBuilderNode<type> { \
		static void buildDefClass(llvm::raw_string_ostream& str)

#define EndDefClassBuilderNode EndNode

#define call_defclass_parent(type) \
		DefClassBuilderNode<type>::buildDefClass(str)
#include "knowledge/defclass_slots.def"
template<typename T>
void defclass() {
	std::string tmp;
	llvm::raw_string_ostream str(tmp);
	str << "(defclass ";
	ElectronClassNameSelector<T>::selectName(str);
	str << " " << "(is-a ";
	ElectronClassInheritanceHierarchy<T>::getSupertypes(str);
	str << " "
	// slots are going to go here
	str << ")";
	// print it out at the end
	printf("%s\n", tmp.c_str());
}
void generateDefClasses() {
#define X(_, fullType, _, _) \
	defclass<fullType>();
#include "knowledge/EngineNodes.def"
#undef X
}

} // namespace knowledge

// Generate the decls for the registration functions
#define X(shortType, fullType, str, unused) \
    extern "C" void RegisterExternalAddress_Native ## shortType (void* theEnv); \
    void PrintNative ## shortType ## Address (void* theEnv, const char* logicalName, void* theValue);  \
    intBool DeallocateNative ## shortType (void* theEnv, void* theValue); \
    void NewNative ## shortType (void* theEnv, DATA_OBJECT* retVal); \
    intBool CallNative ## shortType (void* theEnv, DATA_OBJECT* theValue, DATA_OBJECT* retVal);
#include "knowledge/EngineNodes.def"
#undef X
extern "C" void RegisterEngineBookkeeping(void* theEnv) {
	if(!AllocateEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA, 
				sizeof(knowledge::EngineBookkeeping), NULL)) {
		llvm::report_fatal_error("Error allocating environment data for ENGINE_BOOKKEEPING_DATA");
	}
// call of the registration functions
#define X(shortType, fullType, str, unused) RegisterExternalAddress_Native ## shortType (theEnv);
#include "knowledge/EngineNodes.def"
#undef X

}
extern "C" void RegisterNativeInstance(void* theEnv, void* native, void* instance) {
    std::string iname(EnvGetInstanceName(theEnv, instance));
    EngineBookkeepingData(theEnv)->registerInstance(native, iname);
}
extern "C" bool ContainsNativeInstance(void* theEnv, void* key) {
    return EngineBookkeepingData(theEnv)->containsInstance(key);
}
extern "C" void* GetNativeInstance(void* theEnv, void* key) {
    if (EngineBookkeepingData(theEnv)->containsInstance(key)) {
        std::string tmp = EngineBookkeepingData(theEnv)->getRelatedInstance(key);
        return EnvFindInstance(theEnv, NULL, tmp.c_str(), TRUE);
    } else {
        return NULL;
    }
}
extern "C" void RegisterExternalAddressId(void* theEnv, int type, struct externalAddressType* ea) {
    EngineBookkeepingData(theEnv)->registerExternalAddress(type, InstallExternalAddressType(theEnv, ea));
}
extern "C" bool ContainsExternAddressId(void* theEnv, int type) {
    return EngineBookkeepingData(theEnv)->containsExternalAddress(type);
}
extern "C" int GetExternalAddressId(void* theEnv, int type) {
    return EngineBookkeepingData(theEnv)->getRelatedExternalAddress(type);
}
// Generate the registrationFunctions
#define STR(input) #input
// Automate the building of these types since they'll always be the same
//
// We only support copying already existing external addresses instead of
// building new ones which doesn't actually make sense at this point.
//
// Since we are storing the pointers directly, deallocation really isn't
// necessary, this could change but for now this will work
namespace {
	void printHexIntoClips(void* theEnv, const char* logicalName, const std::string& name, void* ptr) {
		// use c++'s iostream stuff
		std::string t;
		llvm::raw_string_ostream str(t);
		str << "<Pointer-" << name << "-" << ptr << ">";
		EnvPrintRouter(theEnv, logicalName, str.str().c_str());
	}

}
#define X(shortType, fullType, str, unused) \
void RegisterExternalAddress_Native ## shortType (void* theEnv) { \
	struct externalAddressType tmp = { \
		STR(Native ## shortType), \
		PrintNative ## shortType ## Address , \
		PrintNative ## shortType ## Address , \
		DeallocateNative ## shortType , \
		NewNative ## shortType , \
		CallNative ## shortType, \
	}; \
	knowledge::registerExternalAddressId<fullType>(theEnv, &tmp); \
} \
void NewNative ## shortType (void* theEnv, DATA_OBJECT* retVal) { \
	int count; \
	fullType * tmp; \
	DATA_OBJECT x; \
	count = EnvRtnArgCount(theEnv); \
	if (count == 2) { \
		if (EnvArgTypeCheck(theEnv, "new (llvm " STR(shortType) ")", 2, EXTERNAL_ADDRESS, &x) == FALSE) { \
			PrintErrorID(theEnv, "NEW", 1, FALSE); \
			EnvPrintRouter(theEnv, WERROR, "Function new expected an external address as the second argument.\n"); \
			SetEvaluationError(theEnv, TRUE); \
			return; \
		} \
		if (EnvDOToExternalAddressType(theEnv, x) != knowledge::getExternalAddressId<fullType>(theEnv)) { \
			PrintErrorID(theEnv, "NEW", 1, FALSE); \
			EnvPrintRouter(theEnv, WERROR, "Attempted to make a copy of the wrong external address type as " STR(shortType) "!\n"); \
			SetEvaluationError(theEnv, TRUE); \
			return; \
		} \
		tmp = (fullType *)DOToExternalAddress(x); \
		SetpType(retVal, EXTERNAL_ADDRESS); \
		SetpValue(retVal, EnvAddExternalAddress(theEnv, (void*)tmp, knowledge::getExternalAddressId<fullType>(theEnv))); \
		return; \
	}  else { \
		PrintErrorID(theEnv, "NEW", 1, FALSE); \
		EnvPrintRouter(theEnv, WERROR, "Too many or too few arguments passed while trying to construct a new " STR(shortType) "!\n"); \
		SetEvaluationError(theEnv, TRUE); \
		return; \
	} \
} \
intBool DeallocateNative ## shortType (void* theEnv, void* theValue) { \
	return TRUE; \
} \
void PrintNative ## shortType ## Address(void* theEnv, const char* logicalName, void* theValue) { \
	printHexIntoClips(theEnv, logicalName, STR(Native ## shortType), ValueToExternalAddress(theValue)); \
}
#include "knowledge/EngineNodes.def"
#undef X


#define X(a, b, c, unused) \
	int knowledge::ExternalAddressRegistration<b>::indirectId = ExtAddrType(a);
#include "knowledge/EngineNodes.def"
#undef X
