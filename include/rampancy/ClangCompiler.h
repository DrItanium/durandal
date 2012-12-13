#ifndef _rampancy_clang_compiler_h
#define _rampancy_clang_compiler_h

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
#include "ExpertSystem/CLIPSEnvironment.h"

using namespace clang;
using namespace clang::driver;

namespace rampancy {
   class ClangCompiler : public Compiler {
      private:
         char* const *envp;
         CLIPSEnvironment* env;
         KnowledgeConstructor* builder;
      public:
         ClangCompiler(CLIPSEnvironment& e, char* const *ep);
         ~Compiler();
         std::string getCompleteKnowledgeString();
         void resetKnowledgeBuilder();
         int execute(llvm::Module* mod, std::vector<std::string>& args, 
               char* functionName);
         int executeMain(llvm::Module* mod, std::vector<std::string>& args);
         virtual llvm::Module* compile(int argc, const char **argv, 
               bool constructKnowledge = true);
   };
}

#endif
