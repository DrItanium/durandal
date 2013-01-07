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
(defmodule wavefront-scheduling-init
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-identify
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-pathing
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-phi-identify
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-phi-node
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-phi-node-update
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-strip
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-acquire
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-inject
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-acquire
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-slice
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-analyze-init
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-pre-generate-analyze
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-generate-analyze
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-analyze
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-slice-analyze
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-merge-init
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-merge
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-merge-update
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-reopen-blocks
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-ponder
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-rename
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-dependency-analysis
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-advance-init
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-advance-identify
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-advance
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-advance-end
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule wavefront-scheduling-update
           (import core ?ALL)
           (import llvm ?ALL)
           (import types ?ALL)
           (import pipeline ?ALL)
           (import indirect ?ALL)
           (import rampancy ?ALL)
           (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defrule wavefront-scheduling-code::macro-expand
         ?msg <- (message (from pipeline) 
                          (to wavefront-scheduling-code)
                          (action initial-fact))
         ?p <- (object (is-a pass-description) (passes $?passes))
         =>
         (retract ?msg)
         (modify-instance ?p (passes 
                               wavefront-scheduling-init
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
                               wavefront-scheduling-pre-generate-Analyze
                               wavefront-scheduling-generate-analyze
                               wavefront-scheduling-analyze 
                               wavefront-scheduling-slice-analyze
                               wavefront-scheduling-merge-init 
                               wavefront-scheduling-merge 
                               wavefront-scheduling-merge-update 
                               wavefront-scheduling-reopen-blocks
                               wavefront-scheduling-ponder
                               wavefront-scheduling-rename 
                               wavefront-scheduling-dependency-analysis 
                               wavefront-scheduling-advance-init
                               wavefront-scheduling-advance-identify
                               wavefront-scheduling-advance
                               wavefront-scheduling-advance-end
                               wavefront-scheduling-update
                               ;TODO: add more modules
                               $?passes)))
;------------------------------------------------------------------------------
(load* "Stages/Wavefront/WavefrontInitialization.clp")
(load* "Stages/Wavefront/ValidBlockIdentification.clp")
(load* "Stages/Wavefront/WavefrontScheduling.clp")
(load* "Stages/Wavefront/WavefrontPropagation.clp")
(load* "Stages/Wavefront/WavefrontAdvancement.clp")
(load* "Stages/Wavefront/WavefrontDependencyUpdate.clp")
(load* "Stages/Wavefront/WavefrontOperandRename.clp")
(load* "Stages/Wavefront/WavefrontInstructionMerging.clp")
(load* "Stages/Wavefront/WavefrontBlockReopen.clp")
