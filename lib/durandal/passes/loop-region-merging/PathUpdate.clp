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
(defrule loop-region-merging-determinant-construction::construct-determinant-for-region
			(object (is-a Region) 
					  (id ?r))
			(not (exists (object (is-a OwnershipDeterminant) 
										(parent ?r))))
			=>
			(make-instance of OwnershipDeterminant 
								(parent ?r)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-determinant-construction::construct-determinant-for-basicblock
			(object (is-a BasicBlock) 
					  (id ?b))
			(not (exists (object (is-a OwnershipDeterminant) 
										(parent ?b))))
			=>
			(make-instance of OwnershipDeterminant 
								(parent ?b)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-determinant-population::populate-determinant
			?fct <- (message (to loop-region-merging) 
								  (action claim-owns) 
								  (arguments ?a => ?b))
			?obj <- (object (is-a OwnershipDeterminant) 
								 (parent ?b))
			?obj2 <- (object (is-a OwnershipDeterminant) 
								  (parent ?a))
			=>
			(retract ?fct)
			(object-pattern-match-delay 
			  (slot-insert$ ?obj2 PotentialChildren 1 ?b)
			  (slot-insert$ ?obj Claims 1 ?a)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-determinant-resolution::determine-indirect-claim
			?t0 <- (object (is-a OwnershipDeterminant) 
								(parent ?b) 
								(Claims $?v ?a $?x) 
								(IndirectClaims $?ic))
			(object (is-a OwnershipDeterminant) 
					  (parent ~?b) 
					  (PotentialChildren $? ?b $?) 
					  (Claims $? ?a $?))
			?t1 <- (object (is-a OwnershipDeterminant) 
								(parent ?a) 
								(PotentialChildren $?t ?b $?r))
			=>
			;let's see if this is faster
			(modify-instance ?t0 (IndirectClaims ?ic ?a) 
								  (Claims ?v ?x))
			(modify-instance ?t1 (PotentialChildren ?t ?r)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-determinant-indirect-resolution::determine-indirect-indirect-claim
			?t0 <- (object (is-a OwnershipDeterminant) 
								(parent ?b) 
								(Claims $?l ?a $?x) 
								(IndirectClaims $?ic))
			(object (is-a OwnershipDeterminant) 
					  (parent ~?b&~?a) 
					  (IndirectClaims $? ?a $?) 
					  (PotentialChildren $? ?b $?))
			?t1 <- (object (is-a OwnershipDeterminant) 
								(parent ?a)
								(PotentialChildren $?z ?b $?q))
			=>
			(modify-instance ?t0 (IndirectClaims ?ic ?a) 
								  (Claims ?l ?x))
			(modify-instance ?t1 (PotentialChildren ?z ?q)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup::delete-non-existent-references
			?region <- (object (is-a Region) 
									 (contents $? ?b $?))
			(not (exists (object (id ?b))))
			=>
			(object-pattern-match-delay 
			  (bind ?ind0 (member$ ?b (send ?region get-contents)))
			  (slot-delete$ ?region contents ?ind0 ?ind0)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup-update::update-owner-of-target-region
			(object (is-a OwnershipDeterminant) 
					  (parent ?p) 
					  (Claims ?a))
			?obj <- (object (is-a Region) 
								 (id ?p))
			=>
			(modify-instance ?obj (parent ?a)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup-update::update-owner-of-target-basicblock
			(object (is-a OwnershipDeterminant) 
					  (parent ?p) 
					  (Claims ?a))
			?obj <- (object (is-a BasicBlock) 
								 (id ?p))
			=>
			(modify-instance ?obj (parent ?a)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup-update::add-new-child-to-target-region
			(object (is-a OwnershipDeterminant) 
					  (parent ?p)
					  (PotentialChildren $? ?a $?))
			?region <- (object (is-a Region) 
									 (id ?p) 
									 (contents $?c))
			(test (not (member$ ?a ?c)))
			=>
			(slot-insert$ ?region contents 1 ?a))
;------------------------------------------------------------------------------
(defrule loop-region-merging-cleanup-merger::cleanup-ownership-determinants
			"Deletes all of the OwnershipDeterminant objects in a single rule 
			fire"
			?f0 <- (delete ownership-determinants)
			=>
			(retract ?f0)
			(progn$ (?obj (find-all-instances ((?list OwnershipDeterminant)) 
														 TRUE))
					  (unmake-instance ?obj)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup-rename::remove-unowned-elements
			"Now that we have figured out and updated ownership claims it is 
			necessary to remove leftover entries in other regions"
			?r <- (object (is-a Region) 
							  (id ?t) 
							  (contents $?a ?b $?c))
			(object (is-a ParentedObject) 
					  (id ?b) 
					  (parent ~?t))
			=>
			(modify-instance ?r (contents $?a $?c)))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup::FAILURE-too-many-claims-of-ownership
			(object (is-a OwnershipDeterminant) 
					  (parent ?a) 
					  (Claims $?z&:(> (length$ ?z) 1))
					  (id ?name))
			=>
			(printout t "ERROR: " ?name " has more than one claim of ownership on"
						 " it!" crlf "The claims are " ?z crlf)
			(exit))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup::FAILURE-no-remaining-claims-for-region
			(object (is-a OwnershipDeterminant) 
					  (parent ?a) 
					  (Claims)
					  (PotentialChildren $?pc) 
					  (IndirectClaims $?ic))
			(object (is-a Region) 
					  (id ?a) 
					  (IsTopLevelRegion FALSE))
			=>
			(printout t "ERROR: " ?a " has no remaining claims!" crlf 
						 ?a " has " $?pc " as it's potential children." crlf
						 ?a " has " $?ic " as it's indirect claims." crlf)
			(exit))
;------------------------------------------------------------------------------
(defrule loop-region-merging-fixup::FAILURE-no-remaining-claims-for-basicblock
			(object (is-a OwnershipDeterminant) 
					  (parent ?a) 
					  (Claims)
					  (PotentialChildren $?pc) 
					  (IndirectClaims $?ic))
			(object (is-a BasicBlock) 
					  (id ?a)) 
			=>
			(printout t "ERROR: BasicBlock " ?a " has no remaining claims!" crlf 
						 ?a " has " $?pc " as it's potential children." crlf
						 ?a " has " $?ic " as it's indirect claims." crlf)
			(exit))
;------------------------------------------------------------------------------
