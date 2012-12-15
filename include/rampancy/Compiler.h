#ifndef _rampancy_compiler_h
#define _rampancy_compiler_h
/*
 * The rampancy library is a standard way to access compilers and interpreters
 * from within CLIPS. It assumes that you're going to generate a pass
 *
 * This class is abstract
 */
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "llvm/LLVMContext.h"
#include "llvm/ADT/StringRef.h"

namespace rampancy {
   class Compiler : public llvm::ModulePass  {
      private:
         CLIPSEnvironment* env;
         llvm::LLVMContext* context;
      public:
         Compiler(char ID);
         virtual ~Compiler();
         //this one gets arguments straight from CLIPS
         virtual llvm::Module* compile() = 0;
         virtual llvm::Module* compile(int argc, char** argv) = 0;
         virtual llvm::Module* interpret() = 0;
         virtual llvm::Module* interpret(llvm::StringRef input) = 0;
         using llvm::ModulePass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& info);
         virtual void beforeKnowledgeConstruction(llvm::Module* module);
         virtual void afterKnowledgeConstruction(llvm::Module* module);
         using llvm::ModulePass::runOnModule;
         virtual bool runOnModule(llvm::Module* module);
         CLIPSEnvironment* getEnvironment();
         void setEnvironment(CLIPSEnvironment* env);
         llvm::LLVMContext* getContext();
         void setContext(llvm::LLVMContext* context);
   };
}
#endif
