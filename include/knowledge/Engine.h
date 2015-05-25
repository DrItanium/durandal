#ifndef _KNOWLEDGE_ENGINE_H_
#define _KNOWLEDGE_ENGINE_H_
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
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
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/DenseMap.h"
extern "C" {
#include "clips.h"
}
#define ENGINE_BOOKKEEPING_DATA USER_ENVIRONMENT_DATA + 0
extern "C" void RegisterEngineBookkeeping(void* theEnv);
extern "C" void RegisterNativeInstance(void* theEnv, void* native, void* instance);
extern "C" bool ContainsNativeInstance(void* theEnv, void* key);
extern "C" void* GetNativeInstance(void* theEnv, void* key);
#define EngineBookkeepingData(theEnv) \
	((struct knowledge::EngineBookkeeping*) GetEnvironmentData(theEnv, ENGINE_BOOKKEEPING_DATA))
namespace knowledge {
class EngineBookkeeping {
	public:
		EngineBookkeeping();
		~EngineBookkeeping();
		void registerInstance(void* key, std::string& value);
		bool containsInstance(void* key);
		std::string& getRelatedInstance(void* key);
	private:
		// just the pointers ma'am
		llvm::DenseMap<void*, std::string> instanceMap;
};
template<class T>
void* construct(void* theEnv, T* nativeInstance) {
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(theEnv, nativeInstance); 
	if (potentiallyAlreadyExistingInstance != NULL) { 
		return potentiallyAlreadyExistingInstance; 
	} else {
		llvm::raw_string_ostream& str;
		buildInstance(str, theEnv, nativeInstance);
		RegisterInstance(theEnv, nativeInstance, makeInstance(theEnv, str.str().c_str()));
		populateInstance(theEnv, nativeInstance);
		return GetNativeInstance(theEnv, block);
	}
}

void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Module* module);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::BasicBlock* block);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Instruction* inst);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Function* func);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Loop* loop);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Region* region);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Constant* constant);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Value* value);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::User* user);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Operator* op);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Argument* arg);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::GlobalVariable* var);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::GlobalAlias* alias);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::GlobalValue* value);
void buildInstance(llvm::raw_string_ostream& instanceBuilder, void* theEnv, llvm::Type* type);
void populateInstance(void* theEnv, llvm::Module* module);
void populateInstance(void* theEnv, llvm::BasicBlock* block);
void populateInstance(void* theEnv, llvm::Instruction* inst);
void populateInstance(void* theEnv, llvm::Function* func);
void populateInstance(void* theEnv, llvm::Loop* loop);
void populateInstance(void* theEnv, llvm::Region* region);
void populateInstance(void* theEnv, llvm::Constant* constant);
void populateInstance(void* theEnv, llvm::Value* value);
void populateInstance(void* theEnv, llvm::User* user);
void populateInstance(void* theEnv, llvm::Operator* op);
void populateInstance(void* theEnv, llvm::Argument* arg);
void populateInstance(void* theEnv, llvm::GlobalVariable* var);
void populateInstance(void* theEnv, llvm::GlobalAlias* alias);
void populateInstance(void* theEnv, llvm::GlobalValue* value);
void populateInstance(void* theEnv, llvm::Type* type);
}

#endif // _KNOWLEDGE_ENGINE_H_
