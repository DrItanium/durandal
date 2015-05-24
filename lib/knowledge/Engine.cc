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
	void* potentiallyAlreadyExistingInstance = GetNativeInstance(env, input); \
	if (potentiallyAlreadyExistingInstance != NULL) { \
		return potentiallyAlreadyExistingInstance; \
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
void directPutMultifield(void* theEnv, void* target, const std::string& slotName, void* multifield) {
		DATA_OBJECT wrapper;
		SetType(wrapper, MULTIFIELD);
		SetValue(wrapper, multifield);
		EnvDirectPutSlot(theEnv, 
				GetNativeInstance(theEnv, target),
				slotName, 
				&wrapper);
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
			SetMFType(contentsMultifield, index, INSTANCE_NAME);
			SetMFValue(contentsMultifield, index, 
					EnvAddSymbol(theEnv, 
						EnvGetInstanceName(theEnv, 
							convert(theEnv, *it, block))));
		}
		directPutMultifield(theEnv, block, "contents", contentsMultifield);
		llvm::SmallVector<llvm::BasicBlock*, 8> predecessors(pred_begin(block), pred_end(block))
		void *predMultifield = EnvCreateMultifield(theEnv, predecessors->size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = predecessors.begin();
				it != predecessors.end(); ++it, ++index) {
			SetMFType(predMultifield, index, INSTANCE_NAME);
			SetMFValue(predMultifield, index, 
					EnvAddSymbol(theEnv,
						EnvGetInstanceName(theEnv, 
							convert(theEnv, *it, parent))));
		}
		directPutMultifield(theEnv, block, "predecessors", predMultifield);

		llvm::SmallVector<llvm::BasicBlock*, 8> successors(succ_begin(block), succ_end(block));
		void *succMultifield = EnvCreateMultifield(theEnv, successors->size());
		void *producesMultifield = EnvCreateMultifield(theEnv, successors->size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = successors.begin(); 
				it != successors.end(); ++it, ++index) {
			void* result = EnvAddSymbol(theEnv, EnvGetInstanceName(theEnv, convert(theEnv, *it, parent)));
			SetMFType(producesMultifield, index, INSTANCE_NAME);
			SetMFValue(producesMultifield, index, result);
			SetMFType(succMultifield, index, INSTANCE_NAME);
			SetMFValue(succMultifield, index, result);
		}
		directPutMultifield(theEnv, block, "successors", succMultifield);
		directPutMultifield(theEnv, block, "produces", producesMultifield);
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
#undef OnTypeNotRegistered
#undef FIELD
}

