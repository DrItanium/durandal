#ifndef _llvm_functions_h
#define _llvm_functions_h
/*
 * Contains all of the CLIPS=>LLVM and LLVM=>CLIPS wrapper functions.
 * These functions show up in CLIPS as various different things 
 */
#include "ExpertSystem/Types.h"
extern "C" void RegisterExpertSystemFunctions(void *theEnv);
extern "C" void LLVMPrint(void *theEnv);
extern "C" void BatchLoad(void *theEnv);
extern "C" void* IsIdenticalTo(void *theEnv);
extern "C" void* IsIdenticalToWhenDefined(void *theEnv);
extern "C" void* IsSameOperationAs(void *theEnv);
extern "C" PointerAddress CloneInstruction(void *theEnv);
extern "C" void* UnlinkAndMoveInstructionBefore(void *theEnv);
extern "C" void* UnlinkAndMoveInstructionAfter(void *theEnv);
extern "C" void* MoveInstructionBefore(void *theEnv);
extern "C" void* MoveInstructionAfter(void *theEnv);
extern "C" void* UnlinkAndDeleteInstruction(void *theEnv);
extern "C" void* ScheduleInstructions(void *theEnv);
extern "C" void* ReplaceUsesOf(void *theEnv);
extern "C" void* ReplaceAllUsesOf(void *theEnv);
extern "C" PointerAddress CreatePhiNode(void *theEnv);
extern "C" void* UnlinkInstruction(void *theEnv);
extern "C" void* DeleteInstruction(void *theEnv);

#endif
