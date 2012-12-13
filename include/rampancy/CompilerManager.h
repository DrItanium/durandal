#ifndef _rampancy_compiler_manager_h
#define _rampancy_compiler_manager_h
/*
 * The rampancy library is a standard way to access compilers and interpreters
 * from within CLIPS. This Compiler assumes that you're going to be generating
 * an llvm::Module internally. This class is also a 
 *
 * This class is abstract
 */
#include <vector.h>
#include <string.h>
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "ExpertSystem/CLIPSEnvironment.h"
#include "rampancy/Compiler.h"
#include "llvm/ADT/DenseMap.h" 
#include "rampancy/Module.h"

namespace rampancy {
   class CompilerManager : public PassManager {
      private:
         llvm::DenseMap<char*, rampancy::Module*> lookup;
      public:
         CompilerManager(CLIPSEnvironment* env);
         CLIPSEnvironment* getEnvironment();
         llvm::Module* compile(char* logicalName, int argc, char** argv);
         llvm::Module* compile(char* logicalName, std::vector<std::string> args);
         llvm::Module* interpret(char* logicalName, char* input);
         llvm::Module* interpret(char* logicalName, std::string input);
         void add(char* logicalName, Compiler* compiler);
         void run(char* logicalName, int argc, char** argv);
         void run(char* logicalName, std::vector<std::string> args);
         void run(char* logicalName, char* input);
         void run(char* logicalName, std::string input);
   };
}
#endif
