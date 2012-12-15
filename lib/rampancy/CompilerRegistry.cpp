#include "rampancy/CompilerRegistry.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Function.h"
#include "llvm/PassSupport.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ManagedStatic.h"

namespace {
   struct CompilerRegistryImpl {
      typedef llvm::StringMap<rampancy::Compiler*> StringMapType;    
      StringMapType compilerStringMap;
   };
}
namespace rampancy {
   //a quick define to make life far easier for me
#define backingRegistry static_cast < CompilerRegistryImpl* > ( implementation )
   static llvm::ManagedStatic<CompilerRegistry> compilerRegistryObject;
   void* CompilerRegistry::getImpl() const {
      if(!implementation) {
         implementation = new CompilerRegistryImpl();
      }
      return implementation;
   }
   CompilerRegistry* CompilerRegistry::getCompilerRegistry() {
      return &*compilerRegistryObject;
   }
   CompilerRegistry::~CompilerRegistry() {
      CompilerRegistryImpl* impl = backingRegistry;

      delete impl;
      implementation = 0;
   }

   Compiler* CompilerRegistry::getCompiler(char* logicalName) const {
      llvm::StringRef name ((const char*)logicalName);
      return getCompiler(name);
   }

   Compiler* CompilerRegistry::getCompiler(llvm::StringRef logicalName) const {
      CompilerRegistryImpl* impl = backingRegistry;
      CompilerRegistryImpl::StringMapType::const_iterator i = 
         impl->compilerStringMap.find(logicalName);
      return i != impl->compilerStringMap.end() ? i->second : 0;
   }

   void CompilerRegistry::registerCompiler(char* logicalName, 
         Compiler* compiler) {
      llvm::StringRef name ((const char*) logicalName);
      registerCompiler(name, compiler);
   }
   void CompilerRegistry::registerCompiler(llvm::StringRef logicalName, 
         Compiler* compiler) {
      CompilerRegistryImpl* impl = backingRegistry;
      assert((impl->compilerStringMap.find(logicalName) == 
             impl->compilerStringMap.end()) && 
            "Logical name already registered to a compiler!");
      impl->compilerStringMap[logicalName] = compiler;
   }
   void CompilerRegistry::unregisterCompiler(char* logicalName) {
      llvm::StringRef name ((const char*)logicalName);
      unregisterCompiler(name);
   }
   void CompilerRegistry::unregisterCompiler(llvm::StringRef logicalName) {
      CompilerRegistryImpl* impl = backingRegistry;
      assert((impl->compilerStringMap.find(logicalName) !=
              impl->compilerStringMap.end()) &&
            "Logical name is not mapped to a registered compiler!");
      impl->compilerStringMap.erase(logicalName);
   }

#undef backingRegistry 
}
