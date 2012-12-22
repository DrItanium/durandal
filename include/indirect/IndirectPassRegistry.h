#ifndef _indirect_pass_registry_h
#define _indirect_pass_registry_h
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "indirect/IndirectPassHeader.h"
#include "indirect/RegisterIndirectPass.h"
#include "indirect/IndirectPassGenerator.h"

namespace indirect {
   class IndirectPassRegistry {
      public:
         typedef llvm::StringMap<indirect::IndirectPassHeader*> PassHeaderMapType;
         typedef llvm::StringMap<indirect::RegisterIndirectPass*> RegisterIndirectPassMapType;
         typedef llvm::StringMap<indirect::IndirectPassGeneratorBase*> PassGeneratorMapType;
      private:
         PassHeaderMapType registeredIndirectPasses;
         RegisterIndirectPassMapType registeredIndirectPassInfos;
         PassGeneratorMapType registeredPassGenerators;
      public:
         IndirectPassRegistry();
         ~IndirectPassRegistry(); 
         static IndirectPassRegistry* getIndirectPassRegistry();
         template<typename PassGeneratorClass> 
         static IndirectPassGeneratorBase* buildPassGenerator();
         static const void* getUniqueId(const char* name);
         static const void* getUniqueId(llvm::StringRef name);
         static const void* getUniqueId(char* name);
         template<typename PassGeneratorClass>
         void registerPassGenerator(char* name);
         template<typename PassGeneratorClass>
         void registerPassGenerator(const char* name);
         template<typename PassGeneratorClass>
         void registerPassGenerator(llvm::StringRef name);
         void registerIndirectPassHeader(indirect::IndirectPassHeader* header);
         void unregisterIndirectPassHeader(indirect::IndirectPassHeader* header);
         void unregisterIndirectPassHeader(char* name);
         void unregisterIndirectPassHeader(const char* name);
         void unregisterIndirectPassHeader(llvm::StringRef name);
         IndirectPassHeader* getIndirectPassHeader(char* name) const;
         IndirectPassHeader* getIndirectPassHeader(const char* name) const;
         IndirectPassHeader* getIndirectPassHeader(llvm::StringRef name) const;
         llvm::PassInfo* getIndirectPassInfo(char* name) const;
         llvm::PassInfo* getIndirectPassInfo(const char* name) const;
         llvm::PassInfo* getIndirectPassInfo(llvm::StringRef name) const;
         llvm::Pass* createPass(char* name);
         llvm::Pass* createPass(const char* name);
         llvm::Pass* createPass(llvm::StringRef name);
   };
}
#endif
