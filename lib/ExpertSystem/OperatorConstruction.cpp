#include "ExpertSystem/CLIPSOperatorBuilder.h"

CLIPSOperatorBuilder::CLIPSOperatorBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSUserBuilder(nm, ty, namer) { }
void CLIPSOperatorBuilder::addFields(Operator* v, KnowledgeConstructor* kc, char* parent) {
	CLIPSUserBuilder::addFields((User*)v, kc, parent);
}
void CLIPSOperatorBuilder::build(Operator* v, KnowledgeConstructor* kc, char* parent) {
	open();
	addFields(v,kc,parent);
	close();
	std::string str = getCompletedString();
	kc->addToKnowledgeBase((PointerAddress)v, str);
}
CLIPSOverflowingBinaryOperatorBuilder::CLIPSOverflowingBinaryOperatorBuilder(std::string nm, FunctionNamer& namer) : CLIPSOperatorBuilder(nm, "OverflowingBinaryOperator", namer) { }
void CLIPSOverflowingBinaryOperatorBuilder::addFields(OverflowingBinaryOperator* v, KnowledgeConstructor* kc, char* parent) {
	CLIPSOperatorBuilder::addFields((Operator*)v, kc, parent);
	if(v->hasNoUnsignedWrap()) addTrueField("HasNoUnsignedWrap");
	if(v->hasNoSignedWrap()) addTrueField("HasNoSignedWrap");
}
void CLIPSOverflowingBinaryOperatorBuilder::build(OverflowingBinaryOperator* v, KnowledgeConstructor* kc, char* parent) {
	open();
	addFields(v,kc,parent);
	close();
	std::string str = getCompletedString();
	kc->addToKnowledgeBase((PointerAddress)v, str);
}

CLIPSPossiblyExactOperatorBuilder::CLIPSPossiblyExactOperatorBuilder(std::string nm, FunctionNamer& namer) : CLIPSOperatorBuilder(nm, "PossiblyExactOperator", namer) { }
void CLIPSPossiblyExactOperatorBuilder::addFields(PossiblyExactOperator* v, KnowledgeConstructor* kc, char* parent) {
	CLIPSOperatorBuilder::addFields((Operator*)v, kc, parent);
	if(v->isExact()) addTrueField("IsExact");
}
void CLIPSPossiblyExactOperatorBuilder::build(PossiblyExactOperator* v, KnowledgeConstructor* kc, char* parent) {
	open();
	addFields(v,kc,parent);
	close();
	std::string str = getCompletedString();
	kc->addToKnowledgeBase((PointerAddress)v, str);
}
