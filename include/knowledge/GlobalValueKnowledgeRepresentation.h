#ifndef _global_value_knowledge_representation_h
#define _global_value_knowledge_representation_h
#include "knowledge/Traits.h"
#include "knowledge/Types.h"
#include "llvm/GlobalValue.h"
namespace knowledge {
   DECLARE_CLIPS_TYPE_NAME(llvm::GlobalValue, "GlobalValue");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::GlobalValue);
   DECLARE_HAS_CUSTOM_ROUTER_LOGIC(llvm::GlobalValue);
   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::GlobalValue> {
         static void populateKnowledgeRepresentation(llvm::GlobalValue* obj,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::Constant*)obj, krb, kc);
            krb->addField("Alignment", addr->getAlignment());
            if(addr->hasUnnamedAddr()) krb->addTrueField("HasUnnamedAddr"); 
            if(addr->hasDefaultVisibility()) krb->addTrueField("HasDefaultVisibility"); 
            if(addr->hasHiddenVisibility()) krb->addTrueField("HasHiddenVisibility"); 
            if(addr->hasProtectedVisibility()) krb->addTrueField("HasProtectedVisibility"); 
            if(addr->hasSection()) {
               krb->addTrueField("HasSection");
               krb->addField("Section", addr->getSection());
            }
            if(addr->hasExternalLinkage()) krb->addTrueField("HasExternalLinkage"); 
            if(addr->hasAvailableExternallyLinkage()) krb->addTrueField("HasAvailableExternallyLinkage"); 
            if(addr->hasLinkOnceLinkage()) krb->addTrueField("HasLinkOnceLinkage"); 
            if(addr->hasWeakLinkage()) krb->addTrueField("HasWeakLinkage");
            if(addr->hasAppendingLinkage()) krb->addTrueField("HasAppendingLinkage"); 
            if(addr->hasInternalLinkage()) krb->addTrueField("HasInternalLinkage"); 
            if(addr->hasPrivateLinkage()) krb->addTrueField("HasPrivateLinkage"); 
            if(addr->hasLinkerPrivateLinkage()) krb->addTrueField("HasLinkerPrivateLinkage"); 
            if(addr->hasLinkerPrivateWeakLinkage()) krb->addTrueField("HasLinkerPrivateWeakLinkage"); 
            if(addr->hasLinkerPrivateWeakDefAutoLinkage()) krb->addTrueField("HasLinkerPrivateWeakDefAutoLinkage"); 
            if(addr->hasLocalLinkage()) krb->addTrueField("HasLocalLinkage");
            if(addr->hasDLLImportLinkage()) krb->addTrueField("HasDLLImportLinkage");
            if(addr->hasDLLExportLinkage()) krb->addTrueField("HasDLLExportLinkage");
            if(addr->hasExternalWeakLinkage()) krb->addTrueField("HasExternalWeakLinkage");
            if(addr->hasCommonLinkage()) krb->addTrueField("HasCommonLinkage");
            if(addr->mayBeOverridden()) krb->addTrueField("MayBeOverridden");
            if(addr->isWeakForLinker()) krb->addTrueField("IsWeakForLinker");
         }
      };
   template<>
      struct CustomRouterLogic<true> {
         template<>
            static std::string route<llvm::GlobalValue>(llvm::GlobalValue* obj,
                  KnowledgeConstructor* kc, char* parent) {
               //first thing to do is figure out the exact type
               if(llvm::GlobalAlias* o = dyn_cast<llvm::GlobalAlias>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::GlobalVariable* o = dyn_cast<llvm::GlobalVariable>(obj)) {
                  return Route(o, kc, parent);
               } else {
                  //use the default code for this
                  return CustomRouterLogic<false>::route(obj, kc, parent);
               }
            }
      };
}

#endif
