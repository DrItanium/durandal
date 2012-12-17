#include "ExpertSystem/CLIPSConstantBuilder.h"

using namespace llvm;

CLIPSConstantBuilder::CLIPSConstantBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSUserBuilder(nm, ty, namer) { }
void CLIPSConstantBuilder::addFields(Constant* cnst, KnowledgeConstructor* kc, char* parent) {
   CLIPSUserBuilder::addFields((User*)cnst, kc, parent);
   if(cnst->isNullValue()) addTrueField("IsNullValue"); 
   if(cnst->isAllOnesValue()) addTrueField("IsAllOnesValue"); 
   if(cnst->isNegativeZeroValue()) addTrueField("IsNegativeZeroValue"); 
   if(cnst->canTrap()) addTrueField("CanTrap"); 
   if(cnst->isConstantUsed()) addTrueField("IsConstantUsed"); 
}
void CLIPSConstantBuilder::build(Constant* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}
CLIPSBlockAddressBuilder::CLIPSBlockAddressBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "BlockAddress", namer) { }
void CLIPSBlockAddressBuilder::addFields(BlockAddress* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
   addField("Target", addr->getBasicBlock()->getName());
   addField("Function", addr->getFunction()->getName());
}
void CLIPSBlockAddressBuilder::build(BlockAddress* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSConstantAggregateZeroBuilder::CLIPSConstantAggregateZeroBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantAggregateZero", namer) { }
void CLIPSConstantAggregateZeroBuilder::addFields(ConstantAggregateZero* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
}
void CLIPSConstantAggregateZeroBuilder::build(ConstantAggregateZero* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}
CLIPSConstantArrayBuilder::CLIPSConstantArrayBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantArray", namer) { }
void CLIPSConstantArrayBuilder::addFields(ConstantArray* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
}
void CLIPSConstantArrayBuilder::build(ConstantArray* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSConstantExpressionBuilder::CLIPSConstantExpressionBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantExpression", namer) { }
void CLIPSConstantExpressionBuilder::addFields(ConstantExpr* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
   if(addr->isCast()) addTrueField("IsCast");
   if(addr->isCompare()) {
      addTrueField("IsCompare");
      addField("Predicate", addr->getPredicate());
   }
   if(addr->hasIndices()) addTrueField("HasIndices");
   if(addr->isGEPWithNoNotionalOverIndexing()) addTrueField("IsGEPWithNoNotionalOverIndexing");
   addField("Operation", addr->getOpcodeName());
}
void CLIPSConstantExpressionBuilder::build(ConstantExpr* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}


CLIPSConstantFloatingPointBuilder::CLIPSConstantFloatingPointBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantFloatingPoint", namer) { }
void CLIPSConstantFloatingPointBuilder::addFields(ConstantFP* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
   if(addr->isZero()) {
      addTrueField("IsZero");
      addField("Value", 0.0);
   } else {
      if(addr->isNegative()) addTrueField("IsNegative");
      if(addr->isNaN()) addTrueField("IsNaN");
      addField("Value", addr->getValueAPF().convertToDouble());
   }
   //While the APF is arbitrary precision it's not that big of a deal to 
   //lose precision for my purposes
}
void CLIPSConstantFloatingPointBuilder::build(ConstantFP* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSConstantIntegerBuilder::CLIPSConstantIntegerBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantInteger", namer) { }
void CLIPSConstantIntegerBuilder::addFields(ConstantInt* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
   addField("Width", addr->getBitWidth());
   if(addr->isZero()) {
      addTrueField("IsZero");
      addField("Value", 0);
   } else {
      if(addr->isNegative()) {
         addTrueField("IsNegative");
         if(addr->isMinusOne()) addTrueField("IsMinusOne");
         addField("Value", addr->getSExtValue());			
      } else {
         if(addr->isOne()) addTrueField("IsOne");
         addField("Value", addr->getLimitedValue());
      }
   }
}
void CLIPSConstantIntegerBuilder::build(ConstantInt* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSConstantPointerNullBuilder::CLIPSConstantPointerNullBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantPointerNull", namer) { }
void CLIPSConstantPointerNullBuilder::addFields(ConstantPointerNull* addr, KnowledgeConstructor *kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
}
void CLIPSConstantPointerNullBuilder::build(ConstantPointerNull* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSConstantStructBuilder::CLIPSConstantStructBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantStruct", namer) { }
void CLIPSConstantStructBuilder::addFields(ConstantStruct* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
}
void CLIPSConstantStructBuilder::build(ConstantStruct* cnst, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSConstantVectorBuilder::CLIPSConstantVectorBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantVector", namer) { }
void CLIPSConstantVectorBuilder::addFields(ConstantVector* addr, KnowledgeConstructor* kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
   addField("SplatValue", kc->route(addr->getSplatValue(), getNamer()));
}
void CLIPSConstantVectorBuilder::build(ConstantVector* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSGlobalValueBuilder::CLIPSGlobalValueBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSConstantBuilder(nm, ty, namer) { }
void CLIPSGlobalValueBuilder::addFields(GlobalValue* addr, KnowledgeConstructor *kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
   addField("Alignment", addr->getAlignment());
   if(addr->hasUnnamedAddr()) addTrueField("HasUnnamedAddr"); 
   if(addr->hasDefaultVisibility()) addTrueField("HasDefaultVisibility"); 
   if(addr->hasHiddenVisibility()) addTrueField("HasHiddenVisibility"); 
   if(addr->hasProtectedVisibility()) addTrueField("HasProtectedVisibility"); 
   if(addr->hasSection()) {
      addTrueField("HasSection");
      addField("Section", addr->getSection());
   }
   if(addr->hasExternalLinkage()) addTrueField("HasExternalLinkage"); 
   if(addr->hasAvailableExternallyLinkage()) addTrueField("HasAvailableExternallyLinkage"); 
   if(addr->hasLinkOnceLinkage()) addTrueField("HasLinkOnceLinkage"); 
   if(addr->hasWeakLinkage()) addTrueField("HasWeakLinkage");
   if(addr->hasAppendingLinkage()) addTrueField("HasAppendingLinkage"); 
   if(addr->hasInternalLinkage()) addTrueField("HasInternalLinkage"); 
   if(addr->hasPrivateLinkage()) addTrueField("HasPrivateLinkage"); 
   if(addr->hasLinkerPrivateLinkage()) addTrueField("HasLinkerPrivateLinkage"); 
   if(addr->hasLinkerPrivateWeakLinkage()) addTrueField("HasLinkerPrivateWeakLinkage"); 
   if(addr->hasLinkerPrivateWeakDefAutoLinkage()) addTrueField("HasLinkerPrivateWeakDefAutoLinkage"); 
   if(addr->hasLocalLinkage()) addTrueField("HasLocalLinkage");
   if(addr->hasDLLImportLinkage()) addTrueField("HasDLLImportLinkage");
   if(addr->hasDLLExportLinkage()) addTrueField("HasDLLExportLinkage");
   if(addr->hasExternalWeakLinkage()) addTrueField("HasExternalWeakLinkage");
   if(addr->hasCommonLinkage()) addTrueField("HasCommonLinkage");
   if(addr->mayBeOverridden()) addTrueField("MayBeOverridden");
   if(addr->isWeakForLinker()) addTrueField("IsWeakForLinker");
}
void CLIPSGlobalValueBuilder::build(GlobalValue* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}


CLIPSGlobalAliasBuilder::CLIPSGlobalAliasBuilder(std::string nm, FunctionNamer& namer) : CLIPSGlobalValueBuilder(nm, "GlobalAlias", namer) { }
void CLIPSGlobalAliasBuilder::addFields(GlobalAlias* addr, KnowledgeConstructor *kc, char* parent) {
   CLIPSGlobalValueBuilder::addFields((GlobalValue*)addr, kc, parent); 
   addField("Aliasee", kc->route(addr->getAliasee(), getNamer()));
}
void CLIPSGlobalAliasBuilder::build(GlobalAlias* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSGlobalVariableBuilder::CLIPSGlobalVariableBuilder(std::string nm, FunctionNamer& namer) : CLIPSGlobalValueBuilder(nm, "GlobalVariable", namer) { }


void CLIPSGlobalVariableBuilder::addFields(GlobalVariable* addr, KnowledgeConstructor *kc, char* parent) {
   CLIPSGlobalValueBuilder::addFields((GlobalValue*)addr, kc, parent); 
   if(addr->hasInitializer()) {
      addTrueField("HasInitializer");
      addField("Initializer", kc->route(addr->getInitializer(), getNamer()));
   }
   if(addr->hasDefinitiveInitializer()) addTrueField("HasDefinitiveInitializer");
   if(addr->hasUniqueInitializer()) addTrueField("HasUniqueInitializer");
   if(addr->isConstant()) addTrueField("IsConstant"); 
   //if(addr->isThreadLocal()) addTrueField("IsThreadLocal");
}
void CLIPSGlobalVariableBuilder::build(GlobalVariable* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

CLIPSUndefValueBuilder::CLIPSUndefValueBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "UndefValue", namer) { }

void CLIPSUndefValueBuilder::addFields(UndefValue* addr, KnowledgeConstructor *kc, char* parent) {
   CLIPSConstantBuilder::addFields((Constant*)addr, kc, parent); 
}
void CLIPSUndefValueBuilder::build(UndefValue* cnst, KnowledgeConstructor *kc, char* parent) {
   open();
   addFields(cnst, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)cnst, str);
}

