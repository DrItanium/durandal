#ifndef _indirect_module_pass_h
#define _indirect_module_pass_h
#include "llvm/Support/StringRef.h"
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "indirect/IndirectPassWrapper.h"
/* These classes are extensions of the different passes defined in LLVM
 * Unless you want to implement a backend you should never need to mess with
 * these classes directly. 
 *
 * These classes are abstract and are meant to be inherited from to be used to
 * implement backend base template passes. The logic for what to do when
 * running a module pass is run. It is up to the programmer to provide
 * interaction logic if they want to return information from another language
 * into C++. 
 *
 * It is also up to the implementer to provide mechanims to bundle non native
 * passes together if they wish it. 
 */

namespace indirect {
   class IndirectModulePass : public llvm::ModulePass {

      public:
         IndirectModulePass(char& ident);
         using llvm::ModulePass::runOnModule;
         virtual bool runOnModule(llvm::Module &m) = 0;
   };
   class IndirectFunctionPass : public llvm::FunctionPass {
      public:
         IndirectFunctionPass(char& ident);
         using llvm::FunctionPass::runOnFunction;
         virtual bool runOnFunction(llvm::Function& fn) = 0;
   };

   class IndirectBasicBlockPass : public llvm::BasicBlockPass {
      public:
         IndirectBasicBlockPass(char& ident);
         using llvm::BasicBlockPass:runOnBasicBlock;
         virtual bool runOnBasicBlock(llvm::BasicBlock& bb) = 0;
   };
   
   class LoopPass : public llvm::LoopPass {
      public:
         IndirectLoopPass(char& ident);
         using llvm::LoopPass::runOnLoop;
         virtual bool runOnLoop(llvm::Loop* loop, 
               llvm::LPPassManager& lpm) = 0;
   };

   class IndirectMachineFunctionPass : public llvm::MachineFunctionPass {
      public:
         IndirectMachineFunctionPass(char& ident);
         using llvm::MachineFunctionPass::runOnMachineFunction;
         virtual bool runOnMachineFunction(llvm::MachineFunction& mf) = 0;
   };

   class IndirectRegionPass : public llvm::RegionPass {
      public:
         IndirectRegionPass(char& ident);
         using llvm::RegionPass::runOnRegion;
         virtual bool runOnRegion(llvm::Region* region, 
               llvm::RGPassManager& rgm) = 0;
   };

   class IndirectCallGraphSCCPass : public llvm::CallGraphSCCPass {
      public:
         CallGraphSCCPass(char& ident);
         using llvm::CallGraphSCCPass::runOnSCC;
         virtual bool runOnSCC(llvm::CallGraphSCC& scc) = 0;
   };
}

#endif
