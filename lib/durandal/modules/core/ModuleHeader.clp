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
; ModuleHeader.clp - Contains the entry point into the core module. 
;------------------------------------------------------------------------------
; In CLIPS, modules are defined BEFORE their corresponding contents are defined
; so this file doubles as a loader as well. Absolute paths must be used because 
; CLIPS does not understand the concept of partial paths. 
;
; If you want to change the layout of this module then you must update these
; paths. Failure to do so will cause the optimization to not work correctly.
;------------------------------------------------------------------------------

(defmodule core 
			  ; Modify this export to add more classes
			  ; Just remember that these three classes are pretty critical to the rest of
			  ; the optimization
			  (export defclass ?ALL) 
			  ; Modify this export to add more functions to be exposed
			  ; I would advise against removing these functions
			  (export deffunction ?ALL))

(load* "modules/core/Object.clp")
(load* "modules/core/ParentedObject.clp")
(load* "modules/core/InteropObject.clp")
(load* "modules/core/Hint.clp")
(load* "modules/core/List.clp")
(load* "modules/core/ParentedHint.clp")
(load* "modules/core/ParentedList.clp")
;(load* "modules/core/Environment.clp")
; Add more defclasses here
; The programmer is responsible for ordering the load statements so that
; class dependencies are met
(load* "modules/core/SetExtensions.clp")
; Add more deffunctions here
; The programmer is responsible for ordering the load statements so that
; function dependencies are met



