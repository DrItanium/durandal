#include "knowledge/Engine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
extern "C" {
#include "clips.h"
}

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
#define FIELD(name, value) "(" << name << " " << value << ")"
void* convert(void* theEnv, llvm::Module* module) {
	void* tmp = GetInstance(theEnv, module);
	if (tmp != NULL) {
		return tmp;
	} else {
		llvm::raw_string_ostream str;
		void* instance;
		str << "( of llvm::module "
			<< FIELD("triple", module->getTargetTriple()) 
			<< FIELD("data-layout", module->getDataLayoutStr()) 
			<< FIELD("module-identifier", module->getModuleIdentifier())
			<< FIELD("inline-asm", module->getModuleInlineAsm()) << ")";
		instance = makeInstance(theEnv, str);
		RegisterInstance(theEnv, module, instance);
		// go through and register everything now
		return instance;
	}
}

void* convert(void* theEnv, llvm::BasicBlock* block, void* parent) {

}

void* convert(void* theEnv, llvm::Instruction* inst, void* parent) {

}

void* convert(void* theEnv, llvm::Function* func, void* parent) {

}

void* convert(void* theEnv, llvm::Loop* loop, void* parent) {

}

void* convert(void* theEnv, llvm::Region* region, void* parent) {

}

void* convert(void* theEnv, llvm::Constant* constant, void* parent) {

}

void* convert(void* theEnv, llvm::Value* value, void* parent) {

}

void* convert(void* theEnv, llvm::User* user, void* parent) {

}

void* convert(void* theEnv, llvm::Operator* op, void* parent) {

}

void* convert(void* theEnv, llvm::Argument* arg, void* parent) {

}

void* convert(void* theEnv, llvm::GlobalVariable* var) {

}

void* convert(void* theEnv, llvm::GlobalAlias* alias) {

}

void* convert(void* theEnv, llvm::GlobalValue* value) {

}

void* convert(void* theEnv, llvm::Type* type) {

}
#undef FIELD
}

