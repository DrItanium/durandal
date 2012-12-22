#ifndef _indirect_pass_generator_h
#define _indirect_pass_generator_h
#include "llvm/Pass.h"
#include "indirect/IndirectPassHeader.h"
#include "indirect/IndirectPasses.h"
namespace indirect {
   class IndirectPassGeneratorBase {
      public:
         virtual llvm::Pass* createPass(IndirectPassHeader* header) = 0;
   };

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         class IndirectPassGeneratorTemplate : public IndirectPassGeneratorBase {
            public:
               IndirectPassGeneratorTemplate() { }
               IndirectCallGraphSCCPass* generateBaseCallGraphSCCPass(char& id) {
                  return new CGSCCP(id);
               }

               IndirectRegionPass* generateBaseRegionPass(char& id) {
                  return new RP(id);
               }

               IndirectMachineFunctionPass* generateBaseMachineFunctionPass(char& id) {
                  return new MFP(id);
               }

               IndirectLoopPass* generateBaseLoopPass(char& id) {
                  return new LP(id);
               }

               IndirectBasicBlockPass* generateBaseBasicBlockPass(char& id) {
                  return new BBP(id);
               }

               IndirectFunctionPass* generateBaseFunctionPass(char& id) {
                  return new FP(id);
               }

               IndirectModulePass* generateBaseModulePass(char& id) {
                  return new MP(id);
               }
               using indirect::IndirectPassGeneratorBase::createPass;

               virtual llvm::Pass* createPass(IndirectPassHeader* header) {
                  /*
                   * the first thing to do is to determine what kind of a pass this is
                   * supposed to be. 
                   */
                  char& id = *(header->getID());
                  switch(header->getPassType()) {
                     case IndirectPassHeader::Module: 
                        {
                           IndirectModulePass* pass = generateBaseModulePass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     case IndirectPassHeader::Function: 
                        {
                           IndirectFunctionPass* pass = generateBaseFunctionPass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     case IndirectPassHeader::BasicBlock:
                        {
                           IndirectBasicBlockPass* pass = 
                              generateBaseBasicBlockPass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     case IndirectPassHeader::Loop:
                        {
                           IndirectLoopPass* pass = generateBaseLoopPass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     case IndirectPassHeader::Region:
                        {
                           IndirectRegionPass* pass = generateBaseRegionPass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     case IndirectPassHeader::MachineFunction:
                        {
                           IndirectMachineFunctionPass* pass = 
                              generateBaseMachineFunctionPass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     case IndirectPassHeader::CallGraphSCC:
                        {
                           IndirectCallGraphSCCPass* pass = 
                              generateBaseCallGraphSCCPass(id);
                           pass->setIndirectPassHeader(header);
                           return pass;
                        }
                     default: 
                        return 0;
                  }
               }
         };
}
#endif
