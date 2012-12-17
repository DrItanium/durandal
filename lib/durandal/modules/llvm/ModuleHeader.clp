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
; ModuleHeader.clp - Contains the entry point into the llvm module. 
;------------------------------------------------------------------------------
; In CLIPS, modules are defined BEFORE their corresponding contents are defined
; so this file doubles as a loader as well. Absolute paths must be used because 
; CLIPS does not understand the concept of partial paths. 
;
; If you want to change the layout of this module then you must update these
; paths. Failure to do so will cause the optimization to not work correctly.
;------------------------------------------------------------------------------

; Path is modules/llvm/
(defmodule llvm 
 (import core defclass ?ALL)
 (import core deffunction ?ALL)  
 (export defclass ?ALL)
 (export deffunction ?ALL))

; DO NOT MESS WITH THE ORDER OF THESE COMMANDS
; BAD THINGS CAN HAPPEN
; Class loaders
(load* "modules/llvm/LLVMObject.clp")
(load* "modules/llvm/LLVMType.clp")
(load* "modules/llvm/LLVMValue.clp")
(load* "modules/llvm/LLVMUser.clp")
(load* "modules/llvm/LLVMConstant.clp")
(load* "modules/llvm/LLVMOperator.clp")
(load* "modules/llvm/LLVMMetadata.clp")
(load* "modules/llvm/LLVMArgument.clp")
(load* "modules/llvm/Instruction.clp")
(load* "modules/llvm/LLVMInstruction.clp")
(load* "modules/llvm/Diplomat.clp")
(load* "modules/llvm/BasicBlock.clp")
(load* "modules/llvm/MultiBlockContainer.clp")
(load* "modules/llvm/Region.clp")
(load* "modules/llvm/Loop.clp")
(load* "modules/llvm/LLVMModule.clp")

; Function loaders
