#include "indirect/IndirectPassHeader.h"

namespace indirect {
   static std::string empty("");
   IndirectPassHeader::IndirectPassHeader() : isAnalysis(false), 
   isAnalysisGroup(false), isCFGOnlyPass(false) {
      passDescription = 0;
      passName = 0;
      templateSet = 0;
   }

   IndirectPassHeader::~IndirectPassHeader() {
      if(passDescription) {
         delete passDescription;
      } 
      if(passName) {
         delete passName;
      }
      if(templateSet) {
         delete templateSet;
      }
   }

   const char* IndirectPassHeader::getPassDescription() {
      if(passDescription) {
         return passDescription->c_str();
      } else {
         return empty.c_str();
      }
   }
   void IndirectPassHeader::setPassDescription(const char* passDesc) {
      if(passDescription) {
         delete passDescription;
      }
      passDescription = new std::string(passDesc);
   }

   const char* IndirectPassHeader::getPassName() {
      if(passName) {
         return passName->c_str();
      } else {
         return empty.c_str();
      }
   }
   void IndirectPassHeader::setPassName(const char* passNm) {
      if(passName) {
         //totally safe since we make a copy :D
         delete passName;
      }
      passName = new std::string(passNm);
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
      if(templateSet) {
         delete templateSet;
      }
      templateSet = new std::string(templateSt);
   }

   const char* IndirectPassHeader::getTemplateSet() {
      if(templateSet) {
         return templateSet->c_str();
      } else {
         return empty.c_str();
      }
   }

   void IndirectPassHeader::setPassType(
         IndirectPassHeader::IndirectPassType type) {
      passType = type;
   }

   IndirectPassHeader::IndirectPassType IndirectPassHeader::getPassType() {
      return passType;
   }

}
