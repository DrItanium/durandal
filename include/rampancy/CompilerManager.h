/*
 * The CompilerManager is a custom pass manager that handles both dynamic
 * compilation as well as knowledge construction in addition to being a
 * standard pass manager. 
 *
 * It also provides a standard way to register compilers with CLIPS through the
 * IO router system. This makes it really easy to add new compilers without any
 * complex issues. 
 *
 */
#ifndef _rampancy_compiler_manager_h
#define _rampancy_compiler_manager_h

#include <vector.h>
#include <string.h>
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "rampancy/Compiler.h"
#include "llvm/ADT/DenseMap.h" 

namespace rampancy {
   class CompilerManager : public llvm::PassManagerBase {
      private:
      protected:
         extern "C" void registerCompilerWithCLIPS(char* logicalName, Compiler* compiler);
      public:
         CompilerManager(CLIPSEnvironment* env, char* logicalName);
         CLIPSEnvironment* getEnvironment();
         char* getLogicalName();
         llvm::Module* compile(char* logicalName, int argc, char** argv);
         llvm::Module* compile(char* logicalName, std::vector<std::string> args);
         llvm::Module* interpret(char* logicalName, char* input);
         llvm::Module* interpret(char* logicalName, std::string input);
         void add(char* logicalName, Compiler* compiler);
         //provided so I can pull in analysis passes
         //we use a bit of a trick where we provide an extended
         //RampancyAnalysisUsage object which is generated by this
         void run(char* logicalName, int argc, char** argv);
         void run(char* logicalName, std::vector<std::string> args);
         void run(char* logicalName, char* input);
         void run(char* logicalName, std::string input);
         void run(llvm::Module* module);
   };
}
#endif
