#ifndef _ENGINE_DECL_H_
#define _ENGINE_DECL_H_
#include "knowledge/Engine.h"
#include "knowledge/EngineBookkeeping.h"
#include "knowledge/ProcessingNode.h"
#include "knowledge/Dispatcher.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constant.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/Casting.h"
extern "C" {
#include "clips.h"
}
namespace knowledge {
#define X(unused, type, className) \
	ElectronClassNameAssociation(type, className); \
	template<> \
	struct ExternalAddressRegistration<type> { \
		static int indirectId; \
	}; 
#include "knowledge/EngineNodes.def"
#undef X
// SO FUCKING BEAUTIFUL :D
#define Route(type, env, inst) \
	static void dispatch(void* env, type * inst) 
#define BeginCustomDispatch(type, env, inst) \
	template<typename Pass> \
	struct Router<type> { \
		Route(type, env, inst) { \
			WhenInstanceDoesNotExist(env, inst) {

#define EndCustomDispatch } } };



#define CondDispatch(type, env, val) \
	if (type* v = llvm::dyn_cast<type>(val)) return Router<Pass, type>::dispatch(env, v)


BeginCustomDispatch(llvm::TerminatorInst, env, inst)
	CondDispatch(llvm::BranchInst, env, inst);
	CondDispatch(llvm::IndirectBrInst, env, inst);
	CondDispatch(llvm::InvokeInst, env, inst);
	CondDispatch(llvm::ResumeInst, env, inst);
	CondDispatch(llvm::ReturnInst, env, inst);
	CondDispatch(llvm::SwitchInst, env, inst);
	CondDispatch(llvm::UnreachableInst, env, inst);
	Otherwise(llvm::TerminatorInst, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::UnaryInstruction, env, inst) 
	CondDispatch(llvm::AllocaInst, env, inst);
	CondDispatch(llvm::CastInst, env, inst);
	CondDispatch(llvm::ExtractValueInst, env, inst);
	CondDispatch(llvm::LoadInst, env, inst);
	CondDispatch(llvm::VAArgInst, env, inst);
	Otherwise(llvm::UnaryInstruction, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::CmpInst, env, inst)
	CondDispatch(llvm::FCmpInst, env, inst);
	CondDispatch(llvm::ICmpInst, env, inst);
	Otherwise(llvm::CmpInst, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::Instruction, env, inst)
	CondDispatch(llvm::PHINode, env, inst);
	CondDispatch(llvm::StoreInst, env, inst);
	CondDispatch(llvm::BinaryOperator, env, inst);
	CondDispatch(llvm::CallInst, env, inst);
	CondDispatch(llvm::CmpInst, env, inst);
	CondDispatch(llvm::ExtractElementInst, env, inst);
	CondDispatch(llvm::FenceInst, env, inst);
	CondDispatch(llvm::GetElementPtrInst, env, inst);
	CondDispatch(llvm::InsertElementInst, env, inst);
	CondDispatch(llvm::InsertValueInst, env, inst);
	CondDispatch(llvm::LandingPadInst, env, inst);
	CondDispatch(llvm::SelectInst, env, inst);
	CondDispatch(llvm::ShuffleVectorInst, env, inst);
	CondDispatch(llvm::TerminatorInst, env, inst);
	CondDispatch(llvm::UnaryInstruction, env, inst);
	Otherwise(llvm::Instruction, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::User, env, inst) 
	CondDispatch(llvm::Instruction, env, inst);
	CondDispatch(llvm::Constant, env, inst);
	CondDispatch(llvm::Operator, env, inst);
	Otherwise(llvm::User, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::Value, env, inst)
	CondDispatch(llvm::User, env, inst);
	CondDispatch(llvm::BasicBlock, env, inst);
	CondDispatch(llvm::Argument, env, inst);
	// CondDispatch(llvm::InlineAsm, env, value);
	// CondDispatch(llvm::MDNode, env, value);
	// CondDispatch(llvm::MDString, env, value);
	Otherwise(llvm::Value, env, inst);
EndCustomDispatch

BeginCustomDispatch(llvm::CastInst, env, inst)
	CondDispatch(llvm::AddrSpaceCastInst, env, inst);
	CondDispatch(llvm::BitCastInst, env, inst);
	CondDispatch(llvm::FPExtInst, env, inst);
	CondDispatch(llvm::FPToSIInst, env, inst);
	Otherwise(llvm::CastInst, env, inst);
EndCustomDispatch

//TODO: Finish defining this custom dispatch at some point
//BeginCustomDispatch(llvm::IntrinsicInst, env, inst)
//	CondDispatch(llvm::DbgInfoIntrinsic, env, inst);
//	Otherwise(llvm::IntrinsicInst, env, inst);
//EndCustomDispatch

BeginCustomDispatch( llvm::CallInst, env, inst)
	CondDispatch(llvm::IntrinsicInst, env, inst);
	Otherwise(llvm::CallInst, env, inst);
EndCustomDispatch



}
#endif
