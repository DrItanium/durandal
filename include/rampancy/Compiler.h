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

namespace rampancy {
   class Compiler : public llvm::ModulePass  {
      private:
         CLIPSEnvironment* env;
      public:
         Compiler(char ID);
         virtual ~Compiler();
         virtual llvm::Module* compile(CLIPSEnvironment* theEnv, int argc, char** argv) = 0;
         virtual llvm::Module* compile(CLIPSEnvironment* theEnv, std::vector<std::string> args) = 0;
         virtual llvm::Module* interpret(CLIPSEnvironment* theEnv, char* input) = 0;
         virtual llvm::Module* interpret(CLIPSEnvironment* theEnv, std::string input) = 0;
         using llvm::ModulePass::getAnalysisUsage;
         virtual void getAnalysisUsage(llvm::AnalysisUsage& info);
         virtual void beforeKnowledgeConstruction(llvm::Module* module, CLIPSEnvironment* theEnv);
         virtual void afterKnowledgeConstruction(llvm::Module* module, CLIPSEnvironment* theEnv);
         using llvm::ModulePass::runOnModule;
         virtual bool runOnModule(llvm::Module* module);
         virtual bool runOnModule(llvm::Module* module, CLIPSEnvironment* theEnv);
         CLIPSEnvironment* getManagingEnvironment();
         void setManagingEnvironment(CLIPSEnvironment* env);
         //print is optional so I'm not going to mention it
   };
}
#endif
