#ifndef _instruction_knowledge_representation_h
#define _instruction_knowledge_representation_h
#include "knowledge/Types.h"
#include "knowledge/Traits.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
namespace knowledge {
   DECLARE_CLIPS_TYPE_NAME(llvm::Instruction, "Instruction");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::Instruction);
   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::Instruction> {
         static void populateKnowledgeRepresentation(llvm::Instruction* inst,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::User*)inst, krb, kc);
            //we need to find a way to get the time index out for this
            //instruction. Easiest way is to tie an integer to a given pointer
            //in the KnowledgeConstructor. 
            krb->addField("TimeIndex", kc->getTimeIndex(instruction->getParent()));
            krb->addField("Operation", (char*)instruction->getOpcodeName());

            if(instruction->mayWriteToMemory()) krb->addTrueField("MayWriteToMemory");
            if(instruction->mayReadFromMemory()) krb->addTrueField("MayReadFromMemory");
            if(instruction->mayReadOrWriteMemory()) krb->addTrueField("MayReadOrWriteMemory");
            if(instruction->mayHaveSideEffects()) krb->addTrueField("MayHaveSideEffects");
            if(instruction->isBinaryOp()) krb->addTrueField("IsBinaryOp");
            if(instruction->isTerminator()) krb->addTrueField("IsTerminator");
            if(instruction->isShift()) krb->addTrueField("IsShift");
            if(instruction->isCast()) krb->addTrueField("IsCast");
            if(instruction->isArithmeticShift()) krb->addTrueField("IsArithmeticShift"); 
            if(instruction->isLogicalShift()) krb->addTrueField("IsLogicalShift");
            if(instruction->isAssociative()) krb->addTrueField("IsAssociative");
            if(instruction->isCommutative()) krb->addTrueField("IsCommutative");
            krb->addField("DestinationRegisters", GetUniqueName(instruction, kc));
            if(!instruction->use_empty()) {
               krb->openMultifield("Consumers");
               for(Value::use_iterator i = instruction->use_begin(), 
                     e = instruction->use_end(); i != e; ++i) {
                  User* target = *i;
                  PointerAddress ptr = (PointerAddress)target;
                  if(kc->objectAlreadyConvertedToKnowledge(ptr)) {
                     krb->appendValue(kc->getObjectKnowledgeName(ptr));
                  } else if(llvm::Function* o = dyn_cast<llvm::Function>(target)) {
                     krb->appendValue(GetUniqueName(o,kc));
                  } else if(llvm::Instruction* o = dyn_cast<llvm::Instruction>(target)) {
                     krb->appendValue(GetUniqueName(o,kc));
                  } else {
                     krb->appendValue(Route(target, kc, (char*)""));
                  }
               }
               krb->closeMultifield();
            }
         }
      };
   DECLARE_HAS_CUSTOM_NAME_GENERATION_LOGIC(llvm::Instruction);
   template<>
      struct CustomNameGenerationLogic<true> {
         template<>
            static std::string getUniqueName<llvm::Instruction>(
                  llvm::Instruction* obj, 
                  KnowledgeConstructor* kc) {
               if(!obj->getType()->isVoidTy()) {
                  if(obj->hasName()) {
                     std::string tmp(obj->getName().data());
                     return tmp;
                  } else {
                     char* buf = CharBuffer(128);
                     kc->gensym(buf);
                     std::string name(buf);
                     obj->setName(Twine(buf));
                     free(buf);
                     return name;
                  }
               } else {
                  return "";
               }
            }
      };
#define USE_INSTRUCTION_CUSTOM_NAME_GENERATION_LOGIC(Type) \
   DECLARE_HAS_CUSTOM_NAME_GENERATION_LOGIC(Type); \
   template<> \
   struct CustomNameGenerationLogic<true> { \
      template<> \
      static std::string getUniqueName< Type >( \
            Type * obj, \
            KnowledgeConstructor* kc) { \
         return GetUniqueName((llvm::Instruction*)obj, kc); \
      } \
   }
   DECLARE_HAS_CUSTOM_ROUTER_LOGIC(llvm::Instruction);
   template<>
      struct CustomRouterLogic<true> {
         template<>
            static std::string route<llvm::Instruction>(llvm::Instruction* obj,
                  KnowledgeConstructor* kc, char* parent) {
               if(llvm::PhiNode* o = dyn_cast<llvm::PhiNode>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::StoreInst* o = dyn_cast<llvm::StoreInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::BinaryOperator* o = dyn_cast<llvm::BinaryOperator>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::CallInst* o = dyn_cast<llvm::CallInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::CmpInst* o = dyn_cast<llvm::CmpInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::GetElementPtrInst* o = dyn_cast<llvm::GetElementPtrInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::LoadInst* o = dyn_cast<llvm::LoadInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::TerminatorInst* o = dyn_cast<llvm::TerminatorInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::SelectInst* o = dyn_cast<llvm::SelectInst>(obj)) {
                  return Route(o, kc, parent);
               } else if(llvm::UnaryInstruction* o = dyn_cast<llvm::UnaryInstruction*>(obj)) {
                  return Route(o, kc, parent);
               } else {
                  return CustomRouterLogic<false>::route(obj, kc, parent);
               }
            }
      };
   /*
    * CmpInst
    */
   DECLARE_CLIPS_TYPE_NAME(llvm::CmpInst, "CompareInstruction");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::CmpInst);   
   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::CmpInst> {
         //TODO: Finish   
      };
   USE_INSTRUCTION_CUSTOM_NAME_GENERATION_LOGIC(llvm::CmpInst);
   DECLARE_HAS_CUSTOM_ROUTER_LOGIC(llvm::CmpInst);

   /*
    * TerminatorInst
    */
   /*
    * UnaryInstruction
    */ 
}
#endif
