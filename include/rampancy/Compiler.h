#ifndef _rampancy_compiler_h
#define _rampancy_compiler_h
/*
 * The rampancy library is a standard way to access compilers and interpreters
 * from within CLIPS. It assumes that you're going to generate a pass
 *
 * This class is abstract
 */
#include <vector.h>
#include <string.h>
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "rampancy/AnalysisUsage.h"

namespace rampancy {
   class Compiler : public ModulePass  {
      //we need to add a way to add a code callback
      public:
         Compiler(char ID);
         virtual ~Compiler();
         virtual llvm::Module* compile(void* theEnv, int argc, char** argv) = 0;
         virtual llvm::Module* compile(void* theEnv, std::vector<std::string> args) = 0;
         virtual llvm::Module* interpret(void* theEnv, char* input) = 0;
         virtual llvm::Module* interpret(void* theEnv, std::string input) = 0;
         virtual llvm::Module* compile(CLIPSEnvironment* theEnv, int argc, char** argv);
         virtual llvm::Module* compile(CLIPSEnvironment* theEnv, std::vector<std::string> args);
         virtual llvm::Module* interpret(CLIPSEnvironment* theEnv, char* input);
         virtual llvm::Module* interpret(CLIPSEnvironment* theEnv, std::string input);
         virtual bool runOnModule(llvm::Module* module);
         virtual bool runOnModule(void* theEnv, llvm::Module* module) = 0;
         //print is optional so I'm not going to mention it
   };
}
#endif
