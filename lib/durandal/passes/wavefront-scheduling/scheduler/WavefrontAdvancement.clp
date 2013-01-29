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
(defrule wavefront-scheduling-advance-init::MoveContentsToDeleteNodes
			"Moves blocks out of the contents into the closed list"
			(declare (salience 2701))
			?wave <- (object (is-a Wavefront) 
								  (values $?c) 
								  (Closed $?cl))
			(test (or (> (length$ ?c) 0) 
						 (> (length$ ?cl) 0)))
			=>
			(slot-insert$ ?wave DeleteNodes 1 ?c ?cl))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-advance-identify::MarkShouldStayOnWavefront
			(declare (salience 343))
			?wave <- (object (is-a Wavefront) 
								  (DeleteNodes $?a ?b $?c)
								  (Closed $?clos)
								  (values $?contents))

			?bb <- (object (is-a Diplomat) 
								(id ?b) 
								(NextPathElements ?s))
			(object (is-a Diplomat) 
					  (id ?s) 
					  (PreviousPathElements $?ppe))
			(test (not (subsetp ?ppe (create$ ?a ?b ?c))))
			?agObj <- (object (is-a PathAggregate) 
									(parent ?b))
			=>
			(object-pattern-match-delay
			  (if (not (member$ ?b $?clos)) then
				 (bind ?ind (member$ ?b $?contents))
				 (slot-delete$ ?wave values ?ind ?ind)
				 (modify-instance ?wave (Closed $?clos ?b)
										(DeleteNodes $?a $?c))
				 else
				 (modify-instance ?wave (DeleteNodes $?a $?c)))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-advance::DeleteElementFromWavefront
			(declare (salience 180))
			?wave <- (object (is-a Wavefront) 
								  (id ?id) 
								  (DeleteNodes ?a $?rest))
			(object (is-a Diplomat) 
					  (id ?a) 
					  (NextPathElements $?npe))
			=>
			;TODO: Continue to port here
			(object-pattern-match-delay
			  (bind ?ind (member$ ?a (send ?wave get-values)))
			  (bind ?ind2 (member$ ?a (send ?wave get-Closed)))
			  (modify-instance ?wave (DeleteNodes $?rest))
			  (if ?ind then (slot-delete$ ?wave values ?ind ?ind))
			  (if ?ind2 then (slot-delete$ ?wave Closed ?ind2 ?ind2))
			  (assert (message (to wavefront-scheduling)
									 (action add-blocks-into)
									 (arguments ?id => $?npe)))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-advance-end::PutSuccessorsOntoWavefront-Match
			?fct <- (message (to wavefront-scheduling)
								  (action add-blocks-into)
								  (arguments ?id => ?next $?rest))
			?wave <- (object (is-a Wavefront) 
								  (id ?id) 
								  (values $?contents))
			=>
			(modify ?fct (arguments ?id => $?rest))
			(if (not (member$ ?next $?contents)) then
			  (modify-instance ?wave (values $?contents ?next))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-advance-end::PutSuccessorsOntoWavefront-NoMoreElements
			?fct <- (message (to wavefront-scheduling)
								  (action add-blocks-into)
								  (arguments ? =>))
			=>
			(retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-update::PonderRestartOfWavefrontScheduling
			?fct <- (message (to wavefront-scheduling-update)
								  (from pipeline)
								  (action initial-fact))
			?obj <- (object (is-a pass-description) 
								 (passes $?passes))
			=>
			(retract ?fct)
			(bind ?instances (find-all-instances ((?wave Wavefront)) 
															 (> (length$ ?wave:values) 0)))
			(if (> (length$ ?instances) 0) then
			  (modify-instance ?obj 
									 (passes wavefront-scheduling-init
												wavefront-scheduling-identify
												wavefront-scheduling-phi-identify
												wavefront-scheduling-phi-node 
												wavefront-scheduling-phi-node-update
												wavefront-scheduling-pathing
												wavefront-scheduling-strip 
												wavefront-scheduling-inject 
												wavefront-scheduling-acquire 
												wavefront-scheduling-slice 
												wavefront-scheduling-analyze-init 
												wavefront-scheduling-pre-generate-analyze
												wavefront-scheduling-generate-analyze
												wavefront-scheduling-analyze 
												wavefront-scheduling-slice-analyze
												wavefront-scheduling-merge-init 
												wavefront-scheduling-merge 
												wavefront-scheduling-merge-update 
												wavefront-scheduling-reopen-blocks
												wavefront-scheduling-ponder
												wavefront-scheduling-post-ponder
												wavefront-scheduling-rename 
												wavefront-scheduling-dependency-analysis 
												wavefront-scheduling-advance-init
												wavefront-scheduling-advance-identify
												wavefront-scheduling-advance
												wavefront-scheduling-advance-end
												wavefront-scheduling-update
												$?passes))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-final::RetractUnlinkedInstructions
			?bb <- (object (is-a BasicBlock) 
								(id ?b) 
								(UnlinkedInstructions ?i $?rest))
			?instruction <- (object (is-a Instruction) 
											(id ?i) 
											(parent ?b) 
											(pointer ?ptr))
			(object (is-a PathAggregate) 
					  (parent ?b) 
					  (InstructionPropagation $? ?i ?new ?b ! $?))
			(object (is-a Instruction) 
					  (id ?new) 
					  (pointer ?nPtr))
			=>
			;this is a little gross but it is a very easy way to ensure that
			;things work correctly so that the LLVM ModuleVerifier doesn't come
			;down like a hammer on what we're doing
			(llvm-replace-all-uses ?ptr ?nPtr)
			(modify-instance ?bb (UnlinkedInstructions $?rest))
			(llvm-unlink-and-delete-instruction ?ptr)
			(unmake-instance ?instruction))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-final::DisplayFactsAtEnd 
			(declare (salience -10000))
			(message (action DEBUG))
			?fct <- (message (to wavefront-scheduling-final)
								  (action initial-fact))
			=>
			(retract ?fct)
			(facts))
;------------------------------------------------------------------------------

