#include "knowledge/Engine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
extern "C" {
#include "clips.h"
}
#define X(shortType, fullType, str) \
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
#define X(shortType, fullType, str) RegisterExternalAddress_Native ## shortType (theEnv);
#include "knowledge/EngineNodes.def"
#undef X

}
#define STR(input) #input
#define X(shortType, fullType, str) \
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
    }
#include "knowledge/EngineNodes.def"
#undef X


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

namespace knowledge {
EngineBookkeeping::EngineBookkeeping() { }
EngineBookkeeping::~EngineBookkeeping() { }
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

int  EngineBookkeeping::getRelatedExternalAddress(int type) {
    if (type >= 0 && type < RegisteredExternalAddressTypes) {
        return externalAddrs[type];
    } else {
        return -1;
    }
}

}
