#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <memory>
#include <algorithm>
#include "indirect/Indirector.h"
#include "rampancy/ClangCompiler.h"
using namespace llvm;
using namespace indirect; //Can't take it anymore
class TestModulePassTemplate : public indirect::IndirectModulePass {
   public:
      TestModulePassTemplate(char& ident) 
         : indirect::IndirectModulePass(ident) { }
      virtual bool runOnModule(llvm::Module& module) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a module pass and has been run\n";
         return false;
      }
};
class TestFunctionPassTemplate : public indirect::IndirectFunctionPass {
   public:
      TestFunctionPassTemplate(char& ident) 
         : indirect::IndirectFunctionPass(ident) { }
      virtual bool runOnFunction(llvm::Function& fn) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a function pass and has been run\n";
         return false;
      }
};
class TestBasicBlockPassTemplate : public indirect::IndirectBasicBlockPass {
   public:
      TestBasicBlockPassTemplate(char& ident) 
         : indirect::IndirectBasicBlockPass(ident) { }
      virtual bool runOnBasicBlock(llvm::BasicBlock& fn) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a basic block pass and has been run\n";
         return false;
      }
};
class TestRegionPassTemplate : public indirect::IndirectRegionPass {
   public:
      TestRegionPassTemplate(char& ident) 
         : indirect::IndirectRegionPass(ident) { }
      virtual bool runOnRegion(llvm::Region* region, RGPassManager& rgm) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a region pass and has been run\n";
         return false;
      }
};
class TestLoopPassTemplate : public indirect::IndirectLoopPass {
   public:
      TestLoopPassTemplate(char& ident) 
         : indirect::IndirectLoopPass(ident) { }
      virtual bool runOnLoop(llvm::Loop* fn, llvm::LPPassManager& lpm) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a loop pass and has been run\n";
         return false;
      }
};

class TestMachineFunctionPassTemplate : public indirect::IndirectMachineFunctionPass {
   public:
      TestMachineFunctionPassTemplate(char& ident) 
         : indirect::IndirectMachineFunctionPass(ident) { }
      virtual bool runOnMachineFunction(llvm::MachineFunction& fn) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a MachineFunction pass and has been run\n";
         return false;
      }
};

class TestCallGraphSCCPassTemplate : public indirect::IndirectCallGraphSCCPass {
   public:
      TestCallGraphSCCPassTemplate(char& ident) 
         : indirect::IndirectCallGraphSCCPass(ident) { }
      virtual bool runOnSCC(llvm::CallGraphSCC& fn) {
         llvm::errs() << getIndirectPassHeader()->getPassName() <<
            " is a CallGraphSCC pass and has been run\n";
         return false;
      }
};

class TestPassGenerator : public indirect::IndirectPassGeneratorTemplate<
                          TestModulePassTemplate,
                          TestFunctionPassTemplate,
                          TestBasicBlockPassTemplate,
                          TestLoopPassTemplate,
                          TestRegionPassTemplate,
                          TestMachineFunctionPassTemplate,
                          TestCallGraphSCCPassTemplate> {
                          };
