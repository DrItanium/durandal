#ifndef _indirect_pass_registry_h
#define _indirect_pass_registry_h
#include "llvm/Support/StringRef.h"
#include "llvm/Support/DenseMap.h"
#include "llvm/Support/StringMap.h"
#include "indirect/IndirectPassHeader.h"
#include "indirect/RegisterIndirectPass.h"
#include "indirect/IndirectPassGenerator.h"

namespace indirect {
   class IndirectPassRegistry {
         typedef llvm::StringMap<indirect::IndirectPassHeader*> PassHeaderMapType;
         typedef llvm::StringMap<indirect::RegisterIndirectPass*> RegisterIndirectPassMapType;
         typedef llvm::StringMap<indirect::IndirectPassGeneratorBase*> PassGeneratorMapType;
         PassHeaderMapType registeredIndirectPasses;
         RegisterIndirectPassMapType registeredIndirectPassInfos;
         PassGeneratorMapType registeredPassGenerators;
      public:
         IndirectPassRegistry() { }
         ~IndirectPassRegistry(); 
         static IndirectPassRegistry* getIndirectPassRegistry();
         template<typename PassGeneratorClass> 
         static IndirectPassGeneratorBase* buildPassGenerator() {
            return new PassGeneratorClass();
         }
         static const void* getUniqueId(const char* name) {
            return (getIndirectPassRegistry()->getIndirectPassInfo(name))->getTypeInfo();
         }
         static const void* getUniqueId(llvm::StringRef name) {
            return (getIndirectPassRegistry()->getIndirectPassInfo(name))->getTypeInfo();
         }
         static const void* getUniqueId(char* name) {
            return (getIndirectPassRegistry()->getIndirectPassInfo(name))->getTypeInfo();
         }
         template<typename PassGeneratorClass>
         void registerPassGenerator(char* name);
         template<typename PassGeneratorClass>
         void registerPassGenerator(const char* name);
         template<typename PassGeneratorClass>
         void registerPassGenerator(llvm::StringRef name);
         void registerIndirectPassHeader(IndirectPassHeader* header);
         void unregisterIndirectPassHeader(IndirectPassHeader* header);
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
