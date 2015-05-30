#ifndef _ENGINE_BOOKKEEPING_H_
#define _ENGINE_BOOKKEEPING_H_
#include "llvm/ADT/DenseMap.h"
#include "knowledge/ClassNameSelector.h"
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
}
#endif
