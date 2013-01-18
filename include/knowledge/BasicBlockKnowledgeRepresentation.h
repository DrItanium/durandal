#ifndef _basic_block_knowledge_representation_h
#define _basic_block_knowledge_representation_h
#include "knowledge/Traits.h"
#include "knowledge/Types.h"
#include "llvm/BasicBlock.h"

using namespace llvm;
namespace knowledge {
   DECLARE_CLIPS_TYPE_NAME(llvm::BasicBlock, "BasicBlock");
   DECLARE_HAS_KNOWLEDGE_REPRESENTATION_POPULATION_LOGIC(llvm::BasicBlock);

   template<>
      struct KnowledgeRepresentationPopulationLogic<llvm::BasicBlock> {
         static void populateKnowledgeRepresentation(llvm::BasicBlock* bb,
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            populateKnowledgeRepresentation(bb, krb, kc, false);
         }
         static void populateKnowledgeRepresentation(llvm::BasicBlock* bb, 
               KnowledgeRepresentationBuilder* krb,
               KnowledgeConstructor* kc) {
            PopulateKnowledgeRepresentation((llvm::Value*)bb, krb, kc);
            char* name = (char*)krb->getName().c_str();
            char* parent = (char*)krb->getParent().c_str();
            if(bb->isLandingPad()) {
               krb->addTrueField("IsLandingPad");
            }

            if(bb->hasAddressTaken()) {
               krb->addTrueField("HasAddressTaken");
            }
            //only provide predecessors and successors if we aren't in a basic
            //block pass
            //if(!kc->isBasicBlockPass()) {
            pred_iterator pi = pred_begin(bb), 
                          pe = pred_end(bb);
            if(pi != pe) {
               krb->openMultifield("Predecessors");
               for(; pi != pe; ++pi) {
                  BasicBlock* pred = *pi;
                  //we can fixup the parents at a later time if necessary
                  krb->appendValue(Route(pred, kc, parent));
               }
               krb->closeMultifield();
            }
            succ_iterator si = succ_begin(bb), 
                          se = succ_end(bb);
            if(si != se) {
               krb->openMultifield("Successors");
               for(; si != se; ++si) {
                  BasicBlock* succ = *si;
                  PointerAddress pa = (PointerAddress)succ;
                  //we can fixup the parents at a later time if necessary
                  krb->appendValue(Route(succ, kc, parent));
               }
               krb->closeMultifield();
            }
            //}
            if(!bb->empty()) {
               std::string tmp;
               raw_string_ostream data(tmp);
               krb->openMultifield("contents");
               for(BasicBlock::iterator i = bb->begin(), 
                     e = bb->end(); i != e; ++i) {
                  Instruction* inst = i;
                  std::string res = Route(inst, kc, name);
                  krb->appendValue(res);
                  data << " " << res;
               }
               krb->closeMultifield();
               krb->addField("Produces", data.str()); 
            }
         }
      };
}

extern "C" void DefineBasicBlockInteractionFunctions(void* theEnv);

#endif
