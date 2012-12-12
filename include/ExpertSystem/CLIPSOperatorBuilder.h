#ifndef _operator_construction_h
#define _operator_construction_h
#include "llvm/Operator.h"
#include "ExpertSystem/CLIPSUserBuilder.h"

using namespace llvm;
class CLIPSOperatorBuilder : public CLIPSUserBuilder {
   public:
      CLIPSOperatorBuilder(std::string nm, std::string ty, FunctionNamer& namer); 
      void addFields(Operator* v, KnowlegeConstructor* kc, char* parent);
		void build(Operator* v, KnowlegeConstructor* kc, char* parent);
};
class CLIPSOverflowingBinaryOperatorBuilder : public CLIPSOperatorBuilder {
   public:
      CLIPSOverflowingBinaryOperatorBuilder(std::string nm, FunctionNamer& namer);
      void addFields(OverflowingBinaryOperator* v, KnowlegeConstructor* kc, char* parent);
		void build(OverflowingBinaryOperator* v, KnowlegeConstructor* kc, char* parent);
};
class CLIPSPossiblyExactOperatorBuilder : public CLIPSOperatorBuilder {
   public:
      CLIPSPossiblyExactOperatorBuilder(std::string nm, FunctionNamer& namer);
      void addFields(PossiblyExactOperator* v, KnowlegeConstructor* kc, char* parent);
		void build(PossiblyExactOperator* v, KnowlegeConstructor* kc, char* parent);
};

/* for some reason this doesn't exist and as such I have commented it out
 * - Joshua Scoggins 5/20/2012 @ 1909
 *
class CLIPSFPMathOperatorBuilder : public CLIPSOperatorBuilder
{
   public:
      CLIPSFPMathOperatorBuilder(std::string nm, FunctionNamer& namer) : CLIPSOperatorBuilder(nm, "FPMathOperator", namer) { }
      void addFields(FPMathOperator* v, char* parent)
      {
         CLIPSOperatorBuilder::addFields((Operator*)v, parent);
         addField("FPAccuracy", v->getFPAccuracy());
      }
};
*/
#endif
