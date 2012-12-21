#include "indirect/IndirectAnalysisUsageDeclaration.h"
#include "indirect/IndirectPassRegistry.h"
#include "llvm/PassRegistry.h"

namespace indirect {
   IndirectAnalysisUsageDeclaration::IndirectAnalysisUsageDeclaration() 
      : pAll(false) { }

   void IndirectAnalysisUsageDeclaration::addRequired(char* name) {
      required.push_back(name);
   }
   void IndirectAnalysisUsageDeclaration::addRequired(llvm::StringRef name) {
      required.push_back((char*)name.data());
   }

   void IndirectAnalysisUsageDeclaration::addRequiredTransitive(char* name) {
      requiredTransitive.push_back(name);
   }

   void IndirectAnalysisUsageDeclaration::addRequiredTransitive(
         llvm::StringRef name) {
      requiredTransitive.push_back((char*)name.data());
      required.push_back((char*)name.data());
   }

   void IndirectAnalysisUsageDeclaration::addPreserved(char* name) {
      preserved.push_back(name);
   }

   void IndirectAnalysisUsageDeclaration::addPreserved(llvm::StringRef name) {
      preserved.push_back((char*)name.data());
   }
   void IndirectAnalysisUsageDeclaration::setPreservesAll(bool preservesAll) {
      pAll = preservesAll;
   }
   void IndirectAnalysisUsageDeclaration::setPreservesCFG(bool preservesCFG) {
      pCFG = preservesCFG;
   }
   bool IndirectAnalysisUsageDeclaration::preservesAll() {
      return pAll;
   }
   bool IndirectAnalysisUsageDeclaration::preservesCFG() {
      return pCFG;
   }

   /* provided for lazyness as well as a layer of indirection so that changes 
    * to updateAnalysisUsage or operator() does not require * tons of changes 
    * throughout the library if I have to rewrite things.
    */
   void IndirectAnalysisUsageDeclaration::operator() 
      (llvm::AnalysisUsage& usage) {
         updateAnalysisUsage(usage);
      }
   void IndirectAnalysisUsageDeclaration::updateAnalysisUsage(
         llvm::AnalysisUsage& usage) {
      llvm::PassRegistry& registry = *(llvm::PassRegistry::getPassRegistry());
      /* iterate through our sets of names and translate them to be added to 
       * the llvm::AnalysisUsage object.
       */
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b = 
            preserved.begin(), e = preserved.end(); b != e; ++b) {
         char* result = *b;
         /* Strangly enough addPreserved in the llvm::AnalysisUsage class has a
          * version that takes in a llvm::StringRef
          */
         usage.addPreserved(llvm::StringRef(result));
      }
      /* We can access the llvm::PassRegistry to get the indirect passes 
       * because they are registerd with the llvm::PassRegistry when the
       * IndirectPassHeader is registered with the IndirectPassRegistry
       */
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b = 
            required.begin(), e = required.end(); b != e; ++b) {
         char* result = *b;
         const llvm::PassInfo* pi = registry.getPassInfo(
               llvm::StringRef(result));
         char& ptr = *((char*)pi->getTypeInfo());
         usage.addRequiredID(ptr);
      }
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b =
            requiredTransitive.begin(), e = requiredTransitive.end();
            b != e; ++b) {
         char* result = *b;
         const llvm::PassInfo* pi = registry.getPassInfo(
               llvm::StringRef(result));
         char& ptr = *((char*)pi->getTypeInfo());
         usage.addRequiredTransitiveID(ptr);
      }
      /* now we check the preservesAll and preservesCFG flags to see if we need
       * to call those flags
       */
      if(pAll) {
         usage.setPreservesAll();
      }
      if(pCFG) {
         usage.setPreservesCFG();
      }
      //And we're done :D
   }
}
