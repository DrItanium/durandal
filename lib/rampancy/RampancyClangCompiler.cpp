#include "rampancy/ClangCompiler.h"

extern "C" {
#include "clips.h"
}

using namespace llvm;
namespace rampancy {
#define msg(a) (char*) a   
   llvm::sys::Path getExecutablePath(const char* argv0) {
      void *MainAddr = (void*) (intptr_t) getExecutablePath;
      return llvm::sys::Path::GetMainExecutable(argv0, MainAddr);
   }
   ClangCompiler::ClangCompiler() : Compiler(ID) { }
   llvm::Module* ClangCompiler::compile() {
      void* theEnv = getEnvironment()->getEnvironment();
      DATA_OBJECT multifield;
      //if we've gotten this far then it's safe to say the routing was
      //successful
      if(!EnvArgTypeCheck(theEnv, msg("compile"), 2, MULTIFIELD, 
               &multifield)) {
         EnvPrintRouter(theEnv, msg("werror"),
               msg("\nThe second argument must be a multifield!\n"));
         return 0;
      }
      unsigned length = (unsigned)GetDOLength(multifield);
      //if we don't have any args then just kick back
      if(length == 0) {
         EnvPrintRouter(theEnv, msg("werror"),
               msg("\nNo arguments provided!\n"));
         return 0;
      }
      char** argv = (char**)calloc(1 + length, sizeof(char*)); 
      int argc = 1 + length;
      argv[0] = (char*)argv0;
      void* mfptr = GetValue(multifield);
      int mfEnd = GetDOEnd(multifield);
      for(int i = GetDOBegin(multifield); i <= mfEnd; ++i) {
         int mfType = GetMFType(mfptr, i);
         if(mfType == STRING || mfType == SYMBOL) {
            //we need to do this to prevent the CLIPS gc from crashing the
            //party. 
            char* tmp = ValueToString(GetMFValue(mfptr, i));
            char* arg = (char*)calloc(strlen(tmp)+1, sizeof(char));
            sprintf(arg, "%s", tmp);
            argv[i] = arg;
         } else if(mfType == INTEGER) {
            char* arg = CharBuffer(128);
            sprintf(arg, "%lld", ValueToLong(GetMFValue(mfptr, i))); 
            argv[i] = arg;
         } else if(mfType == FLOAT) {
            char* arg = CharBuffer(128);
            sprintf(arg, "%f", ValueToDouble(GetMFValue(mfptr, i)));
            argv[i] = arg;
         } else {
            EnvPrintRouter(theEnv, msg("werror"),
                  msg("\nFound an argument that was not a string, symbol, or number\n"));
            return 0;
         }
      }
      //we just create argv ourselves
      llvm::Module* result = compile(argc, argv);
      for(int i = 1; i < argc; ++i) {
         free(argv[i]);
      }
      free(argv);
      return result;
   }

   llvm::Module* ClangCompiler::compile(int argc, char **argv) {
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

      OwningPtr<CodeGenAction> act(new EmitLLVMOnlyAction(getContext()));
      if(!clang.ExecuteAction(*act))
         return 0;
      return act->takeModule();
   }
   llvm::Module* ClangCompiler::interpret() {
      EnvPrintRouter(getEnvironment()->getEnvironment(), 
            msg("werror"),
            msg("\nInterpreting code isn't supported with clang!\n"));
      return 0;
   }
   llvm::Module* ClangCompiler::interpret(llvm::StringRef input) {
      EnvPrintRouter(getEnvironment()->getEnvironment(), 
            msg("werror"),
            msg("\nInterpreting code isn't supported with clang!\n"));
      return 0;
   }
#undef msg

   char ClangCompiler::ID = 0;
   //set it to blank for now
   const char* ClangCompiler::argv0 = "";

   static RegisterPass<ClangCompiler> clangKnowledgeConstructor(
         "clang", 
         "dynamic clang for use with CLIPS", 
         false,
         false);
}
