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

}
