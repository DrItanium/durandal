;------------------------------------------------------------------------------
;Copyright (c) 2012-2015, Joshua Scoggins 
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
; llvm.clp - all of the classes for representing llvm types in CLIPS
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
           (import core ?ALL)
           (export ?ALL))

; Function loaders

(defclass llvm::thing
  "base class of all llvm types"
  (is-a object
        interop)
  (multislot writes-to
             (visibility public))
  (multislot reads-from
             (visibility public)) )

(defclass llvm::diplomat
  "A Diplomat is a collection of fields that interact with other objects."
  (is-a thing)
  (multislot produces
             (visibility public))
  (multislot consumes
             (visibility public))
  (multislot next-path-elements
             (visibility public))
  (multislot previouspathelements
             (visibility public))
  (multislot paths
             (visibility public))
  (slot has-call-barrier
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-open
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-memory-barrier
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)))

(defclass llvm::struct-type
  (is-a composite-type)
  (slot name
        (type SYMBOL)
        (visibility public))
  (multislot body
             (visibility public))
  (slot is-sized
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-opaque
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-literal
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-packed
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-name
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)))

(defclass llvm::sequential-type
  (is-a composite-type))

(defclass llvm::array-type
  (is-a sequential-type))

(defclass llvm::pointer-type
  (is-a sequential-type))

(defclass llvm::vector-type
  (is-a sequential-type))

(defclass llvm::composite-type
  (is-a type)
  (multislot indicies
             (visibility public)))

(defclass llvm::function-type
  (is-a type)
  (multislot parameters
             (visibility public))
  (slot is-vararg
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)))

(defclass llvm::integer-type
  (is-a type)
  (slot bitmask 
        (type NUMBER)
        (visibility public))
  (slot signbit 
        (type NUMBER)
        (visibility public))
  (slot is-powerof2bytewidth
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)))

