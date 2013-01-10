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
; The idea for wavefront scheduling is that we pull a region, construct
; schedules for all of the blocks within the region, identify blocks that can
; relinquish control over instructions and move them up and out of the given
; block and into blocks on the wavefront.
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-strip::MergePotentiallyValidBlocks
         (declare (salience 2))
         ?pv0 <- (message (to wavefront-scheduling)
                          (action potentially-valid-blocks)
                          (arguments ?e => $?t))
         ?pv1 <- (message (to wavefront-scheduling)
                          (action potentially-valid-blocks)
                          (arguments ?e => $?q))
         (test (and (neq ?pv0 ?pv1) (subsetp ?t ?q)))
         =>
         (retract ?pv1)
         ;make sure that we get matches again!
         (modify ?pv0 (arguments ?e => ?q)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-strip::MergeMemoryPotentiallyValidBlocks
         ?pv0 <- (message (to wavefront-scheduling)
                          (action memory-potentially-valid-blocks)
                          (arguments ?e => $?t))
         ?pv1 <- (message (to wavefront-scheduling)
                          (action memory-potentially-valid-blocks)
                          (arguments ?e => $?q))
         (test (and (neq ?pv0 ?pv1) (subsetp ?t ?q)))
         =>
         (retract ?pv1)
         ;make sure that we get matches again!
         (modify ?pv0 (arguments ?e => $?q)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-strip::MergeCompletelyInvalid
         ?f0 <- (message (to wavefront-scheduling)
                         (action completely-invalid-blocks)
                         (arguments ?e => $?t))
         ?f1 <- (message (to wavefront-scheduling)
                         (action completely-invalid-blocks)
                         (arguments ?e => $?q))
         (test (and (neq ?f0 ?f1) (subsetp ?t ?q)))
         =>
         (retract ?f1)
         (modify ?f0 (arguments ?e => $?q)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-strip::RetractPotentiallyValidBlocksThatAreCompletelyEnclosed
         (message (to wavefront-scheduling)
                  (action completely-invalid-blocks)
                  (arguments ?e => $?t))
         ?f <- (message (to wavefront-scheduling)
                        (action potentially-valid-blocks)
                        (arguments ?e => $?q&:(subsetp ?q ?t)))
         =>
         (retract ?f))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-strip::StripoutIndividualElementsFromPotentiallyValid
         (declare (salience -1))
         ?f <- (message (to wavefront-scheduling)
                        (action potentially-valid-blocks)
                        (arguments ?e => $?before ?car $?rest))
         (message (to wavefront-scheduling)
                  (action completely-invalid-blocks)
                  (arguments ?e => $? ?car $?))
         =>
         (modify ?f (arguments ?e => $?before $?rest)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-strip::RetractEmptyPotentiallyValid
         (declare (salience -100))
         ?f <- (message (to wavefront-scheduling)
                        (action potentially-valid-blocks)
                        (arguments ? => ))
         =>
         (retract ?f))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-inject::InjectPotentiallyValidBlocks-Complete
         ?fct <- (message (to wavefront-scheduling)
                          (action potentially-valid-blocks)
                          (arguments ?e => $?blocks))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e) 
                        (PotentiallyValid $?pv))
         =>
         (retract ?fct)
         (bind ?pvLen (length$ ?pv))
         (if (= 0 ?pvLen) then
           (modify-instance ?pa (PotentiallyValid $?blocks))
           else 
           (bind ?pvs $?pv)
           (bind ?newIndex (+ ?pvLen 1))
           (progn$ (?block ?blocks)
                   (if (not (member$ ?block ?pvs)) then 
                     (bind ?pvs (insert$ ?pvs ?newIndex ?block))
                     (bind ?newIndex (+ ?newIndex 1))))
           (modify-instance ?pa (PotentiallyValid ?pvs))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-inject::InjectCompletelyInvalidBlocks-Complete
         ?fct <- (message (to wavefront-scheduling)
                          (action completely-invalid-blocks)
                          (arguments ?e => $?blocks))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e) 
                        (CompletelyInvalid $?ci))
         =>
         (retract ?fct)
         (bind ?ciLen (length$ ?ci))
         (if (= 0 ?ciLen) then
           (modify-instance ?pa (CompletelyInvalid $?blocks))
           else
           (bind ?cis $?ci)
           (bind ?newIndex (+ 1 (length$ ?ci)))
           (progn$ (?block ?blocks)
                   (if (not (member$ ?block ?cis)) then 
                     (bind ?cis (insert$ ?cis ?newIndex ?block))
                     (bind ?newIndex (+ 1 ?newIndex))))
           (modify-instance ?pa (CompletelyInvalid ?cis))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-inject::InterleavedInjectCompletelyInvalid-AndPotentiallyInvalidBlocks
         (declare (salience 1))
         ?f0 <- (message (to wavefront-scheduling)
                         (action completely-invalid-blocks)
                         (arguments ?e => $?b0))
         ?f1 <- (message (to wavefront-scheduling)
                         (action potentially-valid-blocks)
                         (arguments ?e => $?b1))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e)
                        (CompletelyInvalid $?ci)
                        (PotentiallyValid $?pv))
         =>
         (retract ?f0 ?f1)
         (bind ?ciLen (length$ $?ci))
         (bind ?pvLen (length$ $?pv))
         (bind ?cis $?ci)
         (bind ?pvs $?pv)
         (if (= 0 ?ciLen) then 
           (bind ?cis $?b0)
           else
           (bind ?i0 (+ 1 ?ciLen))
           (progn$ (?b ?b0)
                   (if (not (member$ ?b ?cis)) then
                     (bind ?cis (insert$ ?cis ?i0 ?b))
                     (bind ?i0 (+ ?i0 1)))))
         (if (= 0 ?pvLen) then
           (bind ?pvs $?b1)
           else
           (bind ?i1 (+ 1 ?pvLen))
           (progn$ (?b ?b1)
                   (if (not (member$ ?b ?pvs)) then
                     (bind ?pvs (insert$ ?pvs ?i1 ?b))
                     (bind ?i1 (+ ?i1 1)))))
         (modify-instance ?pa (CompletelyInvalid ?cis) 
                          (PotentiallyValid ?pvs)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-inject::InjectMemoryBarrierBlocks 
         ?fct <- (message (to wavefront-scheduling)
                          (action element-has-memory-barrier)
                          (arguments ?t => ?e))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e)
                        (MemoryBarriers $?mb))
         =>
         (retract ?fct)
         (if (not (member$ ?t $?mb)) then
           (modify-instance ?pa (MemoryBarriers $?mb ?t))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-inject::InjectCallBarrierBlocks 
         ?fct <- (message (to wavefront-scheduling)
                          (action element-has-call-barrier)
                          (arguments ?t => ?e))
         ?pa <- (object (is-a PathAggregate) 
                        (parent ?e) 
                        (CallBarriers $?cb))
         =>
         (retract ?fct)
         (if (not (member$ ?t $?cb)) then
           (modify-instance ?pa (CallBarriers $?cb ?t))))
