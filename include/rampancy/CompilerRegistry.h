#ifndef _rampancy_compiler_registry_h
#define _rampancy_compiler_registry_h

#include "llvm/ADT/StringRef.h"
#include "rampancy/Compiler.h"

namespace rampancy {

   class CompilerRegistry {
      mutable void* implementation;
      void* getImpl() const;

      public:
      CompilerRegistry() : implementation(0) { }
      ~CompilerRegistry();
      static CompilerRegistry* getCompilerRegistry();
      Compiler* getCompiler(char* logicalName) const;
      Compiler* getCompiler(llvm::StringRef logicalName) const;
      void registerCompiler(char* logicalName, Compiler* compiler);
      void registerCompiler(llvm::StringRef logicalName, Compiler* compiler);
      void unregisterCompiler(char* logicalName);
      void unregisterCompiler(llvm::StringRef logicalName);
   };
}
#endif
