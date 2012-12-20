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
; Pass.clp - Pass types that interact with LibIndirect 
;------------------------------------------------------------------------------
(defclass indirect::PassBase 
  "The base class of all passes (native or indirect)"
  (is-a Object) 
  (role abstract)
  (slot pass-name (type SYMBOL STRING))
  (slot pass-description (type STRING)))
;------------------------------------------------------------------------------
(defclass indirect::NativePass 
  "A wrapper over a native LLVM pass"
  (is-a PassBase)
  (role concrete)
  (pattern-match reactive))
;------------------------------------------------------------------------------
(defclass indirect::Pass 
  "Represents an indirect pass that uses LibIndirect to interact with LLVM"
  (is-a PassBase)
  (role concrete)
  (pattern-match reactive)
  (slot entry (type STRING))
  (slot target (type SYMBOL) (allowed-symbols Any Module Function Region Loop
                                              BasicBlock Instruction Immutable))
  (multislot pass-uses (type SYMBOL STRING INSTANCE-NAME) 
             (allowed-classes PassBase)))
;------------------------------------------------------------------------------
