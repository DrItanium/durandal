#include "ExpertSystem/CLIPSTypeBuilder.h"
CLIPSTypeBuilder::CLIPSTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSObjectBuilder(nm, ty, namer) { }
void CLIPSTypeBuilder::addFields(Type* type, KnowledgeConstructor* kc) {
   FunctionNamer& namer = getNamer();
   PointerAddress pointer = (PointerAddress)type;
   if(!namer.pointerRegistered(pointer)) {
      std::pair<PointerAddress, std::string> pair (pointer, getName());
      namer.getTranslationTable().insert(pair);
   }
   setPointer(pointer);
   if(type->isVoidTy()) addTrueField("IsVoidType");
   if(type->isFunctionTy()) addTrueField("IsFunctionType"); 
   if(type->isFPOrFPVectorTy()) {
      addTrueField("IsFPOrFPVectorType");
      if(type->isFloatingPointTy()) addTrueField("IsFloatingPointType");
      if(type->isFloatTy()) addTrueField("IsFloatType");
      if(type->isDoubleTy()) addTrueField("IsDoubleType");
      if(type->isX86_FP80Ty()) addTrueField("IsX86FP80Type"); 
      if(type->isFP128Ty()) addTrueField("IsFP128Type");
      if(type->isPPC_FP128Ty()) addTrueField("IsPPCFP128Type");
      addField("FPMantissaWidth", type->getFPMantissaWidth());
   }
   if(type->isX86_MMXTy()) addTrueField("IsX86MMXType");
   if(type->isLabelTy()) addTrueField("IsLabelType");
   if(type->isMetadataTy()) addTrueField("IsMetadataType");
   if(type->isIntOrIntVectorTy()) {
      addTrueField("IsIntOrIntVectorType");
      if(type->isIntegerTy()) addTrueField("IsIntegerType");
   }
   if(type->isStructTy()) addTrueField("IsStructType"); 
   if(type->isArrayTy()) addTrueField("IsArrayType"); 
   if(type->isPointerTy()) addTrueField("IsPointerType"); 
   if(type->isVectorTy()) {
      addTrueField("IsVectorType"); 
      addField("ScalarSizeInBits", type->getScalarSizeInBits());
   }
   if(type->isEmptyTy()) addTrueField("IsEmptyType"); 
   if(type->isPrimitiveType()) {
      addTrueField("IsPrimitiveType");
      addField("PrimitiveSizeInBits", type->getPrimitiveSizeInBits());
   }
   if(type->isDerivedType()) addTrueField("IsDerivedType"); 
   if(type->isFirstClassType()) addTrueField("IsFirstClassType");
   if(type->isSingleValueType()) addTrueField("IsSingleValueType"); 
   if(type->isAggregateType()) addTrueField("IsAggregateType");
   if(type->isSized()) addTrueField("IsSized");
   if(type->getNumContainedTypes() > 0) {
      openField("Subtypes");
      for(Type::subtype_iterator i = type->subtype_begin(),
            e = type->subtype_end(); i != e; ++i) {
         Type* t = (*i);
         appendInstanceName(kc->route(t, namer));
      }
      closeField();
   }
}
void CLIPSTypeBuilder::build(Type* st, KnowledgeConstructor* kc) {
   open();
   addFields(st, kc);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)st, str);
}

CLIPSFunctionTypeBuilder::CLIPSFunctionTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSTypeBuilder(nm, namer, ty) { }
CLIPSCompositeTypeBuilder::CLIPSCompositeTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSTypeBuilder(nm, namer, ty) { }
CLIPSSequentialTypeBuilder::CLIPSSequentialTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSCompositeTypeBuilder(nm, namer, ty) { }
CLIPSArrayTypeBuilder::CLIPSArrayTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSSequentialTypeBuilder(nm, namer, ty) { }
CLIPSPointerTypeBuilder::CLIPSPointerTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSSequentialTypeBuilder(nm, namer, ty) { }
CLIPSVectorTypeBuilder::CLIPSVectorTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSSequentialTypeBuilder(nm, namer, ty) { } 
CLIPSStructTypeBuilder::CLIPSStructTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSCompositeTypeBuilder(nm, namer, ty) { }
void CLIPSStructTypeBuilder::addFields(StructType* st, KnowledgeConstructor* kc, char* parent) {
   CLIPSTypeBuilder::addFields(st, kc);
   setParent(parent);
   //literal structs never have names (this is what an LLVM assertion statement
   //told me)
   if(!st->isLiteral())  {
      addField("Name", st->getName());
   }
}
void CLIPSStructTypeBuilder::build(StructType* st, KnowledgeConstructor* kc, char* parent) {
   open();
   addFields(st, kc, parent);
   close();
   std::string str = getCompletedString();
   kc->addToKnowledgeBase((PointerAddress)st, str);
}

CLIPSIntegerTypeBuilder::CLIPSIntegerTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty) : CLIPSTypeBuilder(nm, namer, ty) { }
