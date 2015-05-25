#include "knowledge/Engine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/SmallVector.h"
extern "C" {
#include "clips.h"
}
#define buildInstanceHeader(type) \
	void buildInstance(llvm::raw_string_ostream& str, void* theEnv, type * data)
#define populateInstanceHeader(type) \
	void populateInstance(void* theEnv, type * data) 
#define FIELD(name, value) " (" << name << " " << value << ") "
namespace knowledge {
void* makeInstance(void* theEnv, llvm::raw_string_ostream& str) {
	void* result = EnvMakeInstance(theEnv, str.str().c_str());
	if (result == NULL) {
		llvm::raw_string_ostream msg;
		msg << "Couldn't create instance from " << str.str();
		llvm::report_fatal_error(msg.str())
	} 
	return result;
}
void booleanField(llvm::raw_string_ostream& str, const std::string& name, bool value) {
	if (value) {
		str << FIELD(name, "TRUE");
	}
}
template<class T>
void setParent(void* theEnv, T* target)
{
	DATA_OBJECT wrapper;
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, EnvAddSymbol(theEnv,
				EnvGetInstanceName(theEnv,
					convert(theEnv, target->getParent()))));
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
}
template<class C> 
void* getInstanceName(void* theEnv, C* instance) {
	return EnvAddSymbol(theEnv, EnvGetInstanceName(theEnv, convert(theEnv, instance)));
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
template<class C>
void directPutMultifieldInstanceName(void* theEnv, 
		C* rawInstance, const std::string& slotName) {
	void* mf = EnvCreateMultifield(theEnv, rawInstance->size());
	int index = 1;
	for(typename C::iterator it = rawInstance->begin(); 
			it != rawInstance->end(); 
			++it, ++index) {
		SetMFType(mf, index, INSTANCE_NAME);
		SetMFValue(mf, index, getInstanceName(theEnv, *it));
	}
	directPutMultifield(theEnv, rawInstance, slotName, mf, 1, index - 1);
}

buildInstanceHeader(llvm::Module) {
		str << FIELD("triple", data->getTargetTriple()) 
			<< FIELD("data-layout", data->getDataLayoutStr()) 
			<< FIELD("module-identifier", data->getModuleIdentifier())
			<< FIELD("inline-asm", data->getModuleInlineAsm());
}
populateInstanceHeader(llvm::Module) { }

buildInstanceHeader(llvm::BasicBlock) {
	booleanField(str, "is-landing-pad", data->isLandingPad());
	booleanField(str, "has-address-taken", data->hasAddressTaken());
}
populateInstanceHeader(llvm::BasicBlock) {
	setParent(theEnv, data);
	if (data->size() > 0) {
		directPutMultifieldInstanceName(theEnv, data, "contents");
	}
	llvm::SmallVector<llvm::BasicBlock*, 8> predecessors(pred_begin(data), pred_end(data))
		void *predMultifield = EnvCreateMultifield(theEnv, predecessors->size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = predecessors.begin();
			it != predecessors.end(); ++it, ++index) {
		SetMFType(predMultifield, index, INSTANCE_NAME);
		SetMFValue(predMultifield, index, getInstanceName(theEnv, *it));
	}
	directPutMultifield(theEnv, data, "predecessors", predMultifield);

	llvm::SmallVector<llvm::BasicBlock*, 8> successors(succ_begin(data), succ_end(data));
	void *succMultifield = EnvCreateMultifield(theEnv, successors->size());
	void *producesMultifield = EnvCreateMultifield(theEnv, successors->size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = successors.begin(); 
			it != successors.end(); ++it, ++index) {
		void* result = getInstanceName(theEnv, *it);
		SetMFType(producesMultifield, index, INSTANCE_NAME);
		SetMFValue(producesMultifield, index, result);
		SetMFType(succMultifield, index, INSTANCE_NAME);
		SetMFValue(succMultifield, index, result);
	}
	directPutMultifield(theEnv, data, "successors", succMultifield);
	directPutMultifield(theEnv, data, "produces", producesMultifield);
}

buildInstanceHeader(llvm::Argument) {
	str << FIELD("index", arg->getArgNo());
}
populateInstanceHeader(llvm::Argument) {
	setParent(theEnv, arg);
}



#undef FIELD
#undef populateInstanceHeader
#undef buildInstanceHeader
}

