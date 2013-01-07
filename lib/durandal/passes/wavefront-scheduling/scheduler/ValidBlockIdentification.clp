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
;defmodule is wavefront-scheduling-identify
(defrule wavefront-scheduling-identify::AssertIdentifySpansInitial
         (declare (salience 5))
         (object (is-a Wavefront) (parent ?r) (contents $? ?e $?))
         ;select only BasicBlocks
         (object (is-a BasicBlock) (ID ?e))
         =>
         (assert (Picked ?e for ?r)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-identify::IdentifySpanSkips-InvalidBlock
         (declare (salience 4))
         ?fct <- (Picked ?e for ?r)
         ?bb <- (object (is-a BasicBlock) 
                        (id ?e) 
                        (Successors $?a&:(not (eq (length$ $?a) 1))))
         =>
         ;we don't need to assert anything since the block isn't going to get
         ;scheduled
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-identify::IdentifySpans
         (declare (salience 4))
         ?fct <- (Picked ?block for ?)
         ?bb <- (object (is-a BasicBlock) 
                        (id ?block) 
                        (Successors ?) ; we only have one successor
                        (Paths $?paths))
         =>
         (retract ?fct)
         (modify-instance ?bb (IsOpen TRUE))
         ; originally I had a rule here that took a fact asserted in the form
         ; ?e => $?paths which would be inverted individually by separate rule
         ; fires. While this is more like an expert system, it is extremely
         ; inefficient because the agenda has to be updated after each rule
         ; fire. Using this technique allows me to minimize the number of rule
         ; fires to one yet do the same amount of work.
         (progn$ (?path $?paths)
                 (assert (message (to wavefront-scheduling)
                                  (action check-path)
                                  (arguments ?path for block ?block)))))
;------------------------------------------------------------------------------
; defmodule is wavefront-scheduling-pathing
(defrule wavefront-scheduling-pathing::GetFactsBeforePathing
         (declare (salience 10000))
         (message (action Debug))
         (message (action Facts))
         =>
         (printout t "BEFORE: Wavefront Pathing " crlf crlf)
         (facts)
         (printout t crlf crlf))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pathing::DispatchDivideBlock
         (declare (salience 200))
         ?fct <- (message (to wavefront-scheduling)
                          (action check-path)
                          (arguments ?path for block ?block))
         (object (is-a Path) (id ?path) (contents $? ?block $?rest))
         ;we don't need to explicitly match for ?block
         =>
         (retract ?fct)
         (assert (message (to wavefront-scheduling-pathing)
                          (action scan-path)
                          (arguments ?p ?e => $?rest))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pathing::AnalyzePathElements
         ?fct <- (message (to wavefront-scheduling-pathing)
                          (action scan-path)
                          (arguments ?p ?e => ?curr $?rest))
         ?bb <- (object (is-a BasicBlock) 
                        (id ?curr) 
                        (Successors $?succ)
                        (HasCallBarrier ?hcb)
                        (HasMemoryBarrier ?hmb))
         =>
         (bind ?sLen (length$ $?succ))
         (if (= 0 ?sLen) then
           (assert (message (to wavefront-scheduling)
                            (action completely-invalid-blocks)
                            (arguments ?e => ?curr)))
           (retract ?fct)
           else
           (if (or (> ?sLen 1) ?hcb) then
             (assert (message (to wavefront-scheduling)
                              (action completely-invalid-blocks)
                              (arguments ?e => $?rest))
                     (message (to wavefront-scheduling)
                              (action potentially-valid-blocks)
                              (arguments ?e => ?curr)))
             (retract ?fct)
             else 
             (if ?hmb then
               (assert (message (to wavefront-scheduling)
                                (action element-has-memory-barrier)
                                (arguments ?curr => ?e))))
             (modify ?fct (arguments ?p ?e => $?rest))
             (assert (message (to wavefront-scheduling)
                              (action potentially-valid-blocks)
                              (arguments ?e => ?curr))))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pathing::AnalyzePathElements-Region
         ?fct <- (message (to wavefront-scheduling-pathing)
                          (action scan-path)
                          (arguments ?p ?e => ?curr $?rest))
         ?bb <- (object (is-a Region)
                        (id ?curr)
                        (HasCallBarier ?hcb)
                        (HasMemoryBarrier ?hmb))
         =>
         (if ?hcb then
           (assert (message (to wavefront-scheduling)
                            (action completely-invalid-blocks)
                            (arguments ?e => $?rest))
                   (message (to wavefront-scheduling)
                            (action potentially-valid-blocks)
                            (arguments ?e => ?curr)))
           (retract ?fct)
           else
           (if ?hmb then
             (assert (message (to wavefront-scheduling)
                              (action element-has-memory-barrier)
                              (arguments ?curr => ?e))))
           (modify ?fct (arguments ?p ?e => $?rest))
           (assert (message (to wavefront-scheduling)
                            (action potentially-valid-blocks)
                            (arguments ?e => ?curr)))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pathing::RetractCompletedFact
         ?fct <- (message (to wavefront-scheduling)
                          (action scan-path)
                          (arguments ? ? =>))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pathing::PrintoutCompletedFacts 
         (declare (salience -999))
         (message (action Debug))
         (message (action Facts))
         =>
         (printout t "AFTER: Wavefront Pathing" crlf crlf)
         (facts)
         (printout t crlf crlf))
;------------------------------------------------------------------------------
