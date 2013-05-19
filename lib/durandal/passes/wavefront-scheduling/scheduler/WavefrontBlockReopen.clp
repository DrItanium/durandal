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
(defrule wavefront-scheduling-reopen-blocks::AssertReopenBlocksOnWavefront
         ?fct <- (message (to wavefront-scheduling)
                          (action reopen-blocks)
                          (arguments ?cpv))
         ?obj <- (object (is-a CompensationPathVector) 
                         (name ?cpv) 
                         (Failures $?failures))
         =>
         (modify-instance ?obj (Failures))
         (modify ?fct (action reopen)
                 (arguments ?cpv => $?failures)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-reopen-blocks::ReopenBlockOnWavefront
         ?fct <- (message (to wavefront-scheduling)
                          (action reopen)
                          (arguments ?cpv => ?fail $?failures))
         ?wave <- (object (is-a Wavefront) 
                          (Closed $?a ?fail $?b)
                          (values $?cnts))
         ?bb <- (object (is-a BasicBlock) 
                        (name ?fail) 
                        (IsOpen FALSE))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?fail)
                        (InstructionList $?il)
                        (ImpossibleCompensationPathVectors $?icpv))
         =>
         (modify-instance ?bb (IsOpen TRUE))
         (bind ?qs (create$))
         (progn$ (?q ?icpv)
                 (bind ?qObj (instance-address (symbol-to-instance-name ?q)))
                 (bind ?qs (create$ ?qs (send ?qObj get-parent))))
         (modify-instance ?pa (ImpossibleCompensationPathVectors)
                          (InstructionList $?il ?qs)
                          (TargetCompensationPathVectors $?icpv))
         (modify-instance ?wave (contents $?cnts ?fail) 
                          (Closed ?a ?b))
         (modify ?fct (arguments ?cpv => $?failures)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-reopen-blocks::ReaddFailureToCPV
         ?fct <- (message (to wavefront-scheduling)
                          (action reopen)
                          (arguments ?cpv => ?fail $?failures))
         ?wave <- (object (is-a Wavefront) 
           (Closed $?c&:(not (member$ ?fail $?c))))
         ?obj <- (object (is-a CompensationPathVector) 
           (name ?cpv)
           (Failures $?z))
         =>
         (modify ?fct (arguments ?cpv => $?failures))
         (modify-instance ?obj (Failures $?z ?fail)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-reopen-blocks::RetractEmptyReopenFact
         ?fct <- (message (to wavefront-scheduling)
                          (action reopen)
                          (arguments ? =>))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
