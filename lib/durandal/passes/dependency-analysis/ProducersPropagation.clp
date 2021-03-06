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
(defrule dependency-analysis-modification-propagation::PropagateBlockProducers
			(object (is-a BasicBlock) (parent ?r) 
					  (Produces $?produces))
			=>
			(assert (Give ?r the following produced items $?produces)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-modification-propagation::PropagateRegionProducers-ParentExists
			?fct <- (Give ?r the following produced items $?produced)
			?region <- (object (is-a Region) (id ?r) (parent ?p))
			(exists (object (is-a Region) (id ?p)))
			=>
			(retract ?fct)
			(assert (Give ?p the following produced items $?produced))
			(slot-insert$ ?region Produces 1 ?produced))
;------------------------------------------------------------------------------
(defrule dependency-analysis-modification-propagation::PropagateRegionProducers-ParentDoesntExist
			?fct <- (Give ?r the following produced items $?produced)
			?region <- (object (is-a Region) (id ?r) (parent ?p))
			(not (exists (object (is-a Region) (id ?p))))
			=>
			(retract ?fct)
			(slot-insert$ ?region Produces 1 ?produced))
;------------------------------------------------------------------------------
(defrule dependency-analysis-modification-propagation::IdentifyNonLocalDependencies
			?i0 <- (object (is-a Instruction) (parent ?p) (Operands $? ?op $?))
			(object (is-a ParentedObject) (id ?op) (parent ~?p))
			;(test (not (member$ ?op (send ?i0 get-NonLocalDependencies))))
			=>
			;since we don't copy the set of producers at the start anymore we
			;need this operation as well
			(slot-insert$ ?i0 Producers 1 ?op)
			(slot-insert$ ?i0 NonLocalDependencies 1 ?op))
;------------------------------------------------------------------------------
