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
; LLVMValue.clp - CLIPS representation of the LLVM Value type. 
;------------------------------------------------------------------------------
(defclass llvm::LLVMValue 
 (is-a LLVMObject)
  (slot HasAddressTaken (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsLandingPad (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasOneUse (type SYMBOL) (allowed-values FALSE TRUE))
  (slot NumberOfUses (type NUMBER) (range 0 ?VARIABLE))
  (slot Type (visibility public))
  (slot ValueName)
  (multislot Users)
  (slot ValueID (type NUMBER) (range 0 ?VARIABLE))
  (slot RawSubclassOptionalData (type NUMBER) (range 0 ?VARIABLE))
  (slot HasValueHandle (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsDereferenceablePointer (type SYMBOL) (allowed-values FALSE TRUE)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::LLVMValue get-num-uses primary ()
						  (length$ ?self:Users))
;------------------------------------------------------------------------------
(defmessage-handler llvm::LLVMValue has-n-uses primary (?n)
						  (= ?n (length$ ?self:Users)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::LLVMValue has-n-uses-or-more primary (?n)
						  (>= ?n (length$ ?self:Users)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::LLVMValue has-same-subclss-optional-data primary (?v)
						  (= ?self:RawSubclassOptionalData 
							  (send ?v get-RawSubclassOptionalData)))
;------------------------------------------------------------------------------
