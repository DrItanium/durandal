#ifndef _indirect_module_pass_h
#define _indirect_module_pass_h
#include "llvm/ADT/StringRef.h"
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "indirect/IndirectPassHeader.h"
#include "indirect/IndirectPass.h"
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
   class IndirectModulePass : public llvm::ModulePass, public IndirectPass {

      public:
         IndirectModulePass(char& ident);
         using llvm::ModulePass::runOnModule;
         using llvm::ModulePass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnModule(llvm::Module &m) = 0;
   };
   class IndirectFunctionPass : public llvm::FunctionPass, public IndirectPass{
      public:
         IndirectFunctionPass(char& ident);
         using llvm::FunctionPass::runOnFunction;
         using llvm::FunctionPass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnFunction(llvm::Function& fn) = 0;
   };

   class IndirectBasicBlockPass : public llvm::BasicBlockPass, public IndirectPass {
      public:
         IndirectBasicBlockPass(char& ident);
         using llvm::BasicBlockPass::runOnBasicBlock;
         using llvm::BasicBlockPass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnBasicBlock(llvm::BasicBlock& bb) = 0;
   };
   
   class IndirectLoopPass : public llvm::LoopPass, public IndirectPass {
      public:
         IndirectLoopPass(char& ident);
         using llvm::LoopPass::runOnLoop;
         using llvm::LoopPass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnLoop(llvm::Loop* loop, 
               llvm::LPPassManager& lpm) = 0;
   };

   class IndirectMachineFunctionPass : public llvm::MachineFunctionPass, public IndirectPass {
      public:
         IndirectMachineFunctionPass(char& ident);
         using llvm::MachineFunctionPass::runOnMachineFunction;
         using llvm::MachineFunctionPass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnMachineFunction(llvm::MachineFunction& mf) = 0;
   };

   class IndirectRegionPass : public llvm::RegionPass, public IndirectPass {
      public:
         IndirectRegionPass(char& ident);
         using llvm::RegionPass::runOnRegion;
         using llvm::RegionPass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnRegion(llvm::Region* region, 
               llvm::RGPassManager& rgm) = 0;
   };

   class IndirectCallGraphSCCPass : public llvm::CallGraphSCCPass, public IndirectPass {
      public:
         IndirectCallGraphSCCPass(char& ident);
         using llvm::CallGraphSCCPass::runOnSCC;
         using llvm::CallGraphSCCPass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& usage);
         virtual bool runOnSCC(llvm::CallGraphSCC& scc) = 0;
   };

   class PassTypeNotSupported {
      public:
         PassTypeNotSupported(char& id) {
            bool b = false;
            assert(b == true && "Given Pass Type Not Supported");
         }
   };
}

#endif
