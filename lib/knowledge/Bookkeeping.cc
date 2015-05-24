#include "knowledge/Engine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
extern "C" {
#include "clips.h"
}
extern "C" void RegisterEngineBookkeeping(void* theEnv) {
	if(!AllocateEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA, 
				sizeof(knowledge::EngineBookkeeping), NULL)) {
		llvm::report_fatal_error("Error allocating environment data for ENGINE_BOOKKEEPING_DATA");
	}
}
extern "C" void RegisterInstance(void* theEnv, void* native, void* instance) {
    std::string iname(EnvGetInstanceName(theEnv, instance));
    EngineBookkeepingData(theEnv)->registerInstance(native, iname);
}
extern "C" bool ContainsInstance(void* theEnv, void* key) {
    return EngineBookkeepingData(theEnv)->containsInstance(key);
}
extern "C" void* GetInstance(void* theEnv, void* key) {
    if (EngineBookkeepingData(theEnv)->containsInstance(key)) {
        std::string& tmp = EngineBookkeepingData(theEnv)->getRelatedInstance(key);
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
	instanceMap->insert(pair);
}
bool EngineBookkeeping::contains(void* nativeInstance) {
	return instanceMap->find(nativeInstance) != instanceMap->end();
}
std::string& EngineBookkeeping::getRelatedInstance(void* nativeInstance) {
	return (contains(nativeInstance) ? instanceMap[nativeInstance] : NULL);
}
}
