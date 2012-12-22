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

class TestPassGenerator : public indirect::IndirectPassGeneratorTemplate<
                          TestModulePassTemplate,
                          TestFunctionPassTemplate,
                          TestBasicBlockPassTemplate,
                          indirect::PassTypeNotSupported,
                          indirect::PassTypeNotSupported,
                          indirect::PassTypeNotSupported,
                          indirect::PassTypeNotSupported> { 
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
   indirect::IndirectPassHeader fakeModulePass, fakeFunctionPass, fakeBasicBlockPass;
   fakeModulePass.setPassDescription("A test fake indirect module pass");
   fakeModulePass.setPassName("test-indirect-module-pass");
   fakeModulePass.setIsCFGOnlyPass(false);
   fakeModulePass.setIsAnalysis(false);
   fakeModulePass.setIsAnalysisGroup(false);
   fakeModulePass.setTemplateSet("test");
   fakeModulePass.setPassType(IndirectPassHeader::Module);
   indirectRegistry.registerIndirectPassHeader(&fakeModulePass);
   fakeFunctionPass.setPassDescription("A test fake indirect function pass");
   fakeFunctionPass.setPassName("test-indirect-function-pass");
   fakeFunctionPass.setIsCFGOnlyPass(false);
   fakeFunctionPass.setIsAnalysis(false);
   fakeFunctionPass.setIsAnalysisGroup(false);
   fakeFunctionPass.setTemplateSet("test");
   fakeFunctionPass.setPassType(IndirectPassHeader::Function);
   indirectRegistry.registerIndirectPassHeader(&fakeFunctionPass);
   fakeBasicBlockPass.setPassDescription("A test fake indirect basic block pass");
   fakeBasicBlockPass.setPassName("test-indirect-basic-block-pass");
   fakeBasicBlockPass.setIsCFGOnlyPass(false);
   fakeBasicBlockPass.setIsAnalysis(false);
   fakeBasicBlockPass.setIsAnalysisGroup(false);
   fakeBasicBlockPass.setTemplateSet("test");
   fakeBasicBlockPass.setPassType(IndirectPassHeader::BasicBlock);
   indirectRegistry.registerIndirectPassHeader(&fakeBasicBlockPass);

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
   builder.OptLevel = 2;
   builder.DisableSimplifyLibCalls = false;
   builder.populateModulePassManager(PM);
   Pass* mP = indirectRegistry.createPass("test-indirect-module-pass");
   Pass* fP = indirectRegistry.createPass("test-indirect-function-pass");
   Pass* bbP = indirectRegistry.createPass("test-indirect-basic-block-pass");
   tmpPassManager.add(mP);
   tmpPassManager.add(fP);
   tmpPassManager.add(bbP);
   tmpPassManager.add(llvm::createVerifierPass());
   tmpPassManager.run(*module);
   delete mP;
   delete fP;
   delete bbP;
}
