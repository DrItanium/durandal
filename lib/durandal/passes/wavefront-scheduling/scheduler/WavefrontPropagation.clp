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
(defrule wavefront-scheduling-identify::PropagateAggregateInformation
         "Pulls instruction propagation information from all elements on paths 
         that immediately precede this element on the wavefront and merges it 
         into the path aggregate itself"
         (message (to wavefront-scheduling)
                  (action propagate-aggregates)
                  (arguments ?e))
         ;if this element is on the wavefront then we can be certain that all 
         ;of it's predecessors are above it. That is the definition of being on
         ;the wavefront
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e))
         (object (is-a Diplomat) 
                 (name ?e) 
                 (PreviousPathElements $? ?z $?))
         (object (is-a PathAggregate) 
                 (parent ?z) 
                 (InstructionPropagation $? ?targ ?alias ? ! $?))
         =>
         ;replace parent blocks of previous path elements with the name of the
         ;element this was acquired from
         ;(printout t "Put (" ?targ " " ?alias " " ?z "! ) into " ?pp crlf)
         (slot-insert$ ?pa InstructionPropagation 1 ?targ ?alias ?z !))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-identify::RetractAggregationInformation
         (declare (salience -50))
         ?fct <- (message (to wavefront-scheduling)
                          (action propagate-aggregates)
                          (arguments ?))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-identify::AssertPhiNodePropagationPredicateIsBlock
         (declare (salience 1))
         (object (is-a Wavefront) 
                 (parent ?r) 
                 (values $? ?e $?))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e) 
                        (InstructionPropagation ?targ ?alias ?pred ! $?rest))
         =>
         (modify-instance ?pa (InstructionPropagation $?rest))
         (assert (message (to wavefront-scheduling)
                          (action propagation-target)
                          (arguments target ?targ 
                                     alias ?alias 
                                     from ?pred 
                                     for ?e))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node::RemoveDuplicateElements 
         "Removes propagation targets that represent the same value but come from
         different previous blocks/regions. When this is seen it's not necessary to
         create a phi node. This is evident because the aliases are the same across
         multiple paths of execution."
         ?f0 <- (message (to wavefront-scheduling)
                         (action propagation-target)
                         (arguments target ?t 
                                    alias ?a 
                                    from ?p0 
                                    for ?b))
         ?f1 <- (message (to wavefront-scheduling)
                         (action propagation-target)
                         (arguments target ?t 
                                    alias ?a 
                                    from ?p1&~?p0 
                                    for ?b))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?b) 
                        (InstructionPropagation $?ip))
         =>
         (retract ?f0 ?f1)
         (modify-instance ?pa (InstructionPropagation $?ip ?t ?a ?b !)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node::MergePhiNodePropagationWithOtherPropagation
         ?f0 <- (message (to wavefront-scheduling)
                         (action propagation-target)
                         (arguments target ?t 
                                    alias ?a0
                                    from ?p0
                                    for ?b))
         ?f1 <- (message (to wavefront-scheduling)
                         (action propagation-target)
                         (arguments target ?t 
                                    alias ?a1&~?a0
                                    from ?p1&~?p0 
                                    for ?b))
         =>
         (retract ?f0 ?f1)
         (assert (message (to wavefront-scheduling)
                          (action create-phi-node)
                          (arguments target-instruction ?t
                                     block ?b => ?a0 ?p0 ?a1 ?p1))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node::MergePhiNodePropagationWithCreateStatement
         ?f0 <- (message (to wavefront-scheduling)
                         (action propagation-target)
                         (arguments target ?t 
                                    alias ?a0
                                    from ?p0
                                    for ?b))
         ?f1 <- (message (to wavefront-scheduling)
                         (action create-phi-node)
                         (arguments target-instruction ?t 
                                    block ?b => $?targets))
         =>
         (retract ?f0)
         (modify ?f1 (arguments target-instruction ?t
                                block ?b => $?targets ?a0 ?p0)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node::PutUnfulfilledItemsBackIntoPropagationList
         (declare (salience -10))
         ?f0 <- (message (to wavefront-scheduling)
                         (action propagation-target)
                         (arguments target ?t 
                                    alias ?a0
                                    from ?p0
                                    for ?b))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?b)
                        (InstructionPropagation $?ip))
         =>
         (retract ?f0)
         (modify-instance ?pa (InstructionPropagation $?ip ?t ?a0 ?b !)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node::NamePhiNodeFromCreateStatement-NotOriginalBlock
         (declare (salience -12))
         ?fct <- (message (to wavefront-scheduling)
                          (action create-phi-node)
                          (arguments target-instruction ?t 
                                     block ?b => $?elements))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?b)
                           (ScheduledInstructions $?si)
                           (InstructionPropagation $?ip)
                           (ReplacementActions $?ra))

         ?bb <- (object (is-a BasicBlock) 
                        (name ?b) 
                        (contents ?first $?rest)
                        (UnlinkedInstructions 
                          $?ui&:(not (member$ ?t $?ui))))
         (object (is-a Instruction) 
                 (name ?first) 
                 (pointer ?bPtr))
         (object (is-a Instruction) 
                 (name ?t) 
                 (Type ?ty))
         (object (is-a LLVMType) 
                 (name ?ty) 
                 (pointer ?typePtr))
         =>
         (modify ?fct (action update-block-duration)
                 (arguments ?b))
         (bind ?name (sym-cat phi. (gensym*) . ?t))
         (bind ?count (/ (length$ $?elements) 2))
         ;make the phinode in LLVM and get it's address back
         (bind ?phiPtr 
               (llvm-make-phi-node ?name
                                   ?typePtr 
                                   ?count
                                   ?bPtr 
                                   (symbol-to-pointer-list $?elements)))
         ;build a CLIPS representation of it
         (make-instance ?name of PhiNode 
                        (parent ?b)
                        (TimeIndex 0)
                        (pointer ?phiPtr)
                        (IncomingValueCount ?count)
                        (Operands $?elements))
         ;we've scheduled the given original instruction into this block
         ; although it's just a ruse
         (modify-instance ?agObj (ScheduledInstructions $?si ?t)
                          (InstructionPropagation $?ip ?t ?name ?b !)
                          (ReplacementActions $?ra ?t ?name !))
         (modify-instance ?bb (contents ?name ?first $?rest)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node::NamePhiNodeFromCreateStatement-OriginalBlock
         (declare (salience -12))
         ?fct <- (message (to wavefront-scheduling)
                          (action create-phi-node)
                          (arguments target-instruction ?t 
                                     block ?b => $?elements))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?b)
                           (ScheduledInstructions $?si))
         ?bb <- (object (is-a BasicBlock) 
                        (name ?b) 
                        (contents ?first $?rest) 
                        (UnlinkedInstructions 
                          $?ui&:(not (member$ ?t $?ui))))
         (object (is-a Instruction) 
                 (name ?first) 
                 (pointer ?bPtr))
         ?tObj <- (object (is-a Instruction) 
                          (name ?t) 
                          (Type ?ty) 
                          (pointer ?tPtr))
         (object (is-a LLVMType) 
                 (name ?ty) 
                 (pointer ?dataType))
         =>
         (modify ?fct (action update-block-duration)
                 (arguments ?b))
         (bind ?name (sym-cat phi. (gensym*) . ?t))
         (bind ?count (/ (length$ $?elements) 2))
         (bind ?pointers (symbol-to-pointer-list ?elements))
         (bind ?phiPointer 
               (llvm-make-phi-node ?name ?dataType ?count ?bPtr ?pointers))
         (bind ?phiObj (make-instance ?name of PhiNode 
                                      (parent ?b)
                                      (TimeIndex 0)
                                      (pointer ?phiPointer)
                                      (IncomingValueCount ?count)
                                      (Operands $?elements)))
         (llvm-replace-all-uses ?tPtr ?phiPointer)
         (llvm-unlink-and-delete-instruction ?tPtr)
         (unmake-instance ?tObj)
         (modify-instance ?agObj (ScheduledInstructions $?si ?t))
         (modify-instance ?bb (contents ?name ?first $?rest)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-phi-node-update::ReindexBasicBlock 
         ?fct <- (message (to wavefront-scheduling)
                          (action update-block-duration)
                          (arguments ?b))
         (object (is-a BasicBlock) 
                 (name ?b) 
                 (contents $?c))
         =>
         ;this is very much procedural but I frankly don't care
         ;anymore. 
         (progn$ (?t ?c)
                 (bind ?obj (instance-address * (symbol-to-instance-name ?t)))
                 (modify-instance ?obj (TimeIndex (- ?t-index 1))))
         (retract ?fct))
;------------------------------------------------------------------------------