(defclass llvm::type
  (is-a object)
  (slot quick-type
        (type SYMBOL)
        (visibility public))
  (multislot subtypes
             (visibility public))
  (multislot -typemakeup
             (visibility public))
  (slot is-sized
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-aggregate-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-singlevalue-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-firstclass-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-derived-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-primitive-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-empty-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-vector-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-pointer-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-array-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-struct-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-function-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-intorintvector-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-integer-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-metadata-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-label-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-fporfpvector-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-x86mmx-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-floatingpoint-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-ppcfp128-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-fp128-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-x86fp80-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-double-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-float-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-half-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-void-type
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-functionvararg
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::value
  ""
  (is-a object
        )
  (multislot users
             (visibility public))
  (slot has-valuehandle
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-oneuse
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-landingpad
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-addresstaken
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-dereferenceablepointer
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::user
  ""
  (is-a value
        )
  (multislot operands
             (visibility public))

  )

(defclass llvm::undef-value
  ""
  (is-a constant
        )

  )

(defclass llvm::global-alias
  ""
  (is-a global-value
        )

  )

(defclass llvm::global-variable
  ""
  (is-a global-value
        )
  (slot is-constant
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-uniqueinitializer
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-definitiveinitializer
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-initializer
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-threadlocal
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::global-value
  ""
  (is-a constant
        )
  (slot section
        (type SYMBOL)
        (visibility public))
  (slot alignment 
        (type NUMBER)
        (visibility public))
  (slot is-dematerializable
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-materializable
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-declaration
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot maybeoverridden
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-weakforlinker
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-commonlinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-externalweaklinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-dllexportlinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-dllimportlinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-locallinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-linkerprivateweakdefautolinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-linkerprivateweaklinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-linkerprivatelinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-privatelinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-internallinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-appendinglinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-weaklinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-linkoncelinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-availableexternallylinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-externallinkage
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot useemptyexceptconstants
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-section
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-protectedvisibility
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-hiddenvisibility
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-defaultvisibility
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-unnamedaddress
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-unnamedaddr
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::constant-vector
  ""
  (is-a pointer-constant
        )
  (slot splatvalue
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::constant-struct
  ""
  (is-a pointer-constant
        )

  )

(defclass llvm::constant-pointer-null
  ""
  (is-a pointer-constant
        )

  )

(defclass llvm::pointer-constant
  ""
  (is-a constant
        )
  (slot pointer-type
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::constant-integer
  ""
  (is-a constant
        )
  (slot zeroextendedvalue 
        (type NUMBER)
        (visibility public))
  (slot signextendedvalue 
        (type NUMBER)
        (visibility public))
  (slot is-one
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-zero
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-negative
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-minusone
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::constant-floating-point
  ""
  (is-a constant
        )
  (slot is-negative
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-zero
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-nan
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::compare-constant-expression
  ""
  (is-a constant-expression
        )
  (slot predicate 
        (type NUMBER)
        (visibility public))

  )

(defclass llvm::binary-constant-expression
  ""
  (is-a constant-expression
        )

  )

(defclass llvm::constant-expression
  ""
  (is-a constant
        )
  (slot opcodename
        (type SYMBOL)
        (visibility public))
  (multislot indices
             (visibility public))
  (slot has-indices
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-compare
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-cast
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-gepwithnonotionaloverindexing
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::constant-data-vector
  ""
  (is-a constant-data-sequential
        )

  )

(defclass llvm::constant-data-array
  ""
  (is-a constant-data-sequential
        )

  )

(defclass llvm::constant-data-sequential
  ""
  (is-a constant
        )
  (slot element-type
        (type SYMBOL)
        (visibility public))
  (multislot elements
             (visibility public))
  (slot is-string
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-cstring
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::constant-array
  ""
  (is-a constant
        )

  )

(defclass llvm::constant-aggregate-zero
  ""
  (is-a constant
        )
  (slot element
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::block-address
  ""
  (is-a constant
        )
  (slot targetblock
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::constant
  ""
  (is-a user
        )
  (slot relocationinfo
        (type SYMBOL)
        (visibility public))
  (slot cantrap
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-negativezerovalue
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-allonesvalue
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-nullvalue
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-constantused
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::possibly-exact-operator
  ""
  (is-a operator
        )
  (slot is-exact
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::overflowing-binary-operator
  ""
  (is-a operator
        )
  (slot has-nounsignedwrap
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-nosignedwrap
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::fpmath-operator
  ""
  (is-a operator
        )

  )

(defclass llvm::operator
  ""
  (is-a user
        )

  )

(defclass llvm::mdstring
  ""
  (is-a value
        )

  )

(defclass llvm::mdnode
  ""
  (is-a value
        )
  (slot targetfunction
        (type SYMBOL)
        (visibility public))
  (multislot operands
             (visibility public))
  (slot is-functionlocal
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::argument
  ""
  (is-a value
        )
  (multislot attributes
             (visibility public))
  (slot has-nocaptureattribute
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-noaliasattribute
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-nestattribute
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-byvalueattribute
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-structretattribute
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::instruction
  ""
  (is-a user
        )
  (slot operation
        (type SYMBOL)
        (visibility public))
  (slot predicate
        (type SYMBOL)
        (visibility public))
  (slot memorytarget
        (type SYMBOL)
        (visibility public))
  (multislot producers
             (visibility public))
  (multislot consumers
             (visibility public))
  (multislot nonlocaldependencies
             (visibility public))
  (multislot localdependencies
             (visibility public))
  (multislot destinationregisters
             (visibility public))
  (slot timeindex 
        (type NUMBER)
        (visibility public))
  (slot is-associative
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-logicalshift
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-arithmeticshift
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-cast
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-shift
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-terminator
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-binaryop
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-calldependency
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot mayhavesideeffects
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot mayreadorwritememory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot mayreadfrommemory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot maywritetomemory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-dereferenceablepointer
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-valuehandle
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-commutative
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::extract-value-instruction
  ""
  (is-a unary-instruction
        )
  (multislot indices
             (visibility public))

  )

(defclass llvm::fence-instruction
  ""
  (is-a instruction
        )

  )

(defclass llvm::insert-value-instruction
  ""
  (is-a instruction
        )

  )

(defclass llvm::insert-element-instruction
  ""
  (is-a instruction
        )

  )

(defclass llvm::extract-element-instruction
  ""
  (is-a instruction
        )

  )

(defclass llvm::store-instruction
  ""
  (is-a instruction
        memory-modifying-object
        )
  (slot valueoperand
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::shuffle-vector-instruction
  ""
  (is-a instruction
        )
  (multislot shufflemask
             (visibility public))
  (slot mask 
        (type NUMBER)
        (visibility public))

  )

(defclass llvm::select-instruction
  ""
  (is-a instruction
        )
  (slot true-value
        (type SYMBOL)
        (visibility public))
  (slot condition
        (type SYMBOL)
        (visibility public))
  (slot false-value
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::landing-pad-instruction
  ""
  (is-a instruction
        )
  (slot personality-function
        (type SYMBOL)
        (visibility public))
  (multislot clauses
             (visibility public))
  (slot is-catch
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-cleanup
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-filter
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::atomic-rmw-instruction
  ""
  (is-a instruction
        pointer-operand-object
        )
  (slot operation
        (type SYMBOL)
        (visibility public))
  (slot value-operand
        (type SYMBOL)
        (visibility public))
  (slot is-volatile
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::atomic-compare-exchange-instruction
  ""
  (is-a instruction
        pointer-operand-object
        )
  (slot compare-operand
        (type SYMBOL)
        (visibility public))
  (slot new-val-operand
        (type SYMBOL)
        (visibility public))
  (slot is-volatile
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::get-element-pointer-instruction
  ""
  (is-a instruction
        pointer-operand-object
        )
  (slot has-all-constant-indices
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-all-zero-indices
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-indices
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-inbounds
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::unreachable-instruction
  ""
  (is-a terminator-instruction
        )

  )

(defclass llvm::switch-instruction
  ""
  (is-a terminator-instruction
        )
  (slot condition
        (type SYMBOL)
        (visibility public))
  (slot default-destination
        (type SYMBOL)
        (visibility public))
  (multislot cases
             (visibility public))

  )

(defclass llvm::switch-entry
  ""
  (is-a object
        interop
        )
  (slot index 
        (type NUMBER)
        (visibility public))

  )

(defclass llvm::return-instruction
  ""
  (is-a terminator-instruction
        )
  (slot return-value
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::resume-instruction
  ""
  (is-a terminator-instruction
        )
  (slot value
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::invoke-instruction
  ""
  (is-a terminator-instruction
        )
  (multislot arguments
             (visibility public))
  (slot has-struct-ret-attr
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-throw
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-return
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot only-reads-memory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-no-inline
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-by-val-argument
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::indirect-branch-instruction
  ""
  (is-a terminator-instruction
        )

  )

(defclass llvm::branch-instruction
  ""
  (is-a terminator-instruction
        )
  (slot is-unconditional
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-conditional
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::terminator-instruction
  ""
  (is-a instruction
        )
  (multislot successors
             (visibility public))

  )

(defclass llvm::integercompare-instruction
  ""
  (is-a compare-instruction
        )

  )

(defclass llvm::floating-point-compare-instruction
  ""
  (is-a compare-instruction
        )

  )

(defclass llvm::compare-instruction
  ""
  (is-a instruction
        )
  (slot swapped-predicate 
        (type NUMBER)
        (visibility public))
  (slot inverse-predicate 
        (type NUMBER)
        (visibility public))
  (slot predicate 
        (type NUMBER)
        (visibility public))
  (slot opcode 
        (type NUMBER)
        (visibility public))
  (slot is-true-when-equal
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-unsigned
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-signed
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-equality
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-relational
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-int-predicate
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-fp-predicate
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-false-when-equal
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::vaarg-instruction
  (is-a unary-instruction
        pointer-operand-object))

(defclass llvm::load-instruction
  (is-a unary-instruction
        memory-modifying-object))

(defclass llvm::sext-instruction
  (is-a cast-instruction) )

(defclass llvm::ptr-to-int-instruction
  (is-a cast-instruction) )

(defclass llvm::int-to-ptr-instruction
  (is-a cast-instruction))

(defclass llvm::fptrunc-instruction
  (is-a cast-instruction))

(defclass llvm::fp-to-ui-instruction
  (is-a cast-instruction))

(defclass llvm::fp-to-si-instruction
  (is-a cast-instruction) )

(defclass llvm::fp-ext-instruction
  (is-a cast-instruction))

(defclass llvm::bit-cast-instruction
  (is-a cast-instruction))

(defclass llvm::cast-instruction
  (is-a unary-instruction)
  (slot source-type
        (type SYMBOL)
        (visibility public))
  (slot destination-type
        (type SYMBOL)
        (visibility public))
  (slot is-integer-cast
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-lossless-cast
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)))

(defclass llvm::alloca-instruction
  (is-a unary-instruction)
  (slot pointer-type
        (type SYMBOL)
        (visibility public))
  (slot is-array-allocation
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-static-allocation
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-static
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::unary-instruction
  (is-a instruction) )

(defclass llvm::binary-operator
  (is-a instruction)
  (slot has-no-signed-wrap
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-no-unsigned-wrap
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-exact
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)) )

(defclass llvm::memory-intrinsic
  (is-a intrinsic-instruction)
  (slot destination
        (type SYMBOL)
        (visibility public))
  (slot alignment-constant 
        (type NUMBER)
        (visibility public))
  (slot is-volatile
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)) )

(defclass llvm::dbg-info-intrinsic
  (is-a intrinsic-instruction) )

(defclass llvm::intrinsic-instruction
  (is-a call-instruction)
  (slot intrinsic-id 
        (type NUMBER)
        (visibility public)) )

(defclass llvm::call-instruction
  (is-a instruction)
  (slot called-function
        (type SYMBOL)
        (visibility public))
  (slot calling-convention
        (type SYMBOL)
        (visibility public))
  (multislot argument-operands
             (visibility public))
  (multislot attributes
             (visibility public))
  (slot has-by-val-argument
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-struct-ret-attr
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-throw
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-return
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot only-reads-memory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-access-memory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot can-return-twice
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-no-inline
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-tail-call
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-value-handle
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-dereferenceable-pointer
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-inline-asm
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::phi-node
  ""
  (is-a instruction
        )
  (slot has-constant-value
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::memory-modifying-object
  ""
  (is-a pointer-operand-object
        )
  (slot is-unordered
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-simple
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-atomic
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-volatile
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::pointer-operand-object
  ""
  (is-a object
        )
  (slot pointer-operand
        (type SYMBOL)
        (visibility public))

  )

(defclass llvm::basic-block
  ""
  (is-a diplomat
        value
        has-contents
        )
  (multislot predecessors
             (visibility public))
  (multislot successors
             (visibility public))
  (multislot unlinked-instructions
             (visibility public))

  )

(defclass llvm::multi-block-container
  ""
  (is-a diplomat
        has-contents
        interop
        )
  (multislot joins
             (visibility public))
  (multislot safe-paths
             (visibility public))
  (multislot exits
             (visibility public))
  (multislot entrances
             (visibility public))
  (multislot splits
             (visibility public))

  )

(defclass llvm::region
  ""
  (is-a multi-block-container
        )
  (slot is-simple
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-top-level-region
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))

  )

(defclass llvm::loop
  (is-a region)
  (multislot back-edges
             (visibility public))
  (multislot exit-blocks
             (visibility public)) ) 
(defclass llvm::module
  (is-a object
        interop)
  (slot endianness
        (type SYMBOL)
        (visibility public))
  (slot pointer-size
        (type SYMBOL)
        (visibility public)) )

(defclass llvm::inline-asm
  (is-a value)
  (slot has-side-effects
        (type SYMBOL)
        (visibility public))
  (slot is-align-stack
        (type SYMBOL)
        (visibility public)) ) 

(defclass llvm::function
  (is-a global-value
        diplomat
        has-contents)
  (multislot arguments
             (visibility public))
  (slot is-def-trivially-dead
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-struct-ret-attr
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot needs-unwind-table-entry
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-uw-table
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-return
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot only-reads-memory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot does-not-access-memory
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot has-gc
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-intrinsic
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot is-vararg
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE))
  (slot calls-function-that-returns-twice
        (type SYMBOL)
        (visibility public)
        (allowed-values FALSE
                        TRUE)))

