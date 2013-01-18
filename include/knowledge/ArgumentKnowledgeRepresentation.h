#ifndef _argument_knowledge_representation_h
#define _argument_knowledge_representation_h
#include "knowledge/Traits.h"
#include "knowledge/Types.h"
#include "llvm/Argument.h"

namespace knowledge {
   //we don't need to worry about custom name generation
   DECLARE_CLIPS_TYPE_NAME(llvm::Argument, "Argument");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::Argument);

   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::Argument> {
         static void populateKnowledgeRepresentation(llvm::Argument* arg,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::Value*)arg, krb, kc);
            krb->addField("Index", arg->getArgNo());
            if(arg->hasNestAttr()) {
               krb->addTrueField("HasNestAttribute");
            }
            if(arg->hasNoAliasAttr()) {
               krb->addTrueField("HasNoAliasAttribute");
            }
            if(arg->hasNoCaptureAttr()) {
               krb->addTrueField("HasNoCaptureAttribute");
            }
            if(arg->hasStructRetAttr()) {
               krb->addTrueField("HasStructRetAttribute");
            }
         }
      };
}
extern "C" void RegisterArgumentInteractionFunctionsInCLIPS(void* theEnv);

#endif
