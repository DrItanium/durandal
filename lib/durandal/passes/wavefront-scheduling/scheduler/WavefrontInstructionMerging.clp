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
; Contains rules associated with the act of actually scheduling instructions
; into blocks on the wavefront
;
; Written by Joshua Scoggins
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::AssertScheduleCPVIntoTargetBlock 
         (object (is-a Wavefront) 
                 (values $? ?e $?))
         (object (is-a Diplomat) 
                 (name ?e) 
                 (IsOpen TRUE))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e) 
                           (MovableCompensationPathVectors 
                             $?cpvs&:(<> 0 (length$ $?cpvs))))
         =>
         (modify-instance ?agObj (MovableCompensationPathVectors))
         (progn$ (?cpv $?cpvs)
                 (assert (message (to wavefront-scheduling)
                                  (action determine-schedule-style)
                                  (arguments ?cpv => ?e)))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::ScheduleStyleForCPVIsMove
         "This rule attempts to determine if the CPV should be moved into the 
         given block on the wavefront. If this is true then the fact to perform 
         this action will be asserted"
         ?fct <- (message (to wavefront-scheduling)
                          (action determine-schedule-style)
                          (arguments ?cpv => ?e))
         (object (is-a BasicBlock) 
                 (name ?e) 
                 (Paths $?paths))
         (object (is-a CompensationPathVector) 
                 (name ?cpv)
                 (Paths $?cpvPaths))
         ;the two sets are the same
         (test (equal$ ?paths ?cpvPaths))
         =>
         ;change the action...nothing more :D
         (modify ?fct (action move-instruction)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::ScheduleStyleForCPVIsCompensate
         "This rule attempts to determine if the CPV should be copied into the 
         given block on the wavefront. If this is true then the fact to perform 
         this action will be asserted."
         ?fct <- (message (to wavefront-scheduling)
                          (action determine-schedule-style)
                          (arguments ?cpv => ?e))
         (object (is-a BasicBlock) 
                 (name ?e) 
                 (Paths $?paths))
         (object (is-a CompensationPathVector) 
                 (name ?cpv)
                 (Paths $?cpvPaths))
         ;there are more paths in the CPV than in the block
         (test (subsetp ?paths ?cpvPaths))
         =>
         (modify ?fct (action clone-instruction)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::RemoveScheduleStyleForCPV
         (declare (salience 1))
         ?fct <- (message (to wavefront-scheduling)
                          (action determine-schedule-style)
                          (arguments ?cpv => ?e))
         (object (is-a BasicBlock) 
                 (name ?e) 
                 (Paths $?paths))
         (object (is-a CompensationPathVector) 
                 (name ?cpv)
                 (parent ?i)
                 (Paths $?cpvPaths))
         (test (not (subsetp ?paths ?cpvPaths)))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il))
         =>
         ;TODO: Put code in here to delete a given instruction from the target
         ;      instruction list as well. 
         ;
         ; Eventually, I will detect if we are in a loop. If we are then it is
         ; necessary to figure out which paths remain in the loop and those
         ; that exit. 
         ;this should prevent a potential infinite loop
         ;(printout t "Preventing " ?i " from being scheduled into " ?e crlf)
         (retract ?fct)
         (bind ?ind (member$ ?i $?il))
         (if ?ind then (slot-delete$ ?agObj InstructionList ?ind ?ind)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::MoveInstructionIntoBlock
         "Moves the given object into bottom of the given block"
         ?fct <- (message (to wavefront-scheduling)
                          (action move-instruction)
                          (arguments ?cpv => ?e))
         ?newBlock <- (object (is-a BasicBlock) 
                              (name ?e) 
                              (contents $?blockBefore ?last)
                              (Produces $?nBProds))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionPropagation $?agIP)
                           (ScheduledInstructions $?agSI)
                           (ReplacementActions $?agRA))
         ?terminator <- (object (is-a TerminatorInstruction) 
                                (pointer ?tPtr) 
                                (name ?last) 
                                (TimeIndex ?ti) 
                                (parent ?e))
         ?cpvObject <- (object (is-a CompensationPathVector) 
                               (name ?cpv) 
                               (parent ?inst)
                               (ScheduleTargets $?cpvST)
                               (Aliases $?cpvAliases))
         ?newInst <- (object (is-a Instruction) 
                             (name ?inst) 
                             (pointer ?nPtr) 
                             (parent ?otherBlock) 
                             (DestinationRegisters ?register) 
                             (Consumers $?niConsumers))
         ?oldBlock <- (object (is-a BasicBlock) 
                              (name ?otherBlock) 
                              (Produces $?pBefore ?inst $?pRest)
                              (contents $?before ?inst $?rest))
         ;TODO: add another rule where we have to update the consumers list as
         ;      well
         =>
         (object-pattern-match-delay
           ;(printout t "Scheduled " ?inst " into " ?e crlf)
           (modify-instance ?terminator (TimeIndex (+ ?ti 1)))
           ;(modify-instance ?newBlock (Produces ?nBProds ?register))
           (modify-instance ?oldBlock (contents $?before $?rest) 
                            (Produces $?pBefore $?pRest))
           ;(modify-instance ?cpvObject (Paths))
           (modify ?fct (action remove-evidence)
                   (arguments ?inst => $?niConsumers))
           (assert (message (to wavefront-scheduling)
                            (action recompute-block)
                            (arguments ?otherBlock)))
           (bind ?class (class ?newInst))
           (if (eq StoreInstruction ?class) then 
             (modify-instance ?agObj 
                              (ScheduledInstructions $?agSI ?inst ?register)
                              (ReplacementActions $?agRA ?inst ?inst !))
             ;(slot-insert$ ?agObj ScheduledInstructions 1 ?inst ?register)
             (modify-instance ?newBlock 
                              (Produces $?nBProds ?register)
                              (contents $?blockBefore ?inst ?last))
             (modify-instance ?cpvObject 
                              (Paths)
                              (ScheduleTargets ?cpvST ?e ?inst)
                              (Aliases $?cpvAliases ?inst ?e))
             (llvm-unlink-and-move-instruction-before ?nPtr ?tPtr)
             ;(slot-insert$ ?cpvObject ScheduleTargets 1 ?e ?inst)
             ;(slot-insert$ ?cpvObject Aliases 1 ?inst ?e)
             ;(slot-insert$ ?agObj ReplacementActions 1 ?inst ?inst !)
             else
             (bind ?newName (sym-cat movedinstruction. (gensym*) . ?inst))
             (modify-instance ?cpvObject (Paths)
                              (ScheduleTargets ?cpvST ?e ?newName)
                              (Aliases ?cpvAliases ?newName ?e))
             ;(slot-insert$ ?cpvObject ScheduleTargets 1 ?e ?newName)
             ;(slot-insert$ ?cpvObject Aliases 1 ?newName ?e)
             ;(slot-insert$ ?agObj ReplacementActions 1 ?inst ?newName !)
             (modify-instance ?newBlock 
                              (Produces $?nBProds ?register)
                              (contents $?blockBefore ?newName ?last))
             (bind ?newPtr (llvm-clone-instruction ?nPtr ?newName))
             ;purge the list of producers and consumers
             (duplicate-instance ?newInst to ?newName 
                                 (name ?newName) 
                                 (Name ?newName)
                                 (pointer ?newPtr) 
                                 (Producers) 
                                 (Consumers)
                                 (NonLocalDependencies) 
                                 (LocalDependencies)
                                 (TimeIndex ?ti) 
                                 (parent ?e))
             (llvm-move-instruction-before ?newPtr ?tPtr)
             (slot-insert$ ?oldBlock UnlinkedInstructions 1 ?inst)
             (modify-instance ?agObj (ReplacementActions $?agRA ?inst ?newName !)
                              (InstructionPropagation $?agIP ?inst ?newName ?e !)
                              (ScheduledInstructions $?agSI ?inst)))))
