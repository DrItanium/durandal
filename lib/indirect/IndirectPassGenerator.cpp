#include "indirect/IndirectPassGenerator.h"

/*
 * OH BOY!!!! 7 template arguments, thankfully yank, paste, and substitute 
 * exist :D.
 */
namespace indirect { 
   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectCallGraphSCCPass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseCallGraphSCCPass(char& id) {
            return new CGSCCP(id);
         }

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectRegionPass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseRegionPass(char& id) {
            return new RP(id);
         }

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectMachineFunctionPass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseMachineFunctionPass(char& id) {
            return new MFP(id);
         }
   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectLoopPass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseLoopPass(char& id) {
            return new LP(id);
         }

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectBasicBlockPass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseBasicBlockPass(char& id) {
            return new BBP(id);
         }

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectFunctionPass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseFunctionPass(char& id) {
            return new FP(id);
         }

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         IndirectModulePass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         generateBaseModulePass(char& id) {
            return new MP(id);
         }

   template<typename MP, typename FP, typename BBP, typename LP, typename RP, 
      typename MFP, typename CGSCCP> 
         llvm::Pass* 
         IndirectPassGeneratorTemplate<MP, FP, BBP, LP, RP, MFP, CGSCCP>::
         createPass(IndirectPassHeader* header) {
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
}
