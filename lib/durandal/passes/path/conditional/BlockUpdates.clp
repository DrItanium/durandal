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
; BlockUpdates.clp - Contains rules that add paths that a given block is a part
; of
; Written by Joshua Scoggins (6/7/2012)
;------------------------------------------------------------------------------
(defrule paths-conditional-update::initial-rule
         (declare (salience 10000))
         ?f <- (message (to paths-conditional-update)
                        (action initial-fact))
         =>
         (modify ?f (action update)))
;------------------------------------------------------------------------------
(defrule paths-conditional-update::last-rule
         (declare (salience -10000))
         ?f <- (message (to paths-conditional-update)
                        (action update))
         =>
         (retract ?f))
;------------------------------------------------------------------------------
(defrule paths-conditional-update::FAIL-UNFINISHED-PATHS 
         (message (to paths-conditional-update)
                  (action update))
         (test (> (length$ 
                    (find-all-instances ((?path Path)) 
                                        (not ?path:closed))) 0))
         =>
         (printout t "ERROR: Not all paths were closed!" crlf)
         (progn$ (?inst (find-all-instances ((?path Path)) 
                                            (not ?path:closed)))
                 (send ?inst print))
         (printout t "Current facts!" crlf)
         (facts)
         (printout t "Halting Execution!!!" crlf)
         (halt))
;------------------------------------------------------------------------------
(defrule paths-conditional::AddPathToDiplomat
         "Adds the given path name to the target diplomat"
         (declare (salience 1))
         (message (to paths-conditional)
                  (action build-path))
         (object (is-a Path) 
                 (closed TRUE) 
                 (id ?i) 
                 (values $? ?b $?))
         ?d <- (object (is-a Diplomat) 
                       (id ?b))
         (test (not (member$ ?i (send ?d get-Paths))))
         =>
         (slot-insert$ ?d Paths 1 ?i))
;------------------------------------------------------------------------------
(defrule paths-conditional-update::TraversePathForElementInjection
         (message (to paths-conditional-update)
                  (action update))
         (object (is-a Path) 
                 (closed TRUE) 
                 (id ?p) 
                 (values $? ?a ?b $?))
         ?o0 <- (object (is-a Diplomat) 
                        (id ?a))
         ?o1 <- (object (is-a Diplomat) 
                        (id ?b))
         =>
         (bind ?ppe (send ?o1 get-PreviousPathElements))
         (bind ?npe (send ?o0 get-NextPathElements))
         (if (not (member$ ?a ?ppe)) then
           (modify-instance ?o1 (PreviousPathElements ?ppe ?a)))
         (if (not (member$ ?b ?npe)) then
           (modify-instance ?o0 (NextPathElements ?npe ?b))))
;------------------------------------------------------------------------------
