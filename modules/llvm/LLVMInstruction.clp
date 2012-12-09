;------------------------------------------------------------------------------
;Copyright (c) 2012, Joshua Scoggins 
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without
;modification, are permitted provided that the following conditions are met:
;    * Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;    * Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;    * Neither the name of Joshua Scoggins nor the
;      names of its contributors may be used to endorse or promote products
;      derived from this software without specific prior written permission.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;DISCLAIMED. IN NO EVENT SHALL Joshua Scoggins BE LIABLE FOR ANY
;DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;------------------------------------------------------------------------------
; LLVMInstruction.clp - Contains all of the classes that represent the 
;                       many different LLVM instruction types. The base
;                       Instruction type is not in this file
;------------------------------------------------------------------------------
(defclass llvm::PointerOperandObject 
  (is-a core::Object core::ParentedObject core::InteropObject)
  (slot PointerOperand (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::MemoryModifyingObject 
  (is-a llvm::PointerOperandObject)
  (slot Alignment (type NUMBER) (range 0 ?VARIABLE))
  (slot IsAtomic (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsSimple (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsUnordered (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsVolatile (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::PhiNode 
  (is-a llvm::Instruction)
  (slot IncomingValueCount (type NUMBER) (range 0 ?VARIABLE))
  (slot HasConstantValue (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::CallInstruction 
  (is-a llvm::Instruction)
  (slot CalledFunction (type SYMBOL))
  (slot IsDereferenceablePointer (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasValueHandle (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsTailCall (type SYMBOL) (allowed-values FALSE TRUE))
  (multislot ArgumentOperands)
  (slot CallingConvention (type SYMBOL))
  (multislot Attributes)
  (slot IsNoInline (type SYMBOL) (allowed-values FALSE TRUE))
  (slot CanReturnTwice (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DoesNotAccessMemory (type SYMBOL) (allowed-values FALSE TRUE))
  (slot OnlyReadsMemory (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DoesNotReturn (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DoesNotThrow (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasStructRetAttr (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasByValArgument (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsInlineAsm (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::IntrinsicInstruction 
  (is-a llvm::CallInstruction)
  (slot IntrinsicID (type NUMBER)))
;------------------------------------------------------------------------------
(defclass llvm::DBGInfoIntrinsic 
  (is-a llvm::IntrinsicInstruction))
;------------------------------------------------------------------------------
(defclass llvm::MemoryIntrinsic 
  (is-a llvm::IntrinsicInstruction)
  (slot RawDestination)
  (slot Length (type NUMBER) (range 0 ?VARIABLE))
  (slot AlignmentConstant (type NUMBER))
  (slot Alignment (type NUMBER) (range 0 ?VARIABLE))
  (slot IsVolatile (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DestinationAddressSpace (type NUMBER) (range 0 ?VARIABLE))
  (slot Destination (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::BinaryOperator 
  (is-a llvm::Instruction)
  (slot HasNoUnsignedWrap (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasNoSignedWrap (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsExact (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::UnaryInstruction 
 (is-a llvm::Instruction))
;------------------------------------------------------------------------------
(defclass llvm::AllocaInstruction 
 (is-a llvm::UnaryInstruction)
  (slot IsStaticAllocation (type SYMBOL) (allowed-values FALSE TRUE))
  (slot Alignment (type NUMBER) (range 0 ?VARIABLE))
  (slot PointerType (type SYMBOL)) ; from AllocaInst::getType
  (slot PointerType-Pointer (type NUMBER) (range 0 ?VARIABLE))
  (slot IsArrayAllocation (type SYMBOL) (allowed-values FALSE TRUE)) 
  (slot ArraySize (type NUMBER) (range 0 ?VARIABLE))
  (slot IsStatic (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::CastInstruction 
 (is-a llvm::UnaryInstruction)
  (slot IsIntegerCast (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsLosslessCast (type SYMBOL) (allowed-values FALSE TRUE))
  (slot SourceType (type SYMBOL))
  (slot DestinationType (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::BitCastInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::FPExtInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::FPToSIInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::FPToUIInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::FPTruncInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::IntToPtrInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::PtrToIntInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::SExtInstruction 
 (is-a llvm::CastInstruction))
;------------------------------------------------------------------------------
(defclass llvm::LoadInstruction 
 (is-a llvm::UnaryInstruction llvm::MemoryModifyingObject)
  (slot PointerAddressSpace (type NUMBER) (range 0 ?VARIABLE)))
;------------------------------------------------------------------------------
(defclass llvm::VAArgInstruction 
 (is-a llvm::UnaryInstruction llvm::PointerOperandObject))
;------------------------------------------------------------------------------
(defclass llvm::CompareInstruction 
 (is-a llvm::Instruction)
  (slot SignedPredicate)
  (slot UnsignedPredicate)
  (slot IsFPPredicate (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsIntPredicate (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsRelational (type SYMBOL) (allowed-values FALSE TRUE))
  (slot Predicate (type NUMBER))
  (slot InversePredicate (type NUMBER))
  (slot SwappedPredicate (type NUMBER))
  (slot Opcode (type NUMBER))
  (slot IsEquality (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsSigned (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsUnsigned (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsTrueWhenEqual (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFalseWhenEqual (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::FloatingPointCompareInstruction 
 (is-a llvm::CompareInstruction))
;------------------------------------------------------------------------------
(defclass llvm::IntegerCompareInstruction 
 (is-a llvm::CompareInstruction))
;------------------------------------------------------------------------------
(defclass llvm::TerminatorInstruction 
 (is-a llvm::Instruction)
  (multislot Successors))
;------------------------------------------------------------------------------
(defclass llvm::BranchInstruction 
 (is-a llvm::TerminatorInstruction)
  (slot IsUnconditional (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsConditional (type SYMBOL) (allowed-values FALSE TRUE))
  (slot Condition))
;------------------------------------------------------------------------------
(defclass llvm::IndirectBranchInstruction 
 (is-a llvm::TerminatorInstruction)
  (slot Address (type NUMBER) (range 0 ?VARIABLE)))
;------------------------------------------------------------------------------
;We store the destinations within the destination registers
(defclass llvm::InvokeInstruction 
 (is-a llvm::TerminatorInstruction)
  (multislot Arguments)
  (slot CalledFunction)
  (slot NormalDestination)
  (slot UnwindDestination)
  (slot IsNoInline (type SYMBOL) (allowed-values FALSE TRUE))
  (slot OnlyReadsMemory (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DoesNotReturn (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DoesNotThrow (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasStructRetAttr (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasByValArgument (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::ResumeInstruction 
 (is-a llvm::TerminatorInstruction)
  (slot Value (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::ReturnInstruction 
 (is-a llvm::TerminatorInstruction)
  (slot ReturnValue (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::SwitchEntry 
  (is-a core::Object core::ParentedObject core::InteropObject)
  (slot Index (type NUMBER))
  (slot Target (allowed-classes BasicBlock InteropObject SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::SwitchInstruction 
 (is-a llvm::TerminatorInstruction)
  (slot Condition (type SYMBOL))
  (slot DefaultDestination (type SYMBOL))
  (multislot Cases))
;------------------------------------------------------------------------------
(defclass llvm::UnreachableInstruction 
 (is-a llvm::TerminatorInstruction))
;------------------------------------------------------------------------------
(defclass llvm::GetElementPointerInstruction 
 (is-a llvm::Instruction llvm::PointerOperandObject)
  (slot PointerAddressSpace (type NUMBER) (range 0 ?VARIABLE))
  (slot NumIndices (type NUMBER) (range 0 ?VARIABLE))
  (slot HasIndices (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasAllZeroIndices (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasAllConstantIndices (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsInBounds (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::AtomicCompareExchangeInstruction 
 (is-a llvm::Instruction llvm::PointerOperandObject)
  (slot IsVolatile (type SYMBOL) (allowed-values FALSE TRUE))
  (slot CompareOperand (type SYMBOL))
  (slot NewValOperand (type SYMBOL))
  (slot PointerAddressSpace (type NUMBER) (range 0 ?VARIABLE)))
;------------------------------------------------------------------------------
(defclass llvm::AtomicRMWInstruction 
 (is-a llvm::Instruction llvm::PointerOperandObject)
  (slot Operation (type SYMBOL))
  (slot IsVolatile (type SYMBOL) (allowed-values FALSE TRUE))
  (slot ValueOperand (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::LandingPadInstruction 
 (is-a llvm::Instruction)
  (slot PersonalityFunction (type SYMBOL))
  (slot IsCleanup (type SYMBOL) (allowed-values FALSE TRUE))
  (multislot Clauses)
  (slot IsCatch (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFilter (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::SelectInstruction 
 (is-a llvm::Instruction)
  (slot Condition (type SYMBOL))
  (slot TrueValue (type SYMBOL))
  (slot FalseValue (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::ShuffleVectorInstruction 
 (is-a llvm::Instruction)
  (slot Mask (type NUMBER))
  (multislot ShuffleMask (type NUMBER) (cardinality 0 16)))
;------------------------------------------------------------------------------
(defclass llvm::StoreInstruction 
 (is-a llvm::Instruction llvm::MemoryModifyingObject)
  (slot ValueOperand (type SYMBOL)))
;------------------------------------------------------------------------------
;This is one of those instructions that have to be done inside C++
; I'm not wasting my time pulling in the entire contents of these types
(defclass llvm::ExtractElementInstruction 
 (is-a llvm::Instruction))
;------------------------------------------------------------------------------
(defclass llvm::InsertElementInstruction 
 (is-a llvm::Instruction))
;------------------------------------------------------------------------------
(defclass llvm::InsertValueInstruction 
 (is-a llvm::Instruction))
;------------------------------------------------------------------------------
(defclass llvm::FenceInstruction 
 (is-a llvm::Instruction))
;------------------------------------------------------------------------------
(defclass llvm::ExtractValueInstruction 
 (is-a llvm::UnaryInstruction)
  (multislot Indices)
  (slot AggregateOperand))
;------------------------------------------------------------------------------
