#include "rampancy/Compiler.h"
namespace rampancy {
   llvm::Module* Compiler::compile(CLIPSEnvironment* env, int argc, char** argv) {
      return compile(env->getEnvironment(), argc, argv);
   }
   llvm::Module* Compiler::compile(CLIPSEnvironment* env, std::vector<std::string> args) {
      return compile(env->getEnvironment(), args);
   }
   llvm::Module* Compiler::interpret(CLIPSEnvironment* env, char* input) {
      return interpret(env->getEnvironment(), input);
   }
   llvm::Module* Compiler::interpret(CLIPSEnvironemnt* env, std::string input) {
      return interpret(env->getEnvironment(), input);
   }
}
