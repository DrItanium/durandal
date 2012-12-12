#ifndef _instruction_construction_h
#define _instruction_construction_h
#include "ExpertSystem/CLIPSUserBuilder.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"

class CLIPSInstructionBuilder : public CLIPSUserBuilder {
	public:
		CLIPSInstructionBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSUserBuilder(nm, ty, namer) { }
		void addFields(Instruction* instruction, KnowlegeConstructor *kc, char* parent, bool addDestinationRegisters = true);
		void build(Instruction* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSPHINodeBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSPHINodeBuilder(std::string nm, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, "PhiNode", namer) { }
		void addFields(PHINode* instruction, KnowlegeConstructor *kc, char* parent);
		void build(PHINode* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSStoreInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSStoreInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, "StoreInstruction", namer) { }
		void addFields(StoreInst* target, KnowlegeConstructor *kc, char* parent);
		void build(StoreInst* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSBinaryOperatorBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSBinaryOperatorBuilder(std::string nm, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, "BinaryOperator", namer) { }
		void addFields(BinaryOperator* target, KnowlegeConstructor *kc, char* parent);
		void build(BinaryOperator* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSCallInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSCallInstructionBuilder(std::string nm, FunctionNamer& namer, std::string ty = "CallInstruction") : CLIPSInstructionBuilder(nm, ty, namer) { }
		void addFields(CallInst* target, KnowlegeConstructor *kc, char* parent);
		void build(CallInst* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSCompareInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSCompareInstructionBuilder(std::string nm, FunctionNamer& namer, std::string ty = "CompareInstruction") : CLIPSInstructionBuilder(nm, ty, namer) { }
		void addFields(CmpInst* target, KnowlegeConstructor *kc, char* parent);
		void build(CmpInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSFPCompareInstructionBuilder : public CLIPSCompareInstructionBuilder {
	public:
		CLIPSFPCompareInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSCompareInstructionBuilder(nm, namer, "FloatingPointCompareInstruction") { }
		void addFields(FCmpInst* target, KnowlegeConstructor *kc, char* parent);
		void build(FCmpInst* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSIntCompareInstructionBuilder : public CLIPSCompareInstructionBuilder {
	public:
		CLIPSIntCompareInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSCompareInstructionBuilder(nm, namer, "IntegerCompareInstruction") { }
		void addFields(ICmpInst* target, KnowlegeConstructor *kc, char* parent);
		void build(ICmpInst* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSGetElementPtrInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSGetElementPtrInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, "GetElementPointerInstruction", namer) { }
		void addFields(GetElementPtrInst* target, KnowlegeConstructor *kc, char* parent);
		void build(GetElementPtrInst* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSTerminatorInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSTerminatorInstructionBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, ty, namer) { }
		void addFields(TerminatorInst* target, KnowlegeConstructor *kc, char* parent);
		void build(TerminatorInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSBranchInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	public:
		CLIPSBranchInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "BranchInstruction", namer) { }
		void addFields(BranchInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(BranchInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSIndirectBranchInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	public:
		CLIPSIndirectBranchInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "IndirectBranchInstruction", namer) { }
		void addFields(IndirectBrInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(IndirectBrInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSInvokeInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	//this is different from a call instruction because there are two extra
	//arguments that define the normal and exception jump points
	public:
		CLIPSInvokeInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "InvokeInstruction", namer) { }
		void addFields(InvokeInst* target, KnowlegeConstructor *kc, char* parent);
		void build(InvokeInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSResumeInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	public:
		CLIPSResumeInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "ResumeInstruction", namer) { }
		void addFields(ResumeInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(ResumeInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSReturnInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	public:
		CLIPSReturnInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "ReturnInstruction", namer) { }
		void addFields(ReturnInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(ReturnInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSSwitchInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	public:
		CLIPSSwitchInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "SwitchInstruction", namer) { }
		void addFields(SwitchInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(SwitchInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSUnreachableInstructionBuilder : public CLIPSTerminatorInstructionBuilder {
	public:
		CLIPSUnreachableInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSTerminatorInstructionBuilder(nm, "UnreachableInstruction", namer) { }
		void addFields(UnreachableInst* instruction, KnowlegeConstructor *kc, char* parent);
		void build(UnreachableInst* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSSelectInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSSelectInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, "SelectInstruction", namer) { }
		void addFields(SelectInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(SelectInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSUnaryInstructionBuilder : public CLIPSInstructionBuilder {
	public:
		CLIPSUnaryInstructionBuilder(std::string nm, std::string ty, FunctionNamer& namer) : CLIPSInstructionBuilder(nm, ty, namer) { }
		void addFields(UnaryInstruction* inst, KnowlegeConstructor *kc, char* parent);
		void build(UnaryInstruction* instruction, KnowlegeConstructor *kc, char* parent);
};

class CLIPSAllocaInstructionBuilder : public CLIPSUnaryInstructionBuilder {
	public:
		CLIPSAllocaInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSUnaryInstructionBuilder(nm, "AllocaInstruction", namer) { }
		void addFields(AllocaInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(AllocaInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSCastInstructionBuilder : public CLIPSUnaryInstructionBuilder {
	public:
		CLIPSCastInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSUnaryInstructionBuilder(nm, "CastInstruction", namer) { }
		void addFields(CastInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(CastInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSExtractValueInstructionBuilder : public CLIPSUnaryInstructionBuilder {
	public: 
		CLIPSExtractValueInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSUnaryInstructionBuilder(nm, "ExtractValueInstruction", namer) { }
		void addFields(ExtractValueInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(ExtractValueInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSLoadInstructionBuilder : public CLIPSUnaryInstructionBuilder {
	public: 
		CLIPSLoadInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSUnaryInstructionBuilder(nm, "LoadInstruction", namer) { }
		void addFields(LoadInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(LoadInst* instruction, KnowlegeConstructor *kc, char* parent);
};
class CLIPSVAArgInstructionBuilder : public CLIPSUnaryInstructionBuilder {
	public: 
		CLIPSVAArgInstructionBuilder(std::string nm, FunctionNamer& namer) : CLIPSUnaryInstructionBuilder(nm, "VAArgInstruction", namer) { }
		void addFields(VAArgInst* inst, KnowlegeConstructor *kc, char* parent);
		void build(VAArgInst* instruction, KnowlegeConstructor *kc, char* parent);
};
#endif