;------------------------------------------------------------------------------
; now that we have a path aggregate we need to get the list of instruction
; CPV's that represent valid movable instructions for the given block on the
; wavefront. 
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::SelectValidCPVs 
         (object (is-a Wavefront) 
                 (values $? ?e $?))
         (object (is-a BasicBlock) 
                 (id ?e) 
                 (IsOpen TRUE))
         (object (is-a PathAggregate) 
                 (parent ?e)
                 (PotentiallyValid $?pv))
         =>
         (assert (For ?e find CPVs for $?pv)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::FindValidCPVsForBlock
         ?fct <- (For ?e find CPVs for ?pv $?pvs)
         (object (is-a BasicBlock) 
                 (id ?pv) 
                 (contents $?instructions))
         =>
         (retract ?fct)
         (assert (For ?e find CPVs for $?pvs)
                 (Get CPVs out of ?pv for ?e using $?instructions)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::SkipRegionsForFindingValidCPVsForBlock
         ?fct <- (For ?e find CPVs for ?pv $?pvs)
         (object (is-a Region) 
                 (id ?pv)) 
         =>
         (retract ?fct)
         (assert (For ?e find CPVs for $?pvs)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::RetractValidCPVsForBlock
         ?fct <- (For ? find CPVs for)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::IgnorePHIInstructions
         (declare (salience 1))
         ?fct <- (Get CPVs out of ?pv for ?e using ?inst $?insts)
         (object (is-a PhiNode) 
                 (id ?inst))
         =>
         (retract ?fct)
         (assert (Get CPVs out of ?pv for ?e using $?insts)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::IgnoreCallInstructions
         (declare (salience 1))
         ?fct <- (Get CPVs out of ?pv for ?e using ?inst $?insts)
         (object (is-a CallInstruction) 
                 (id ?inst))
         =>
         (retract ?fct)
         (assert (Get CPVs out of ?pv for ?e using $?insts)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::IgnoreTerminatorInstructions
         (declare (salience 1))
         ?fct <- (Get CPVs out of ?pv for ?e using ?inst $?insts)
         (object (is-a TerminatorInstruction) 
                 (id ?inst))
         =>
         (retract ?fct)
         (assert (Get CPVs out of ?pv for ?e using $?insts)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::DisableInstructionsDependentOnDestinationPhis
         (declare (salience 2))
         ?fct <- (Get CPVs out of ?pv for ?e using ?inst $?insts)
         ;make sure that the parent block is the same
         (object (is-a Instruction)
                 (id ?inst) 
                 (parent ?p) 
                 (DestinationRegisters $? ?reg $?))
         (object (is-a PhiNode) 
                 (id ?reg) 
                 (parent ?p))
         =>
         (retract ?fct)
         (assert (Get CPVs out of ?pv for ?e using $?insts)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::DisableInstructionsDependentOnLocalPhis
         (declare (salience 2))
         ?fct <- (Get CPVs out of ?pv for ?e using ?inst $?insts)
         ;make sure that the parent block is the same 
         (object (is-a Instruction) 
                 (id ?inst) 
                 (LocalDependencies $? ?reg $?))
         (object (is-a PhiNode) 
                 (id ?reg))
         =>
         (retract ?fct)
         (assert (Get CPVs out of ?pv for ?e using $?insts)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::TagValidCPVs
         ?fct <- (Get CPVs out of ?pv for ?e using ?inst $?insts)
         ?i <- (object (is-a Instruction) 
                       (id ?inst) 
                       (IsTerminator FALSE) 
                       (HasCallDependency FALSE))
         =>
         (retract ?fct)
         (assert (Get CPVs out of ?pv for ?e using $?insts)
                 (Marked ?inst as valid for block ?e)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::RetractDrainedGetCPVFacts
         ?fct <- (Get CPVs out of ?pv for ?e using)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::ReloadCPVIntoNewAggregate
         "Put the CPV that has already been created into the target path 
         aggregate"
         ?fct <- (Marked ?inst as valid for block ?e)
         (object (is-a CompensationPathVector) 
                 (parent ?inst) 
                 (id ?cpvID))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il)
                           (ScheduledInstructions $?si)
                           (CompensationPathVectors $?cpvs)
                           (ImpossibleCompensationPathVectors 
                             $?icpv&:(not (member$ ?cpvID $?icpv))))
         (object (is-a Instruction) 
                 (id ?inst) 
                 (NonLocalDependencies $?nlds)
                 (DestinationRegisters ?reg))
         =>
         (retract ?fct)
         (bind ?ils $?il)
         (bind ?sis $?si)
         (if (not (member$ ?inst ?ils)) then
           (bind ?ils (create$ ?ils ?inst)))
         (if (and (neq ?reg ?inst)
                  (not (member$ ?reg ?ils))) then
           (bind ?ils (create$ ?ils ?reg)))
         (progn$ (?nld ?nlds)
                 (if (not (member$ ?nld ?sis)) then
                   (bind ?sis (create$ ?sis ?nld))))
         (modify-instance ?agObj (CompensationPathVectors $?cpvs ?cpvID)
                          (InstructionList ?ils)
                          (ScheduledInstructions ?sis)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::CPVIsImpossible
         "Target CPV is considered to be impossible to schedule. However, we should
         still schedule the non-local dependencies to make sure that all non-local
         dependencies are scheduled"
         ?fct <- (Marked ?inst as valid for block ?e)
         (object (is-a CompensationPathVector) 
                 (parent ?inst) 
                 (id ?cpvID))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (ImpossibleCompensationPathVectors 
                             $?icpv&:(member$ ?cpvID $?icpv))
                           (ScheduledInstructions $?si))
         (object (is-a Instruction) 
                 (id ?inst) 
                 (NonLocalDependencies $?nlds))
         =>
         (retract ?fct)
         ;add the non-local dependencies
         (bind ?sLen (length$ ?si))
         (if (= 0 ?sLen) then
           (modify-instance ?agObj (ScheduledInstructions ?nlds))
           else
           (bind ?sis $?si)
           (progn$ (?nld ?nlds)
                   (if (not (member$ ?nld $?sis)) then
                     (bind ?sis (create$ ?sis ?nld))))
           (modify-instance ?agObj (ScheduledInstructions ?sis))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::MakeCPV 
         ?fct <- (Marked ?inst as valid for block ?e)
         (not (exists (object (is-a CompensationPathVector)
                              (parent ?inst))))
         (object (is-a Instruction) 
                 (id ?inst) 
                 (parent ?pv) 
                 (DestinationRegisters ?reg) 
                 (NonLocalDependencies $?nlds))
         (object (is-a BasicBlock) 
                 (id ?pv) 
                 (Paths $?paths))
         ?pa <- (object (is-a PathAggregate) 
                        (id ?ag) 
                        (parent ?e)
                        (CompensationPathVectors $?cpv)
                        (InstructionList $?il)
                        (ScheduledInstructions $?si))
         =>
         ; We need to disable the stores from moving when their dependencies
         ; 
         ; YOU DON'T EVEN WANT TO KNOW WHAT I'M GOING TO DO TO YOU
         (retract ?fct)
         (bind ?ilLen (length$ ?il))
         (bind ?siLen (length$ ?si))
         (bind ?ils $?il)
         (bind ?sis $?si)
         (if (= 0 ?ilLen) then
           (if (neq ?inst ?reg) then
             (bind ?ils ?inst ?reg)
             else
             (bind ?ils ?inst)) 
           else
           (if (not (member$ ?inst ?ils)) then
             (bind ?ils (create$ ?ils ?inst)))
           (if (and (neq ?inst ?reg)
                    (not (member$ ?reg ?ils))) then
             (bind ?ils (create$ ?ils ?reg))))
         (if (= 0 ?siLen) then
           (bind ?sis $?nlds)
           else
           (progn$ (?nld ?nlds) 
                   (if (not (member$ ?nld ?sis)) then
                     (bind ?sis (create$ ?sis ?nld)))))
         (bind ?name (instance-name-to-symbol 
                       (instance-name
                         (make-instance of CompensationPathVector
                                        (parent ?inst)
                                        (Paths $?paths)
                                        (OriginalBlock ?pv)))))
         (modify-instance ?pa 
                          (ScheduledInstructions ?sis)
                          (InstructionList ?ils)
                          (CompensationPathVectors 
                            $?cpv ?name))) 
;------------------------------------------------------------------------------
; Now we go through and attempt to schedule the instruction represented by 
; each CPV into the block on the wavefront. I call this stage merge. I had some
; far raunchier names but this is my thesis. Some of the potential names were
; OneeChanTheresNoWayThatCanFit, ImAMexiCan, Copulation, and many more.
; 
; I guess the biggest question would be why? And my answer would be why not?
; These terms just popped into my head.
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::SetifyInstructionList
         ?pa <- (object (is-a PathAggregate) 
                        (InstructionList $?a ?b $?c ?b $?d))
         =>
         ;we found a single duplicate, let's setify the whole InstructionList
         ;now to make sure.
         (bind ?ils (create$ ?b))
         (progn$ (?i (create$ ?a ?c ?d))
                 (if (not (member$ ?i ?ils)) then
                   (bind ?ils (create$ ?ils ?i))))
         (modify-instance ?pa (InstructionList ?ils)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::GenerateInitialSliceFactsForElementsOnTheWavefront 
         (object (is-a Wavefront) 
                 (parent ?r) 
                 (values $? ?e $?))
         (object (is-a BasicBlock) 
                 (id ?e) 
                 (IsOpen TRUE))
         (object (is-a PathAggregate) 
                 (parent ?e) 
                 (CompensationPathVectors $?cpv&:(> (length$ ?cpv) 0)))
         =>
         (assert (Generate slices for block ?e in ?r using $?cpv)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::GenerateFactForSlicesFromCPV
         ?fct <- (Generate slices for block ?e in ?r using ?cpv $?cpvs)
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i)
                 (Paths $?paths))
         (object (is-a Instruction) 
                 (id ?i) 
                 (parent ?b))
         =>
         (retract ?fct)
         (assert (Generate slices for block ?e in ?r using $?cpvs)
                 (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths $?paths)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::RetractEmptySlicesCreationFact
         ?fct <- (Generate slices for block ? in ? using)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::QueryCanCreateSliceForPath
         ?fct <- (Generate slices for block ?e in ?r with cpv ?cpv 
                           with stop block ?b using paths ?path $?paths)
         (object (is-a Path) 
                 (id ?path) 
                 (values $? ?e $?))
         =>
         (retract ?fct)
         (assert (Generate slice for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using path ?path)
                 (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths $?paths)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::QueryCantCreateSliceForPath
         ?fct <- (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths ?path $?paths)
         (object (is-a Path) 
                 (id ?path) 
                 (values $?z&:(not (member$ ?e ?z))))
         =>
         (retract ?fct)
         (assert (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths $?paths)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::TryConstructNewSlice
         ?fct <- (Generate slice for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using path ?path)
         (not (exists (object (is-a Slice) 
                              (parent ?b) 
                              (target-path ?path) 
                              (target-block ?e))))
         (object (is-a Path) 
                 (id ?path) 
                 ; we want the elements between the last block and the block on
                 ; the wavefront. With multifield pattern matching, this is
                 ; really easy
                 (values $? ?e $?slice ?b $?))
         =>
         (retract ?fct)
         (make-instance of Slice 
                        (parent ?b) 
                        (target-path ?path)
                        (target-block ?e) 
                        (contents $?slice)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::SliceAlreadyExists
         ?fct <- (Generate slice for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using path ?path)
         (exists (object (is-a Slice) 
                         (parent ?b) 
                         (target-path ?path) 
                         (target-block ?e)))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice::RemoveSliceAnalysisFact
         ?fct <- (Generate slices for block ? in ? with cpv ? with stop block ? 
                           using paths)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
;only construct slices as we see fit as we can just reference them again.
;------------------------------------------------------------------------------
; Now that we have slices constructed it's time to run through the
; analyze-merge cycle. First up, analyze. In this phase we need to see if it
; is possible to schedule into the block. Actually, I can see that we already
; did the hard work as any regions that would have prevented code from moving
; up would have already prevented the block from being selected. Thus we should
; just check to see if we have a local dependency
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze-init::InitialCPVSetupForPathAggregate
         "Load all of the compensation path vectors for the given path 
         aggregate into the aggregates TargetCompensationPathVectors 
         multifield"
         (object (is-a Wavefront) 
                 (values $? ?blkID $?))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?blkID) 
                           (CompensationPathVectors 
                             $?cpvIDs&:(> (length$ ?cpvIDs) 0)))
         =>
         (modify-instance ?agObj 
                          (TargetCompensationPathVectors $?cpvIDs)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze-init::SetifyTargetCompensationPathVectors
         ?pa <- (object (is-a PathAggregate) 
                        (TargetCompensationPathVectors $?a ?b $?c ?b $?d))
         =>
         (bind ?tcpv (create$ ?b))
         (progn$ (?i (create$ ?a ?c ?d))
                 (if (not (member$ ?i ?tcpv)) then
                   (bind ?tcpv (create$ ?tcpv ?i))))
         (modify-instance ?pa (TargetCompensationPathVectors ?tcpv)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-pre-generate-analyze::SelectCPVForAnalysis
         (object (is-a Wavefront) 
                 (parent ?r) 
                 (values $? ?e $?))
         ?bb <- (object (is-a BasicBlock) 
                        (id ?e) 
                        (IsOpen TRUE))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e) 
                           (TargetCompensationPathVectors 
                             $?cpvs&:(> (length$ ?cpvs) 0)))
         =>
         ;clear out the cpvs
         (modify-instance ?agObj 
                          (TargetCompensationPathVectors))
         (bind ?result (create$))
         ;object of this loop is to figure out if there are any remaining
         ;paths tied to the target path aggregate that can reach the block on
         ;the wavefront. 
         (progn$ (?cpv $?cpvs)
                 (bind ?det FALSE)
                 (bind ?cpvName (instance-address * (symbol-to-instance-name
                                                      ?cpv)))
                 (bind ?paths (send ?cpvName get-Paths))
                 ;we look through the set of paths that the target CPV is on.
                 ;We try to find one that contains the target block on the
                 ;wavefront
                 (progn$ (?p ?paths)
                         (if ?det then 
                           ;we found a path that contains ?e
                           (break)
                           else 
                           (bind ?o2 (instance-address *
                                                       (symbol-to-instance-name ?p)))
                           (bind ?o2C (send ?o2 get-values))
                           (bind ?det (or ?det (member$ ?e ?o2C)))))
                 ;Outer loop
                 (if ?det then 
                   (bind ?result (create$ ?result ?cpv))))
         ; if we didn't find any valid CPVs then this message will be 
         ; automatically retracted in the next module
         (assert (message (to wavefront-scheduling)
                          (action analyze-block-using-cpvs)
                          (arguments ?e ?r => $?result))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-generate-analyze::SegmentCPVsApart
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-block-using-cpvs)
                          (arguments ?e ?r => ?cpv $?cpvs))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i))
         =>
         (modify ?fct (arguments ?e ?r => $?cpvs))
         (assert (message (to wavefront-scheduling)
                          (action analyze-instruction-with-associate-cpv-for)
                          (arguments ?i ?cpv ?e))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-generate-analyze::RetractCPVSegmentationFact
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-block-using-cpvs)
                          (arguments ? ? =>))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-generate-analyze::TargetInstructionDoesNotHaveACorrespondingCPV
         "Sometimes it turns out that sometimes store instructions will add 
         certain instructions to the instruction list even though they don't 
         have a valid CPV.  This rule removes those elements from the path 
         aggregate"
         ?pa <- (object (is-a PathAggregate) 
                        (InstructionList $?b ?a $?c))
         (not (exists (object (is-a CompensationPathVector) 
                              (parent ?a))))
         =>
         (modify-instance ?pa (InstructionList $?b $?c)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-generate-analyze::TargetInstructionIsNotRegisteredWithTheTargetPathAggregate
         "Sometimes it turns out that sometimes store instructions will add 
         certain instructions to the instruction list even though they don't 
         have a valid CPV registered with the path aggregate. This rule removes 
         those elements from the path aggregate"
         ?pa <- (object (is-a PathAggregate) 
                        (InstructionList $?b ?a $?c)
                        (CompensationPathVectors $?cpvs))
         (object (is-a CompensationPathVector) 
                 (parent ?a) 
                 (id ?id&:(not (member$ ?id $?cpvs))))
         =>
         (modify-instance ?pa (InstructionList $?b $?c)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::TargetCPVIsImpossibleToScheduleIntoTargetBlock
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-instruction-with-associate-cpv-for)
                          (arguments ?i ?cpv ?e))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e) 
                           (InstructionList $?il)
                           (ScheduledInstructions $?sched))
         (object (is-a Instruction) 
                 (id ?i) 
                 (LocalDependencies $?ld)
                 (NonLocalDependencies $?nld))
         (test (not (and (subsetp ?ld ?il)
                         (subsetp ?nld ?sched))))
         =>
         ;reuse the memory of ?fct for something else
         (modify ?fct (action cant-schedule)
                 (arguments ?cpv ?e ever))
         (bind ?ind (member$ ?i ?il))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::TargetCPVCantBeScheduledIntoTargetBlockYet
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-instruction-with-associate-cpv-for)
                          (arguments ?i ?cpv ?e))
         ?paObj <- (object (is-a PathAggregate) 
                           (parent ?e) 
                           (InstructionList $?il)
                           (ScheduledInstructions $?sched)
                           (CompensationPathVectors $?cpvs))
         ;TODO: test to see if putting parts of the test condition into the
         ;match conditions of the previous CE will work correctly or even allow
         ;for piecewise evaluation
         (object (is-a Instruction) 
                 (id ?i) 
                 (LocalDependencies $?ld) 
                 (NonLocalDependencies $?nld))
         (test (and (not (subsetp ?ld ?sched))
                    (subsetp ?ld ?il)
                    (subsetp ?nld ?sched)))
         =>
         ;cheaper way to 
         (modify ?fct (action cant-schedule)
                 (arguments ?cpv ?e now)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::TargetCPVNeedsToBeSliceAnalyzed
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-instruction-with-associate-cpv-for)
                          (arguments ?i ?cpv ?e))
         (object (is-a PathAggregate) 
                 (parent ?e) 
                 (ScheduledInstructions $?sched))
         (object (is-a Instruction) 
                 (id ?i) 
                 (parent ?b) 
                 (LocalDependencies $?ld&:(subsetp ?ld ?sched)))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (Paths $?paths))
         =>
         (retract ?fct)
         (bind ?validPaths (create$))
         (progn$ (?z ?paths)
                 (bind ?obj (instance-address * (symbol-to-instance-name ?z)))
                 (bind ?contents (send ?obj get-values))
                 (if (member$ ?e ?contents) then
                   (bind ?validPaths (create$ ?validPaths ?z))))
         (if (> (length$ ?validPaths) 0) then
           (assert (Pull slices for range ?e to ?b for instruction ?i { 
                         associated cpv ?cpv } using paths $?validPaths))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::CreateSliceSegments
         ?fct <- (Pull slices for range ?e to ?b for instruction ?i {
                       associated cpv ?cpv } using paths ?path $?paths)
         (object (is-a Slice) 
                 (parent ?b) 
                 (target-block ?e) 
                 (target-path ?path)
                 (id ?s))
         =>
         (retract ?fct)
         (assert (Pull slices for range ?e to ?b for instruction ?i {
                       associated cpv ?cpv } using paths $?paths)
                 (message (to wavefront-scheduling)
                          (action analyze-slice)
                          (arguments ?s ?e ?cpv))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::RetractSliceSegmenterFact
         ?fct <- (Pull slices for range ? to ? for instruction ? {
                       associated cpv ? } using paths)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::FAILURE-MISSING-SLICE 
         ?fct <- (Pull slices for range ?e to ?b for instruction ?i {
                       associated cpv ?cpv } using paths ?path $?paths)
         (not (exists (object (is-a Slice) 
                              (parent ?b) 
                              (target-block ?e)
                              (target-path ?path))))
         =>
         (facts)
         (printout t "ERROR: Couldn't find an associated slice for " crlf
                   "TargetBlock = " ?e crlf
                   "Parent = " ?b crlf
                   "TargetPath = " ?path crlf
                   "TargetPath contents = " (send (symbol-to-instance-name
                                                    ?path) get-contents) crlf
                   "Rest of paths are = " $?paths crlf)
         (halt))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::single-single=>multi
         ?f0 <- (message (to wavefront-scheduling)
                         (action analyze-slice)
                         (arguments ?s0 ?e ?cpv))
         ?f1 <- (message (to wavefront-scheduling)
                         (action analyze-slice)
                         (arguments ?s1&~?s1 ?e ?cpv))
         =>
         (retract ?f1)
         (modify ?f0 (action analyze-slices)
                 (arguments ?e ?cpv => ?s0 ?s1)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::single=>multi
         (declare (salience -3))
         ?f0 <- (message (to wavefront-scheduling)
                         (action analyze-slice)
                         (arguments ?s0 ?e ?cpv))
         =>
         (modify ?f0 (action analyze-slices)
                 (arguments ?e ?cpv => ?s0)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::multi-multi=>multi
         ?f0 <- (message (to wavefront-scheduling)
                         (action analyze-slices)
                         (arguments ?e ?cpv => $?q))
         ?f1 <- (message (to wavefront-scheduling)
                         (action analyze-slices)
                         (arguments ?e ?cpv => $?z))
         (test (neq ?f0 ?f1)) 
         =>
         (modify ?f0 (arguments ?e ?cpv => $?q $?z))
         (retract ?f1))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-analyze::SetifyAnalyzeSlicesFact
         (declare (salience -1))
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => $?a ?b $?c ?b $?d))
         =>
         (bind ?tmp (create$ ?b))
         (progn$ (?s (create$ ?a ?c ?d))
                 (if (not (member$ ?s ?tmp)) then
                   (bind ?tmp (create$ ?tmp ?s))))
         (modify ?fct (arguments ?e ?cpv => ?tmp)))
