#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "llvm/Module.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/TargetSelect.h"

#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/FunctionNamer.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "rampancy/RampancyCompiler.h"

extern "C" {
#include "clips.h"
}
using namespace clang;
using namespace clang::driver;

namespace rampancy {
   llvm::sys::Path getExecutablePath(const char* argv0) {
      void *MainAddr = (void*) (intptr_t) getExecutablePath;
      return llvm::sys::Path::GetMainExecutable(argv0, MainAddr);
   }
   Compiler::Compiler(CLIPSEnvironment& e, char* arg0, char* const *ep) {
      argv0 = arg0;
      envp = ep;
      env = &e;
      mainAddr = (void*) (intptr_t) getExecutablePath;
      path = getExecutablePath(argv0);
      diagClient = new TextDiagnosticPrinter(llvm::errs(), DiagnosticOptions());
      diagID = new IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs());
      diags = new DiagnosticEngine(diagID, diagClient);
      theDriver = new Driver(path.str(), llvm::sys::getDefaultTargetTriple(),
            "a.out", false, *diags);
      theDriver->setTitle("rampancy knowledge compiler");
      builder = new KnowledgeConstructor;
   }
   Compiler::~Compiler() {
      delete builder;
      delete theDriver;
      delete diags;
      delete diagClient;
      delete diagID;
   }

   void Compiler::resetKnowledgeBuilder() {
      builder->resetInstanceStream();
   }
   int Compiler::execute(llvm::Module* mod, std::vector<std::string>& args, char* functionName) {
         std::string error;
         OwningPtr<llvm::ExecutionEngine> ee(
               llvm::ExecutionEngine::createJIT(mod, &error));
         if(!ee) {
            llvm::errs() << "unable to make execution engine: " << error << '\n';
            return 255;
         }
         llvm::Function* entryFunction = mod->getFunction(functionName);
         if(!entryFunction) {
            llvm::errs() << functionName << " function not found in module.\n";
            return 255;
         }
         args.push_front(Mod->getModuleIdentifier());
         return ee->runFunctionAsMain(entryFunction, args, envp);
   }
   int Compiler::executeMain(llvm::Module* mod, std::vector<std::string>& args) {
      return executeMain(mod, args, "main"); 
   }

   llvm::Module* Compiler::compile(int argc, const char **argv) {
      llvm::Module* result;
      resetKnowledgeBuilder();   
      llvm::SmallVector<const char*, 16> args(argv, argv + argc);
      args.push_back("-fsyntax-only");
      OwningPtr<Compilation> c(theDriver->BuildCompilation(args));
      if(!c) return 0;

      const driver::JobList &jobs = c->getJobs();
      if(jobs.size() != 1 || !isa<driver::Command>(*jobs.begin())) {
         SmallString<256> msg;
         llvm::raw_svector_ostream os(msg);
         c->PrintJob(os, c->getJobs(), "; ", true);
         Diags->Report(diag::err_fe_expected_compiler_job) << os.str();
         return 0;
      }

      const driver::Command *cmd = cast<driver::Command>(*jobs.begin());
      if(llvm::StringRef(cmd->getCreator().getName()) != "clang") {
         diags->report(diags::err_fe_expected_clang_command);
         return 0;
      }

      const driver::ArgStringList &ccArgs = cmd->getArguments();
      OwningPtr<CompilerInvocation> ci(new CompilerInvocation);
      CompilerInvocation::CreateFromArgs(*ci, 
                                     const_cast<const char **>(ccArgs.data()),
                                     const_cast<const char **>(ccArgs.data()) +
                                       ccArgs.size(),
                                     *diags);
      if(ci->getHeaderSearchOpts().Verbose) {
         llvm::errs() << "rampancy compilation:\n";
         llvm::errs() << "  clang invocation:\n";
         c->PrintJob(llvm::errs(), c->getJobs(), "\n", true);
         llvm::errs() << "\n";
      }

      CompilerInstance clang;
      clang.setInvocation(ci.take());
      clang.createDiagnostics(int(ccArgs.size()), 
            const_cast<char**>(ccAargs.data()));
      if(!clang.hasDiagnostics())
         return 0;
      if(clang.getHeaderSearchOpts().UseBuiltinIncludes &&
            clang.getHeaderSearchOpts().ResourceDir.empty()) {
         clang.getHeaderSearchOpts().ResourceDir = 
            CompilerInvocation::GetResourcesPath(argv[0], mainAddr);
      }

      OwningPtr<CodeGenAction> act(new EmitLLVMOnlyAction());
      if(!clang.ExecuteAction(*act))
         return 1;
      return act->takeModule();
   }
   void Compiler::compileToKnowledge(int argc, const char **argv, 
         bool getRegions, bool getLoops) {
      llvm::Module mod;
      mod = compile(argc, argv);
      if(mod) {
         builder->route(mod, getRegions, getLoops);
         return 1;
      }  else {
         return 0;
      }
   }
   std::string Compiler::getCompleteKnowledgeString() {
      return builder->getInstancesAsString();
   }
}

