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
                          (argument ?t => ?e))
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
                 (contents $? ?e $?))
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
         (object (is-a BasicBlock) (ID ?pv) (Contents $?instructions))
         =>
         (retract ?fct)
         (assert (For ?e find CPVs for $?pvs)
                 (Get CPVs out of ?pv for ?e using $?instructions)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-acquire::SkipRegionsForFindingValidCPVsForBlock
         (Stage WavefrontSchedule $?)
         (Substage Acquire $?)
         ?fct <- (For ?e find CPVs for ?pv $?pvs)
         (object (is-a Region) (ID ?pv)) 
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
         (if (= 0 (?sLen)) then
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
         (modify-instance ?pa 
                          (ScheduledInstructions ?sis)
                          (InstructionList ?ils)
                          (CompensationPathVectors 
                            $?cpv (instance-name-to-symbol 
                                    (make-instance of CompensationPathVector
                                                   (parent ?inst)
                                                   (Paths $?paths)
                                                   (OriginalBlock ?pv))))))
;------------------------------------------------------------------------------
; Now we go through and attempt to schedule the instruction represented by 
; each CPV into the block on the wavefront. I call this stage merge. I had some
; far raunchier names but this is my thesis. Some of the potential names were
; OneeChanTheresNoWayThatCanFit, ImAMexiCan, Copulation, and many more.
; 
; I guess the biggest question would be why? And my answer would be why not?
; These terms just popped into my head.
;------------------------------------------------------------------------------
(defrule SetifyInstructionList
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?pa <- (object (is-a PathAggregate) (InstructionList $?a ?b $?c ?b $?d))
         =>
         (modify-instance ?pa (InstructionList $?a ?b $?c $?d)))
;------------------------------------------------------------------------------
(defrule GenerateInitialSliceFactsForElementsOnTheWavefront 
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         (object (is-a Wavefront) (Parent ?r) (Contents $? ?e $?))
         (object (is-a BasicBlock) (ID ?e) (IsOpen TRUE))
         (object (is-a PathAggregate) (Parent ?e) 
                 (CompensationPathVectors $?cpv))
         (test (> (length$ ?cpv) 0))
         =>
         (assert (Generate slices for block ?e in ?r using $?cpv)))
;------------------------------------------------------------------------------
(defrule GenerateFactForSlicesFromCPV
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?fct <- (Generate slices for block ?e in ?r using ?cpv $?cpvs)
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i)
                 (Paths $?paths))
         (object (is-a Instruction) (ID ?i) (Parent ?b))
         =>
         (retract ?fct)
         (assert (Generate slices for block ?e in ?r using $?cpvs)
                 (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths $?paths)))
;------------------------------------------------------------------------------
(defrule RetractEmptySlicesCreationFact
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?fct <- (Generate slices for block ? in ? using)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule QueryCanCreateSliceForPath
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?fct <- (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths ?path $?paths)
         (object (is-a Path) (ID ?path) (Contents $? ?e $?))
         ;(test (member$ ?e ?z))
         =>
         (retract ?fct)
         (assert (Generate slice for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using path ?path)
                 (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths $?paths)))
;------------------------------------------------------------------------------
(defrule QueryCantCreateSliceForPath
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?fct <- (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths ?path $?paths)
         (object (is-a Path) (ID ?path) (Contents $?z))
         (test (not (member$ ?e ?z)))
         =>
         (retract ?fct)
         (assert (Generate slices for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using paths $?paths)))
;------------------------------------------------------------------------------
(defrule TryConstructNewSlice
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?fct <- (Generate slice for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using path ?path)
         (not (exists (object (is-a Slice) (Parent ?b) (TargetPath ?path) 
                              (TargetBlock ?e))))
         (object (is-a Path) (ID ?path) (Contents $? ?e $?slice ?b $?))
         =>
         (retract ?fct)
         (make-instance (gensym*) of Slice (Parent ?b) (TargetPath ?path)
                        (TargetBlock ?e) (Contents $?slice)))
