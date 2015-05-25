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
#define X(a, b, c) \
	int ExternalAddressRegistration<b>::externalAddressId = ExtAddrType(a);
#include "knowledge/EngineNodes.def"
#undef X
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
void booleanField(llvm::raw_string_ostream& str, const std::string& name, bool value) {
	if (value) {
		str << FIELD(name, "TRUE");
	}
}
template<class C> 
void* getInstanceName(void* theEnv, C* instance) {
	return EnvAddSymbol(theEnv, 
			EnvGetInstanceName(theEnv, 
				dispatch(theEnv, instance)));
}
template<class T>
void setParent(void* theEnv, T* target)
{
	DATA_OBJECT wrapper;
	void* instanceName = getInstanceName(theEnv, target->getParent());
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, instanceName);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
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
	int index = 0;
	setParent(theEnv, data);
	if (data->size() > 0) {
		void* mf = EnvCreateMultifield(theEnv, data->size());
		index = 1;
		for(llvm::BasicBlock::iterator it = data->begin();
				it != data->end();
				++it, ++index) {
			llvm::Instruction& inst = *it;
			SetMFType(mf, index, INSTANCE_NAME);
			SetMFValue(mf, index, getInstanceName(theEnv, &inst));
		}
		directPutMultifield(theEnv, data, "contents", mf, 1, index - 1);
	}
	llvm::SmallVector<llvm::BasicBlock*, 8> predecessors(pred_begin(data), pred_end(data));
	void *predMultifield = EnvCreateMultifield(theEnv, predecessors.size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = predecessors.begin();
			it != predecessors.end(); ++it, ++index) {
		SetMFType(predMultifield, index, INSTANCE_NAME);
		SetMFValue(predMultifield, index, getInstanceName(theEnv, *it));
	}
	directPutMultifield(theEnv, data, "predecessors", predMultifield, 
			1, predecessors.size());

	llvm::SmallVector<llvm::BasicBlock*, 8> successors(succ_begin(data), succ_end(data));
	void *succMultifield = EnvCreateMultifield(theEnv, successors.size());
	void *producesMultifield = EnvCreateMultifield(theEnv, successors.size());
	index = 1;
	for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = successors.begin(); 
			it != successors.end(); ++it, ++index) {
		void* result = getInstanceName(theEnv, *it);
		SetMFType(producesMultifield, index, INSTANCE_NAME);
		SetMFValue(producesMultifield, index, result);
		SetMFType(succMultifield, index, INSTANCE_NAME);
		SetMFValue(succMultifield, index, result);
	}
	directPutMultifield(theEnv, data, "successors", succMultifield, 
			1, successors.size());
	directPutMultifield(theEnv, data, "produces", producesMultifield, 
			1, successors.size());
}

buildInstanceHeader(llvm::Argument) {
	str << FIELD("index", data->getArgNo());
}
populateInstanceHeader(llvm::Argument) {
	setParent(theEnv, data);
}


#undef FIELD
#undef populateInstanceHeader
#undef buildInstanceHeader
}