;(slot-insert$ ?agObj InstructionPropagation 1 ?inst ?newName ?e !)
;(slot-insert$ ?agObj ScheduledInstructions 1 ?inst))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::CloneInstructionIntoBlock
         "Moves the given object into bottom of the given block"
         ?fct <- (message (to wavefront-scheduling)
                          (action clone-instruction)
                          (arguments ?cpv => ?e))
         ?newBlock <- (object (is-a BasicBlock) 
                              (name ?e) 
                              (contents $?blockBefore ?last))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e))
         ?terminator <- (object (is-a TerminatorInstruction) 
                                (pointer ?tPtr) 
                                (name ?last) 
                                (TimeIndex ?ti) 
                                (parent ?e))
         ?cpvObject <- (object (is-a CompensationPathVector) 
                               (name ?cpv) 
                               (parent ?inst) 
                               (Paths $?cpvPaths))
         ?newInst <- (object (is-a Instruction) 
                             (name ?inst) 
                             (pointer ?nPtr) 
                             (parent ?otherBlock) 
                             (DestinationRegisters ?register))
         =>
         ;we also need to update all CPVs within 
         (object-pattern-match-delay
           (bind ?newName (sym-cat compensation.copy. (gensym*) . ?inst))
           ;(printout t "Scheduled " ?inst " into " ?e " from " ?otherBlock 
           ;            " as " ?newName crlf)
           (bind ?newPtr (llvm-clone-instruction ?nPtr ?newName))
           ;purge the list of producers and consumers
           (duplicate-instance ?newInst to ?newName 
                               (name ?newName) 
                               (Name ?newName)
                               (pointer ?newPtr) 
                               (parent ?e)
                               (TimeIndex (+ ?ti 1)))
           (llvm-move-instruction-before ?newPtr ?tPtr)
           ;we add the original name so that we don't have to do
           ; an insane number of updates to the CPVs that follow
           ; this object
           (bind ?class (class ?newInst))
           (if (eq StoreInstruction ?class) then 
             (slot-insert$ ?agObj ScheduledInstructions 1 ?inst ?register)
             else
             (slot-insert$ ?agObj InstructionPropagation 1 ?inst ?newName ?e !)
             (slot-insert$ ?agObj ScheduledInstructions 1 ?inst))
           (slot-insert$ ?newBlock Produces 1 ?register)
           (modify-instance ?newBlock (contents $?blockBefore ?newName ?last))
           (slot-insert$ ?cpvObject ScheduleTargets 1 ?e ?newName)
           (slot-insert$ ?cpvObject Aliases 1 ?newName ?e) 
           (slot-insert$ ?agObj ReplacementActions 1 ?inst ?newName !)
           (modify ?fct (action recompute-block)
                   (arguments ?otherBlock))
           (assert (message (to wavefront-scheduling)
                            (action reopen-blocks)
                            (arguments ?cpv)))
           (bind ?leftOvers (create$))
           (progn$ (?z ?cpvPaths)
                   (bind ?cPath (instance-address * 
                                 (symbol-to-instance-name ?z)))
                   (if (not (member$ ?e (send ?cPath get-values))) then
                     (bind ?leftOvers (insert$ ?leftOvers 1 ?z))))
           (modify-instance ?cpvObject (Paths ?leftOvers))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::FAILURE-CLONE
         (declare (salience -768))
         ?fct <- (message (to wavefront-scheduling)
                          (action clone-instruction)
                          (arguments ?cpv => ?e))
         (object (is-a CompensationPathVector) 
                 (name ?cpv) 
                 (parent ?p))
         =>
         (printout t "ERROR: Didn't clone " ?p " into " ?e crlf)
         (halt))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::FAILURE-MOVE
         (declare (salience -768))
         ?fct <- (message (to wavefront-scheduling)
                          (action move-instruction)
                          (arguments ?cpv => ?e))
         (object (is-a CompensationPathVector) 
                 (name ?cpv) 
                 (parent ?p))
         =>
         (printout t "ERROR: Didn't move " ?p " into " ?e crlf)
         (halt))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge::FAILURE-SCHEDULE-STYLE
         (declare (salience -768))
         (message (to wavefront-scheduling)
                  (action determine-schedule-style)
                  (arguments ?cpv => ?e))
         (object (is-a CompensationPathVector) 
                 (name ?cpv) 
                 (parent ?p)
                 (Aliases $?aliases)
                 (Paths $?cpvPaths))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e))
         (object (is-a Instruction) 
                 (name ?p) 
                 (parent ?bb))
         (object (is-a BasicBlock) 
                 (name ?e) 
                 (parent ?r) 
                 (Paths $?paths))
         (object (is-a Wavefront) 
                 (parent ?r) 
                 (values $?z) 
                 (Closed $?y))
         (object (is-a Region) 
                 (name ?r) 
                 (Entrances ?x $?))
         (object (is-a BasicBlock) 
                 (name ?x) 
                 (Paths $?allPaths))
         =>
         (printout t "ERROR: Couldn't figure out scheduling stype for " ?p 
                   " which is targeted for " ?e crlf
                   "Blocks on the wavefront = " ?z crlf
                   "Closed Blocks = " ?y crlf
                   "For reference ?cpvPaths = " ?cpvPaths crlf
                   "For reference aliases of ?cpv are = " $?aliases crlf
                   "For reference ?paths = " ?paths crlf
                   "Parent of " ?p " is " ?bb crlf
                   "Printing out the path aggregate for " ?p crlf )
         (send ?pa print)
         (progn$ (?apath ?allPaths)
                 (bind ?aObj (instance-name (symbol-to-instance-name ?apath)))
                 (printout t "  " ?apath " = " (send ?aObj get-contents) crlf))
         (facts)
         (halt))
;------------------------------------------------------------------------------
