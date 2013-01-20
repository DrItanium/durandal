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
			(object (is-a CallInstruction) 
					  (parent ?p) 
					  (id ?t0) 
					  (ArgumentOperands $? ?o $?))
			(object (is-a Instruction) 
					  (id ?o) 
					  (parent ?p))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?o => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t0 => ?o))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkLocalDependency 
			(object (is-a Instruction&~CallInstruction) 
					  (parent ?p) 
					  (id ?t0) 
					  (Operands $? ?o $?))
			(object (is-a Instruction) 
					  (id ?o) 
					  (parent ?p))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?o => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t0 => ?o))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkInstructionsThatHappenBeforeCall-WritesToMemory
			;the parent is implied by the fact that it's part of the basic block
			(object (is-a BasicBlock) 
					  (contents $?before ?n0 $?))
			(object (is-a CallInstruction) 
					  (id ?n0) 
					  (MayWriteToMemory TRUE))
			=>
			(progn$ (?n1 ?before)
					  (assert (message (to dependency-analysis)
											 (action instruction-produces)
											 (arguments ?n1 => ?n0))
								 (message (to dependency-analysis)
											 (action instruction-consumes)
											 (arguments ?n0 => ?n1)))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkInstructionsThatHappenBeforeCall-HasSideEffects
			;the parent is implied by the fact that it's part of the basic block
			(Stage Analysis $?)
			(object (is-a BasicBlock) 
					  (contents $?a ?n0 $?))
			(object (is-a CallInstruction) 
					  (id ?n0) 
					  (MayHaveSideEffects TRUE))
			=>
			(progn$ (?n1 ?a)
					  (assert (message (to dependency-analysis)
											 (action instruction-produces)
											 (arguments ?n1 => ?n0))
								 (message (to dependency-analysis)
											 (action instruction-consumes)
											 (arguments ?n0 => ?n1)))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkCallInstructionDependency-ModifiesMemory
			"Creates a series of dependencies for all instructions following a 
			call instruction if it turns out that the call could modify memory."
			(object (is-a BasicBlock) 
					  (id ?p)
					  (contents $? ?name $?rest))
			(object (is-a CallInstruction) 
					  (id ?name) 
					  ;removing parent match reduces join network complexity
					  (MayWriteToMemory TRUE))
			=>
			(assert (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?p)))
			(progn$ (?following ?rest)
					  (assert (message (to dependency-analysis)
											 (action instruction-has-a-call-barrier)
											 (arguments ?following))
								 ;(message (to dependency-analysis)
								 ;	      (action instruction-consumes)
								 ;		   (arguments ?following => ?name))
								 (message (to dependency-analysis)
											 (action instruction-produces)
											 (arguments ?name => ?following)))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkCallInstructionDependency-InlineAsm
			"Creates a series of dependencies for all instructions following a 
			call instruction if it turns out that the call is inline asm."
			(object (is-a BasicBlock) 
					  (id ?p) 
					  (contents $? ?name $?rest))
			(object (is-a CallInstruction) 
					  (id ?name) 
					  ;removing parent reduces join network complexity
					  (IsInlineAsm TRUE))
			=>
			(assert (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?p)))
			(progn$ (?following ?rest)
					  (assert (message (to dependency-analysis)
											 (action instruction-has-a-call-barrier)
											 (arguments ?following))
								 ;(message (to dependency-analysis)
								 ;	      (action instruction-consumes)
								 ;		   (arguments ?following => ?name))
								 (message (to dependency-analysis)
											 (action instruction-produces)
											 (arguments ?name => ?following)))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MarkCallInstructionDependency-SideEffects
			"Creates a series of dependencies for all instructions following a 
			call instruction if it turns out that the call has side effects."
			(object (is-a BasicBlock) 
					  (id ?p)
					  (contents $? ?name $?rest))
			(object (is-a CallInstruction) 
					  (id ?name) 
					  ;removing parent reduces join network complexity 
					  (MayHaveSideEffects TRUE)) 
			=>
			(assert (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?p)))
			(progn$ (?following ?rest)
					  (assert (message (to dependency-analysis)
											 (action instruction-has-a-call-barrier)
											 (arguments ?following))
								 ;(message (to dependency-analysis)
								 ;	      (action instruction-consumes)
								 ;		   (arguments ?following => ?name))
								 (message (to dependency-analysis)
											 (action instruction-produces)
											 (arguments ?name => ?following)))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::FlagCallBarrierForDiplomat-HasParent
			;(declare (salience -10))
			?fct <- (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?z))
			?d <- (object (is-a Diplomat) 
							  (id ?z) 
							  (HasCallBarrier FALSE)
							  (parent ?p))
			(exists (object (is-a Diplomat) 
								 (id ?p)))
			=>
			(modify ?fct (arguments ?p))
			(modify-instance ?d (HasCallBarrier TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::PropagateCallBarrierForDiplomat-HasParent
			;(declare (salience -10))
			?fct <- (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?z))
			(object (is-a Diplomat) 
							  (id ?z) 
							  (HasCallBarrier TRUE)
							  (parent ?p))
			(exists (object (is-a Diplomat) 
								 (id ?p)))
			=>
			(modify ?fct (arguments ?p)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::FlagCallBarrierForDiplomat-NoParent
			;(declare (salience -10))
			?fct <- (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?z))
			?d <- (object (is-a Diplomat) 
							  (id ?z) 
							  (HasCallBarrier FALSE)
							  (parent ?p))
			(not (exists (object (is-a Diplomat) 
										(id ?p))))
			=>
			(retract ?fct)
			(modify-instance ?d (HasCallBarrier TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::PropagateCallBarrierForDiplomat-NoParent
			;(declare (salience -10))
			?fct <- (message (to dependency-analysis)
								  (action element-has-a-call-barrier)
								  (arguments ?z))
			(object (is-a Diplomat) 
							  (id ?z) 
							  (HasCallBarrier TRUE)
							  (parent ?p))
			(not (exists (object (is-a Diplomat) 
										(id ?p))))
			=>
			(retract ?fct))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::MarkHasACallDependency-Set
			?fct <- (message (to dependency-analysis)
								  (action instruction-has-a-call-barrier)
								  (arguments ?target))
			?inst <- (object (is-a Instruction) 
								  (id ?target) 
								  (HasCallDependency FALSE))
			=>
			(retract ?fct)
			(modify-instance ?inst (HasCallDependency TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis-update::MarkHasACallDependency-Ignore
			?fct <- (message (to dependency-analysis)
								  (action instruction-has-a-call-barrier)
								  (arguments ?target))
			(object (is-a Instruction) 
								  (id ?target) 
								  (HasCallDependency TRUE))
			=>
			(retract ?fct))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::StoreDependency-UNKNOWN
			(object (is-a BasicBlock) 
					  (contents $? ?t0 $? ?t1 $?))
			(object (is-a StoreInstruction) 
					  (id ?t0)
					  (MemoryTarget UNKNOWN))
			(object (is-a StoreInstruction|LoadInstruction) 
					  (id ?t1))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t1 => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::LoadDependency-UNKNOWN
			(object (is-a BasicBlock) 
					  (contents $? ?t0 $? ?t1 $?))
			(object (is-a LoadInstruction) 
					  (id ?t0) 
					  (MemoryTarget UNKNOWN))
			(object (is-a StoreInstruction) 
					  (id ?t1))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t1 => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::Store=>LoadDependency
			(object (is-a BasicBlock) 
					  (contents $? ?t0 $? ?t1 $?))
			(object (is-a StoreInstruction) 
					  (id ?t0)
					  (MemoryTarget ?sym0&~UNKNOWN))
			(object (is-a LoadInstruction) 
					  (id ?t1) 
					  (MemoryTarget ?sym0))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t1 => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::Store=>StoreDependency
			(object (is-a BasicBlock) 
					  (contents $? ?t0 $? ?t1 $?))
			(object (is-a StoreInstruction) 
					  (id ?t0)
					  (MemoryTarget ?sym0&~UNKNOWN))
			(object (is-a StoreInstruction) 
					  (id ?t1) 
					  (MemoryTarget ?sym0))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t1 => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis::Load=>StoreDependency
			(object (is-a BasicBlock)
					  (contents $? ?t0 $? ?t1 $?))
			(object (is-a LoadInstruction) 
					  (id ?t0)
					  (MemoryTarget ?sym0&~UNKNOWN)) 
			(object (is-a StoreInstruction) 
					  (id ?t1) 
					  (MemoryTarget ?sym0))
			=>
			(assert (message (to dependency-analysis)
								  (action instruction-consumes)
								  (arguments ?t1 => ?t0))
					  (message (to dependency-analysis)
								  (action instruction-produces)
								  (arguments ?t0 => ?t1))))
;------------------------------------------------------------------------------
