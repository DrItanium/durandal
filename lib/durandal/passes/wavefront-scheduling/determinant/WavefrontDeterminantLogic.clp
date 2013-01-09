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
; FrequencyAnalysis.clp - Determines which regions to apply wavefront
; scheduling to 
; Written by Joshua Scoggins (6/30/2012) 
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-determinant::InstanceFrequencyCounter
         "Creates a frequency counter hint for basic blocks"
         (declare (salience 2))
         (object (is-a Region) 
                 (id ?p) 
                 (class Region))
         (not (exists (object (is-a FrequencyAnalysis) 
                              (parent ?p))))
         =>
         (make-instance of FrequencyAnalysis (parent ?p)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-determinant::IncrementFrequencyCounter-BasicBlock
         "Goes through a given Region counting the number of basic blocks found
         within the region. Valid blocks are blocks that contain more than one 
         instruction as we don't want to count JS nodes as they don't usually 
         contain code."
         (declare (salience 1))
         (object (is-a Region) 
                 (id ?p) 
                 (class Region) 
                 (contents $? ?t $?))
         (object (is-a BasicBlock) 
                 (id ?t) 
                 (parent ?p) 
                 (contents $?insts&:(> (length$ $?insts) 1)))
         ?wa <- (object (is-a FrequencyAnalysis) 
                        (parent ?p))
         =>
         (send ?wa increment-frequency))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-determinant::ImplyEnoughBlocks
         "There are enough blocks within the target region to make it a 
         candidate for wavefront scheduling. Make a hint that says this."
         ?fa <- (object (is-a FrequencyAnalysis) 
                        (parent ?p) 
                        (frequency ?z&:(and (< ?z 100) (> ?z 1))))
         ?region <- (object (is-a Region) 
                            (id ?p)
                            (class Region))
         =>
         (assert (message (to wavefront-scheduling)
                          (action can-wavefront-schedule)
                          (arguments ?p))
                 (message (to paths-conditional)
                          (action valid-pathing-target)
                          (arguments ?p))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-determinant-cleanup::cleanup-frequency-analysis
         ?fct <- (message (from pipeline) 
                          (to wavefront-scheduling-determinant-cleanup)
                          (action initial-fact))
         =>
         (retract ?fct)
         (progn$ (?fa (find-all-instances ((?freq FrequencyAnalysis)) TRUE))
                 (unmake-instance ?fa)))
;------------------------------------------------------------------------------
