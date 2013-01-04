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
; BlockDependencyCreation.clp - Contains rules pertaining to the creation of
; data dependencies between different instructions inside of a basic block
; Written by Joshua Scoggins (7/1/2012)
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkLocalDependency-Call
 			(declare (salience 1))
         (object (is-a CallInstruction) (parent ?p) (id ?t0) 
                        (ArgumentOperands $? ?o $?))
         (object (is-a Instruction) (id ?o) (parent ?p))
         =>
         (assert (Instruction ?o produces ?t0)
                 (Instruction ?t0 consumes ?o)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkLocalDependency 
         ?i0 <- (object (is-a Instruction&~CallInstruction) (parent ?p) (id ?t0) 
                        (Operands $? ?o $?))
         (object (is-a Instruction) (id ?o) (parent ?p))
         =>
         (assert (Instruction ?o produces ?t0)
                 (Instruction ?t0 consumes ?o)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkInstructionsThatHappenBeforeCall-WritesToMemory
         (object (is-a BasicBlock) (id ?v) (contents $?before ?n0 $?))
         (object (is-a CallInstruction) (id ?n0) (parent ?v) 
                 (MayWriteToMemory TRUE))
         =>
         (progn$ (?n1 ?before)
                 (assert (Instruction ?n0 consumes ?n1)
                         (Instruction ?n1 produces ?n0))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkInstructionsThatHappenBeforeCall-HasSideEffects
         (Stage Analysis $?)
         (object (is-a BasicBlock) (id ?p) (contents $?a ?n0 $?))
         (object (is-a CallInstruction) (id ?n0) (parent ?p)
                 (MayHaveSideEffects TRUE))
         =>
         (progn$ (?n1 ?a)
                 (assert (Instruction ?n0 consumes ?n1)
                         (Instruction ?n1 produces ?n0))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkCallInstructionDependency-ModifiesMemory
         "Creates a series of dependencies for all instructions following a 
         call instruction if it turns out that the call could modify memory."
         (object (is-a BasicBlock) (id ?p) (contents $? ?name $?rest))
         (object (is-a CallInstruction) (id ?name) (parent ?p)
                 (MayWriteToMemory TRUE))
         =>
         (assert (Element ?p has a CallBarrier))
         (progn$ (?following ?rest)
                 (assert (Instruction ?following has a CallDependency)
                         ;(Instruction ?following consumes ?name)
                         (Instruction ?name produces ?following))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkCallInstructionDependency-InlineAsm
         "Creates a series of dependencies for all instructions following a 
         call instruction if it turns out that the call is inline asm."
         (object (is-a BasicBlock) (id ?p) (contents $? ?name $?rest))
         (object (is-a CallInstruction) (id ?name) (parent ?p) 
                 (IsInlineAsm TRUE))
         =>
         (assert (Element ?p has a CallBarrier))
         (progn$ (?following ?rest)
                 (assert (Instruction ?following has a CallDependency)
                         ;(Instruction ?following consumes ?name)
                         (Instruction ?name produces ?following))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkCallInstructionDependency-SideEffects
         "Creates a series of dependencies for all instructions following a 
         call instruction if it turns out that the call has side effects."
         (object (is-a CallInstruction) (id ?name) (parent ?p)
                 (MayHaveSideEffects TRUE)) 
         (object (is-a BasicBlock) (id ?p) (contents $? ?name $?rest))
         =>
         (assert (Element ?p has a CallBarrier))
         (progn$ (?following ?rest)
                 (assert (Instruction ?following has a CallDependency)
                         ;(Instruction ?following consumes ?name)
                         (Instruction ?name produces ?following))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::FlagCallBarrierForDiplomat-HasParent
         ;(declare (salience -10))
         ?fct <- (Element ?z has a CallBarrier)
         ?d <- (object (is-a Diplomat) (id ?z) (parent ?p) 
                       (HasCallBarrier FALSE))
         (exists (object (is-a Diplomat) (id ?p)))
         =>
         (retract ?fct)
         (assert (Element ?p has a CallBarrier))
         (modify-instance ?d (HasCallBarrier TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::PropagateCallBarrierForDiplomat-HasParent
         ;(declare (salience -10))
         ?fct <- (Element ?z has a CallBarrier)
         ?d <- (object (is-a Diplomat) (id ?z) (parent ?p) 
                       (HasCallBarrier TRUE))
         (exists (object (is-a Diplomat) (id ?p)))
         =>
         (retract ?fct)
         (assert (Element ?p has a CallBarrier)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::FlagCallBarrierForDiplomat-NoParent
         ;(declare (salience -10))
         ?fct <- (Element ?z has a CallBarrier)
         ?d <- (object (is-a Diplomat) (id ?z) (parent ?p) 
                       (HasCallBarrier FALSE))
         (not (exists (object (is-a Diplomat) (id ?p))))
         =>
         (retract ?fct)
         (modify-instance ?d (HasCallBarrier TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::PropagateCallBarrierForDiplomat-NoParent
         ;(declare (salience -10))
         ?fct <- (Element ?z has a CallBarrier)
         ?d <- (object (is-a Diplomat) (id ?z) (parent ?p) 
                       (HasCallBarrier TRUE))
         (not (exists (object (is-a Diplomat) (id ?p))))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::MarkHasACallDependency-Set
         ?fct <- (Instruction ?target has a CallDependency)
         ?inst <- (object (is-a Instruction) (id ?target) 
                          (HasCallDependency FALSE))
         =>
         (retract ?fct)
         (modify-instance ?inst (HasCallDependency TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::MarkHasACallDependency-Ignore
         ?fct <- (Instruction ?target has a CallDependency)
         ?inst <- (object (is-a Instruction) (id ?target) 
                          (HasCallDependency TRUE))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::StoreToLoadDependency
         (object (is-a StoreInstruction) (parent ?p) (id ?t0)
                 (TimeIndex ?ti0) (MemoryTarget ?sym0))
         (object (is-a LoadInstruction) (parent ?p) (id ?t1) 
                 (TimeIndex ?ti1&:(< ?ti0 ?ti1)) (MemoryTarget ?sym1))
         (test (or (eq ?sym0 ?sym1) (eq ?sym0 UNKNOWN)))
         =>
         (assert (Instruction ?t1 consumes ?t0)
                 (Instruction ?t0 produces ?t1)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::StoreToStoreDependency
         (object (is-a StoreInstruction) (parent ?p) (id ?t0)
                 (TimeIndex ?ti0) (MemoryTarget ?sym0))
         (object (is-a StoreInstruction) (parent ?p) (id ?t1) 
                 (TimeIndex ?ti1&:(< ?ti0 ?ti1)) (MemoryTarget ?sym1))
         (test (or (eq ?sym0 ?sym1) (eq ?sym0 UNKNOWN)))
         =>
         (assert (Instruction ?t1 consumes ?t0)
                 (Instruction ?t0 produces ?t1)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::LoadToStoreDependency
         (object (is-a LoadInstruction) (parent ?p) (id ?t0)
                 (TimeIndex ?ti0) (MemoryTarget ?sym0)) 
         (object (is-a StoreInstruction) (parent ?p) (id ?t1) 
                 (TimeIndex ?ti1&:(< ?ti0 ?ti1)) (MemoryTarget ?sym1))
         (test (or (eq ?sym0 ?sym1) (eq ?sym0 UNKNOWN)))
         =>
         (assert (Instruction ?t1 consumes ?t0)
                 (Instruction ?t0 produces ?t1)))
;------------------------------------------------------------------------------