;------------------------------------------------------------------------------
(defrule SliceAlreadyExists
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
         ?fct <- (Generate slice for block ?e in ?r with cpv ?cpv with stop 
                           block ?b using path ?path)
         (exists (object (is-a Slice) (Parent ?b) (TargetPath ?path) 
                         (TargetBlock ?e)))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule RemoveSliceAnalysisFact
         (Stage WavefrontSchedule $?)
         (Substage Slice $?)
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
(defrule InitialCPVSetupForPathAggregate
         "Load all of the compensation path vectors for the given path 
         aggregate into the aggregates TargetCompensationPathVectors 
         multifield"
         (Stage WavefrontSchedule $?)
         (Substage AnalyzeInit $?)
         (object (is-a Wavefront) (Contents $? ?blkID $?))
         ?agObj <- (object (is-a PathAggregate) (Parent ?blkID) 
                           (CompensationPathVectors $?cpvIDs))
         (test (> (length$ ?cpvIDs) 0))
         =>
         (modify-instance ?agObj (TargetCompensationPathVectors $?cpvIDs)))
;------------------------------------------------------------------------------
(defrule SetifyTargetCompensationPathVectors
         (Stage WavefrontSchedule $?)
         (Substage AnalyzeInit $?)
         ?pa <- (object (is-a PathAggregate) 
                        (TargetCompensationPathVectors $?a ?b $?c ?b $?d))
         =>
         (modify-instance ?pa (TargetCompensationPathVectors $?a ?b $?c $?d)))
;------------------------------------------------------------------------------
(defrule SelectCPVForAnalysis
         (Stage WavefrontSchedule $?)
         (Substage GenerateAnalyze0 $?)
         (object (is-a Wavefront) (Parent ?r) (Contents $? ?e $?))
         ?bb <- (object (is-a BasicBlock) (ID ?e) (IsOpen TRUE))
         ;(not (exists (Schedule ?e for ?r)))
         ?agObj <- (object (is-a PathAggregate) (Parent ?e) 
                           (TargetCompensationPathVectors $?cpvs))
         (test (> (length$ ?cpvs) 0))
         =>
         ;clear out the cpvs
         (modify-instance ?agObj (TargetCompensationPathVectors))
         (bind ?result (create$))
         (progn$ (?cpv $?cpvs)
                 (bind ?det FALSE)
                 (progn$ (?p (send (symbol-to-instance-name ?cpv) get-Paths))
                         (if ?det then 
                           (break)
                           else 
                           (bind ?o2 (symbol-to-instance-name ?p))
                           (bind ?det 
                                 (or ?det 
                                     (member$ ?e 
                                              (send ?o2 get-Contents))))))
                 (if ?det then 
                   (bind ?result (create$ ?result ?cpv))))
         (assert (Analyze block ?e for ?r using cpvs $?result)))
;------------------------------------------------------------------------------
(defrule SegmentCPVsApart
         (Stage WavefrontSchedule $?)
         (Substage GenerateAnalyze $?)
         ?fct <- (Analyze block ?e for ?r using cpvs ?cpv $?cpvs)
         (object (is-a BasicBlock) (ID ?e))
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i))
         =>
         ;(printout t "Analyze instruction " ?i " { associated cpv " ?cpv 
         ; " } for " ?e crlf)
         (retract ?fct)
         (assert (Analyze block ?e for ?r using cpvs $?cpvs)
                 (Analyze instruction ?i { associated cpv ?cpv } for ?e)))
;------------------------------------------------------------------------------
(defrule RetractCPVSegmentationFact
         (Stage WavefrontSchedule $?)
         (Substage GenerateAnalyze $?)
         ?fct <- (Analyze block ? for ? using cpvs)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule TargetInstructionDoesNotHaveACorrespondingCPV
         "Sometimes it turns out that sometimes store instructions will add 
         certain instructions to the instruction list even though they don't 
         have a valid CPV.  This rule removes those elements from the path 
         aggregate"
         (Stage WavefrontSchedule $?)
         (Substage GenerateAnalyze $?)
         ?pa <- (object (is-a PathAggregate) (Parent ?e) 
                        (InstructionList $?b ?a $?c))
         (not (exists (object (is-a CompensationPathVector) (Parent ?a))))
         =>
         ;(printout t "NOTE: Removed " ?a " from the path aggregate of " ?e 
         ;            " because a CPV wasn't tied to the instruction" crlf)
         (modify-instance ?pa (InstructionList $?b $?c)))
