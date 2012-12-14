#ifndef _rampancy_compiler_h
#define _rampancy_compiler_h
/*
 * The rampancy library is a standard way to access compilers and interpreters
 * from within CLIPS. It assumes that you're going to generate a pass
 *
 * This class is abstract
 */
#include <vector>
#include <string.h>
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "llvm/LLVMContext.h"

namespace rampancy {
   class Compiler : public llvm::ModulePass  {
      private:
         CLIPSEnvironment* env;
         llvm::LLVMContext* context;
      public:
         Compiler(char ID);
         virtual ~Compiler();
         virtual llvm::Module* compile(int argc, char** argv) = 0;
         virtual llvm::Module* compile(std::vector<std::string> args) = 0;
         virtual llvm::Module* interpret(char* input) = 0;
         virtual llvm::Module* interpret(std::string input) = 0;
         using llvm::ModulePass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& info);
         virtual void beforeKnowledgeConstruction(llvm::Module* module);
         virtual void afterKnowledgeConstruction(llvm::Module* module);
         using llvm::ModulePass::runOnModule;
         virtual bool runOnModule(llvm::Module* module);
         CLIPSEnvironment* getEnvironment();
         void setEnvironment(CLIPSEnvironment* env);
         llvm::LLVMContext* getLLVMContext();
         void setLLVMContext(llvm::LLVMContext* context);
         //print is optional so I'm not going to mention it
   };
}
#endif
