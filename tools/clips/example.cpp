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
#include "rampancy/Compiler.h"
#include "rampancy/CompilerManager.h"
#include "rampancy/CompilerRegistry.h"
#include "rampancy/Cortex.h"
#include "rampancy/ClangCompiler.h"
using namespace llvm;
extern "C" {
#include <stdio.h>
#include "setup.h"
#include "clips.h"
}

extern "C" void UserFunctions(void);
extern "C" void EnvUserFunctions(void *);
int main(int argc, char** argv) {

   //set argv[0] for clang
   rampancy::ClangCompiler::argv0 = argv[0];
   rampancy::Cortex* rampantCortex = rampancy::Cortex::getRampantCortex();
   rampancy::CompilerRegistry* compilerRegistry = 
      rampancy::CompilerRegistry::getCompilerRegistry();
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
   rampancy::initializeClangCompilerPass(registry);
   rampancy::ClangCompiler cc;
   //set the 
   StringRef clangName("clang");
   compilerRegistry->registerCompiler(clangName, &cc);

   RerouteStdin(rampantCortex->getEnvironment()->getEnvironment(),argc,argv);
   CommandLoop(rampantCortex->getEnvironment()->getEnvironment());
}