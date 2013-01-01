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
; NOTE: If you want to define your own place to load passes from then remember
; that it is up to you to load it initially.
;------------------------------------------------------------------------------

;This is part of the MAIN module
(definstances PersistentPassRegistry
 ([paths] of Pass
  (entry-point "passes/path/PassHeader.clp")
  (pass-name paths)
  (pass-description "Generate the set of paths through a given function") 
  (pass-type Function)
  (need-loops TRUE)
  (need-regions TRUE)
  (passes paths)
  (required loops regions))
  
  
 ;([pass-valid-paths] of Pass
 ; (entry "passes/path/valid-only/PassHeader.clp")
 ; (pass-name "valid-paths")
 ; (pass-description "Generate the set of paths through user-defined valid regions in a function")
 ; (target Function))
 ;([pass-paths] of Pass
 ; (entry "passes/path/all-paths/PassHeader.clp")
 ; (pass-name "paths")
 ; (pass-description "Generate the set of all paths through all regions in a function")
 ; (target Function))

)
