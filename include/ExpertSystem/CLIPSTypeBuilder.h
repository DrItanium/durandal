#ifndef _clips_type_builder_h
#define _clips_type_builder_h
#include "ExpertSystem/CLIPSObjectBuilder.h"
#include "llvm/Type.h"
using namespace llvm;
class CLIPSTypeBuilder : public CLIPSObjectBuilder {
	public:
		CLIPSTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "LLVMType");
		void addFields(Type* type, KnowledgeConstructor* kc);
		void build(Type* st, KnowledgeConstructor* kc);
};
class CLIPSFunctionTypeBuilder : public CLIPSTypeBuilder {
	public:
		CLIPSFunctionTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "FunctionType");
};
class CLIPSCompositeTypeBuilder : public CLIPSTypeBuilder {
	public:
		CLIPSCompositeTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "CompositeType");
};
class CLIPSSequentialTypeBuilder : public CLIPSCompositeTypeBuilder {
	public:
		CLIPSSequentialTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "SequentialType");
};
class CLIPSArrayTypeBuilder : public CLIPSSequentialTypeBuilder {
	public:
		CLIPSArrayTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "ArrayType");
};
class CLIPSPointerTypeBuilder : public CLIPSSequentialTypeBuilder {
	public:
		CLIPSPointerTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "PointerType");

};
class CLIPSVectorTypeBuilder : public CLIPSSequentialTypeBuilder {
	public:
		CLIPSVectorTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "VectorType");

};
class CLIPSStructTypeBuilder : public CLIPSCompositeTypeBuilder {
	public:
		CLIPSStructTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "StructType");
		void addFields(StructType* st, KnowledgeConstructor* kc, char* parent);
		void build(StructType* st, KnowledgeConstructor* kc, char* parent);
};

class CLIPSIntegerTypeBuilder : public CLIPSTypeBuilder {
	public:
		CLIPSIntegerTypeBuilder(std::string nm, FunctionNamer& namer, std::string ty = "IntegerType");
};
#endif
