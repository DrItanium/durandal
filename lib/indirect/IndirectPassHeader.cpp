#include "indirect/IndirectPassHeader.h"

namespace indirect {
   IndirectPassHeader::IndirectPassHeader() : isAnalysis(false), 
   isAnalysisGroup(false), isCFGOnlyPass(false) {

   }

	IndirectPassHeader::~IndirectPassHeader() {
	}

   const char* IndirectPassHeader::getPassDescription() {
      return passDescription.c_str();
   }
	void IndirectPassHeader::setPassDescription(const char* passDesc) {
		std::string tmp(passDesc);
		passDescription = tmp;
	}

   const char* IndirectPassHeader::getPassName() {
      return passName.c_str();
   }
	void IndirectPassHeader::setPassName(const char* passNm) {
		std::string tmp(passNm);
		passDescription = tmp;
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
		std::string tmp (templateSet);
		templateSet = tmp;
   }

   const char* IndirectPassHeader::getTemplateSet() {
      return templateSet.c_str();
   }

   void IndirectPassHeader::setPassType(
         IndirectPassHeader::IndirectPassType type) {
      passType = type;
   }

   IndirectPassHeader::IndirectPassType IndirectPassHeader::getPassType() {
      return passType;
   }

}