;------------------------------------------------------------------------------
(defrule TargetInstructionIsNotRegisteredWithTheTargetPathAggregate
         "Sometimes it turns out that sometimes store instructions will add 
         certain instructions to the instruction list even though they don't 
         have a valid CPV registered with the path aggregate. This rule removes 
         those elements from the path aggregate"
         (Stage WavefrontSchedule $?)
         (Substage GenerateAnalyze $?)
         ?pa <- (object (is-a PathAggregate) (Parent ?e) 
                        (InstructionList $?b ?a $?c)
                        (CompensationPathVectors $?cpvs))
         (object (is-a CompensationPathVector) (Parent ?a) (ID ?id))
         (test (not (member$ ?id $?cpvs)))
         =>
         ;(printout t "NOTE: Removed " ?a " from the path aggregate of " ?e 
         ;" because the corresponding CPV wasn't registered with the path"
         ;" aggregate" crlf)
         (modify-instance ?pa (InstructionList $?b $?c)))
;------------------------------------------------------------------------------
(defrule TargetCPVIsImpossibleToScheduleIntoTargetBlock
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Analyze instruction ?i { associated cpv ?cpv } for ?e)
         ?agObj <- (object (is-a PathAggregate) (Parent ?e) 
                           (InstructionList $?il)
                           (ScheduledInstructions $?sched))
         (object (is-a Instruction) (ID ?i) (LocalDependencies $?ld)
                 (NonLocalDependencies $?nld))
         (test (not (and (subsetp ?ld ?il)
                         (subsetp ?nld ?sched))))
         =>
         ;(printout t ?i " is impossible to schedule into " ?e crlf)
         ;(printout t "Local Dependencies = " ?ld crlf)
         ;(printout t "Non Local Dependencies = " ?nld crlf)
         ;(printout t "Instruction List = " ?il crlf)
         ;(printout t "Schedule = " ?sched crlf)
         (retract ?fct)
         (bind ?ind (member$ ?i ?il))
         (if ?ind then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (Cant schedule ?cpv for ?e ever)))
;------------------------------------------------------------------------------

(defrule TargetCPVCantBeScheduledIntoTargetBlockYet
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Analyze instruction ?i { associated cpv ?cpv } for ?e)
         ?paObj <- (object (is-a PathAggregate) (Parent ?e) 
                           (InstructionList $?il)
                           (ScheduledInstructions $?sched)
                           (CompensationPathVectors $?cpvs))
         (object (is-a Instruction) (ID ?i) (LocalDependencies $?ld) 
                 (NonLocalDependencies $?nld) (Parent ?parent))
         (test (and (not (subsetp ?ld ?sched))
                    (subsetp ?ld ?il)
                    (subsetp ?nld ?sched)))
         =>
         ;(printout t "Can't schedule " ?i " into " ?e " right now!" crlf)
         ;(printout t "Non Local Dependencies = " ?nld crlf)
         ;(printout t "Local Dependencies = " ?ld crlf)
         ;(printout t "Scheduled = " ?sched crlf)
         ;(printout t "Instruction List = " ?il crlf)
         ;(printout t "From = " ?parent crlf)
         ;(printout t "CPVS = " ?cpvs crlf)
         ;(facts)
         (retract ?fct)
         (assert (Cant schedule ?cpv for ?e now)))
;------------------------------------------------------------------------------
(defrule TargetCPVNeedsToBeSliceAnalyzed
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Analyze instruction ?i { associated cpv ?cpv } for ?e)
         (object (is-a PathAggregate) (Parent ?e) 
                 (ScheduledInstructions $?sched))
         (object (is-a Instruction) (ID ?i) (Parent ?b) 
                 (LocalDependencies $?ld))
         (test (subsetp ?ld ?sched))
         (object (is-a CompensationPathVector) (ID ?cpv) (Paths $?paths))
         =>
         (retract ?fct)
         (bind ?validPaths (create$))
         (foreach ?z ?paths
                  (bind ?obj (instance-name (symbol-to-instance-name ?z)))
                  (if (member$ ?e (send ?obj get-Contents)) then
                    (bind ?validPaths (create$ ?validPaths ?z))))
         (if (> (length$ ?validPaths) 0) then
           (assert (Pull slices for range ?e to ?b for instruction ?i { 
                         associated cpv ?cpv } using paths $?validPaths))))
