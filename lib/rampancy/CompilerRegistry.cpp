#include "rampancy/CompilerRegistry.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Function.h"
#include "llvm/PassSupport.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ManagedStatic.h"

namespace rampancy {
   //a quick define to make life far easier for me
   static llvm::ManagedStatic<CompilerRegistry> compilerRegistryObject;
   CompilerRegistry* CompilerRegistry::getCompilerRegistry() {
      return &*compilerRegistryObject;
   }
   CompilerRegistry::~CompilerRegistry() {
   }

   Compiler* CompilerRegistry::getCompiler(char* logicalName) const {
      llvm::StringRef name ((const char*)logicalName);
      return getCompiler(name);
   }

   Compiler* CompilerRegistry::getCompiler(llvm::StringRef logicalName) const {
      StringMapType::const_iterator i = 
         compilerStringMap.find(logicalName);
      return i != compilerStringMap.end() ? i->second : 0;
   }

   void CompilerRegistry::registerCompiler(char* logicalName, 
         Compiler* compiler) {
      llvm::StringRef name ((const char*) logicalName);
      registerCompiler(name, compiler);
   }
   void CompilerRegistry::registerCompiler(llvm::StringRef logicalName, 
         Compiler* compiler) {
      assert((compilerStringMap.find(logicalName) == 
             compilerStringMap.end()) && 
            "Logical name already registered to a compiler!");
      compilerStringMap[logicalName] = compiler;
   }
   void CompilerRegistry::unregisterCompiler(char* logicalName) {
      llvm::StringRef name ((const char*)logicalName);
      unregisterCompiler(name);
   }
   void CompilerRegistry::unregisterCompiler(llvm::StringRef logicalName) {
      assert((compilerStringMap.find(logicalName) !=
              compilerStringMap.end()) &&
            "Logical name is not mapped to a registered compiler!");
      compilerStringMap.erase(logicalName);
   }

}
