#ifndef _rampancy_llvm_ir_compiler_h
#define _rampancy_llvm_ir_compiler_h

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
#include "llvm/Support/IRReader.h"

#include "ExpertSystem/KnowledgeConstructor.h"
#include "ExpertSystem/FunctionNamer.h"
#include "ExpertSystem/KnowledgeConstructionEngine.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "rampancy/Compiler.h"

namespace rampancy {
   
   class LLVMIRCompiler : public Compiler {
      public:
         LLVMIRCompiler(); 
         using Compiler::compile;
         using Compiler::interpret;
         virtual llvm::Module* compile(); 
         virtual llvm::Module* compile(int argc, char** argv);
         llvm::Module* compile(const std::string &fileName, 
               llvm::SMDiagnostic &err);
         virtual llvm::Module* interpret();
         virtual llvm::Module* interpret(llvm::StringRef input);
   };
}

#endif
