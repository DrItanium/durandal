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
#include "rampancy/Compiler.h"

using namespace clang;
using namespace clang::driver;

namespace rampancy {
   class ClangCompiler : public Compiler {
      public:
         static char ID;
         //HACK HACK!
         //Used to get around the fact that we may not have immediate access to
         //arg0. Thus we need to register this at program startup
         static const char* argv0;
         ClangCompiler(); 
         using Compiler::compile;
         using Compiler::interpret;
         virtual llvm::Module* compile(); 
         virtual llvm::Module* compile(int argc, char** argv);
         virtual llvm::Module* interpret();
         virtual llvm::Module* interpret(llvm::StringRef input);
   };
}

#endif
