#include "rampancy/ClangCompiler.h"

extern "C" {
#include "clips.h"
}

namespace rampancy {
   llvm::sys::Path getExecutablePath(const char* argv0) {
      void *MainAddr = (void*) (intptr_t) getExecutablePath;
      return llvm::sys::Path::GetMainExecutable(argv0, MainAddr);
   }
   ClangCompiler::ClangCompiler(CLIPSEnvironment& e, char* const *ep) {
      envp = ep;
      env = &e;
      builder = new KnowledgeConstructor;
   }
   ClangCompiler::~ClangCompiler() {
      if(builder) delete builder;
   }

   void ClangCompiler::resetKnowledgeBuilder() {
      builder->resetInstanceStream();
   }
   int ClangCompiler::execute(llvm::Module* mod, std::vector<std::string>& args, char* functionName) {
      llvm::InitializeNativeTarget();

      std::string error;
      std::vector<std::string>::iterator it;
      it = args.begin();
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
      args.insert(it, mod->getModuleIdentifier());
      return ee->runFunctionAsMain(entryFunction, args, envp);
   }
   int ClangCompiler::executeMain(llvm::Module* mod, std::vector<std::string>& args) {
      return execute(mod, args, "main"); 
   }

   llvm::Module* ClangCompiler::compile(int argc, const char **argv, bool constructKnowledge) {
      resetKnowledgeBuilder();   

      void* mainAddr = (void*) (intptr_t) getExecutablePath;
      llvm::sys::Path path = getExecutablePath(argv[0]);
      TextDiagnosticPrinter* diagClient = new TextDiagnosticPrinter(llvm::errs(), DiagnosticOptions());
      IntrusiveRefCntPtr<DiagnosticIDs> diagID(new DiagnosticIDs());
      DiagnosticsEngine diags (diagID, diagClient);
      Driver theDriver(path.str(), llvm::sys::getDefaultTargetTriple(),
            "a.out", false, diags);
      theDriver.setTitle("rampancy knowledge compiler");

      llvm::SmallVector<const char*, 16> args(argv, argv + argc);
      args.push_back("-fsyntax-only");
      //args.push_back("-S");
      OwningPtr<Compilation> c(theDriver.BuildCompilation(args));
      if(!c) return 0;

      const driver::JobList &jobs = c->getJobs();
      if(jobs.size() != 1 || !isa<driver::Command>(*jobs.begin())) {
         SmallString<256> msg;
         llvm::raw_svector_ostream os(msg);
         c->PrintJob(os, c->getJobs(), "; ", true);
         diags.Report(diag::err_fe_expected_compiler_job) << os.str();
         return 0;
      }

      const driver::Command *cmd = cast<driver::Command>(*jobs.begin());
      if(llvm::StringRef(cmd->getCreator().getName()) != "clang") {
         diags.Report(diag::err_fe_expected_clang_command);
         return 0;
      }

      const driver::ArgStringList &ccArgs = cmd->getArguments();
      OwningPtr<CompilerInvocation> ci(new CompilerInvocation);
      CompilerInvocation::CreateFromArgs(*ci, 
            const_cast<const char **>(ccArgs.data()),
            const_cast<const char **>(ccArgs.data()) +
            ccArgs.size(),
            diags);
      if(ci->getHeaderSearchOpts().Verbose) {
         llvm::errs() << "rampancy compilation:\n";
         llvm::errs() << "  clang invocation:\n";
         c->PrintJob(llvm::errs(), c->getJobs(), "\n", true);
         llvm::errs() << "\n";
      }

      CompilerInstance clang;
      clang.setInvocation(ci.take());

      clang.createDiagnostics(int(ccArgs.size()), 
            const_cast<char**>(ccArgs.data()));
      if(!clang.hasDiagnostics())
         return 0;
      if(clang.getHeaderSearchOpts().UseBuiltinIncludes &&
            clang.getHeaderSearchOpts().ResourceDir.empty()) {
         clang.getHeaderSearchOpts().ResourceDir = 
            CompilerInvocation::GetResourcesPath(argv[0], mainAddr);
      }

      OwningPtr<CodeGenAction> act(new EmitLLVMOnlyAction());
      if(!clang.ExecuteAction(*act))
         return 0;
      llvm::Module* module = act->takeModule();
      if(module && constructKnowledge) {
         builder->route(module);
         std::string result = getCompleteKnowledgeString();
         env->makeInstances((char*)result.c_str());
      } 
      return module;
   }
   std::string ClangCompiler::getCompleteKnowledgeString() {
      return builder->getInstancesAsString();
   }
}