;------------------------------------------------------------------------------
; now that we have a list of slices to look at it's time to check and see if
; the given cpv can be moved up based on the slice. If it can't then assert 
; a fact that says as much
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::RetractSliceAnalysis
         "Retract all slice analysis if it turns out there is a failure fact"
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => $?))
         (exists (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?e ?)))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::AnalyzeSliceContentsForFailure-ProducerLowerThanTargetBlock
         "Does a check to make sure that non local dependencies prevent an 
         instruction from being moved upward into the target block"
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => ?s $?ss))
         (object (is-a Slice) 
                 (id ?s) 
                 (target-block ?e) 
                 (parent ?b)
                 (contents $? ?element $?))
         (object (id ?element) 
                 (Produces $? ?nld $?))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i))
         (object (is-a Instruction) (id ?i) 
                 (DestinationRegisters ?dr)
                 (NonLocalDependencies $? ?nld $?))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il))
         =>
         (retract ?fct)
         (bind ?ind (member$ ?i ?il))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?e ever))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::AnalyzeSliceContentsForFailure-CallBarrier
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => ?s $?ss))
         (object (is-a Slice) 
                 (id ?s) 
                 (target-block ?e) 
                 (parent ?b)
                 (contents $? ?element $?))
         (object (id ?element) 
                 (HasCallBarrier TRUE))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i))
         (object (is-a Instruction) 
                 (id ?i) 
                 (DestinationRegisters ?dr))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il))
         =>
         (retract ?fct)
         (bind ?ind (member$ ?i $?il))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?e ever))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::SliceTargetHasMemoryBarrier
         "The given slice has an element that contains a memory barrier. 
         A memory barrier is only created when analysis has failed to ascertain
         what is being read from or written to in memory."
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => ?s $?ss))
         (object (is-a Slice) 
                 (id ?s) 
                 (target-block ?e) 
                 (parent ?b) 
                 (contents $? ?element $?))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i))
         (object (is-a LoadInstruction|StoreInstruction) 
                 (id ?i)
                 (DestinationRegisters ?dr))
         (object (id ?element) 
                 (HasMemoryBarrier TRUE))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il))
         =>
         (retract ?fct)
         (bind ?ind (member$ ?i $?il))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?e ever))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::SliceTargetDoesntHaveMemoryBarrier-ModifiesSameMemory
         "The given slice has an element that contains a entry in the WritesTo 
         list that is the same thing as the given load or store instruction"
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => ?s $?ss))
         (object (is-a Slice) 
                 (id ?s) 
                 (target-block ?e) 
                 (parent ?b) 
                 (contents $? ?element $?))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i))
         (object (is-a LoadInstruction|StoreInstruction) 
                 (id ?i)
                 (MemoryTarget ?mt) 
                 (DestinationRegisters ?dr))
         (object (id ?element) 
                 (HasMemoryBarrier FALSE) 
                 (HasCallBarrier FALSE)
                 (WritesTo $? ?mt $?))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il))
         =>
         (retract ?fct)
         (bind ?ind (member$ ?i $?il))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?e ever))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::SliceTargetDoesntHaveMemoryBarrier-HasUnknownReference
         "Does now allow loads or stores to be moved above the given element
         regardless of if a memory barrier exists or not. This is because there
         is an unknown loader element"
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => ?s $?ss))
         (object (is-a Slice) 
                 (id ?s) 
                 (target-block ?e) 
                 (parent ?cpv) 
                 (contents $? ?element $?))
         (object (is-a CompensationPathVector) 
                 (id ?cpv) 
                 (parent ?i))
         (object (is-a LoadInstruction|StoreInstruction) 
                 (id ?i) 
                 (parent ?q) 
                 (DestinationRegisters ?dr))
         (object (id ?element) 
                 (WritesTo $? UNKNOWN $?))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (InstructionList $?il))
         =>
         (retract ?fct)
         (bind ?ind (member$ ?i $?il))
         (bind ?ind (member$ ?i (send ?agObj get-InstructionList)))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?e ever))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::CanScheduleIntoBlockOnSlice
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv => ?s $?ss))
         =>
         (modify ?fct (arguments ?e ?cpv => $?ss)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-slice-analyze::CanScheduleInstructionThisIteration
         (declare (salience -3))
         ?fct <- (message (to wavefront-scheduling)
                          (action analyze-slices)
                          (arguments ?e ?cpv =>))
         =>
         ;modify the template to fit our needs :D
         (modify ?fct (action can-schedule)
                 (arguments ?cpv ?e)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge-init::AddCPVToSuccessList
         ?fct <- (message (to wavefront-scheduling)
                          (action can-schedule)
                          (arguments ?cpv ?e))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?e)
                           (MovableCompensationPathVectors $?mcpv))
         =>
         (retract ?fct)
         (modify-instance ?agObj (MovableCompensationPathVectors $?mcpv ?cpv)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge-init::FailCPVForNow
         ?fct <- (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?b now))
         ?ag <- (object (is-a PathAggregate) 
                        (parent ?b)
                        (StalledCompensationPathVectors $?scpv))
         =>
         (retract ?fct)
         (modify-instance ?ag (StalledCompensationPathVectors $?scpv ?cpv)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-merge-init::RemoveCPVFromService
         ?fct <- (message (to wavefront-scheduling)
                          (action cant-schedule)
                          (arguments ?cpv ?b ever))
         ?agObj <- (object (is-a PathAggregate) 
                           (parent ?b)
                           (ImpossibleCompensationPathVectors $?icpv))
         ?cpvObj <- (object (is-a CompensationPathVector) 
                            (id ?cpv) 
                            (Failures $?fails))
         =>
         (retract ?fct)
         (object-pattern-match-delay
           (modify-instance ?cpvObj (Failures $?fails ?b))
           (modify-instance ?agObj (ImpossibleCompensationPathVectors 
                                     $?icpv ?cpv))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-ponder::PonderMovementIteration
         (declare (salience 100))
         (object (is-a Wavefront) 
                 (id ?r) 
                 (values $? ?e $?))
         ?ag <- (object (is-a PathAggregate) 
                        (parent ?e) 
                        (id ?pa)
                        (StalledCompensationPathVectors 
                          $?scpv&:(> (length$ $?scpv) 0)))
         =>
         ;we use an explicit fact like this so that it's easy to match against
         ;and only one version of this fact will exist at any given time within
         ;the fact pool of the wavefront-scheduling-ponder module
         (assert (Another Movement Required))
         (modify-instance ?ag (StalledCompensationPathVectors)
                          (TargetCompensationPathVectors $?scpv)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-ponder::AnotherMovementIsRequired
         ?f <- (Another Movement Required)
         ?obj <- (object (is-a pass-description) (passes $?passes))
         =>
         (retract ?f)
         (modify-instance ?obj (passes wavefront-scheduling-pre-generate-analyze
                                       wavefront-scheduling-generate-analyze
                                       wavefront-scheduling-analyze 
                                       wavefront-scheduling-slice-analyze
                                       wavefront-scheduling-merge-init 
                                       wavefront-scheduling-merge 
                                       wavefront-scheduling-merge-update 
                                       wavefront-scheduling-reopen-blocks
                                       wavefront-scheduling-ponder
                                       $?passes)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-post-ponder::FinishSchedulingIntoBlock
         ?fct <- (message (from pipeline)
                          (to wavefront-scheduling-post-ponder)
                          (action initial-fact))
         =>
         (retract ?fct)
         (progn$ (?instance (find-all-instances ((?wave Wavefront)) TRUE))
                 (progn$ (?child (send ?instance get-values))
                         (bind ?name (instance-address llvm (symbol-to-instance-name ?child)))
                         (modify-instance ?name 
                                          (IsOpen FALSE)))))
;------------------------------------------------------------------------------
