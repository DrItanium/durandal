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
; Init.clp - Contains the defmodules that make up the process of
; loop-region-merging.
;------------------------------------------------------------------------------
(defmodule loop-region-building-flatlist-build
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-flatlist-expand
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-flatlist-claim
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-flatlist-arbitrate
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-flatlist-resolve
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-determinant-construction
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-determinant-population
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-determinant-resolution
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-determinant-indirect-resolution
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-fixup
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-fixup-update
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-fixup-rename
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defmodule loop-region-building-cleanup-merger
			  (import core ?ALL)
			  (import llvm ?ALL)
			  (import types ?ALL)
			  (import rampancy ?ALL)
			  (import indirect ?ALL)
			  (import pipeline ?ALL)
			  (import MAIN ?ALL))
;------------------------------------------------------------------------------
(defrule loop-region-merging::modify-pass-description
			"Modifies the target pass description to load the target subpasses"
			?fct <- (message (to loop-region-merging) (action initial-fact))
			?obj <- (object (is-a pass-description) (passes $?passes))
			=>
			(retract ?fct)
			(modify-instance ?obj 
								  (passes 
									 loop-region-building-flatlist-build
									 loop-region-building-flatlist-expand
									 loop-region-building-flatlist-claim
									 loop-region-building-flatlist-arbitrate
									 loop-region-building-flatlist-resolve
									 loop-region-building-determinant-construction
									 loop-region-building-determinant-population
									 loop-region-building-determinant-resolution
									 loop-region-building-determinant-indirect-resolution
									 loop-region-building-fixup
									 loop-region-building-fixup-update
									 loop-region-building-fixup-rename
									 loop-region-building-cleanup-merger
									 $?passes)))
;------------------------------------------------------------------------------
