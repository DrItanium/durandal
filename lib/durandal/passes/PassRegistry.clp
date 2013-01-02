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
; PassRegistry.clp - Contains the listing of the standard set of passes. 
;------------------------------------------------------------------------------
; If you want to define a group of passes to be executed without reset being
; called in between then define a pass but do not provide an entry point and a
; set of passes to execute in the passes multislot
;------------------------------------------------------------------------------

(definstances PersistentPassRegistry
 ([test] of Pass
  (entry-point "passes/test/PassHeader.clp")
  (pass-name test)
  (pass-description "A test of the indirect pass system")
  (pass-type Module)
  (need-loops TRUE)
  (need-regions TRUE)
  (passes test)
  (required loops regions)
  (preserves-cfg TRUE)
  (preserves-all TRUE))
 ([paths] of Pass
  (entry-point "passes/path/all/PassHeader.clp")
  (pass-name paths)
  (pass-description "Generate the set of paths through a given function") 
  (pass-type Function)
  (need-loops TRUE)
  (need-regions TRUE)
  (passes paths)
  (required loops regions))
 ([paths-conditional] of Pass
  (entry-point "passes/path/conditional/PassHeader.clp")
  (pass-name paths-conditional)
  (pass-description "Generate the set of paths through a set of target regions")
  (pass-type Function)
  (need-loops TRUE)
  (need-regions TRUE)
  (passes paths-conditional)
  (required loops regions))
 ;Add more passes here
)
;------------------------------------------------------------------------------
