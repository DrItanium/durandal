#include "ExpertSystem/CLIPSValueBuilder.h"
using namespace llvm;
CLIPSValueBuilder::CLIPSValueBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSObjectBuilder(nm, ty, namer) { }
void CLIPSValueBuilder::setType(Type* t, KnowledgeConstructor* kc) {
	PointerAddress ptr = (PointerAddress)t;
   FunctionNamer& namer = getNamer();
   if(namer.pointerRegistered(ptr)) {
      addInstanceNameField("Type", namer.nameFromPointer(ptr));
   } else {
      addInstanceNameField("Type", kc->route(t, namer));
   }
}

void CLIPSValueBuilder::addFields(Value* val, KnowledgeConstructor *kc, char* parent) {
	CLIPSObjectBuilder::addFields((PointerAddress)val, kc, parent);
	setType(val->getType(), kc);
	addField("Name", val->getName());
	if(val->isDereferenceablePointer()) addTrueField("IsDereferenceablePointer");
	if(val->hasValueHandle()) addTrueField("HasValueHandle"); 
	if(val->hasOneUse()) {
      addTrueField("HasOneUse");
      addField("NumberOfUses", 1);
   } else {
      addField("NumberOfUses", val->getNumUses());
   }
}

void CLIPSValueBuilder::build(Value* val, KnowledgeConstructor *kc, char* parent) {
	open();
	addFields(val, kc, parent);
	close();
	std::string str = getCompletedString();
	kc->addToKnowledgeBase((PointerAddress)val, str);
}
