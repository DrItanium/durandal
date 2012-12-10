
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
; PathBuilding.clp - Contains rules that build up the paths through the given
; region 
;------------------------------------------------------------------------------
(defrule path::add-to-path-copy
			"Makes a copy of the current path object and concatenates the symbol 
			in question to the end of the list. This rule is fired when the 
			reference count of the given path object is greater than one."
			(declare (salience 1))
			?fct <- (Add ?next to ?id)
			?hint <- (object (is-a Path) (closed FALSE) (id ?id) (parent ?p) 
								  (reference-count ?rc&:(> ?rc 1))
								  (values $?contents))
			=>
			(send ?hint decrement-reference-count)
			(retract ?fct)
			(make-instance of Path (parent ?p) (values $?contents ?next)))
;------------------------------------------------------------------------------
(defrule path::add-to-path-concat
			"Concatenates the next element of the path directly to the original 
			path object. This rule fires when the reference count of the path is 
			equal to one"
			(declare (salience 1))
			?fct <- (Add ?next to ?id)
			?hint <- (object (is-a Path) (closed FALSE) (id ?id) 
								  (reference-count 1) (values $?values))
			=>
			(retract ?fct)
			(modify-instance ?hint (reference-count 0) (values $?values ?next)))
;------------------------------------------------------------------------------
(defrule path::close-path-update
			"Closes a path via an in-place update"
			(declare (salience 1))
			?fct <- (Close ?id with ?bb)
			?hint <- (object (is-a Path) (closed FALSE) (id ?id) 
								  (reference-count 1))
			=>
			(retract ?fct)
			(modify-instance ?hint (reference-count 0) (closed TRUE) (exit ?bb)))
;------------------------------------------------------------------------------
(defrule path::close-path-copy 
			"Closes a path by making a copy of the target path"
			(declare (salience 1))
			?fct <- (Close ?id with ?bb)
			?hint <- (object (is-a Path) (id ?id) (closed FALSE) (parent ?p)
								  (reference-count ?rc&:(> ?rc 1))
								  (values $?values))
			=>
			(send ?hint decrement-reference-count)
			(retract ?fct)
			(make-instance of Path (parent ?p) 
			                       (closed TRUE) 
										  (exit ?bb) 
			                       (values $?values)))
;------------------------------------------------------------------------------
