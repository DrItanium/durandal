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
; LLVMFunction.clp - Represents the LLVM Function class
;------------------------------------------------------------------------------
(defclass llvm::Function
  (is-a GlobalValue Diplomat List)
  (slot ReturnType)
  (slot FunctionType)
  (slot IsVarArg (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IntrinsicID)
  (slot IsIntrinsic (type SYMBOL) (allowed-values FALSE TRUE))
  (slot CallingConvention 
        (type SYMBOL)
        (allowed-values Unknown intel-ocl-bi spir-kernel spir-func
                        HiPE mblaze-svol mblaze-intr ptx-device ptx-kernel x86-thiscall
                        msp430-intr arm-aapcs-vfp arm-aapcs arm-apcs x86-fastcall x86-stdcall
                        GHC Cold Fast C))
  (slot HasGC (type SYMBOL) (allowed-values FALSE TRUE))
  (slot GC)
  (slot DoesNotAccessMemory (type SYMBOL) (allowed-values FALSE TRUE))
  (slot OnlyReadsMemory (type SYMBOL) (allowed-values FALSE TRUE))
  (slot DoesNotReturn (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasUWTable (type SYMBOL) (allowed-values FALSE TRUE))
  (slot NeedsUnwindTableEntry (type SYMBOL) (allowed-values FALSE TRUE))
  (slot HasStructRetAttr (type SYMBOL) (allowed-values FALSE TRUE))
  (slot IsDefTriviallyDead (type SYMBOL) (allowed-values FALSE TRUE))
  (slot CallsFunctionThatReturnsTwice 
        (type SYMBOL) 
        (allowed-values FALSE TRUE)) 
  (slot EntryBlock)
  (multislot Arguments))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function put-GC after 
                    (?str)
                    (llvm-function-set-gc ?self:pointer ?str))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function clear-GC primary 
                    ()
                    (llvm-function-clear-gc ?self:pointer)
                    (bind ?self:GC ""))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function delete-body primary 
                    ()
                    (llvm-function-delete-body ?self:pointer)
                    (bind ?self:contents (create$)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function remove-from-parent primary
                    ()
                    (llvm-function-remove-from-parent ?self:pointer)
                    (bind ?self:parent nil))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function erase-from-parent primary
                    ()
                    (llvm-function-erase-from-parent ?self:pointer)
                    (delete-instance))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function put-DoesNotAccessMemory after
                    (?symbol)
                    (if ?symbol then
                      (llvm-function-set-does-not-access-memory 
                        ?self:pointer)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function set-does-not-alias primary
                    (?index)
                    (llvm-function-set-does-not-alias ?self:pointer ?index)) 
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function set-does-not-capture primary
                    (?index)
                    (llvm-function-set-does-not-capture ?self:pointer ?index))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function put-DoesNotThrow after
                    (?symbol)
                    (if ?symbol then
                      (llvm-function-set-does-not-throw ?self:pointer)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function put-HasUWTable after
                    (?symbol)
                    (if ?symbol then
                      (llvm-function-set-has-uw-table ?self:pointer)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function put-DoesNotReturn after
                    (?symbol)
                    (if ?symbol then
                      (llvm-function-set-does-not-return ?self:pointer)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function put-OnlyReadsMemory after
                    (?symbol)
                    (if ?symbol then
                      (llvm-function-set-only-reads-memory ?self:pointer)))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function drop-all-references primary
                    ()
                    (llvm-function-drop-all-references 
                      ?self:pointer))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function does-not-alias primary
                    (?index)
                    (llvm-function-set-does-not-alias ?self:pointer ?index))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function does-not-capture primary
                    (?index)
                    (llvm-function-does-not-capture ?self:pointer ?index))
;------------------------------------------------------------------------------
(defmessage-handler llvm::Function get-param-alignment primary
                    (?index)
                    (llvm-function-get-param-alignment ?self:pointer ?index))
;------------------------------------------------------------------------------
