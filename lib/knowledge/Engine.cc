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
template<class T>
void setParent(void* theEnv, T* target)
{
	DATA_OBJECT wrapper;
	SetType(wrapper, INSTANCE_NAME);
	SetValue(wrapper, EnvAddSymbol(theEnv,
				EnvGetInstanceName(theEnv,
					convert(theEnv, target->getParent()))));
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, target), "parent", &wrapper);
}
template<class C> 
void* getInstanceName(void* theEnv, C* instance) {
	return EnvAddSymbol(theEnv, EnvGetInstanceName(theEnv, convert(theEnv, instance)));
}
void directPutMultifield(void* theEnv, void* nativeInstance,
		const std::string& slotName, void* multifieldData, int multifieldBegin,
		int multifieldEnd) {
	DATA_OBJECT wrapper;
	SetType(wrapper, MULTIFIELD);
	SetValue(wrapper, multifieldData);
	SetDOBegin(wrapper, multifieldBegin);
	SetDOEnd(wrapper, multifieldEnd);
	EnvDirectPutSlot(theEnv, GetNativeInstance(theEnv, nativeInstance),
			slotName.c_str(), &wrapper);
}
template<class C>
void directPutMultifieldInstanceName(void* theEnv, 
		C* rawInstance, const std::string& slotName) {
	void* mf = EnvCreateMultifield(theEnv, rawInstance->size());
	int index = 1;
	for(typename C::iterator it = rawInstance->begin(); 
			it != rawInstance->end(); 
			++it, ++index) {
		SetMFType(mf, index, INSTANCE_NAME);
		SetMFValue(mf, index, getInstanceName(theEnv, *it));
	}
	directPutMultifield(theEnv, rawInstance, slotName, mf, 1, index - 1);
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

void* convert(void* theEnv, llvm::BasicBlock* block) {
	OnTypeNotRegistered(theEnv, block) {
		llvm::raw_string_ostream str;
		str << "(of llvm::basic-block "
			<< booleanField(str, "is-landing-pad", block->isLandingPad())
			<< booleanField(str, "has-address-taken", block->hasAddressTaken())
			<< ")";
		RegisterInstance(theEnv, block, 
				makeInstance(theEnv, str.str().c_str()));
		setParent(theEnv, block);
		if (block->size() > 0) {
			directPutMultifieldInstanceName(theEnv, block, "contents");
		}
		llvm::SmallVector<llvm::BasicBlock*, 8> predecessors(pred_begin(block), pred_end(block))
		void *predMultifield = EnvCreateMultifield(theEnv, predecessors->size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*,8>::iterator it = predecessors.begin();
				it != predecessors.end(); ++it, ++index) {
			SetMFType(predMultifield, index, INSTANCE_NAME);
			SetMFValue(predMultifield, index, getInstanceName(theEnv, *it));
		}
		directPutMultifield(theEnv, block, "predecessors", predMultifield);

		llvm::SmallVector<llvm::BasicBlock*, 8> successors(succ_begin(block), succ_end(block));
		void *succMultifield = EnvCreateMultifield(theEnv, successors->size());
		void *producesMultifield = EnvCreateMultifield(theEnv, successors->size());
		index = 1;
		for (llvm::SmallVector<llvm::BasicBlock*, 8>::iterator it = successors.begin(); 
				it != successors.end(); ++it, ++index) {
			void* result = getInstanceName(theEnv, *it);
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

void* convert(void* theEnv, llvm::Argument* arg) {
	OnTypeNotRegistered(theEnv, arg) {
		llvm::raw_string_ostream str;
		str << "(of llvm::basic-block "
			<< FIELD("index", arg->getArgNo())
			<< booleanField(str, "has-nest-attribute", arg->hasNextAttr())
			<< booleanField(str, "has-no-alias-attribute", arg->hasNoAliasAttr())
			<< booleanField(str, "has-no-capture-attribute", arg->hasNoCaptureAttr())
			<< booleanField(str, "has-non-null-attr", arg->hasNonNullAttr())
			<< ")";
		RegisterInstance(theEnv, block, 
				makeInstance(theEnv, str.str().c_str()));
		setParent(theEnv, block);
		return GetNativeInstance(theEnv, arg);
	}
}

void* convert(void* theEnv, llvm::Instruction* inst) {

}

void* convert(void* theEnv, llvm::Function* func) {

}

void* convert(void* theEnv, llvm::Constant* constant) {

}

void* convert(void* theEnv, llvm::Value* value) {

}

void* convert(void* theEnv, llvm::User* user) {

}

void* convert(void* theEnv, llvm::Operator* op) {

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

