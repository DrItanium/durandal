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
      public:
         enum IndirectPassType {
            Module,
            Function,
            BasicBlock,
            Loop,
            Region,
            MachineFunction,
            CallGraphSCC,

            /* Adding this field get's rid of the warning that having a comma
             * attached to the last element of an enumerator list is a C++11
             * specific feature. 
             */
            PassTypeCount
         };
      private:
			std::string passDescription; //full name
			std::string passName; //registrationName
			std::string templateSet;
         bool isCFGOnlyPass;
         bool isAnalysis;
         bool isAnalysisGroup;
         IndirectPassType passType;
      public:
         IndirectPassHeader();
			~IndirectPassHeader();
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
         void setTemplateSet(const char* templateSet);
         /*
          * Get the template set that this pass is meant for. Failure to
          * provide this will cause an assertion to occur when attempting to
          * create a pass.
          */
			const char* getTemplateSet();
			void setPassType(char* name);
         void setPassType(IndirectPassHeader::IndirectPassType type);
         IndirectPassHeader::IndirectPassType getPassType();
   };
}

#endif
