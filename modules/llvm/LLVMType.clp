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
(defclass llvm::LLVMType 
 (is-a LLVMObject)
  (slot IsVoidType (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsHalfType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFloatType (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsDoubleType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsX86FP80Type  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFP128Type  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsPPCFP128Type  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFloatingPointType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsX86MMXType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFPOrFPVectorType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsLabelType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsMetadataType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsIntegerType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsIntOrIntVectorType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFunctionType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsStructType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsArrayType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsPointerType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsVectorType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsEmptyType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsPrimitiveType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsDerivedType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsFirstClassType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsSingleValueType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsAggregateType  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsSized  (type SYMBOL) (allowed-values FALSE TRUE))
  (slot PrimitiveSizeInBits (type NUMBER) (range 0 ?VARIABLE))
  (slot ScalarSizeInBits (type NUMBER) (range 0 ?VARIABLE))
  (slot FPMantissaWidth (type NUMBER) (range 0 ?VARIABLE))
  (slot ScalarType (allowed-classes LLVMType))
  (multislot Subtypes)
  (multislot TypeMakeup (type SYMBOL))
  (slot IsFunctionVarArg (type SYMBOL) (allowed-values FALSE TRUE))
  (slot QuickType (type SYMBOL)))
;------------------------------------------------------------------------------
(defclass llvm::IntegerType 
 (is-a LLVMType)
 (slot BitWidth (type NUMBER) (range 0 ?VARIABLE))
 (slot BitMask (type NUMBER))
 (slot SignBit (type NUMBER))
 (slot IsPowerOf2ByteWidth (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defclass llvm::FunctionType 
 (is-a LLVMType)
 (slot IsVarArg (type SYMBOL) (allowed-values FALSE TRUE))
 (slot ReturnType (allowed-classes LLVMType))
 (multislot Parameters))
;------------------------------------------------------------------------------
(defmessage-handler llvm::FunctionType get-num-params primary 
 ()
 (length$ ?self:Parameters))
;------------------------------------------------------------------------------
(defmessage-handler llvm::FunctionType get-param-type primary 
 (?index)
 (nth$ ?index ?self:Parameters))
;------------------------------------------------------------------------------
(defclass llvm::CompositeType 
 (is-a LLVMType)
 (multislot Indicies))
;------------------------------------------------------------------------------
(defclass llvm::StructType 
 (is-a CompositeType)
 (slot Name (type SYMBOL))
 (slot IsPacked (type SYMBOL) (allowed-values FALSE TRUE))
 (slot IsLiteral (type SYMBOL) (allowed-values FALSE TRUE))
 (slot IsOpaque (type SYMBOL) (allowed-values FALSE TRUE))
 (slot IsSized (type SYMBOL) (allowed-values FALSE TRUE))
 (slot HasName (type SYMBOL) (allowed-values FALSE TRUE))
 (multislot Body))
;------------------------------------------------------------------------------
(defmessage-handler llvm::StructType set-body primary 
 (?element ?isPacked)
 (bind ?self:IsPacked ?isPacked)
 (bind ?self:Body ?element))
;------------------------------------------------------------------------------
(defmessage-handler llvm::StructType get-num-elements 
 ()
 (length$ ?self:Body))
;------------------------------------------------------------------------------
(defmessage-handler llvm::StructType get-element-type 
 (?n)
 (nth$ ?n ?self:Body))
;------------------------------------------------------------------------------
(defclass llvm::SequentialType 
 (is-a CompositeType)
 (slot ElementType (allowed-classes LLVMType)))
;------------------------------------------------------------------------------
(defclass llvm::VectorType 
 (is-a SequentialType)
 (slot BitWidth (type NUMBER) (range 0 ?VARIABLE))
 (slot NumElements (type NUMBER) (range 0 ?VARIABLE)))
;------------------------------------------------------------------------------
(defclass llvm::PointerType 
 (is-a SequentialType)
 (slot AddressSpace (type NUMBER) (range 0 ?VARIABLE)))
;------------------------------------------------------------------------------
(defclass llvm::ArrayType 
 (is-a SequentialType)
 (slot NumElements (type NUMBER) (range 0 ?VARIABLE)))
;------------------------------------------------------------------------------
