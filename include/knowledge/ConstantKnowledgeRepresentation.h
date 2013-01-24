#ifndef _constant_knowledge_representation_h
#define _constant_knowledge_representation_h
#include "knowledge/Traits.h"
#include "knowledge/Types.h"
#include "llvm/Constant.h"
namespace knowledge {
   DECLARE_CLIPS_TYPE_NAME(llvm::Constant, "Constant");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::Constant);
   DECLARE_HAS_CUSTOM_ROUTER_LOGIC(llvm::Constant);
   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::Constant> {
         static void populateKnowledgeRepresentation(llvm::Constant* cnst,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::User*)cnst, krb, kc);
            if(cnst->isNullValue()) krb->addTrueField("IsNullValue"); 
            if(cnst->isAllOnesValue()) krb->addTrueField("IsAllOnesValue"); 
            if(cnst->isNegativeZeroValue()) krb->addTrueField("IsNegativeZeroValue"); 
            if(cnst->canTrap()) krb->addTrueField("CanTrap"); 
            if(cnst->isConstantUsed()) krb->addTrueField("IsConstantUsed"); 
         }
      };
   template<>
      struct CustomRouterLogic<true> {
         template<>
            static std::string route<llvm::Constant>(llvm::Constant* obj,
                  KnowledgeConstructor* kc,
                  char* parent) {
               //we need to determine what we're looking at type-wise
               if(llvm::UndefValue* o = dyn_cast<llvm::UndefValue>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::BlockAddress* o = dyn_cast<llvm::BlockAddress>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantAggregateZero* o = dyn_cast<llvm::ConstantAggregateZero>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantArray* o = dyn_cast<llvm::ConstantArray>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantExpr* o = dyn_cast<llvm::ConstantExpr>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantFP* o = dyn_cast<llvm::ConstantFP>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantInt* o = dyn_cast<llvm::ConstantInt>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantPointerNull* o = dyn_cast<llvm::ConstantPointerNull>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::ConstantStruct* o = dyn_cast<llvm::ConstantStruct>(obj)) {
                  return Route(o, kc, parent); 
               } else if(llvm::ConstantVector* o = dyn_cast<llvm::ConstantVector>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::GlobalValue* o = dyn_cast<llvm::GlobalValue>(obj)) {
                  return Route(o, kc, parent);
               } else {
                  //use the default custom routing logic to prevent code
                  //duplication
                  return CustomRouterLogic<false>::route(obj, kc, parent);
               }
            }
      };

   extern "C" void RegisterLLVMConstantFunctions(void* theEnv);

   /* 
    * UndefValue 
    */
   DECLARE_CLIPS_TYPE_NAME(llvm::UndefValue, "UndefValue");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::UndefValue);
   KNOWLEDGE_REPRESENTATION_POPULATION_ONLY_CALLS_SUPERTYPE(llvm::UndefValue, llvm:Constant);

   /* 
    * BlockAddress
    */
   DECLARE_CLIPS_TYPE_NAME(llvm::BlockAddress, "BlockAddress");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::BlockAddress);
   template<> 
      struct KnowledgeRepresentationPopulationLogic<llvm::BlockAddress> {
         static void populateKnowledgeRepresentation(llvm::BlockAddress* obj,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::Constant*)obj, krb, kc);
            //save them as pointer addresses
            //that way we don't mess up parents 
            krb->addField("Target", (PointerAddress)addr->getBasicBlock());
            krb->addField("Function", (PointerAddress)addr->getFunction());
         }
      };
   /*
    * ConstantAggregateZero
    */
   DECLARE_CLIPS_TYPE_NAME(llvm::ConstantAggregateZero, "ConstantAggregateZero");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::ConstantAggregateZero);
   KNOWLEDGE_REPRESENTATION_POPULATION_ONLY_CALLS_SUPERTYPE(llvm::ConstantAggregateZero, llvm:Constant);

   /*
    * ConstantArray
    */
   DECLARE_CLIPS_TYPE_NAME(llvm::ConstantArray, "ConstantArray");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::ConstantArray);
   KNOWLEDGE_REPRESENTATION_POPULATION_ONLY_CALLS_SUPERTYPE(llvm::ConstantArray, llvm:Constant);

}
#endif
