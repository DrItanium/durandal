#include "knowledge/Engine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/SmallVector.h"
extern "C" {
#include "clips.h"
}
#define FIELD(name, value) " (" << name << " " << value << ") "
namespace knowledge {
#define X(a, b, c) \
	int knowledge::ExternalAddressRegistration<b>::indirectId = ExtAddrType(a);
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
inline void booleanField(llvm::raw_string_ostream& str, const std::string& name, bool value) {
	if (value) {
		str << FIELD(name, "TRUE");
	}
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


#undef FIELD
}