;------------------------------------------------------------------------------
(defrule CreateSliceSegments
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Pull slices for range ?e to ?b for instruction ?i {
                       associated cpv ?cpv } using paths ?path $?paths)
         (object (is-a Slice) (Parent ?b) (TargetBlock ?e) (TargetPath ?path)
                 (ID ?s))
         =>
         (retract ?fct)
         (assert (Pull slices for range ?e to ?b for instruction ?i {
                       associated cpv ?cpv } using paths $?paths)
                 (Analyze slice ?s for ?e and cpv ?cpv)))
;------------------------------------------------------------------------------
(defrule RetractSliceSegmenterFact
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Pull slices for range ? to ? for instruction ? {
                       associated cpv ? } using paths)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule FAILURE-MISSING-SLICE 
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Pull slices for range ?e to ?b for instruction ?i {
                       associated cpv ?cpv } using paths ?path $?paths)
         (not (exists (object (is-a Slice) (Parent ?b) (TargetBlock ?e)
                              (TargetPath ?path))))
         =>
         (facts)
         (printout t "ERROR: Couldn't find an associated slice for " crlf
                   "TargetBlock = " ?e crlf
                   "Parent = " ?b crlf
                   "TargetPath = " ?path crlf
                   "TargetPath contents = " (send (symbol-to-instance-name
                                                    ?path) get-Contents) crlf
                   "Rest of paths are = " $?paths crlf)
         (exit))

;------------------------------------------------------------------------------
(defrule MergeSliceAnalysisFacts-SingleSingle
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?f0 <- (Analyze slice ?s0 for ?e and cpv ?cpv)
         ?f1 <- (Analyze slice ?s1&~?s0 for ?e and cpv ?cpv)
         ;(test (neq ?f0 ?f1))
         =>
         (retract ?f0 ?f1)
         (assert (Analyze in ?e using cpv ?cpv and slices ?s0 ?s1)))
;------------------------------------------------------------------------------
(defrule ConvertSingleSliceRule
         (declare (salience -3))
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?f0 <- (Analyze slice ?s0 for ?e and cpv ?cpv)
         =>
         (retract ?f0)
         (assert (Analyze in ?e using cpv ?cpv and slices ?s0)))
;------------------------------------------------------------------------------
(defrule MergeSliceAnalysisFacts-MultiMulti
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?f0 <- (Analyze in ?e using cpv ?cpv and slices $?q)
         ?f1 <- (Analyze in ?e using cpv ?cpv and slices $?z)
         (test (neq ?f0 ?f1)) 
         =>
         (retract ?f0 ?f1)
         (assert (Analyze in ?e using cpv ?cpv and slices $?z $?q)))
;------------------------------------------------------------------------------
(defrule SetifyAnalyzeSlicesFact
         (declare (salience -1))
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices $?a ?b $?c ?b $?d)
         =>
         (retract ?fct)
         (assert (Analyze in ?e using cpv ?cpv and slices $?a ?b $?c $?d)))
;------------------------------------------------------------------------------
(defrule ERROR-ANALYSIS-FAILURE
         (declare (salience -900))
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         (Analyze instruction ?i for ?blkID)
         ?inst <- (object (is-a Instruction) (ID ?i))
         (object (is-a PathAggregate) (Parent ?blkID) 
                 (ScheduledInstructions $?si))
         ?cpv <- (object (is-a CompensationPathVector) (Parent ?i))
         =>
         (printout t "ERROR: ANALYZE INSTRUCTION " ?i " WASN'T MATCHED!!!" crlf
                   "SCHEDULED INSTRUCTIONS: " $?si crlf)
         (send ?inst print)
         (printout t crlf)
         (send ?cpv print)
         (exit))
