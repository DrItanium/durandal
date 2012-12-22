#ifndef _indirect_pass_header_h
#define _indirect_pass_header_h
#include "indirect/IndirectAnalysisUsageDeclaration.h"
#include "indirect/IndirectUniqueIdentifier.h"
/* An indirect pass header is a magical and wonderful class that contains a
 * wide assortment of information ranging from the name of a pass and it's
 * description to what it requires 
 */
namespace indirect {
   class IndirectPassHeader : public IndirectAnalysisUsageDeclaration, 
   public IndirectUniqueIdentifier {
      private:
         const char* passDescription; //full name
         const char* passName; //registrationName
         bool isCFGOnlyPass;
         bool isAnalysis;
         bool isAnalysisGroup;
      public:
         IndirectPassHeader();
         const char* getPassDescription();
         void setPassDescription(const char* description);
         const char* getPassName();
         void setPassName(const char* name);
         void setIsCFGOnlyPass(bool isCFGOnly);
         bool getIsCFGOnlyPass();
         void setIsAnalysis(bool isAnalysisPass);
         bool getIsAnalysis();
         void setIsAnalysisGroup(bool _isAnalysisGroup);
         bool getIsAnalysisGroup();
   };
}

#endif
