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
; Now we need to rename operands as need be within the blocks that these
; instructions have been scheduled into
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::AssertReplacementActions
         "Iterates through the replacement actions multifield and asserts facts 
         related to the replacement of given values with another value"
         (declare (salience 100))
         (object (is-a PathAggregate) 
                 (parent ?e) 
                 (ReplacementActions $? ?orig ?new ! $?))
         =>
         ; I have turned you into a cheese sandwich, what do you say to that?
         (assert (message (to wavefront-scheduling)
                          (action replace-uses)
                          (arguments of ?orig 
                                     with ?new 
                                     block ?e))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceUses
         (declare (salience 20))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-uses)
                          (arguments of ?orig
                                     with ?new
                                     block ?e))
         (object (is-a Instruction) 
                 (id ?orig) 
                 (pointer ?oPtr))
         (object (is-a Instruction) 
                 (id ?new) 
                 (pointer ?nPtr))
         (object (is-a BasicBlock) 
                 (id ?e) 
                 (contents $? ?new $?rest))
         =>
         (bind ?ptrList (create$))
         (bind ?symList (create$))
         (bind ?i0 1)
         (progn$ (?var $?rest)
                 (bind ?obj (instance-address * (symbol-to-instance-name ?var)))
                 (bind ?oOps (send ?obj get-Operands))
                 (if (member$ ?orig ?oOps) then
                   (bind ?ptrTmp (send ?obj get-Pointer))
                   (bind ?ptrList (insert$ ?ptrList ?i0 ?ptrTmp))
                   (bind ?symList (insert$ ?symList ?i0 ?var))
                   (bind ?i0 (+ ?i0 1))))
         ;reuse ?fct's memory
         (modify ?fct (action replace-uses-clips)
                 (arguments ?orig => ?new for ?symList))
         (assert (message (to wavefront-scheduling)
                          (action replace-uses-llvm)
                          (arguments ?oPtr => ?nPtr for ?ptrList))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceUsesInLLVM
         (declare (salience -1))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-uses-llvm)
                          (arguments ?from => ?to for $?p2))
         =>
         (if (llvm-replace-uses ?from ?to ?p2) then 
           (retract ?fct) 
           else
           (printout t
                     "Some kind of error occured when trying to replace uses. " 
                     crlf "Make sure that you've done arguments correctly. " 
                     crlf "The failing rule is ReplaceUsesInLLVM." crlf
                     "?from = " ?from crlf
                     "?to = " ?to crlf
                     "?p2 = " ?p2 crlf
                     "Now I'm halting execution" crlf)
           (halt)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceUsesInCLIPS
         (declare (salience -1))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-uses-clips)
                          (arguments ?from => ?to for ?symbol $?rest))
         ?inst <- (object (is-a Instruction) 
                          (id ?symbol) 
                          (Operands $?operands) 
                          (LocalDependencies $?locDep)
                          (NonLocalDependencies $?nLocDep))
         =>
         (modify-instance ?inst (Operands) 
                          (LocalDependencies) 
                          (NonLocalDependencies))
         (modify ?fct (arguments ?from => ?to for $?rest))
         (assert (message (to wavefront-scheduling)
                          (action replace-operands)
                          (arguments ?from => ?to
                                     replacement ?symbol
                                     operands $?operands))
                 (message (to wavefront-scheduling)
                          (action replace-local-dependencies)
                          (arguments ?from => ?to 
                                     in ?symbol
                                     contents $?locDep))
                 (message (to wavefront-scheduling)
                          (action replace-non-local-dependencies)
                          (arguments ?from => ?to 
                                     in ?symbol
                                     contents $?nLocDep))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceUsesInCLIPS-End
         (declare (salience -1))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-uses-clips)
                          (arguments ? => ? for))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualLocalDependencyEntries-NoMatch
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-local-dependencies)
                          (arguments ?from => ?to
                                     in ?symbol
                                     contents ?curr&~?from $?rest))
         ?inst <- (object (is-a Instruction) 
                          (id ?symbol)
                          (LocalDependencies $?locDep))
         =>
         (modify-instance ?inst (LocalDependencies $?locDep ?curr))
         (modify ?fct (arguments ?from => ?to 
                                 in ?symbol 
                                 contents $?rest)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualLocalDependencyEntries-Match
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-local-dependencies)
                          (arguments ?from => ?to
                                     in ?symbol
                                     contents ?from $?rest))
         ?inst <- (object (is-a Instruction) 
                          (id ?symbol)
                          (LocalDependencies $?locDep))
         =>
         (object-pattern-match-delay
           (modify-instance ?inst (LocalDependencies $?locDep ?to))
           (modify ?fct (arguments ?from => ?to 
                                   in ?symbol 
                                   contents $?rest))))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualNonLocalDependencyEntries-NoMatch
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-non-local-dependencies)
                          (arguments ?from => ?to
                                     in ?symbol
                                     contents ?curr&~?from $?rest))
         ?inst <- (object (is-a Instruction) 
                          (id ?symbol)
                          (NonLocalDependencies $?nld))
         =>
         (modify-instance ?inst (NonLocalDependencies $?nld ?curr))
         (modify ?fct (arguments ?from => ?to
                                 in ?symbol
                                 contents $?rest)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualNonLocalDependencyEntries-Match
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-non-local-dependencies)
                          (arguments ?from => ?to
                                     in ?symbol
                                     contents ?from $?rest))
         ?inst <- (object (is-a Instruction) 
                          (id ?symbol)
                          (NonLocalDependencies $?nld))
         =>
         (modify-instance ?inst (NonLocalDependencies $?nld ?to))
         (modify ?fct (arguments ?from => ?to
                                 in ?symbol
                                 contents $?rest)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualInstructionUses-NoMatch
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-operands)
                          (arguments ?f => ?t 
                                     replacement ?s
                                     operands ?op&~?f $?ops))
         ?inst <- (object (is-a Instruction) 
                          (id ?s)
                          (Operands $?operands))
         =>
         (modify-instance ?inst (Operands $?operands ?op))
         (modify ?fct (arguments ?f => ?t 
                                 replacement ?s
                                 operands $?ops)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualInstructionUses-Match
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-operands)
                          (arguments ?f => ?t 
                                     replacement ?s
                                     operands ?f $?ops))
         ?inst <- (object (is-a Instruction) 
                          (id ?s)
                          (Operands $?operands))
         =>
         (modify-instance ?inst (Operands $?operands ?t))
         (modify ?fct (arguments ?f => ?t 
                                 replacement ?s
                                 operands $?ops)))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-rename::ReplaceIndividualInstructionUses-Empty
         (declare (salience -2))
         ?fct <- (message (to wavefront-scheduling)
                          (action replace-operands)
                          (arguments ? => ? 
                                     replacement ?
                                     operands))
         =>
         (retract ?fct))
;------------------------------------------------------------------------------
