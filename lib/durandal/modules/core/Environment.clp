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
; Environment.clp - A wrapper over a CLIPS environment
;------------------------------------------------------------------------------
(defclass core::Environment 
 "A wrapper over a clips environment"
 (is-a Object InteropObject))
 ;(role concrete)
 ;(slot pointer (source composite) (access initialize-only)
 ; (visibility public)))

(defmessage-handler core::Environment init around ()
 (call-next-handler)
 (if (eq ?self:pointer 0) then
  (bind ?self:pointer (env-create))))

(defmessage-handler core::Environment destroy primary ()
                    (if (not (is-currently-executing-environment ?self:pointer)) then
                      (env-destroy ?self:pointer)
                      (return (delete-instance))
                      else
                      (printout 
                        werror
                        "ERROR: attempted to destroy the executing environment."
                        crlf)
                      (return FALSE)))

(defmessage-handler core::Environment is-currently-executing-environment primary () 
                    (return (is-currently-executing-environment ?self:pointer)))

(defmessage-handler core::Environment run ($?count)
                    (bind ?size (length$ $?count))
                    (if (= 0 ?size) then 
                      (if (is-currently-executing-environment ?self:pointer) then
                        (run)
                        else
                        (env-run ?self:pointer))
                      else
                      (bind ?num (nth$ 1 ?count))
                      (if (and (> ?num 0) (integerp ?num)) then
                        (if (is-currently-executing-environment ?self:pointer) then
                          (run ?num)
                          else
                          (env-run ?self:pointer ?num))
                        else
                        (printout werror "ERROR: invalid count provided." crlf))))

(defmessage-handler core::Environment eval (?string)
                    (if (stringp ?string) then
                      (if (is-currently-executing-environment ?self:pointer) then
                        (eval ?string)
                        else
                        (env-eval ?self:pointer ?string))
                      else
                      (printout werror "ERROR: string expected" crlf)))

(defmessage-handler core::Environment build (?string)
                    (if (stringp ?string) then
                      (if (is-currently-executing-environment ?self:pointer) then
                        (build ?string)
                        else
                        (env-build ?self:pointer ?string))
                      else
                      (printout werror "ERROR: string expected" crlf)))

(defmessage-handler core::Environment facts ()
                    (if (is-currently-executing-environment ?self:pointer) then
                      (facts)
                      else
                      (env-facts ?self:pointer)))
(defmessage-handler core::Environment instances () 
                    (if (is-currently-executing-environment ?self:pointer) then
                      (instances)
                      else
                      (env-instances ?self:pointer)))

(defmessage-handler core::Environment rules ()
                    (if (is-currently-executing-environment ?self:pointer) then
                      (rules)
                      else
                      (env-rules ?self:pointer)))

(defmessage-handler core::Environment assert ($?elements)
                    (bind ?length (length$ ?elements))
                    (if (= 0 ?length) then
                      (printout werror "ERROR: No arguments provided" crlf)
                      else
							 (bind ?t (implode$ $?elements))
                      (env-assert-string ?self:pointer 
							  (format nil "(%s)" ?t))))
