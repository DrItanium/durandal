#ifndef _constant_construction_h
#define _constant_construction_h
#include <string.h>
#include "ExpertSystem/CLIPSUserBuilder.h"
#include "llvm/Constant.h"
#include "llvm/Constants.h"
#include "llvm/GlobalVariable.h"
#include "llvm/GlobalValue.h"
#include "llvm/GlobalAlias.h"

using namespace llvm;
class CLIPSConstantBuilder : public CLIPSUserBuilder {
   public:
      CLIPSConstantBuilder(std::string nm, std::string ty, FunctionNamer& namer);
      void addFields(Constant* cnst, KnowledgeConstructor* kc, char* parent);
		void build(Constant* cnst, KnowledgeConstructor* kc, char* parent);
};

class CLIPSBlockAddressBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSBlockAddressBuilder(std::string nm, FunctionNamer& namer);
      void addFields(BlockAddress* addr, KnowledgeConstructor* kc, char* parent);
		void build(BlockAddress* cnst, KnowledgeConstructor* kc, char* parent);
};
class CLIPSConstantAggregateZeroBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantAggregateZeroBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantAggregateZero* addr, KnowledgeConstructor* kc, char* parent);
		void build(ConstantAggregateZero* cnst, KnowledgeConstructor* kc, char* parent);
};
class CLIPSConstantArrayBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantArrayBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantArray* addr, KnowledgeConstructor* kc, char* parent); 
		void build(ConstantArray* cnst, KnowledgeConstructor* kc, char* parent);
};

class CLIPSConstantExpressionBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantExpressionBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantExpr* addr, KnowledgeConstructor* kc, char* parent);
		void build(ConstantExpr* cnst, KnowledgeConstructor *kc, char* parent);
};


class CLIPSConstantFloatingPointBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantFloatingPointBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantFP* addr, KnowledgeConstructor* kc, char* parent);
		void build(ConstantFP* cnst, KnowledgeConstructor* kc, char* parent);
};

class CLIPSConstantIntegerBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantIntegerBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantInt* addr, KnowledgeConstructor* kc, char* parent);
		void build(ConstantInt* cnst, KnowledgeConstructor* kc, char* parent);
};

class CLIPSConstantPointerNullBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantPointerNullBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantPointerNull* addr, KnowledgeConstructor *kc, char* parent);
		void build(ConstantPointerNull* cnst, KnowledgeConstructor *kc, char* parent);
};

class CLIPSConstantStructBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantStructBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantStruct* addr, KnowledgeConstructor* kc, char* parent);
		void build(ConstantStruct* cnst, KnowledgeConstructor* kc, char* parent);
};

class CLIPSConstantVectorBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSConstantVectorBuilder(std::string nm, FunctionNamer& namer);
      void addFields(ConstantVector* addr, KnowledgeConstructor* kc, char* parent);
		void build(ConstantVector* cnst, KnowledgeConstructor *kc, char* parent);
};

class CLIPSGlobalValueBuilder : public CLIPSConstantBuilder
{
   public:
      CLIPSGlobalValueBuilder(std::string nm, std::string ty, FunctionNamer& namer);
      void addFields(GlobalValue* addr, KnowledgeConstructor *kc, char* parent);
		void build(GlobalValue* cnst, KnowledgeConstructor *kc, char* parent);
};


class CLIPSGlobalAliasBuilder : public CLIPSGlobalValueBuilder {
   public:
      CLIPSGlobalAliasBuilder(std::string nm, FunctionNamer& namer);
      void addFields(GlobalAlias* addr, KnowledgeConstructor *kc, char* parent);
		void build(GlobalAlias* cnst, KnowledgeConstructor *kc, char* parent);
};

class CLIPSGlobalVariableBuilder : public CLIPSGlobalValueBuilder {
   public:
      CLIPSGlobalVariableBuilder(std::string nm, FunctionNamer& namer); 
      void addFields(GlobalVariable* addr, KnowledgeConstructor *kc, char* parent);
		void build(GlobalVariable* cnst, KnowledgeConstructor *kc, char* parent);
};

class CLIPSUndefValueBuilder : public CLIPSConstantBuilder {
   public:
      CLIPSUndefValueBuilder(std::string nm, FunctionNamer& namer);
      void addFields(UndefValue* addr, KnowledgeConstructor *kc, char* parent);
		void build(UndefValue* cnst, KnowledgeConstructor *kc, char* parent);
};

/*
 * This does not exist in my version of LLVM so...yeah it's commented out for
 * the time being
 * -Joshua Scoggins 5/20/2012 @ 1746
 class CLIPSBinaryConstantExpressionBuilder : public CLIPSConstantExpressionBuilder
 {
 public:
 CLIPSBinaryConstantExpressionBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantExpressionBuilder(nm, "BinaryConstantExpression", namer) { }
 void addFields(BinaryConstantExpr* addr, char* parent)
 {
 CLIPSConstantBuilder::addFields((Constant*)addr, parent); 
 }
 void imbueKnowledge(BinaryConstantExpr* addr)
 {
 CLIPSConstantBuilder::imbueKnowledge((Constant*)addr);
 }
 };
 class CLIPSCompareConstantExpressionBuilder : public CLIPSConstantExpressionBuilder
 {
 public:
 CLIPSCompareConstantExpressionBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantExpressionBuilder(nm, "CompareConstantExpression", namer) { }
 void addFields(CompareConstantExpr* addr, char* parent)
 {
 CLIPSConstantBuilder::addFields((Constant*)addr, parent); 
 addField("ActualPredicate", addr->predicate);
 }
 void imbueKnowledge(CompareConstantExpr* addr)
 {
 CLIPSConstantBuilder::imbueKnowledge((Constant*)addr);
 }
 };
 */
/* ConstantDataSequential doesn't exist in llvm 3.0 :/
 * But I have a feeling it is in newer versions of LLVM so I'm keeping the code
 * here for those newer versions
 * - Joshua Scoggins 5/20/2012 @ 1726 PST
 class CLIPSConstantDataSequentialBuilder : public CLIPSConstantBuilder
 {
 public:
 CLIPSConstantDataSequentialBuilder(std::string nm, FunctionNamer& namer) : CLIPSConstantBuilder(nm, "ConstantDataSequential", namer) { }
 void addFields(ConstantDataSequential* addr, char* parent);
 void imbueKnowledge(ConstantDataSequential* addr)
 {
 CLIPSConstantBuilder::imbueKnowledge((Constant*)addr);
 send("put-ElementType", Route(addr->getElementType()));
 }
 };
*/
#endif
