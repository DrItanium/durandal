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
; PassHandler.clp - Contains the logic necessary to handle pass execution
;------------------------------------------------------------------------------
(defrule pipeline::build-pass-descriptor
			?fct <- (message (to pipeline) 
								  (action pass-description) 
								  (arguments $?passes))
			=>
			(retract ?fct)
			(make-instance of pass-descriptor (passes $?passes)))
;------------------------------------------------------------------------------
(defrule pipeline::next-pass
			?obj <- (object (is-a pass-descriptor) (passes ?first $?rest))
			=>
			(assert (message (from pipeline) (to ?first) (action initial-fact)))
			(modify-instance ?obj (passes $?rest))
			;remove what is currently on the focus stack
			(pop-focus)
			(focus ?first pipeline))
;------------------------------------------------------------------------------
(defrule pipeline::terminate-pass
			?obj <- (object (is-a pass-descriptor) (passes))
			=>
			(unmake-instanec ?obj)
			(pop-focus))
;------------------------------------------------------------------------------