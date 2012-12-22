#include "llvm/Support/ManagedStatic.h"
#include "llvm/PassRegistry.h"
#include "indirect/IndirectPassRegistry.h"

namespace indirect {
   static llvm::ManagedStatic<IndirectPassRegistry> indirectPassRegistryObject;
   IndirectPassRegistry* IndirectPassRegistry::getIndirectPassRegistry() {
      return &*indirectPassRegistryObject;
   }
   template<typename PassGeneratorClass> 
      IndirectPassGeneratorBase* IndirectPassRegistry::buildPassGenerator() {
         return new PassGeneratorClass();
      }
   const void* IndirectPassRegistry::getUniqueId(const char* name) {
      return (getIndirectPassRegistry()->getIndirectPassInfo(name))->getTypeInfo();
   }
   const void* IndirectPassRegistry::getUniqueId(char* name) {
      return (getIndirectPassRegistry()->getIndirectPassInfo(name))->getTypeInfo();
   }
   const void* IndirectPassRegistry::getUniqueId(llvm::StringRef name) {
      return (getIndirectPassRegistry()->getIndirectPassInfo(name))->getTypeInfo();
   }
   IndirectPassRegistry::IndirectPassRegistry() {

   }

   IndirectPassRegistry::~IndirectPassRegistry() {

   }

   template<typename PassGeneratorClass>
      void IndirectPassRegistry::registerPassGenerator(char* name) {
         registerPassGenerator<PassGeneratorClass>(
               llvm::StringRef((const char*)name));
      }
   template<typename PassGeneratorClass>
      void IndirectPassRegistry::registerPassGenerator(const char* name) {
         registerPassGenerator<PassGeneratorClass>(
               llvm::StringRef(name));
      }
   template<typename PassGeneratorClass>
      void IndirectPassRegistry::registerPassGenerator(llvm::StringRef name) {
         assert((registeredPassGenerators.find(name) == 
                  registeredPassGenerators.end()) && 
               "Pass generator already registered!");
         registeredPassGenerators[name] = 
            IndirectPassRegistry::buildPassGenerator<PassGeneratorClass>();
      }
   void IndirectPassRegistry::registerIndirectPassHeader(
         IndirectPassHeader* header) {
      llvm::StringRef passName(header->getPassName());
      assert((registeredIndirectPasses.find(passName) ==
               registeredIndirectPasses.end()) && 
            "Indirect pass already registered!");
      registeredIndirectPasses[passName] = header;
      //now we need to register with the pass registry
      //first, register it with our indirect passinfo registrar
      RegisterIndirectPass* p = new RegisterIndirectPass(header);
      registeredIndirectPassInfos[passName] = p;
      //then, add it to the pass registry
      llvm::PassRegistry::getPassRegistry()->registerPass(*p);
   }

   void IndirectPassRegistry::unregisterIndirectPassHeader(
         indirect::IndirectPassHeader* header) {
      llvm::StringRef name (header->getPassName());
      unregisterIndirectPassHeader(name);
   }
   void IndirectPassRegistry::unregisterIndirectPassHeader(
         llvm::StringRef name) {
      assert((registeredIndirectPasses.find(name) !=
               registeredIndirectPasses.end()) && "indirect pass does not exist!");
      assert((registeredIndirectPassInfos.find(name) !=
               registeredIndirectPassInfos.end()) && 
            "indirect pass exists but was never registered! o_O no clue how this happened!");
      registeredIndirectPasses.erase(name);
      registeredIndirectPassInfos.erase(name);
      const llvm::PassInfo* pi = 
         llvm::PassRegistry::getPassRegistry()->getPassInfo(name);
      
      llvm::PassRegistry::getPassRegistry()->unregisterPass(*pi);
   }
   void IndirectPassRegistry::unregisterIndirectPassHeader(const char* name) {
      unregisterIndirectPassHeader(llvm::StringRef(name));
   }
   void IndirectPassRegistry::unregisterIndirectPassHeader(char* name) {
      unregisterIndirectPassHeader((const char*)name);
   }
   IndirectPassHeader* IndirectPassRegistry::getIndirectPassHeader(
         char* name) const {
      return getIndirectPassHeader((const char*)name);
   }
   IndirectPassHeader* IndirectPassRegistry::getIndirectPassHeader(
         const char* name) const {
      return getIndirectPassHeader(llvm::StringRef(name));
   }
   IndirectPassHeader* IndirectPassRegistry::getIndirectPassHeader(
         llvm::StringRef name) const {
      assert((registeredIndirectPasses.find(name) !=
               registeredIndirectPasses.end()) && 
            "Given indirect pass does not exist!");
      IndirectPassHeader* header = &*registeredIndirectPasses[name];
      return header;
   }

   llvm::PassInfo* IndirectPassRegistry::getIndirectPassInfo(
         char* name) const {
      return getIndirectPassInfo((const char*)name);
   }

   llvm::PassInfo* IndirectPassRegistry::getIndirectPassInfo(
         const char* name) const {
      return getIndirectPassInfo(llvm::StringRef(name));
   }

   llvm::PassInfo* IndirectPassRegistry::getIndirectPassInfo(
         llvm::StringRef name) const {
      return (llvm::PassInfo*)llvm::PassRegistry::getPassRegistry()->getPassInfo(name);
   }
}
