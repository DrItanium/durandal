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
(defrule wavefront-scheduling-pre-init::generate-wavefront-object
         (declare (salience 3))
         ?msg <- (message (to wavefront-scheduling)
                          (action can-wavefront-schedule)
                          (arguments ?p))
         (not (exists (object (is-a Wavefront) 
                              (parent ?p))))
         =>
         (retract ?msg)
         (make-instance of Wavefront 
                        (parent ?p)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::retract-wavefront-creation-action
         (declare (salience 3))
         ?msg <- (message (to wavefront-scheduling)
                          (action can-wavefront-schedule)
                          (arguments ?p))
         (exists (object (is-a Wavefront) 
                         (parent ?p)))
         =>
         (retract ?msg))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::assert-add-block-to-wavefront
         (declare (salience 2))
         (object (is-a Wavefront) 
                 (parent ?r))
         (object (is-a Region) 
                 (name ?r) 
                 (Entrances $? ?e $?))
         (object (is-a BasicBlock) 
                 (name ?e) 
                 (parent ?r))
         =>
         (assert (Add ?e to wavefront for ?r)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::assert-add-region-to-wavefront
         (declare (salience 2))
         (object (is-a Wavefront) 
                 (parent ?r))
         (object (is-a Region) 
                 (name ?r) 
                 (Entrances $? ?e $?))
         (object (is-a BasicBlock) 
                 (name ?e) 
                 (parent ~?r))
         (object (is-a Region) 
                 (parent ?r) 
                 (Entrances $? ?e $?) 
                 (name ?q))
         =>
         (assert (Add ?q to wavefront for ?r)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::convert-single-single=>multi
         (declare (salience 1))
         ?f0 <- (Add ?v0 to wavefront for ?r)
         ?f1 <- (Add ?v1&~?v0 to wavefront for ?r)
         =>
         (retract ?f0 ?f1)
         (assert (In wavefront ?r add ?v0 ?v1)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::convert-multi-multi=>multi
         (declare (salience 1))
         ?f0 <- (In wavefront ?r add $?a)
         ?f1 <- (In wavefront ?r add $?b)
         (test (neq ?f0 ?f1))
         =>
         (retract ?f0 ?f1)
         (assert (In wavefront ?r add $?a $?b)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::convert-single=>multi
         (declare (salience 1))
         ?f0 <- (Add ?v0 to wavefront for ?r)
         =>
         (retract ?f0)
         (assert (In wavefront ?r add ?v0)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-init::construct-wavefront-for-region
         (declare (salience -1))
         ?f <- (In wavefront ?r add $?w)
         ?wave <- (object (is-a Wavefront) 
                          (parent ?r))
         =>
         (retract ?f)
         (modify-instance ?wave (values $?w)))
;------------------------------------------------------------------------------
