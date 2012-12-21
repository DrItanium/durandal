#ifndef _indirect_analysis_usage_declaration_h
#define _indirect_analysis_usage_declaration_h
#include "llvm/Pass.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
/* Defines the actions that would normally be provided by getAnalysisUsage but
 * in an indirect manner.
 */
namespace indirect {
   class IndirectAnalysisUsageDeclaration {
      public:
         typedef SmallVector<char*, 32> VectorType;
      private:
         bool pAll;
         bool pCFG;
         VectorType required, requiredTransitive, preserved;
      public:
         IndirectAnalysisUsageDeclaration();
         void addRequired(char* name);
         void addRequired(llvm::StringRef name);
         void addRequiredTransitive(char* name);
         void addRequiredTransitive(llvm::StringRef name);
         void addPreserved(char* name);
         void addPreserved(llvm::StringRef name);
         void setPreservesAll(bool preservesAll);
         void setPreservesCFG(bool preservsCFG);
         bool preservesAll();
         bool preservesCFG();
         void updateAnalysisUsage(llvm::AnalysisUsage& usage);
         void operator() (llvm::AnalysisUsage& usage);
   };
}

#endif
