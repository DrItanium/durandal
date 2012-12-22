#include "indirect/IndirectPassHeader.h"

namespace indirect {
   IndirectPassHeader::IndirectPassHeader() {

   }

   const char* IndirectPassHeader::getPassDescription() {
      return passDescription;
   }
   void IndirectPassHeader::setPassDescription(const char* passDesc) {
      passDescription = passDesc;
   }

   const char* IndirectPassHeader::getPassName() {
      return passName;
   }

   void IndirectPassHeader::setPassName(const char* passNm) {
      passName = passNm;
   }

   void IndirectPassHeader::setIsCFGOnlyPass(bool isCFGOnly) {
      isCFGOnlyPass = isCFGOnly;
   }

   bool IndirectPassHeader::getIsCFGOnlyPass() {
      return isCFGOnlyPass;
   }

   void IndirectPassHeader::setIsAnalysis(bool isAnalysisPass) {
      isAnalysis = isAnalysisPass;
   }

   bool IndirectPassHeader::getIsAnalysis() {
      return isAnalysis;
   }

   void IndirectPassHeader::setIsAnalysisGroup(bool _isAnalysisGroup) {
      isAnalysisGroup = _isAnalysisGroup;
   }

   bool IndirectPassHeader::getIsAnalysisGroup() {
      return isAnalysisGroup;
   }
}
