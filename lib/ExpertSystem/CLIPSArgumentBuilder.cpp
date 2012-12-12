#include "ExpertSystem/CLIPSArgumentBuilder.h"
CLIPSArgumentBuilder::CLIPSArgumentBuilder(std::string nm, FunctionNamer& namer) : CLIPSValueBuilder(nm, "Argument", namer) { }
void CLIPSArgumentBuilder::addFields(Argument* arg, KnowledgeConstructor *kc, char* parent) {
   CLIPSValueBuilder::addFields((Value*)arg, kc, parent);
   addField("Index", arg->getArgNo());
   if(arg->hasNestAttr()) addTrueField("HasNestAttribute");
   if(arg->hasNoAliasAttr()) addTrueField("HasNoAliasAttribute");
   if(arg->hasNoCaptureAttr()) addTrueField("HasNoCaptureAttribute");
   if(arg->hasStructRetAttr()) addTrueField("HasStructRetAttribute");
}
void CLIPSArgumentBuilder::build(Argument* arg, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(arg, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)arg, str);
}
