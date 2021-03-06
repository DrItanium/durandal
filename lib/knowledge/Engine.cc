#include "knowledge/Engine.h"
#include "knowledge/ClassSelector.h"
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
#include "llvm/IR/InlineAsm.h"
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
#include "llvm/ADT/Optional.h"
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
#define ExtAddrType(name) ExternalAddressTypes:: RegisterExternalAddressId_ ## name
enum class ExternalAddressTypes {
	// according to the c++ spec these values will start from 0
#define X(a, b, c, unused) \
	RegisterExternalAddressId_ ## a ,
#include "knowledge/EngineNodes.def"
#undef X
	RegisteredExternalAddressTypes,
};

class EngineBookkeeping {
	public:
		EngineBookkeeping();
		virtual ~EngineBookkeeping();
		void registerInstance(void* key, const std::string& value);
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
		int externalAddrs[static_cast<int>(ExternalAddressTypes::RegisteredExternalAddressTypes)];
};
EngineBookkeeping::EngineBookkeeping() { }
EngineBookkeeping::~EngineBookkeeping() { }
void* makeInstance(void* theEnv, const std::string& str) {
	void* result = EnvMakeInstance(theEnv, str.c_str());
	if (result == nullptr) {
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

template
void field<const std::string&>(llvm::raw_string_ostream& str, const std::string& name, const std::string& value);
template
void field<const char*>(llvm::raw_string_ostream& str, const std::string& name, const char* value);

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

#define X(unused, type, className, unused2) \
	template<> \
	struct ExternalAddressRegistration<type> { \
		static const int indirectId = static_cast<int>(ExtAddrType(unused)); \
	}; 
#include "knowledge/EngineNodes.def"
#undef X

// SO FUCKING BEAUTIFUL :D
#define WhenInstanceDoesNotExist(env, instance) \
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, instance); \
if (potentiallyAlreadyExistingInstance != nullptr) { \
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

template<typename T, typename Pass>
void* InstanceQueryNode<T,Pass>::queryInstanceName(void* env, T* inst, Pass* p) {
	return EnvAddSymbol(env, EnvGetInstanceName(env, dispatch(env, inst, p)));
}

template<typename T, typename Pass = llvm::Pass>
void* getInstanceName(void* theEnv, T* instance, Pass* pass) {
	return InstanceQueryNode<T,Pass>::queryInstanceName(theEnv, instance, pass);
}

template<typename T, typename Pass = llvm::Pass>
void* getInstanceName(void* theEnv, T& instance, Pass* pass) {
	return InstanceQueryNode<T,Pass>::queryInstanceName(theEnv, &instance, pass);
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
template<typename T, typename Pass = llvm::Pass>
void populate(void* env, T* instance, Pass* pass) {
	InstancePopulatorNode<T, Pass>::populateInstance(env, instance, pass);
}

template<typename T, typename Pass = llvm::Pass>
void build(llvm::raw_string_ostream& str, void* env, T* instance, Pass* pass) {
	InstanceBuilderNode<T, Pass>::buildInstance(str, env, instance, pass);
}
template<typename T, typename Pass>
void* ProcessingNode<T,Pass>::constructInstance(void* env, T* inst, Pass* pass) {
	std::string tmp;
	llvm::raw_string_ostream str(tmp);
	str << "( of ";
	ElectronClassNameSelector<T>::selectName(str);
	str << " ";
	build<T, Pass>(str, env, inst, pass);
	str << ")";
	RegisterNativeInstance(env, inst, makeInstance(env, str.str().c_str()));
	populate<T, Pass>(env, inst, pass);
	// install the native instance
	installNativeInstance(env, inst);
	return GetNativeInstance(env, inst);
}

// SO FUCKING BEAUTIFUL :D
#define Route(type, env, inst) \
	static void* dispatch(void* env, type * inst, Pass* pass) 
#define BeginCustomDispatch(type, env, inst) \
	template<typename Pass> \
	struct Router<llvm:: type, Pass> { \
		Route(llvm:: type, env, inst) { \
			WhenInstanceDoesNotExist(env, inst) {

#define CondDispatch(type, env, val) \
	if (llvm:: type* v = llvm::dyn_cast<llvm:: type>(val)) return Router<llvm:: type, Pass>::dispatch(env, v, pass)
#define Otherwise(type, env, val) \
	return ProcessingNode<type, Pass>::constructInstance(env, val, pass)
#define EndCustomDispatch(type, env, inst) \
	Otherwise(llvm:: type, env, inst); \
} } };

#include "knowledge/DispatchTable.def"

// Generic Pass handling
#define NewNode_Partial(type, name) \
template<typename Pass> \
struct name <type, Pass> 

#define NewNode_Full(type, passType, name) \
template<> \
struct name <type, passType>

#define EndNode };

//Build Instance Node
#define BuildInstanceHeader_Full(type, passType) \
	static void buildInstance(llvm::raw_string_ostream& str, void* env, type * t, passType* p) 
#define BuildInstanceHeader_Partial(type) \
	BuildInstanceHeader_Full(type, Pass)

#define BeginInstanceBuilderNode_Partial(type) \
	NewNode_Partial(type, InstanceBuilderNode) { \
		BuildInstanceHeader_Partial(type)

#define BeginInstanceBuilderNode_Full(type, passType) \
	NewNode_Full(type, passType, InstanceBuilderNode) { \
		BuildInstanceHeader_Full(type, passType)


#define EndInstanceBuilderNode EndNode


// Instance Populator Node
#define PopulateInstanceHeader_Full(type, passType) \
	static void populateInstance(void* env, type * t, passType * p)

#define PopulateInstanceHeader_Partial(type) \
	PopulateInstanceHeader_Full(type, Pass)

#define BeginInstancePopulatorNode_Partial(type) \
	NewNode_Partial(type, knowledge::InstancePopulatorNode) { \
		PopulateInstanceHeader_Partial(type)

#define BeginInstancePopulatorNode_Full(type, passType) \
	NewNode_Full(type, passType, knowledge::InstancePopulatorNode) { \
		PopulateInstanceHeader_Full(type, passType)

#define EndInstancePopulatorNode EndNode

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define BeginFull(type, pass) \
	BeginInstanceBuilderNode_Full(llvm:: type, llvm:: pass) {
#define Begin(type) \
	BeginInstanceBuilderNode_Partial(llvm:: type) {
#define super(type) \
		populate(env, (llvm:: type*)t, p);
#define End } };
#define EndFull(_, __) } };
#define X(_, name, op, type, ...) CAT(X, type)(name, op, __VA_ARGS__)
#define XMultifield(b, c, ...)
#define XReference(b, c, ...)
#define XValue(name, op, ...) \
		field (str, name, op);
#define XConditionalValue(name, op, cond, ...) \
		if (cond) XValue(name, op, __VA_ARGS__)
#include "knowledge/builder_nodes.def"
#include "knowledge/builder_specializations.def"
#undef super
#undef X
#undef XValue
#undef XMultifield
#undef XReference
#undef Y
#undef Begin
#undef End
#undef EndFull
#undef BeginFull
#undef XConditionalValue

template<typename T, typename P = llvm::Pass>
void customPopulationLogic(void* env, T* t, P* p) { }

void customPopulationLogic(void* env, llvm::BasicBlock* t, llvm::BasicBlockPass* p) { }
void customPopulationLogic(void* env, llvm::BasicBlock* blk, llvm::Pass* p) {
	// if it isn't a BasicBlockPass then absorb everything we can
	int index = 0;
	knowledge::setParent(env, blk, p);
	llvm::SmallVector<llvm::BasicBlock*, 8> preds(pred_begin(blk), pred_end(blk));
	void *predmf = EnvCreateMultifield(env, preds.size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = preds.begin();
			it != preds.end(); ++it, ++index) {
		SetMFType(predmf, index, INSTANCE_NAME);
		SetMFValue(predmf, index, knowledge::getInstanceName(env, *it, p));
	}
	directPutMultifield(env, blk, "predecessors", predmf, 1, preds.size());

	llvm::SmallVector<llvm::BasicBlock*, 8> succs(succ_begin(blk), succ_end(blk));
	void *succmf = EnvCreateMultifield(env, succs.size());
	void *prodmf = EnvCreateMultifield(env, succs.size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = succs.begin(); 
			it != succs.end(); ++it, ++index) {
		void* result = knowledge::getInstanceName(env, *it, p);
		SetMFType(prodmf, index, INSTANCE_NAME);
		SetMFValue(prodmf, index, result);
		SetMFType(succmf, index, INSTANCE_NAME);
		SetMFValue(succmf, index, result);
	}
	directPutMultifield(env, blk, "successors", succmf, 1, succs.size());
	directPutMultifield(env, blk, "produces", prodmf, 1, succs.size());

}

template<typename I, typename P>
void constructInstanceMultifield(void* env, int count, void* native, P* p,  const std::string& name, I begin, I end) {
	if (count > 0) {
		void* mf = EnvCreateMultifield(env, count);
		int index = 1;
		for (I it = begin; it != end; ++it, ++index) {
			SetMFType(mf, index, INSTANCE_NAME);
			SetMFValue(mf, index, knowledge::getInstanceName(env, *it, p));
		}
		directPutMultifield(env, native, name, mf, 1, index - 1);
	}
}
// special case for BasicBlock's not iterating over a list of instruction pointers

// Populator Node constructors
#define BeginFull(type, pass) \
		BeginInstancePopulatorNode_Full(llvm:: type, llvm:: pass) {
#define Begin(type) \
		BeginInstancePopulatorNode_Partial(llvm:: type) {
#define End customPopulationLogic(env, t, p); } };
#define EndFull(type, pass) customPopulationLogic<llvm:: type, llvm:: pass>(env, t, p); } };
#define super(type) \
			populate(env, (llvm:: type*)t, p);
#define XReference(name, op, ...) \
		directPutInstanceName(env, t, name, knowledge::getInstanceName(env, op, p));
#define XMultifield(name, _, begin, end, count, ...) \
		constructInstanceMultifield(env, count, t, p, name, begin, end);
#define XValue(_, __, ...)
#define XConditionalValue(name, op, cond, ...)
#define X(_, name, op, type, ...) \
		CAT(X, type)(name, op, __VA_ARGS__)
#include "knowledge/builder_nodes.def"
#include "knowledge/builder_specializations.def"
#undef X
#undef super
#undef Begin
#undef End
#undef XValue
#undef XConditionalValue
#undef XMultifield
#undef XReference
#undef BeginFull
#undef EndFull

//-----------------------------------------------------------------------------
template<typename T>
void registerExternalAddressId(void* theEnv, struct externalAddressType* ea) {
	RegisterExternalAddressId(theEnv, knowledge::ExternalAddressRegistration<T>::indirectId, ea);
}

template<typename T>
bool containsExternalAddressId(void* theEnv) {
	return ContainsExternalAddressId(theEnv, knowledge::ExternalAddressRegistration<T>::indirectId);
}

template<typename T>
int getExternalAddressId(void* theEnv) {
	return GetExternalAddressId(theEnv, knowledge::ExternalAddressRegistration<T>::indirectId);
}
void EngineBookkeeping::registerInstance(void* nativeInstance, const std::string& clipsInstance) {
	instanceMap.insert({nativeInstance, clipsInstance});
}
bool EngineBookkeeping::containsInstance(void* nativeInstance) {
	return instanceMap.find(nativeInstance) != instanceMap.end();
}
std::string EngineBookkeeping::getRelatedInstance(void* nativeInstance) {
	return (containsInstance(nativeInstance) ? instanceMap[nativeInstance] : "");
}

void EngineBookkeeping::registerExternalAddress(int type, int id) {
	if (containsExternalAddress(type)) {
        externalAddrs[type] = id;
    }
}
bool EngineBookkeeping::containsExternalAddress(int type) {
    return type >= 0 && type < static_cast<int>(ExternalAddressTypes::RegisteredExternalAddressTypes);
}

int EngineBookkeeping::getRelatedExternalAddress(int type) {
	return containsExternalAddress(type) ? externalAddrs[type] : -1;
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
	return result != nullptr;
}
#define declareConvert(type, passType) \
	bool convert(void* e, llvm:: type * t, llvm:: passType * p) { return conv(e, t, p); }
declareConvert(Function, FunctionPass);
declareConvert(BasicBlock, BasicBlockPass);
declareConvert(Module, ModulePass);
declareConvert(Loop, LoopPass);
declareConvert(Region, RegionPass);
#undef declareConvert


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
				sizeof(knowledge::EngineBookkeeping), nullptr)) {
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
        return EnvFindInstance(theEnv, nullptr, tmp.c_str(), TRUE);
    } else {
        return nullptr;
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
} \
intBool CallNative ## shortType (void* theEnv, DATA_OBJECT* theValue, DATA_OBJECT* retVal) { return 0; }

#include "knowledge/EngineNodes.def"
#undef X
