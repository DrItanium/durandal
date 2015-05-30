#ifndef _GENERIC_PROCESSING_H
#define _GENERIC_PROCESSING_H
#include "knowledge/Runtime.h"

namespace knowledge {
/*
template<typename Pass, typename T>
struct ProcessingNode {
	static void* constructInstance(void* env, T* inst, Pass& pass);
	static void populateInstance(void* env, T* instance, Pass& pass);
	static void buildInstance(llvm::raw_string_ostream& str, void* theEnv, T* data, Pass& pass);
	static void* getInstanceName(void* theEnv, T* instance, Pass& pass);
	static void setParent(void* theEnv, T* target, Pass& pass);
};
*/
template<typename Pass>
struct ProcessingNode<Pass, llvm::BasicBlock> {
	static void buildInstance(llvm::raw_string_ostream& str, void* env, llvm::BasicBlock* instance, Pass& pass) {
		booleanField(str, "is-landing-pad", data->isLandingPad());
		booleanField(str, "has-address-taken", data->hasAddressTaken());
	}
	static void populateInstance(void* env, T* instance, Pass& pass) {
		int index = 0;
		setParent<P, llvm::BasicBlock>(env, data);
		if (data->size() > 0) {
			void* mf = EnvCreateMultifield(env, data->size());
			index = 1;
			for(llvm::BasicBlock::iterator it = data->begin();
					it != data->end();
					++it, ++index) {
				llvm::Instruction& inst = *it;
				SetMFType(mf, index, INSTANCE_NAME);
				SetMFValue(mf, index, getInstanceName(env, &inst));
			}
			directPutMultifield(env, data, "contents", mf, 1, index - 1);
		}
		llvm::SmallVector<llvm::BasicBlock*, 8> predecessors(pred_begin(data), pred_end(data));
		void *predMultifield = EnvCreateMultifield(env, predecessors.size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = predecessors.begin();
				it != predecessors.end(); ++it, ++index) {
			SetMFType(predMultifield, index, INSTANCE_NAME);
			SetMFValue(predMultifield, index, getInstanceName(env, *it, pass));
		}
		directPutMultifield(env, data, "predecessors", predMultifield, 
				1, predecessors.size());

		llvm::SmallVector<llvm::BasicBlock*, 8> successors(succ_begin(data), succ_end(data));
		void *succmf = EnvCreateMultifield(env, successors.size());
		void *producesmf = EnvCreateMultifield(env, successors.size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = successors.begin(); 
				it != successors.end(); ++it, ++index) {
			void* result = getInstanceName(env, *it);
			SetMFType(producesmf, index, INSTANCE_NAME);
			SetMFValue(producesmf, index, result);
			SetMFType(succmf, index, INSTANCE_NAME);
			SetMFValue(succmf, index, result);
		}
		directPutMultifield(env, data, "successors", succmf, 1, successors.size());
		directPutMultifield(env, data, "produces", producesmf, 1, successors.size());
	}
};

}
#endif
