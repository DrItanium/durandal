#ifndef _clips_pass_templates_h
#define _clips_pass_templates_h
#include "indirect/IndirectPasses.h"
#include "pipeline/clips/CLIPSPass.h"
using namespace indirect;
namespace pipeline {
   namespace clips {
      class CLIPSModulePass : public IndirectModulePass, public CLIPSPass {
         public:
            CLIPSModulePass(char& id) : IndirectModulePass(id) { }
            using IndirectModulePass::runOnModule;
            virtual bool runOnModule(llvm::Module& module);
      };
      class CLIPSFunctionPass : public IndirectFunctionPass, public CLIPSPass {
         public:
            CLIPSFunctionPass(char& id) : IndirectFunctionPass(id) { }
            using IndirectFunctionPass::runOnFunction;
            virtual bool runOnFunction(llvm::Function& fn);
      };
      class CLIPSBasicBlockPass : public IndirectBasicBlockPass, public CLIPSPass {
         public:
            CLIPSBasicBlockPass(char& id) : IndirectBasicBlockPass(id) { }
            using IndirectBasicBlockPass::runOnBasicBlock;
            virtual bool runOnBasicBlock(llvm::BasicBlock& bb);
      };
      class CLIPSLoopPass : public IndirectLoopPass, public CLIPSPass {
         public:
            CLIPSLoopPass(char& id) : IndirectLoopPass(id) { }
            using IndirectLoopPass::runOnLoop;
            virtual bool runOnLoop(llvm::Loop* l, llvm::LPPassManager& lpm);
      };
      class CLIPSRegionPass : public IndirectRegionPass, public CLIPSPass {
         public:
            CLIPSRegionPass(char& id) : IndirectRegionPass(id) { }
            using IndirectRegionPass::runOnRegion;
            virtual bool runOnRegion(llvm::Region* r, llvm::RGPassManager& rg);
      };
      class CLIPSMachineFunctionPass : public IndirectMachineFunctionPass, public CLIPSPass {
         public:
            CLIPSMachineFunctionPass(char& id) : IndirectMachineFunctionPass(id) { }
            using IndirectMachineFunctionPass::runOnMachineFunction;
            virtual bool runOnMachineFunction(llvm::MachineFunction& mf);
      };
      class CLIPSCallGraphSCCPass : public IndirectCallGraphSCCPass, public CLIPSPass {
         public:
            CLIPSCallGraphSCCPass(char& id) : IndirectCallGraphSCCPass(id) { }
            using IndirectCallGraphSCCPass::runOnSCC;
            virtual bool runOnSCC(llvm::CallGraphSCC& scc);
      };
   }
}

#endif
