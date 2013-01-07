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
; This stage is the actual act of scheduling the blocks on the wavefront. 
; The first step is to reacquire all dependencies of the given blocks by
; running the same rules as before. The only difference is that we have to do
; it specially for the blocks on the wavefront. 
; 
; I'm thinking of just copying the rules from the analysis pass to here. It
; would be a duplication but I frankly don't care anymore. 
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::CreateDependencyAnalysisTargets
         (declare (salience 10))
         (object (is-a Wavefront) 
                 (parent ?r) 
                 (contents $? ?e $?))
         (object (is-a BasicBlock) 
                 (id ?e))
         (object (is-a PathAggregate) 
                 (parent ?e) 
                 (OriginalStopIndex ?si))
         =>
         ;only look at instructions starting at the original stop index. This
         ;prevents unncessary recomputation
         (assert (Evaluate ?e for dependencies starting at ?si)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::IdentifyWAR
         "Identifies a WAR dependency between two instructions. It will not 
         match if it turns out the values are constant integers or constant 
         floating point values"
         (Evaluate ?p for dependencies starting at ?si)
         ;TODO: Rewrite this to be more efficient
         ?i0 <- (object (is-a Instruction) 
                        (parent ?p) 
                        (id ?t0)
                        (TimeIndex ?ti0)
                        (Operands $? ?c $?))
         (object (is-a TaggedObject&~ConstantInteger&~ConstantFloatingPoint) 
                 (id ?c))
         ?i1 <- (object (is-a Instruction) 
                        (parent ?p) 
                        (id ?t1)
                        (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?ti0 ?ti1)))
                        (DestinationRegisters $? ?c $?))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?t1 => ?t0))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::IdentifyRAW
         "Identifies a RAW dependency between two instructions in the same 
         block. It will not match if it turns out that the values are constant 
         integers or constant floating point values."
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a Instruction) 
                 (parent ?p) 
                 (id ?t0)
                 (TimeIndex ?ti0)
                 (DestinationRegisters $? ?c $?))
         (object (is-a TaggedObject&~ConstantInteger&~ConstantFloatingPoint) 
                 (id ?c))
         (object (is-a Instruction) 
                 (parent ?p) 
                 (id ?t1)
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?ti0 ?ti1)))
                 (Operands $? ?c $?))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?t1 => ?t0))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::IdentifyWAW
         "Identifies a WAW dependency between two instructions in the same 
         block. It will not match if it turns out that the values are constant 
         integers or constant floating point values."
         (Evaluate ?p for dependencies starting at ?si)
         ?i0 <- (object (is-a Instruction) 
                        (parent ?p) 
                        (id ?t0)
                        (TimeIndex ?ti0)
                        (DestinationRegisters $? ?c $?))
         (object (is-a TaggedObject&~ConstantInteger&~ConstantFloatingPoint) 
                 (id ?c))
         ?i1 <- (object (is-a Instruction) 
                        (parent ?p) 
                        (id ?t1) 
                        (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?ti0 ?ti1))) 
                        (DestinationRegisters $? ?c $?))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?t1 => ?t0))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
