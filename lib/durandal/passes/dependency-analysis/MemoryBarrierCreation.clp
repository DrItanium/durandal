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
; MemoryBarrierCreation.clp - Contains rules pertaining to the creation of
; memory barriers to prevent loads and stores from being moved above a specific
; point within the program during the act of scheduling.
; Written by Joshua Scoggins (7/1/2012)
;------------------------------------------------------------------------------
; Barriers consist of reads and writes that can't be verified as to their exact
; source. 
;------------------------------------------------------------------------------
; TODO: Put in a field for load and store instructions that describes what
; is being referenced. By default this slot is UNKNOWN.
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::AssertLoadBarrierEvaluation
			"Creates a fact that tells the system to analyze the given load 
			instruction to see if it is necessary to create a memory barrier for 
			the given instruction"
			(declare (salience 5))
			(object (is-a LoadInstruction) (name ?t0) (Operands ?target $?))
			=>
			(assert (Analyze ?target for load ?t0)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::AssertStoreBarrierEvaluation
			"Creates a fact that tells the system to analyze the given store 
			instruction to see if it is necessary to create a memory barrier for 
			the given instruction"
			(declare (salience 5))
			(object (is-a StoreInstruction) (name ?t0) 
					  (DestinationRegisters ?target))
			=>
			(assert (Analyze ?target for store ?t0)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::IdentifyGetElementPointerLoadBarrier
			"Creates a load memory barrier hint if it turns out that the load 
			instruction refers to a getelementptr instruction but that 
			getelementptr doesn't refer to a alloca instruction or constant."
			(declare (salience 4))
			?fct <- (Analyze ?target for load ?t0)
			?i0 <- (object (is-a LoadInstruction) (name ?t0) (parent ?p))
			(object (is-a GetElementPointerInstruction) (name ?target) 
					  (Operands ?a $?))
			(object (is-a ~AllocaInstruction&~Constant) (name ?a))
			(object (is-a BasicBlock) (name ?p) (parent ?r))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget UNKNOWN))
			(assert (element ?p reads from UNKNOWN)
					  (Element ?p has a MemoryBarrier)
					  (Element ?r has a MemoryBarrier)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithReadFrom-Alloca
			"Does a check to see if the load instruction refers directly to an
			AllocaInstruction or Constant. If it does then mark it as though the 
			block reads from it"
			(declare (salience 4))
			?fct <- (Analyze ?target for load ?t0)
			?i0 <- (object (is-a LoadInstruction) (name ?t0) (parent ?p))
			(object (is-a AllocaInstruction) (name ?target))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?target))
			(assert (element ?p reads from ?target)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithReadFrom-Constant
			"Does a check to see if the load instruction refers directly to an
			AllocaInstruction or Constant. If it does then mark it as though the 
			block reads from it"
			(declare (salience 4))
			?fct <- (Analyze ?target for load ?t0)
			?i0 <- (object (is-a LoadInstruction) (name ?t0) (parent ?p))
			(object (is-a Constant) (name ?target))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?target))
			(assert (element ?p reads from ?target)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithReadFrom-GetElementPointer-Alloca
			"Does a check to see if a given LoadInstruction referring to a
			GetElementPointerInstruction refers to an AllocaInstruction or 
			Constant. If it does then mark it in the ReadsFrom multifield"
			(declare (salience 4))
			?fct <- (Analyze ?target for load ?t0)
			?i0 <- (object (is-a LoadInstruction) (name ?t0) (parent ?p))
			(object (is-a GetElementPointerInstruction) (name ?target) 
					  (Operands ?a $?))
			(object (is-a AllocaInstruction) (name ?a))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?a))
			(assert (element ?p reads from ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithReadFrom-GetElementPointer-Constant
			"Does a check to see if a given LoadInstruction referring to a
			GetElementPointerInstruction refers to an AllocaInstruction or 
			Constant. If it does then mark it in the ReadsFrom multifield"
			(declare (salience 4))
			?fct <- (Analyze ?target for load ?t0)
			?i0 <- (object (is-a LoadInstruction) (name ?t0) (parent ?p))
			(object (is-a GetElementPointerInstruction) (name ?target) 
					  (Operands ?a $?))
			(object (is-a Constant) (name ?a))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?a))
			(assert (element ?p reads from ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::IdentifyGeneralLoadBarrier
			"Creates a load memory barrier hint if it turns out that the load 
			instruction in question refers to a register that isn't an 
			AllocaInstruction, GetElementPointerInstruction, or Constant."
			(declare (salience 3))
			?fct <- (Analyze ?target for load ?t0)
			?i0 <- (object (is-a LoadInstruction) (name ?t0) (parent ?p))
			(object (is-a BasicBlock) (name ?p) (parent ?r))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget UNKNOWN))
			(assert (element ?p reads from UNKNOWN)
					  (Element ?p has a MemoryBarrier)
					  (Element ?r has a MemoryBarrier)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::IdentifyGetElementPointerStoreBarrier
			"Creates a store memory barrier hint if it turns out that the store 
			instruction refers to a getelementptr instruction but that 
			getelementptr doesn't refer to a alloca instruction or constant."
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (parent ?p) (name ?t0))
			(object (is-a GetElementPointerInstruction) (name ?target) 
					  (Operands ?a $?))
			(object (is-a ~AllocaInstruction&~Constant) (name ?a))
			(object (is-a BasicBlock) (name ?p) (parent ?r))
			=>
			(modify-instance ?i0 (MemoryTarget UNKNOWN))
			(retract ?fct)
			(assert (element ?p writes to UNKNOWN)
					  (Element ?p has a MemoryBarrier)
					  (Element ?r has a MemoryBarrier)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithWriteTo-Alloca
			"Does a check to see if the load instruction refers directly to an
			AllocaInstruction or Constant. If it does then mark it as though the 
			block writes to it"
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a AllocaInstruction) (name ?target))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?target))
			(assert (element ?p writes to ?target)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithWriteTo-Constant
			"Does a check to see if the store instruction refers directly to an
			AllocaInstruction or Constant. If it does then mark it as though the 
			block writes to it"
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a Constant) (name ?target))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?target))
			(assert (element ?p writes to ?target)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithWriteTo-Single-Alloca
			"Does a check to see if the store instruction refers directly to an
			AllocaInstruction or Constant. If it does then mark it as though the 
			block writes to it"
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a AllocaInstruction) (name ?target))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?target))
			(assert (element ?p writes to ?target)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithWriteTo-Single-Constant
			"Does a check to see if the store instruction refers directly to an
			AllocaInstruction or Constant. If it does then mark it as though the 
			block writes to it"
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a Constant) (name ?target))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?target))
			(assert (element ?p writes to ?target)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithWriteTo-GetElementPointer-Alloca
			"Does a check to see if a given StoreInstruction referring to a
			GetElementPointerInstruction refers to an AllocaInstruction or 
			Constant. If it does then mark it in the WritesTo multifield"
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a GetElementPointerInstruction) (name ?target) 
					  (Operands ?a $?))
			(object (is-a AllocaInstruction) (name ?a))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?a))
			(assert (element ?p writes to ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::PopulateBasicBlockWithWriteTo-GetElementPointer-Constant
			"Does a check to see if a given StoreInstruction referring to a
			GetElementPointerInstruction refers to an AllocaInstruction or 
			Constant. If it does then mark it in the WritesTo multifield"
			(declare (salience 4))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a GetElementPointerInstruction) (name ?target) 
					  (Operands ?a $?))
			(object (is-a Constant) (name ?a))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget ?a))
			(assert (element ?p writes to ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::IdentifyGeneralStoreBarrier
			"Creates a store memory barrier hint if it turns out that the load 
			instruction in question refers to a register that isn't an 
			AllocaInstruction, GetElementPointerInstruction, or Constant."
			(declare (salience 3))
			?fct <- (Analyze ?target for store ?t0)
			?i0 <- (object (is-a StoreInstruction) (name ?t0) (parent ?p))
			(object (is-a BasicBlock) (name ?p) (parent ?r))
			=>
			(retract ?fct)
			(modify-instance ?i0 (MemoryTarget UNKNOWN))
			(assert (element ?p writes to UNKNOWN)
					  (Element ?p has a MemoryBarrier)
					  (Element ?r has a MemoryBarrier)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::InitializeReadsFromMerger
			?f0 <- (element ?p reads from $?t0)
			(not (exists (message (to dependency-analysis)
										 (action reads-from-list)
										 (arguments ?p => $?))))
			=>
			(retract ?f0)
			(assert (message (to dependency-analysis)
								  (action reads-from-list)
								  (arguments ?p => $?t0))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::InitializeWritesToMerger
			?f0 <- (element ?p writes to $?t0)
			(not (exists (message (to dependency-analysis)
										 (action writes-to-list)
										 (arguments ?p => $?))))
			=>
			(retract ?f0)
			(assert (message (to dependency-analysis)
								  (action writes-to-list)
								  (arguments ?p => $?t0))))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MergeReadsFrom
			?f0 <- (element ?p reads from $?t0)
			?f1 <- (message (to dependency-analysis)
								 (action reads-from-list)
								 (arguments ?p => $?t1))
			=>
			(retract ?f0)
			(modify ?f1 (arguments ?p => $?t1 $?t0)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::MergeWritesTo
			?f0 <- (element ?p writes to $?t0)
			?f1 <- (message (to dependency-analysis)
								 (action writes-to-list)
								 (arguments ?p => $?t1))
			=>
			(retract ?f0)
			(modify ?f1 (arguments ?p => $?t1 $?t0)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::InsertIntoDiplomatReadsFrom-ParentDoesntExist
			(declare (salience -9))
			?fct <- (message (to dependency-analysis)
								  (action reads-from-list)
								  (arguments ?p => $?t))
			?bb <- (object (is-a Diplomat) (name ?p) (parent ?q))
			(not (exists (object (is-a Diplomat) 
										(name ?q))))
			=>
			(retract ?fct)
			(slot-insert$ ?bb ReadsFrom 1 ?t))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::InsertIntoDiplomatReadsFrom-ParentExists
			(declare (salience -9))
			?fct <- (message (to dependency-analysis)
								  (action reads-from-list)
								  (arguments ?p => $?t))
			?bb <- (object (is-a Diplomat) 
								(name ?p) 
								(parent ?q))
			(exists (object (is-a Diplomat) 
								 (name ?q)))
			=>
			(modify ?fct (arguments ?q => ?t))
			(slot-insert$ ?bb ReadsFrom 1 ?t))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::InsertIntoDiplomatWritesTo-ParentDoesntExist
			(declare (salience -9))
			?fct <- (message (to dependency-analysis)
								  (action writes-to-list)
								  (arguments ?p => $?t))
			?bb <- (object (is-a Diplomat) 
								(name ?p) 
								(parent ?q))
			(not (exists (object (is-a Diplomat) 
										(name ?q))))
			=>
			(retract ?fct)
			(slot-insert$ ?bb WritesTo 1 ?t))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::InsertIntoDiplomatWritesTo-ParentExists
			(declare (salience -9))
			?fct <- (message (to dependency-analysis)
								  (action writes-to-list)
								  (arguments ?p => $?t))
			?bb <- (object (is-a Diplomat) (name ?p) (parent ?q))
			(exists (object (is-a Diplomat) (name ?q)))
			=>
			(modify ?fct (arguments ?q => $?t))
			(slot-insert$ ?bb WritesTo 1 ?t))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::UpdateDiplomatHasMemoryBarrier
			(declare (salience -10))
			?fct <- (Element ?b has a MemoryBarrier)
			?obj <- (object (is-a Diplomat) (name ?b) (HasMemoryBarrier FALSE)
								 (parent ?p))
			(exists (object (is-a Diplomat) (name ?p)))
			=>
			(retract ?fct)
			(assert (Element ?p has a MemoryBarrier))
			(modify-instance ?obj (HasMemoryBarrier TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::RetractDiplomatHasMemoryBarrier
			(declare (salience -10))
			?fct <- (Element ?b has a MemoryBarrier)
			?obj <- (object (is-a Diplomat) (name ?b) (HasMemoryBarrier TRUE)
								 (parent ?p))
			(exists (object (is-a Diplomat) (name ?p)))
			=>
			(retract ?fct)
			(assert (Element ?p has a MemoryBarrier)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::UpdateDiplomatHasMemoryBarrier-ParentDoesntExist
			(declare (salience -10))
			?fct <- (Element ?b has a MemoryBarrier)
			?obj <- (object (is-a Diplomat) (name ?b) (HasMemoryBarrier FALSE)
								 (parent ?p))
			(not (exists (object (is-a Diplomat) (name ?p))))
			=>
			(retract ?fct)
			(modify-instance ?obj (HasMemoryBarrier TRUE)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-analysis::RetractDiplomatHasMemoryBarrier-ParentDoesntExist
			(declare (salience -10))
			?fct <- (Element ?b has a MemoryBarrier)
			?obj <- (object (is-a Diplomat) (name ?b) (HasMemoryBarrier TRUE)
								 (parent ?p))
			(not (exists (object (is-a Diplomat) (name ?p))))
			=>
			(retract ?fct))
;------------------------------------------------------------------------------