int main(int argc, char** argv) {
   /*
    * This program will compile and run a file on the target module
    */
   LLVMContext* context = new LLVMContext(); 
   rampancy::ClangCompiler clang;
   clang.setContext(context);
   PassRegistry &registry = *PassRegistry::getPassRegistry();
   initializeCore(registry);
   initializeScalarOpts(registry);
   initializeVectorization(registry);
   initializeIPO(registry);
   initializeAnalysis(registry);
   initializeIPA(registry);
   initializeTransformUtils(registry);
   initializeInstCombine(registry);
   initializeInstrumentation(registry);
   initializeTarget(registry);
   /*
    * Register the template set with the IndirectPassRegistry
    */
   IndirectPassRegistry& indirectRegistry = *IndirectPassRegistry::getIndirectPassRegistry();
   indirectRegistry.registerPassGenerator<TestPassGenerator>("test");
   /*
    * Create some fake passes to see if the registration mechanism works
    */
   indirect::IndirectPassHeader fakeModulePass, fakeFunctionPass,
      fakeBasicBlockPass, fakeRegionPass, fakeCallGraphSCCPass, 
      fakeLoopPass, fakeModulePass2, fakeFunctionPass2;
   fakeModulePass.setPassDescription("A test fake indirect module pass");
   fakeModulePass.setPassName("test-indirect-module-pass");
   fakeModulePass.setIsCFGOnlyPass(false);
   fakeModulePass.setIsAnalysis(false);
   fakeModulePass.setIsAnalysisGroup(false);
   fakeModulePass.setTemplateSet("test");
   fakeModulePass.setPassType(IndirectPassHeader::Module);
   indirectRegistry.registerIndirectPassHeader(&fakeModulePass);
   fakeModulePass2.setPassDescription("A second test fake indirect module pass");
   fakeModulePass2.setPassName("test-indirect-module-pass2");
   fakeModulePass2.setIsCFGOnlyPass(false);
   fakeModulePass2.setIsAnalysis(false);
   fakeModulePass2.setIsAnalysisGroup(false);
   fakeModulePass2.setTemplateSet("test");
   fakeModulePass2.setPassType(IndirectPassHeader::Module);
   indirectRegistry.registerIndirectPassHeader(&fakeModulePass2);
   fakeFunctionPass.setPassDescription("A test fake indirect function pass");
   fakeFunctionPass.setPassName("test-indirect-function-pass");
   fakeFunctionPass.setIsCFGOnlyPass(false);
   fakeFunctionPass.setIsAnalysis(false);
   fakeFunctionPass.setIsAnalysisGroup(false);
   fakeFunctionPass.setTemplateSet("test");
   fakeFunctionPass.setPassType(IndirectPassHeader::Function);
   indirectRegistry.registerIndirectPassHeader(&fakeFunctionPass);
   fakeFunctionPass2.setPassDescription("A second test fake indirect function pass");
   fakeFunctionPass2.setPassName("test-indirect-function-pass2");
   fakeFunctionPass2.setIsCFGOnlyPass(false);
   fakeFunctionPass2.setIsAnalysis(false);
   fakeFunctionPass2.setIsAnalysisGroup(false);
   fakeFunctionPass2.setTemplateSet("test");
   fakeFunctionPass2.setPassType(IndirectPassHeader::Function);
   fakeFunctionPass2.addRequired("loops");
   fakeFunctionPass2.addRequired("regions");
   indirectRegistry.registerIndirectPassHeader(&fakeFunctionPass2);
   fakeBasicBlockPass.setPassDescription("A test fake indirect basic block pass");
   fakeBasicBlockPass.setPassName("test-indirect-basic-block-pass");
   fakeBasicBlockPass.setIsCFGOnlyPass(false);
   fakeBasicBlockPass.setIsAnalysis(false);
   fakeBasicBlockPass.setIsAnalysisGroup(false);
   fakeBasicBlockPass.setTemplateSet("test");
   fakeBasicBlockPass.setPassType(IndirectPassHeader::BasicBlock);
   indirectRegistry.registerIndirectPassHeader(&fakeBasicBlockPass);
   fakeRegionPass.setPassDescription("A test fake indirect region pass");
   fakeRegionPass.setPassName("test-indirect-region-pass");
   fakeRegionPass.setIsCFGOnlyPass(false);
   fakeRegionPass.setIsAnalysis(false);
   fakeRegionPass.setIsAnalysisGroup(false);
   fakeRegionPass.setTemplateSet("test");
   fakeRegionPass.setPassType(IndirectPassHeader::Region);
   indirectRegistry.registerIndirectPassHeader(&fakeRegionPass);
   fakeCallGraphSCCPass.setPassDescription("A test fake indirect scc pass");
   fakeCallGraphSCCPass.setPassName("test-indirect-scc-pass");
   fakeCallGraphSCCPass.setIsCFGOnlyPass(false);
   fakeCallGraphSCCPass.setIsAnalysis(false);
   fakeCallGraphSCCPass.setIsAnalysisGroup(false);
   fakeCallGraphSCCPass.setTemplateSet("test");
   fakeCallGraphSCCPass.setPassType(IndirectPassHeader::CallGraphSCC);
   indirectRegistry.registerIndirectPassHeader(&fakeCallGraphSCCPass);
   fakeLoopPass.setPassDescription("A test fake indirect loop pass");
   fakeLoopPass.setPassName("test-indirect-loop-pass");
   fakeLoopPass.setIsCFGOnlyPass(false);
   fakeLoopPass.setIsAnalysis(false);
   fakeLoopPass.setIsAnalysisGroup(false);
   fakeLoopPass.setTemplateSet("test");
   fakeLoopPass.setPassType(IndirectPassHeader::Loop);
   indirectRegistry.registerIndirectPassHeader(&fakeLoopPass);

   llvm::Module* module = clang.compile(argc, argv);
   PassManagerBuilder builder;
   PassManager tmpPassManager;
   TargetLibraryInfo *tli = 
      new TargetLibraryInfo(Triple(module->getTargetTriple()));
   tmpPassManager.add(tli);
   TargetData *td = 0;
   const std::string &moduleDataLayout = module->getDataLayout();
   if(!moduleDataLayout.empty())
      td = new TargetData(moduleDataLayout);
   if(td)
      tmpPassManager.add(td);
   PassManager& PM = tmpPassManager;
   //add em all!
   builder.OptLevel = 0;
   builder.DisableSimplifyLibCalls = false;
   builder.populateModulePassManager(PM);
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-module-pass"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-function-pass"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-basic-block-pass"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-region-pass"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-loop-pass"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-scc-pass"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-module-pass2"));
   tmpPassManager.add(indirectRegistry.createPass("test-indirect-function-pass2"));
   tmpPassManager.add(llvm::createVerifierPass());
   tmpPassManager.run(*module);
}
