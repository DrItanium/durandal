#include "llvm/Support/ManagedStatic.h"
#include "llvm/PassRegistry.h"
#include "indirect/IndirectPassRegistry.h"

namespace indirect {
   static llvm::ManagedStatic<IndirectPassRegistry> indirectPassRegistryObject;
   IndirectPassRegistry* IndirectPassRegistry::getIndirectPassRegistry() {
      return &*indirectPassRegistryObject;
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
      //And we're done since RegisterIndirectPass already does the registration
      //with the llvm PassRegistry
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
      IndirectPassRegistry::PassHeaderMapType::const_iterator i = 
         registeredIndirectPasses.find(name);
      return i != registeredIndirectPasses.end() ? 
         i->second : 0;
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

   llvm::Pass* IndirectPassRegistry::createPass(char* name) {
      return createPass((const char*)name);
   }

   llvm::Pass* IndirectPassRegistry::createPass(const char* name) {
      return createPass(llvm::StringRef(name));
   }

   llvm::Pass* IndirectPassRegistry::createPass(llvm::StringRef name) {
      /* 
       * first thing to do is get the corresponding indirect pass header.
       * We will be using this to determine which template set to use.
       */
      IndirectPassHeader* i = getIndirectPassHeader(name);
      assert(i != 0 && "Indirect pass does not exist!");
      /*
       * now we grab the corresponding pass generator.
       * This will be used to create the corresponding pass.
       */
      llvm::StringRef templateName (i->getTemplateSet());
      IndirectPassRegistry::PassGeneratorMapType::const_iterator j = 
         registeredPassGenerators.find(templateName);
      assert(j != registeredPassGenerators.end() && 
            "Target template set does not exist!");
      IndirectPassGeneratorBase* base = j->second;
      /*
       * Use that generator to create a pass. 
       */
      return base->createPass(i);
   }
}
