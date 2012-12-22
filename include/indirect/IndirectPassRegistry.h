#ifndef _indirect_pass_registry_h
#define _indirect_pass_registry_h
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "indirect/IndirectPassHeader.h"
#include "indirect/RegisterIndirectPass.h"
#include "indirect/IndirectPassGenerator.h"

/* 
 * In order to keep track of indirect passes it's necessary to define a new
 * PassRegistry that keeps track of extra information while interacting with
 * llvm::PassRegistry automatically.
 */
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
         template<class PassGeneratorClass> 
            static IndirectPassGeneratorBase* buildPassGenerator() {
               return new PassGeneratorClass();
            }
         static const void* getUniqueId(const char* name);
         static const void* getUniqueId(llvm::StringRef name);
         static const void* getUniqueId(char* name);
         template<class PassGeneratorClass>
            void registerPassGenerator(char* name) {
               registerPassGenerator<PassGeneratorClass>(
                     llvm::StringRef((const char*)name));
            }
         template<class PassGeneratorClass>
            void registerPassGenerator(const char* name) {
               registerPassGenerator<PassGeneratorClass>(
                     llvm::StringRef(name));
            }
         template<class PassGeneratorClass>
            void registerPassGenerator(llvm::StringRef name) {
               assert((registeredPassGenerators.find(name) == 
                        registeredPassGenerators.end()) && 
                     "Pass generator already registered!");
               registeredPassGenerators[name] = 
                  IndirectPassRegistry::buildPassGenerator<PassGeneratorClass>();
            }
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
