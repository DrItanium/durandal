#include "indirect/IndirectPassHeader.h"

namespace indirect {
   IndirectPassHeader::IndirectPassHeader() : isAnalysis(false), 
   isAnalysisGroup(false), isCFGOnlyPass(false) {

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

   void IndirectPassHeader::setTemplateSet(const char* templateSt) {
      templateSet = templateSt;
   }

   const char* IndirectPassHeader::getTemplateSet() {
      return templateSet;
   }

   void IndirectPassHeader::setPassType(
         IndirectPassHeader::IndirectPassType type) {
      passType = type;
   }

   IndirectPassHeader::IndirectPassType getPassType() {
      return passType;
   }

}