;these call instruction checks only work for new instructions or those that
; dont have a call dependency. As that was the only way they got into the 
; block in the first place
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::MarkCallInstructionDependency-ModifiesMemory
         "Creates a series of dependencies for all instructions following a 
         call instruction if it turns out that the call could modify memory."
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a CallInstruction) 
                 (id ?name) 
                 (parent ?p) 
                 (DoesNotAccessMemory FALSE) (OnlyReadsMemory FALSE) 
                 (MayWriteToMemory TRUE)
                 (TimeIndex ?t0))
         (object (is-a Instruction) 
                 (parent ?p) 
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?t0 ?ti1)))
                 (HasCallDependency FALSE) (id ?following))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?following => ?name))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?name => ?following))
                 (message (to wavefront-scheduling)
                          (action instruction-has-a-call-dependency)
                          (arguments ?following))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::MarkCallInstructionDependency-InlineAsm
         "Creates a series of dependencies for all instructions following a 
         call instruction if it turns out that the call is inline asm."
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a CallInstruction) 
                 (id ?name) 
                 (parent ?p) 
                 (IsInlineAsm TRUE)
                 (TimeIndex ?t0))
         (object (is-a Instruction) 
                 (parent ?p) 
                 (id ?following)
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?t0 ?ti1)))
                 (HasCallDependency FALSE))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?following => ?name))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?name => ?following))
                 (message (to wavefront-scheduling)
                          (action instruction-has-a-call-dependency)
                          (arguments ?following))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::MarkCallInstructionDependency-SideEffects
         "Creates a series of dependencies for all instructions following a 
         call instruction if it turns out that the call is inline asm."
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a CallInstruction) 
                 (id ?name) 
                 (parent ?p) 
                 (MayHaveSideEffects TRUE) 
                 (MayWriteToMemory TRUE) 
                 (TimeIndex ?t0))
         (object (is-a Instruction) 
                 (parent ?p) 
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?t0 ?ti1)))
                 (HasCallDependency FALSE) 
                 (id ?following))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?following => ?name))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?name => ?following))
                 (message (to wavefront-scheduling)
                          (action instruction-has-a-call-dependency)
                          (arguments ?following))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::MarkNonLocalDependencies
         (Evaluate ?p for dependencies starting at ?si)
         ?inst <- (object (is-a Instruction) 
                          (parent ?p) 
                          (TimeIndex ?t&:(>= ?t ?si))
                          (NonLocalDependencies $?nld)
                          (Operands $? ?o $?))
         (object (is-a Instruction) 
                 (id ?o) 
                 (parent ~?p))
         =>
         (modify-instance ?inst (NonLocalDependencies $?nld ?o)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::Wavefront-MarkHasCallDependency
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action instruction-has-a-call-dependency)
                          (arguments ?f))

         ?obj <- (object (is-a Instruction) 
                         (id ?f))
         =>
         (modify-instance ?obj (HasCallDependency TRUE))
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::InjectConsumers
         "Adds a given consumer to the target instruction"
         (declare (salience -5))
         ?fct <- (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?target => ?id))
         ?inst <- (object (is-a Instruction) 
                          (id ?id)
                          (Consumers $?consumers))
         =>
         (retract ?fct)
         (if (not (member$ ?target $?consumers)) then
           (modify-instance ?inst (Consumers $?consumers ?target))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::InjectProducers
         "Adds a given producer to the target instruction"
         (declare (salience -5))
         ?fct <- (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?target => ?id))
         ?inst <- (object (is-a Instruction) 
                          (id ?id)
                          (LocalDependencies $?ld)
                          (Producers $?prod))
         =>
         (retract ?fct)
         (modify-instance ?inst 
                          (LocalDependencies $?ld 
                                             (if (not (member$ ?target $?ld)) 
                                               then ?target))
                          (Producers $?prod 
                                     (if (not (member$ ?target $?prod)) 
                                       then ?target))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::StoreToLoadDependency
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a StoreInstruction) 
                 (parent ?p) 
                 (id ?t0)
                 (TimeIndex ?ti0) 
                 (MemoryTarget ?sym0))
         (object (is-a LoadInstruction) 
                 (parent ?p) 
                 (id ?t1) 
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?ti0 ?ti1)))
                 (MemoryTarget ?sym1))
         (test (or (eq ?sym0 ?sym1) 
                   (eq ?sym0 UNKNOWN)))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?t1 => ?t0))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::StoreToStoreDependency
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a StoreInstruction) 
                 (parent ?p) 
                 (id ?t0)
                 (TimeIndex ?ti0) 
                 (MemoryTarget ?sym0))
         (object (is-a StoreInstruction) 
                 (parent ?p) 
                 (id ?t1) 
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?ti0 ?ti1)))
                 (MemoryTarget ?sym1))
         (test (or (eq ?sym0 ?sym1) 
                   (eq ?sym0 UNKNOWN)))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?t1 => ?t0))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::LoadToStoreDependency
         (Evaluate ?p for dependencies starting at ?si)
         (object (is-a LoadInstruction) 
                 (parent ?p) 
                 (id ?t0)
                 (TimeIndex ?ti0) 
                 (MemoryTarget ?sym0)) 
         (object (is-a StoreInstruction) 
                 (parent ?p) 
                 (id ?t1) 
                 (TimeIndex ?ti1&:(and (>= ?ti1 ?si) (< ?ti0 ?ti1)))
                 (MemoryTarget ?sym1))
         (test (or (eq ?sym0 ?sym1) 
                   (eq ?sym0 UNKNOWN)))
         =>
         (assert (message (to wavefront-scheduling)
                          (action instruction-consumes)
                          (arguments ?t1 => ?t0))
                 (message (to wavefront-scheduling)
                          (action instruction-produces)
                          (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-dependency-analysis::FinishedDependencyAnalysis 
         (declare (salience -800))
         ?fct <- (Evaluate ?p for dependencies starting at ?v)
         (object (is-a BasicBlock) 
                 (id ?p) 
                 (parent ?r))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule RemoveInstructionsFromProducers
         (declare (salience 768))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Remove evidence of ?tInst from instructions ?inst $?insts)
         ?iObj <- (object (is-a Instruction) 
                          (ID ?inst) 
                          (Producers $?pb ?tInst $?pa)
                          (LocalDependencies $?ldb ?tInst $?lda))
         =>
         (retract ?fct)
         (assert (Remove evidence of ?tInst from instructions $?insts))
         (modify-instance ?iObj (Producers $?pb $?pa)
                          (LocalDependencies $?ldb $?lda))
         (slot-insert$ ?iObj NonLocalDependencies 1 ?tInst))
;------------------------------------------------------------------------------
(defrule RetractRemoveInstructionsFromProducers
         (declare (salience 768))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Remove evidence of ? from instructions)
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule StartRecomputeBlock
         (declare (salience 100))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Recompute block ?b)
         ?bb <- (object (is-a BasicBlock) (ID ?b) 
                        (Contents $?instructions ?last))
         (object (is-a TerminatorInstruction) (ID ?last))
         =>
         (modify-instance ?bb (ReadsFrom) (WritesTo) (HasMemoryBarrier FALSE))
         (retract ?fct)
         (assert (Recompute block ?b with instructions $?instructions)))