;------------------------------------------------------------------------------
; now that we have a list of slices to look at it's time to check and see if
; the given cpv can be moved up based on the slice. If it can't then assert 
; a fact that says as much
;------------------------------------------------------------------------------
(defrule RetractSliceAnalysis
         "Retract all slice analysis if it turns out there is a failure fact"
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices $?)
         (exists (Cant schedule ?cpv for ?e ?))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule AnalyzeSliceContentsForFailure-ProducerLowerThanTargetBlock
         "Does a check to make sure that non local dependencies prevent an 
         instruction from being moved upward into the target block"
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices ?s $?ss)
         (object (is-a Slice) (ID ?s) (TargetBlock ?e) (Parent ?b)
                 (Contents $? ?element $?))
         (object (ID ?element) (Produces $? ?nld $?))
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i))
         (object (is-a Instruction) (ID ?i) (DestinationRegisters ?dr)
                 (NonLocalDependencies $? ?nld $?))
         ?agObj <- (object (is-a PathAggregate) (Parent ?e))
         =>
         ; (printout t "Failed Instruction " ?i " because producer is lower "
         ;             "than block " ?e " on the wavefront" crlf)
         (retract ?fct)
         (bind ?ind (member ?i (send ?agObj get-InstructionList)))
         (if (neq FALSE ?ind) then
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (Cant schedule ?cpv for ?e ever)))
;------------------------------------------------------------------------------
(defrule AnalyzeSliceContentsForFailure-CallBarrier
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices ?s $?ss)
         (object (is-a Slice) (ID ?s) (TargetBlock ?e) (Parent ?b)
                 (Contents $? ?element $?))
         (object (ID ?element) (HasCallBarrier TRUE))
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i))
         (object (is-a Instruction) (ID ?i) (DestinationRegisters ?dr))
         ?agObj <- (object (is-a PathAggregate) (Parent ?e))
         =>
         (retract ?fct)
         ;(printout t "Removed " ?i " from being scheduled - "
         ;            "Call Barrier" crlf)
         (bind ?ind (member$ ?i (send ?agObj get-InstructionList)))
         (if (neq FALSE ?ind) then 
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (Cant schedule ?cpv for ?e ever)))
;------------------------------------------------------------------------------
(defrule SliceTargetHasMemoryBarrier
         "The given slice has an element that contains a memory barrier. 
         A memory barrier is only created when analysis has failed to ascertain
         what is being read from or written to in memory."
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices ?s $?ss)
         (object (is-a Slice) (ID ?s) (TargetBlock ?e) 
                 (Parent ?b) (Contents $? ?element $?))
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i))
         (object (is-a LoadInstruction|StoreInstruction) (ID ?i)
                 (DestinationRegisters ?dr))
         (object (ID ?element) (HasMemoryBarrier TRUE))
         ?agObj <- (object (is-a PathAggregate) (Parent ?e))
         =>
         (retract ?fct)
         ;(printout t "Removed " ?i " from being scheduled into " ?e 
         ;            " - MemoryBarrier" crlf)
         (bind ?ind (member$ ?i (send ?agObj get-InstructionList)))
         (if (neq FALSE ?ind) then 
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (Cant schedule ?cpv for ?e ever)))
;------------------------------------------------------------------------------
(defrule SliceTargetDoesntHaveMemoryBarrier-ModifiesSameMemory
         "The given slice has an element that contains a entry in the WritesTo 
         list that is the same thing as the given load or store instruction"
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices ?s $?ss)
         (object (is-a Slice) (ID ?s) (TargetBlock ?e) 
                 (Parent ?b) (Contents $? ?element $?))
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i))
         ?instruction <- (object (is-a LoadInstruction|StoreInstruction) 
                                 (ID ?i)
                                 (MemoryTarget ?mt) 
                                 (DestinationRegisters ?dr))
         (object (ID ?element) (HasMemoryBarrier FALSE) (HasCallBarrier FALSE)
                 (WritesTo $? ?mt $?))
         ?agObj <- (object (is-a PathAggregate) (Parent ?e))
         =>
         (retract ?fct)
         (bind ?ind (member$ ?i (send ?agObj get-InstructionList)))
         (if (neq FALSE ?ind) then 
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         ;(printout t "Removed " ?i " from being scheduled into " 
         ;					 ?e " because " ?element " - ModifiesSameMemory" crlf)
         (assert (Cant schedule ?cpv for ?e ever)))
;------------------------------------------------------------------------------
(defrule SliceTargetDoesntHaveMemoryBarrier-HasUnknownReference
         "Does now allow loads or stores to be moved above the given element
         regardless of if a memory barrier exists or not. This is because there
         is an unknown loader element"
         (Stage WavefrontSchedule $?)
         (Substage Analyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices ?s $?ss)
         (object (is-a Slice) (ID ?s) (TargetBlock ?e) 
                 (Parent ?cpv) (Contents $? ?element $?))
         (object (is-a CompensationPathVector) (ID ?cpv) (Parent ?i))
         (object (is-a LoadInstruction|StoreInstruction) (ID ?i) 
                 (Parent ?q) (DestinationRegisters ?dr))
         (object (ID ?element) (WritesTo $? UNKNOWN $?))
         ?agObj <- (object (is-a PathAggregate) (Parent ?e))
         =>
         (retract ?fct)
         ;(printout t "Removed " ?i " from being scheduled from block " ?q 
         ;					 " unknown reference" crlf)
         (bind ?ind (member$ ?i (send ?agObj get-InstructionList)))
         (if (neq FALSE ?ind) then 
           (slot-delete$ ?agObj InstructionList ?ind ?ind))
         (assert (Cant schedule ?cpv for ?e ever)))
