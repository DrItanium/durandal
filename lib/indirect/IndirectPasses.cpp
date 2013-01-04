#include "indirect/IndirectPasses.h"

namespace indirect {
   IndirectModulePass::IndirectModulePass(char& ident) 
      : llvm::ModulePass(ident) {

      }
   void IndirectModulePass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }

   IndirectFunctionPass::IndirectFunctionPass(char& ident) 
      : llvm::FunctionPass(ident) {

      }
   void IndirectFunctionPass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }

   IndirectBasicBlockPass::IndirectBasicBlockPass(char& ident) 
      : llvm::BasicBlockPass(ident) {

      }
   void IndirectBasicBlockPass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }
   IndirectRegionPass::IndirectRegionPass(char& ident) 
      : llvm::RegionPass(ident) {

      }
   void IndirectRegionPass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }
   IndirectLoopPass::IndirectLoopPass(char& ident) 
      : llvm::LoopPass(ident) {

      }
   void IndirectLoopPass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }
   IndirectMachineFunctionPass::IndirectMachineFunctionPass(char& ident) 
      : llvm::MachineFunctionPass(ident) {

      }
   void IndirectMachineFunctionPass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }
   IndirectCallGraphSCCPass::IndirectCallGraphSCCPass(char& ident) 
      : llvm::CallGraphSCCPass(ident) {

      }
   void IndirectCallGraphSCCPass::getAnalysisUsage(llvm::AnalysisUsage& usage) const {
      applyIndirectAnalysisUsage(usage);
   }
}