;------------------------------------------------------------------------------
(defrule RecomputeNonMemoryInstructionForBlock
         (declare (salience 99))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Recompute block ?b with instructions ?inst $?rest)
         (object (is-a BasicBlock) (ID ?b))
         (object (is-a Instruction&~LoadInstruction&~StoreInstruction) 
                 (ID ?inst) (Parent ?b))
         =>
         (retract ?fct)
         (assert (Recompute block ?b with instructions $?rest)))
;------------------------------------------------------------------------------
(defrule RecomputeLoadInstructionForBlock
         (declare (salience 99))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Recompute block ?b with instructions ?inst $?rest)
         (object (is-a LoadInstruction) (ID ?inst) (Parent ?b) 
                 (MemoryTarget ?mt)) 
         ?bb <- (object (is-a BasicBlock) (ID ?b))
         =>
         (if (not (member$ ?mt (send ?bb get-ReadsFrom))) then
           (slot-insert$ ?bb ReadsFrom 1 ?mt))
         (retract ?fct)
         (assert (Recompute block ?b with instructions $?rest)))
;------------------------------------------------------------------------------
(defrule RecomputeStoreInstructionForBlock
         (declare (salience 99))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Recompute block ?b with instructions ?inst $?rest)
         (object (is-a StoreInstruction) (ID ?inst) (Parent ?b) 
                 (MemoryTarget ?mt))
         ?bb <- (object (is-a BasicBlock) (ID ?b))
         =>
         (if (not (member$ ?mt (send ?bb get-WritesTo))) then
           (slot-insert$ ?bb WritesTo 1 ?mt))
         (retract ?fct)
         (assert (Recompute block ?b with instructions $?rest)))
;------------------------------------------------------------------------------
(defrule FinishRecomputationForBlock
         (declare (salience 98))
         (Stage WavefrontSchedule $?)
         (Substage MergeUpdate $?)
         ?fct <- (Recompute block ?b with instructions)
         ?bb <- (object (is-a BasicBlock) (ID ?b) (ReadsFrom $?rf)
                        (WritesTo $?wt))
         =>
         (retract ?fct)
         (if (or (member$ UNKNOWN ?rf)
                 (member$ UNKNOWN ?wt)) then
           (modify-instance ?bb (HasMemoryBarrier TRUE))))
;------------------------------------------------------------------------------
