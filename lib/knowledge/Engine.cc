#include "knowledge/Engine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/SmallVector.h"
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
#define FIELD(name, value) " (" << name << " " << value << ") "
#define OnTypeNotRegistered(env, input) \
	void* tmp = GetNativeInstance(env, input); \
	if (tmp != NULL) { \
		return tmp; \
	} else 
llvm::raw_string_ostream& booleanField(llvm::raw_string_ostream& str, 
		const std::string& name, bool value) {
	if (!value) {
		return str;
	} else {
		return (str << FIELD(name, "TRUE"));
	}

}
llvm::raw_string_ostream& setParent(llvm::raw_string_ostream& str, void* theEnv, void* parent) {
	if (parent == NULL) {
		return str;
	} else {
		void* result = GetNativeInstance(theEnv, parent);
		if (result == NULL) {
			llvm::report_fatal_error("Attempted to get an assocaited instance for a non registered type");
		} else {
			return str << FIELD("parent", << "[" << EnvGetInstanceName(theEnv, result) << "]");
		}
	}
}
void* convert(void* theEnv, llvm::Module* module) {
	OnTypeNotRegistered(theEnv, module) {
		llvm::raw_string_ostream str;
		str << "( of llvm::module "
			<< FIELD("triple", module->getTargetTriple()) 
			<< FIELD("data-layout", module->getDataLayoutStr()) 
			<< FIELD("module-identifier", module->getModuleIdentifier())
			<< FIELD("inline-asm", module->getModuleInlineAsm()) << ")";
		RegisterInstance(theEnv, module, makeInstance(theEnv, str));
		// go through and register everything now
		return GetNativeInstance(theEnv, module)
	}
}
llvm::raw_string_ostream& build(llvm::raw_string_ostream& str, 
		void* theEnv, llvm::Value* value, void* parent) 
{
	str << setParent(str, theEnv, parent);
	return str;
}

void* convert(void* theEnv, llvm::BasicBlock* block, void* parent) {
	OnTypeNotRegistered(theEnv, block) {
		llvm::raw_string_ostream str;
		str << "(of llvm::basic-block "
			<< build(str, theEnv, (llvm::Value*)block, parent)
			<< booleanField(str, "is-landing-pad", block->isLandingPad())
			<< booleanField(str, "has-address-taken", block->hasAddressTaken())
			<< ")";
		RegisterInstance(theEnv, block, 
				makeInstance(theEnv, str.str().c_str()));
		void* contentsMultifield = EnvCreateMultifield(theEnv, block->size());
		int index = 1;
		for(llvm::BasicBlock::iterator it = block->begin(); it != block->end(); ++it, ++index) {
			SetMFType(contentsMultifield, 1, INSTANCE_NAME);
			SetMFValue(contentsMultifield, 1, 
					EnvAddSymbol(theEnv, 
						EnvGetInstanceName(theEnv, 
							convert(theEnv, *it, block))));
		}
		DATA_OBJECT contentsWrapper;
		SetType(&contentsWrapper, MULTIFIELD);
		SetValue(&contentsWrapper, contentsMultifield);
		EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, block), "contents", contentsWrapper);
		llvm::SmallVector<std;:string, 8> predContainer;
		for(llvm::pred_iterator it = pred_begin(block); it != pred_end(block); ++it) {
			std::string tmp(EnvGetInstanceName(theEnv,
						convert(theEnv, *it, parent)));
			predContainer.push_back(tmp);
		}
		//void* preds = EnvCreateMultifield(theEnv, 

		// now go through and create everything
		return GetNativeInstance(theEnv, block);
	}
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

