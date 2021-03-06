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
; PathConstruction.clp - Contains rules devoted to starting the construction of
; a given path through a given region. Rewritten to take advantage of modules
;------------------------------------------------------------------------------
(defrule paths-conditional::initialize-path-construction-region
         (declare (salience 3))
         (message (to paths-conditional)
                  (action build-path))
         ?fct <- (message (to paths-conditional)
                          (action valid-pathing-target)
                          (arguments ?id))
         ?r0 <- (object (is-a Region) 
                        (id ?id) 
                        (Entrances $? ?a $?) 
                        (contents $? ?z $?))
         (object (is-a Region&~Loop) 
                 (id ?z) 
                 (parent ?id) 
                 (Entrances $? ?a $?))
         (object (is-a BasicBlock) 
                 (id ?a) 
                 (parent ?n&~?id))
         =>
         (retract ?fct)
         (make-instance of Path 
                        (parent ?id) 
                        (values ?z)))
;------------------------------------------------------------------------------
(defrule paths-conditional::initialize-path-construction-basicblock
         (declare (salience 3))
         (message (to paths-conditional)
                  (action build-path))
         ?fct <- (message (to paths-conditional)
                          (action valid-pathing-target)
                          (arguments ?id))
         ?r0 <- (object (is-a Region) 
                        (id ?id) 
                        (Entrances $? ?a $?))
         (object (is-a BasicBlock) 
                 (id ?a) 
                 (parent ?n))
         =>
         (retract ?fct)
         (make-instance of Path 
                        (parent ?n) 
                        (values ?a)))
;------------------------------------------------------------------------------
