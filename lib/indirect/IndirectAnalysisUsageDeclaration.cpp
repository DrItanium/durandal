#include "indirect/IndirectAnalysisUsageDeclaration.h"
#include "indirect/IndirectPassRegistry.h"
#include "llvm/PassRegistry.h"

namespace indirect {
   IndirectAnalysisUsageDeclaration::IndirectAnalysisUsageDeclaration() 
      : pAll(false) { }
   IndirectAnalysisUsageDeclaration::~IndirectAnalysisUsageDeclaration() {
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b = 
            preserved.begin(), e = preserved.end(); b != e; ++b) {
         delete (*b);
      }
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b = 
            required.begin(), e = required.end(); b != e; ++b) {
         delete (*b);
      }
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b =
            requiredTransitive.begin(), e = requiredTransitive.end();
            b != e; ++b) {
         delete (*b);
      }
   }
#define STRING_PTR(into,from) std::string* into = new std::string( from )
   void IndirectAnalysisUsageDeclaration::addRequired(char* name) {
      STRING_PTR(tmp, (const char*)name);
      required.push_back(tmp);
   }
   void IndirectAnalysisUsageDeclaration::addRequired(const char* name) {
      STRING_PTR(tmp,name);
      required.push_back(tmp);
   }
   void IndirectAnalysisUsageDeclaration::addRequired(llvm::StringRef name) {
      STRING_PTR(tmp, name.data());
      required.push_back(tmp);
   }

   void IndirectAnalysisUsageDeclaration::addRequiredTransitive(char* name) {
      STRING_PTR(tmp, (const char*)name);
      requiredTransitive.push_back(tmp);
      required.push_back(tmp);
   }
   void IndirectAnalysisUsageDeclaration::addRequiredTransitive(const char* name) {
      STRING_PTR(tmp, name);
      requiredTransitive.push_back(tmp);
      required.push_back(tmp);
   }
   void IndirectAnalysisUsageDeclaration::addRequiredTransitive(
         llvm::StringRef name) {
      STRING_PTR(tmp, name.data());
      requiredTransitive.push_back(tmp);
      required.push_back(tmp);
   }

   void IndirectAnalysisUsageDeclaration::addPreserved(char* name) {
      STRING_PTR(tmp, (const char*)name);
      preserved.push_back(tmp);
   }

   void IndirectAnalysisUsageDeclaration::addPreserved(const char* name) {
      STRING_PTR(tmp, name);
      preserved.push_back(tmp);
   }

   void IndirectAnalysisUsageDeclaration::addPreserved(llvm::StringRef name) {
      STRING_PTR(tmp, name.data());
      preserved.push_back(tmp);
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
         char* result = (char*)(*b)->c_str();
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
         char* result = (char*)(*b)->c_str();
         const llvm::PassInfo* pi = registry.getPassInfo(
               llvm::StringRef(result));
         char& ptr = *((char*)pi->getTypeInfo());
         usage.addRequiredID(ptr);
      }
      for(IndirectAnalysisUsageDeclaration::VectorType::const_iterator b =
            requiredTransitive.begin(), e = requiredTransitive.end();
            b != e; ++b) {
         char* result = (char*)(*b)->c_str();
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
#undef STRING_PTR
}
