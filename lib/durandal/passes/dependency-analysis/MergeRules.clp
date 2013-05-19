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
; MergeRules.clp - Contains all of the merge rules used in the analysis stages
; Written by Joshua Scoggins (11/18/2012)
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-merge::generate-initial-consumer-list
         ?f0 <- (message (to dependency-analysis)
                         (action instruction-consumes)
                         (arguments ?a => ?id))
         (not (exists (message (to dependency-analysis)
                               (action instruction-consume-list)
                               (arguments ?id => $?))))
         =>
         (modify ?f0 (action instruction-consume-list)
                 (arguments ?id => ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-merge::generate-initial-producer-list
         ?f0 <- (message (to dependency-analysis)
                         (action instruction-produces)
                         (arguments ?a => ?id))
         (not (exists (message (to dependency-analysis)
                               (action instruction-produce-list)
                               (arguments ?id => $?))))
         =>
         (modify ?f0 (action instruction-produce-list)
                 (arguments ?id => ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-merge::add-to-consumer-list
         (declare (salience 1))
         ?f0 <- (message (to dependency-analysis)
                         (action instruction-consumes)
                         (arguments ?a => ?id))
         ?f1 <- (message (to dependency-analysis)
                         (action instruction-consume-list)
                         (arguments ?id => $?elements))
         =>
         (retract ?f0)
         (modify ?f1 (arguments ?id => $?elements ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-merge::add-to-producer-list
         (declare (salience 1))
         ?f0 <- (message (to dependency-analysis)
                         (action instruction-produces)
                         (arguments ?a => ?id))
         ?f1 <- (message (to dependency-analysis)
                         (action instruction-produce-list)
                         (arguments ?id => $?elements))
         =>
         (retract ?f0)
         (modify ?f1 (arguments ?id => $?elements ?a)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-inject::InjectConsumers-Producers-And-LocalDependencies
         "Performs the actions of InjectConsumers and
         InjectProducersAndLocalDependencies in a single rule fire."
         (declare (salience 1))
         ?f0 <- (message (to dependency-analysis)
                         (action instruction-produce-list)
                         (arguments ?id => $?t0))
         ?f1 <- (message (to dependency-analysis)
                         (action instruction-consume-list)
                         (arguments ?id => $?t1))
         ?inst <- (object (is-a Instruction) 
                          (name ?id) 
                          (Consumers $?c) 
                          (Producers $?p) 
                          (LocalDependencies $?ld))
         =>
         (retract ?f0 ?f1)
         (bind ?cs $?c)
         (bind ?ps $?p)
         (bind ?lds $?ld)
         (if (= 0 (length$ ?cs)) then
           (bind ?cs ?t1)
           else
           (progn$ (?target ?t1)
                   (if (not (member$ ?target ?cs)) then
                     (bind ?cs (insert$ ?cs 1 ?target)))))
         (progn$ (?target ?t0)
                 (if (not (member$ ?target ?lds)) then
                   (bind ?lds (insert$ ?lds 1 ?target)))
                 (if (not (member$ ?target ?ps)) then
                   (bind ?ps (insert$ ?ps 1 ?target))))
         (modify-instance ?inst (Consumers ?cs) (Producers ?ps) 
                          (LocalDependencies ?lds)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-inject::InjectConsumers
         "Adds a given consumer to the target instruction"
         ?fct <- (message (to dependency-analysis)
                          (action instruction-consume-list)
                          (arguments ?id => $?targets))
         ?inst <- (object (is-a Instruction) 
                          (name ?id) 
                          (Consumers $?cs))
         =>
         (retract ?fct)
         (bind ?cons $?cs)
         (if (= 0 (length$ ?cons)) then
           (bind ?cons ?targets)
           else
           (progn$ (?target ?targets)
                   (if (not (member$ ?target ?cons)) then
                     (bind ?cons (insert$ ?cons 1 ?target)))))
         (modify-instance ?inst (Consumers ?cons)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-inject::InjectProducersAndLocalDependencies
         "Adds a given producer to the target instruction."
         ?fct <- (message (to dependency-analysis)
                          (action instruction-produce-list)
                          (arguments ?id => $?targets))
         ?inst <- (object (is-a Instruction) 
                          (name ?id) 
                          (Producers $?ps)
                          (LocalDependencies $?ld))
         =>
         (retract ?fct)
         (bind ?prods $?ps)
         (bind ?lds $?ld)
         (progn$ (?target ?targets)
                 (if (not (member$ ?target ?lds)) then
                   (bind ?lds (create$ ?lds ?target)))
                 (if (not (member$ ?target ?prods)) then
                   (bind ?prods (create$ ?prods ?target))))
         (modify-instance ?inst (Producers ?prods) (LocalDependencies ?lds)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-make-set::SetifyInstructionProducers
         ?inst <- (object (is-a Instruction) 
                          (Producers $?a ?b $?c ?b $?d))
         =>
         ;if we find one duplicate instance then just convert the whole thing
         ;into a list
         (bind ?mid (create$ ?b))
         (bind ?all (create$ ?a ?c ?d))
         (progn$ (?t ?all)
                 (if (not (member$ ?t ?mid)) then
                   (bind ?mid (insert$ ?mid (+ ?t-index 1) ?t))))
         (modify-instance ?inst (Producers ?mid)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-make-set::SetifyInstructionConsumers
         ?inst <- (object (is-a Instruction) 
                          (Consumers $?a ?b $?c ?b $?d))
         =>
         (bind ?mid (create$ ?b))
         (bind ?all (create$ ?a ?c ?d))
         (progn$ (?t ?all)
                 (if (not (member$ ?t ?mid)) then
                   (bind ?mid (insert$ ?mid (+ ?t-index 1) ?t))))
         (modify-instance ?inst (Consumers ?mid)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-make-set::SetifyLocalDependencies
         ?inst <- (object (is-a Instruction) 
                          (LocalDependencies $?a ?b $?c ?b $?d))
         =>
         (bind ?mid (create$ ?b))
         (bind ?all (create$ ?a ?c ?d))
         (progn$ (?t ?all)
                 (if (not (member$ ?t ?mid)) then
                   (bind ?mid (insert$ ?mid (+ ?t-index 1) ?t))))
         (modify-instance ?inst (LocalDependencies ?mid)))
;------------------------------------------------------------------------------
(defrule dependency-analysis-extended-memory-analysis-make-set::SetifyNonLocalDependencies
         ?inst <- (object (is-a Instruction) 
                          (NonLocalDependencies $?a ?b $?c ?b $?d))
         =>
         (bind ?mid (create$ ?b))
         (bind ?all (create$ ?a ?c ?d))
         (progn$ (?t ?all)
                 (if (not (member$ ?t ?mid)) then
                   (bind ?mid (insert$ ?mid (+ ?t-index 1) ?t))))
         (modify-instance ?inst (NonLocalDependencies ?mid)))
;------------------------------------------------------------------------------
