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
; Object.clp - Base class of all objects 
;------------------------------------------------------------------------------
(defclass core::Object 
  "Base class of all objects"
  (is-a USER)
  (slot id (type SYMBOL) (visiblity public) (access initialize-only))
  (slot class (type SYMBOL) (visiblity public) (access initialize-only)))
;------------------------------------------------------------------------------
(defmessage-handler core::Object init around 
						  "Initializes the object, setting the id and class of the 
						  object" ()
						  (call-next-handler)
						  ;we want to set this information as the last thing before 
						  ;we return 
						  (bind ?self:id (instance-name-to-symbol 
												 (instance-name ?self)))
						  (bind ?self:class (class ?self)))
;------------------------------------------------------------------------------
(defclass core::ParentedObject 
  "An object that has a parent field." 
  (is-a core::Object)
  (slot parent (type SYMBOL) (visibility public)))
;------------------------------------------------------------------------------
