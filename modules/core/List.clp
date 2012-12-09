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
; List.clp - represents a wrapper over a multifield
;------------------------------------------------------------------------------
(defclass core::List 
 (is-a Object)
 (multislot contents (visibility public)))
;------------------------------------------------------------------------------
(defmessage-handler core::List count primary
 "Returns the count of the given list." ()
 (length$ ?self:contents))
;------------------------------------------------------------------------------
(defmessage-handler core::List add primary
 (?first $?rest)
 (slot-direct-insert$ contents (+ 2 (length$ ?self:contents)) ?first $?rest))
;------------------------------------------------------------------------------
(defmessage-handler core::List add-to-front primary
 (?first $?rest)
 (slot-direct-insert$ contents 1 ?first $?rest))
;------------------------------------------------------------------------------
(defmessage-handler core::List clear primary ()
 (bind ?self:contents (create$)))
;------------------------------------------------------------------------------
(defmessage-handler core::List remove primary
 (?element $?elements)
 (bind ?self:contents (delete-member$ ?self:contents ?element $?elements)))
;------------------------------------------------------------------------------
(defmessage-handler core::List remove-range primary 
 (?start ?finish)
 (slot-direct-delete$ contents ?start ?finish))
;------------------------------------------------------------------------------
(defmessage-handler core::List remove-at primary 
 (?index)
 (slot-direct-delete$ contents ?index ?index))
;------------------------------------------------------------------------------
(defmessage-handler core::List element-at primary 
 (?index)
 (nth$ ?index ?self:contents))
;------------------------------------------------------------------------------
(defmessage-handler core::List contains primary 
 (?item)
 (member$ ?item ?self:contents))
;------------------------------------------------------------------------------
(defmessage-handler core::List contains-subset primary 
 ($?subset)
 (subsetp ?subset ?self:contents))
;------------------------------------------------------------------------------
(defmessage-handler core::List reverse primary
 "Reverses the order of the values in contents" ()
 (bind ?copy (create$))
 (progn$ (?element ?self:contents)
  (bind ?copy (create$ ?element ?copy)))
 (bind ?self:contents ?copy))
;------------------------------------------------------------------------------
(defmessage-handler core::List exists primary 
 "Takes in the name of a function and applies the function to each element of
 the list until either the list is completed or a value is found. If TRUE is
 returned then it means that the query was successful and an element was found
 that satsified the given function. If FALSE is returned then none of the
 elements in this list satisfy the given function."
 (?fn)
 (progn$ (?element ?self:contents)
  (if (funcall ?fn ?element) then
	(return TRUE)))
 (return FALSE))
;------------------------------------------------------------------------------
(defmessage-handler core::List apply primary
 "Returns a new multifield with the results of the given function applied to
 each element of this list"
 (?fn)
 (bind ?list (create$))
 (progn$ (?e ?self:contents)
  (bind ?list (create$ ?list (funcall ?fn ?e))))
 (return ?list))
;------------------------------------------------------------------------------
(defmessage-handler core::List first primary
 "Returns the first element of this list as a list." 
 () 
 (first$ ?self:contents))
;------------------------------------------------------------------------------
(defmessage-handler core::List rest primary
 "Returns a list missing the first element of the original list"
 ()
 (rest$ ?self:contents))
;------------------------------------------------------------------------------
