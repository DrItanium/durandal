#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_
#include "knowledge/EngineBookkeeping.h"
#include "knowledge/ProcessingNode.h"
namespace knowledge {
template<typename Pass, typename T>
struct Router {
	static void* dispatch(void* env, T& inst) {
		return Router<Pass,T>::dispatch(env, &inst);
	}
	static void* dispatch(void* env, T* inst) {
		WhenInstanceDoesNotExist(env, inst) {
			return ProcessingNode<Pass,T>::constructInstance(env, inst);
		}
	}
};
}
#endif
