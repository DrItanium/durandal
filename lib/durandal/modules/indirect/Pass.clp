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
  (slot pass-name (type SYMBOL STRING) (visibility public))
  (slot pass-description (type STRING) (visibility public)))
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
  (slot pass-type (type SYMBOL) (allowed-symbols Unknown Module Function Region
																 Loop BasicBlock CallGraphSCC MachineFunction))
  (slot is-analysis (type SYMBOL) (allowed-symbols FALSE TRUE))
  (slot is-cfg (type SYMBOL) (allowed-symbols FALSE TRUE))
  (slot need-loops (type SYMBOL) (allowed-symbols FALSE TRUE))
  (slot need-regions (type SYMBOL) (allowed-symbols FALSE TRUE))
  (slot preserves-all (type SYMBOL) (allowed-symbols FALSE TRUE))
  (slot preserves-cfg (type SYMBOL) (allowed-symbols FALSE TRUE))
  (multislot passes (type SYMBOL))
  (multislot required (type SYMBOL))
  (multislot required-transitive (type SYMBOL))
  (multislot preserved (type SYMBOL))
  ;begin CLIPS specific slots
  (slot entry-point (type STRING))
  (message-handler register)
  (message-handler unregister)
  (message-handler is-registered))
;------------------------------------------------------------------------------
(defmessage-handler indirect::Pass init around () 
						  (call-next-handler)
						  ;automatically load the code if there is an entry point
						  (if (and (> (str-length ?self:entry-point) 0)
									  (not (defmodule-exists ?self:pass-name))) then
							 (batch* ?self:entry-point))
						  (if (not (pass-registered ?self:pass-name)) then
							 (register-pass ?self:pass-name 
												 ?self:pass-description
												 ?self:pass-type
												 ?self:is-analysis
												 ?self:is-cfg
												 ?self:need-regions
												 ?self:need-loops
												 ?self:passes
												 ?self:required
												 ?self:required-transitive
												 ?self:preserved
												 ?self:preserves-all
												 ?self:preserves-cfg)))
;------------------------------------------------------------------------------
(defmessage-handler indirect::Pass register ()
						  (if (not (pass-registered ?self:pass-name)) then
							 (register-pass ?self:pass-name 
												 ?self:pass-description
												 ?self:pass-type
												 ?self:is-analysis
												 ?self:is-cfg
												 ?self:need-regions
												 ?self:need-loops
												 ?self:passes
												 ?self:required
												 ?self:required-transitive
												 ?self:preserved
												 ?self:preserves-all
												 ?self:preserves-cfg)
							 (return TRUE)
							 else
							 (printout werror "ERROR: pass already registered" crlf)
							 (return FALSE)))

;------------------------------------------------------------------------------
(defmessage-handler indirect::Pass unregister ()
						  (unregister-pass ?self:pass-name))
;------------------------------------------------------------------------------
(defmessage-handler indirect::Pass is-registered () 
						  (pass-registered ?self:pass-name))
;------------------------------------------------------------------------------