;------------------------------------------------------------------------------
(defrule CanScheduleIntoBlockOnSlice
         (declare (salience -2))
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices ?s $?ss)
         =>
         (retract ?fct)
         (assert (Analyze in ?e using cpv ?cpv and slices $?ss)))
;------------------------------------------------------------------------------
(defrule CanScheduleInstructionThisIteration
         (declare (salience -3))
         (Stage WavefrontSchedule $?)
         (Substage SliceAnalyze $?)
         ?fct <- (Analyze in ?e using cpv ?cpv and slices)
         =>
         (retract ?fct)
         (assert (Can schedule ?cpv for ?e)))
;------------------------------------------------------------------------------
(defrule AddCPVToSuccessList
         (Stage WavefrontSchedule $?)
         (Substage MergeInit $?)
         ?fct <- (Can schedule ?cpvID for ?blkID)
         ?agObj <- (object (is-a PathAggregate) (Parent ?blkID))
         =>
         (retract ?fct)
         (slot-insert$ ?agObj MovableCompensationPathVectors 1 ?cpvID))
;------------------------------------------------------------------------------
(defrule FailCPVForNow
         (Stage WavefrontSchedule $?)
         (Substage MergeInit $?)
         ?fct <- (Cant schedule ?cpvID for ?blkID now)
         ?agObj <- (object (is-a PathAggregate) (Parent ?blkID))
         =>
         (retract ?fct)
         (slot-insert$ ?agObj StalledCompensationPathVectors 1 ?cpvID))
;------------------------------------------------------------------------------
(defrule RemoveCPVFromService
         (Stage WavefrontSchedule $?)
         (Substage MergeInit $?)
         ?fct <- (Cant schedule ?cpvID for ?blkID ever)
         ?agObj <- (object (is-a PathAggregate) (Parent ?blkID))
         ?cpvObj <- (object (is-a CompensationPathVector) (ID ?cpvID) 
                            (Parent ?i))
         =>
         (retract ?fct)
         (slot-insert$ ?cpvObj Failures 1 ?blkID)
         (slot-insert$ ?agObj ImpossibleCompensationPathVectors 1 ?cpvID))
;------------------------------------------------------------------------------
(defrule PonderMovementIteration
         (declare (salience 100))
         (Stage WavefrontSchedule $?)
         (Substage Ponder $?)
         (object (is-a Wavefront) (ID ?r) (Contents $? ?e $?))
         ?ag <- (object (is-a PathAggregate) (Parent ?e) (ID ?pa)
                        (StalledCompensationPathVectors $?scpv))
         (test (> (length$ $?scpv) 0))
         =>
         (assert (Another Movement Required))
         (modify-instance ?ag (StalledCompensationPathVectors)
                          (TargetCompensationPathVectors $?scpv)))
;------------------------------------------------------------------------------
(defrule AnotherMovementIsRequired
         (Stage WavefrontSchedule $?)
         ?ponder <- (Substage Ponder $?rest)
         ?f <- (Another Movement Required)
         =>
         ;this returns a tuple
         (retract ?ponder ?f)
         (assert (Substage GenerateAnalyze0 
                           GenerateAnalyze 
                           Analyze 
                           SliceAnalyze 
                           MergeInit 
                           Merge 
                           MergeUpdate
                           ReopenBlocks 
                           Ponder 
                           $?rest)))
;------------------------------------------------------------------------------
(defrule FinishSchedulingIntoBlock
         (declare (salience -1))
         (Stage WavefrontSchedule $?)
         (Substage Ponder $?rest)
         =>
         (progn$ (?instance (find-all-instances ((?wave Wavefront)) TRUE))
                 (progn$ (?child (send ?instance get-Contents))
                         (modify-instance 
                           (symbol-to-instance-name ?child)
                           (IsOpen FALSE)))))
;------------------------------------------------------------------------------
