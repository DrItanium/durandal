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
; Hint.clp - An object used to signify something is a hint. It also has the
; ability to be reference counted if necessary
;------------------------------------------------------------------------------
(defclass core::Hint 
  "An object that makes it easy to create temporary entities."
  (is-a Object)
  (multislot values (visibility public))
  (slot reference-count (type NUMBER) (visibility public))
  (slot type (visibility public) (type SYMBOL))
  (slot point (visibility public) (type SYMBOL)))
;------------------------------------------------------------------------------
(defmessage-handler core::Hint increment-reference-count primary 
"Incrementes the reference count of the given hint.  If no parameters are given 
then the default is one. If a parameter is given then that is the value 
incremented by. If more than one parameter is given then those numbers are 
added together and then added to the current reference count"
                    ($?by)
                    (bind ?len (length$ ?by))
                    (switch (length$ ?by)
                            (case 0 then 
                              (bind ?self:reference-count 
                                    (+ 1 ?self:reference-count)))
                            (case 1 then 
                              (bind ?self:reference-count 
                                    (eval (format nil "(+ %i %i)", (first$ ?by)
                                                  ?self:reference-count))))
                            (default 
                              (bind ?v (eval (format nil "(+ %s)" 
                                                     (implode$ ?by))))
                              (bind ?self:reference-count 
                                    (+ ?self:reference-count ?v)))))
;------------------------------------------------------------------------------
(defmessage-handler core::Hint decrement-reference-count primary
"Decrements the reference count of the given hint.  If no parameters are given 
then the default is one. If a parameter is given then that is the value 
decremented by. If more than one parameter is given then those numbers are 
added together and then subtracted from the current reference count"
                    ($?by)
                    (bind ?len (length$ ?by))
                    (switch (length$ ?by)
                            (case 0 then 
                              (bind ?self:reference-count 
                                    (- ?self:reference-count 1)))
                            (case 1 then 
                              (bind ?self:reference-count 
                                    (eval (format nil "(- %i %i)" 
                                           ?self:reference-count
                                                  (first$ ?by)))))
                            (default 
                              ;add all of the element together so that the
                              ;subtraction is correct
                              (bind ?v (eval (format nil "(+ %s)" 
                                                     (implode$ ?by))))
                              (bind ?self:reference-count 
                                    (- ?self:reference-count ?v)))))
;------------------------------------------------------------------------------
