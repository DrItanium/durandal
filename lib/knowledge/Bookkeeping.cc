#include "knowledge/Engine.h"
#include "knowledge/EngineBookkeeping.h"
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
// Automate the building of these types since they'll always be the same
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
extern "C" void RegisterExternalAddressId(void* theEnv, int type, struct externalAddressType* ea) {
    EngineBookkeepingData(theEnv)->registerExternalAddress(type, InstallExternalAddressType(theEnv, ea));
}
extern "C" bool ContainsExternAddressId(void* theEnv, int type) {
    return EngineBookkeepingData(theEnv)->containsExternalAddress(type);
}
extern "C" int GetExternalAddressId(void* theEnv, int type) {
    return EngineBookkeepingData(theEnv)->getRelatedExternalAddress(type);
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
#define DefaultNewImplementation(shortType, fullType) \
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
            if (DOGetExternalAddressType(x) != knowledge::getExternalAddress<fullType>(theEnv)) { \
                PrintErrorID(theEnv, "NEW", 1, FALSE); \
                EnvPrintRouter(theEnv, WERROR, "Attempted to make a copy of the wrong external address type as " STR(shortType) "!\n"); \
                SetEvaluationError(theEnv, TRUE); \
                return; \
            } \
            tmp = DOToExternalAddress(x); \
            SetpType(retVal, EXTERNAL_ADDRESS); \
            SetpValue(retVal, EnvAddExternalAddress(theEnv, (void*)tmp, \
                        knowledge::getExternalAddress<fullType>(theEnv)); \
            return; \
        }  else { \
            PrintErrorID(theEnv, "NEW", 1, FALSE); \
            EnvPrintRouter(theEnv, WERROR, "Too many or too few arguments passed while trying to construct a new " STR(shortType) "!\n"); \
            SetEvaluationError(theEnv, TRUE); \
            return; \
        } \
    } 

#define DefaultDeallocateImplementation(shortType) \
    intBool DeallocateNative ## shortType (void* theEnv, void* theValue) { \
        return TRUE; \
    }
#undef DefaultNewImplementation
#undef DefaultDeallocateImplementation
