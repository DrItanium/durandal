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
               using indirect::IndirectPassGeneratorBase::createPass;
               virtual llvm::Pass* createPass(IndirectPassHeader* header);
               IndirectModulePass* generateBaseModulePass(char& id);
               IndirectFunctionPass* generateBaseFunctionPass(char& id);
               IndirectBasicBlockPass* generateBaseBasicBlockFunctionPass(char& id);
               IndirectLoopPass* generateBaseLoopPass(char& id);
               IndirectMachineFunctionPass* generateBaseMachineFunctionPass(char& id);
               IndirectRegionPass* generateBaseRegionPass(char& id);
               IndirectCallGraphSCCPass* generateBaseCallGraphSCCPass(char& id);
         };
}
#endif
