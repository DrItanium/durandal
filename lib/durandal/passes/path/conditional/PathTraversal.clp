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
; PathTraversal.clp - Contains rules that handle the act of traversing the
; region during path construction. Modified to take advantage of modules
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-basicblock-to-basicblock
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) (id ?id) 
                          (parent ?p) 
                          (closed FALSE)
                          (values $?before ?curr))
         (object (is-a BasicBlock) 
                 (id ?curr) 
                 (parent ?p) 
                 (Successors $? ?next $?))
         (object (is-a BasicBlock) 
                 (id ?next) 
                 (parent ?p))
         (test (and (neq ?next ?curr) 
                    (not (member$ ?next $?before))))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action add-to-path)
                 (arguments ?next => ?id)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-region-to-basicblock
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (closed FALSE) 
                          (parent ?p) 
                          (values $?before ?curr))
         (object (is-a Region) 
                 (id ?curr) 
                 (parent ?p) 
                 (Exits $? ?next $?))
         (object (is-a BasicBlock) 
                 (id ?next) 
                 (parent ?p))
         (test (and (neq ?next ?curr) 
                    (not (member$ ?next $?before))))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action add-to-path)
                 (arguments ?next => ?id)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-basicblock-to-region
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id)
                          (closed FALSE) 
                          (parent ?p) 
                          (values $?before ?curr))
         (object (is-a BasicBlock) 
                 (id ?curr) 
                 (parent ?p) 
                 (Successors $? ?s $?))
         (object (is-a Region) 
                 (Entrances $? ?s $?) 
                 (id ?next) 
                 (parent ?p))
         (test (and (neq ?next ?curr) 
                    (not (member$ ?next $?before))))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action add-to-path)
                 (arguments ?next => ?id)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-region-to-region
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (closed FALSE) 
                          (parent ?p) 
                          (values $?before ?curr))
         (object (is-a Region) 
                 (id ?curr) 
                 (parent ?p) 
                 (Exits $? ?e $?))
         (object (is-a Region) 
                 (id ?next) 
                 (parent ?p) 
                 (Entrances $? ?e $?)) 
         (test (and (neq ?next ?curr) 
                    (not (member$ ?next $?before))))
         ; even if the entrance is part of a nested region...we really don't 
         ; care it will still be accurate thanks to the way llvm does things
         =>
         (send ?path increment-reference-count)
         (modify ?f (action add-to-path)
                 (arguments ?next => ?id)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-region-no-exit
         "We are at a region that doesn't have an exit...Not sure if LLVM 
         allows this but let's handle it."
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (parent ?p) 
                          (closed FALSE) 
                          (values $? ?a))
         (object (is-a Region) 
                 (id ?a) 
                 (parent ?p) 
                 (Exits))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => nil)))
;------------------------------------------------------------------------------
(defrule paths-conditional::travseral-basicblock-no-exit
         "We are at a basic block that has no successors...usually the end of a
         function"
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (parent ?p) 
                          (closed FALSE) 
                          (values $? ?a))
         (object (is-a BasicBlock) 
                 (id ?a) 
                 (parent ?p) 
                 (Successors))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => nil)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-basicblock-to-basicblock-cycle
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) (id ?id) 
                          (parent ?p) 
                          (values $?before ?curr) 
                          (closed FALSE))
         (object (is-a BasicBlock) 
                 (id ?curr) 
                 (parent ?p) 
                 (Successors $? ?next $?))
         (object (is-a BasicBlock) 
                 (id ?next) 
                 (parent ?p))
         (test (or (eq ?next ?curr) 
                   (member$ ?next $?before)))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => ?next)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-region-to-basicblock-cycle
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) (id ?id) 
                          (closed FALSE) 
                          (parent ?p) 
                          (values $?before ?curr))
         (object (is-a Region) 
                 (id ?curr) 
                 (parent ?p) 
                 (Exits $? ?next $?))
         (object (is-a BasicBlock) 
                 (id ?next) 
                 (parent ?p))
         (test (or (eq ?next ?curr) 
                   (member$ ?next $?before)))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => ?next)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-basicblock-to-region-cycle
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (closed FALSE) 
                          (parent ?p) 
                          (values $?before ?curr))
         (object (is-a BasicBlock) 
                 (id ?curr) 
                 (parent ?p) 
                 (Successors $? ?s $?))
         (object (is-a Region) 
                 (id ?next) 
                 (parent ?p) 
                 (Entrances $? ?s $?))
         (test (or (eq ?next ?curr) 
                   (member$ ?next $?before)))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => ?next)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-region-to-region-cycle
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (closed FALSE) 
                          (parent ?p) 
                          (values $?before ?curr))
         (object (is-a Region) 
                 (id ?curr) 
                 (parent ?p) 
                 (Exits $? ?e $?))
         (object (is-a Region) 
                 (id ?next) 
                 (parent ?p) 
                 (Entrances $? ?e $?)) 
         (test (or (eq ?next ?curr) 
                   (member$ ?next $?before)))
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?next => ?id)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-basicblock-to-exit
         "Marks the current path as finished because we've reached an exit to 
         the current region"
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (parent ?p) 
                          (closed FALSE)
                          (values $? ?curr))
         (object (is-a BasicBlock) 
                 (id ?curr) 
                 (parent ?p) 
                 (Successors $? ?e $?))
         (object (is-a Region) 
                 (id ?p) 
                 (Exits $? ?e $?))
         ;since the current block has an exit for this region we mark it
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => ?e)))
;------------------------------------------------------------------------------
(defrule paths-conditional::traversal-region-to-exit
         (declare (salience 2))
         ?f <- (message (to paths-conditional)
                        (action build-path)
                        (arguments ?id))
         ?path <- (object (is-a Path) 
                          (id ?id) 
                          (closed FALSE) 
                          (parent ?p) 
                          (values $? ?c))
         (object (is-a Region) 
                 (id ?c) 
                 (parent ?p) 
                 (Exits $? ?e $?))
         (object (is-a Region) 
                 (id ?p) 
                 (Exits $? ?e $?))
         ; both the inner and outer regions have the same exit...thus the
         ; curent nested region is a terminator for one path
         =>
         (send ?path increment-reference-count)
         (modify ?f (action close-path)
                 (arguments ?id => ?e)))
;------------------------------------------------------------------------------
