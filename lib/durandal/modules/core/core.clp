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
; All of the core functions, objects, modules, etc
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

;(load* "modules/core/Object.clp")
;(load* "modules/core/ParentedObject.clp")
;(load* "modules/core/InteropObject.clp")
;(load* "modules/core/Hint.clp")
;(load* "modules/core/List.clp")
;(load* "modules/core/ParentedHint.clp")
;(load* "modules/core/ParentedList.clp")
;(load* "modules/core/Environment.clp")
; Add more defclasses here
; The programmer is responsible for ordering the load statements so that
; class dependencies are met
;(load* "modules/core/SetExtensions.clp")
;(load* "modules/core/DefModuleExtensions.clp")
; Add more deffunctions here
; The programmer is responsible for ordering the load statements so that
; function dependencies are met



;------------------------------------------------------------------------------
(defclass core::object 
  "Base class of all objects"
  (is-a USER)
  (slot parent
        (type INSTANCE
              SYMBOL)
        (allowed-symbols nil)
        (storage local)
        (visibility public)))
;------------------------------------------------------------------------------
(defclass core::has-contents
  "Denotes that a class has contents"
  (is-a USER)
  (multislot contents 
             (visibility public)))
;------------------------------------------------------------------------------
; InteropObject - An interface that exposes a pointer address. It does not
; inherit from Object
;------------------------------------------------------------------------------
(defclass core::interop
  "Interface to an item outside of the CLIPS environment"
  (is-a USER)
  (slot pointer 
        (type INTEGER 
              EXTERNAL-ADDRESS) 
        (visibility public)))

;uncomment this code and watch the LLVM PassManager fail with a general
;protection fault

;(defmessage-handler core::interop init around 
;                    () 
;                    (call-next-handler)
;                    (if (and (numberp ?self:pointer)
;                             (not (= 0 ?self:pointer))) then
;                      (bind ?self:pointer (to-pointer ?self:pointer))))
;------------------------------------------------------------------------------
; Hint.clp - An object used to signify something is a hint. It also has the
; ability to be reference counted if necessary
;------------------------------------------------------------------------------
(defclass core::hint 
  "An object that makes it easy to create temporary entities."
  (is-a object)
  (multislot values 
             (visibility public))
  (slot reference-count 
        (type NUMBER) 
        (visibility public))
  (slot type 
        (visibility public) 
        (type SYMBOL))
  (slot point 
        (visibility public) 
        (type SYMBOL))
  (message-handler increment-reference-count 
                   primary)
  (message-handler decrement-reference-count 
                   primary))
;------------------------------------------------------------------------------
(defmessage-handler core::hint increment-reference-count primary 
                    "Incrementes the reference count of the given hint.  If no parameters are given 
                    then the default is one. If a parameter is given then that is the value 
                    incremented by. If more than one parameter is given then those numbers are 
                    added together and then added to the current reference count"
                    ($?by)
                    (bind ?self:reference-count 
                          (+ ?self:reference-count
                             (expand$ (if (= (length$ ?by) 0) then
                                        (create$ 1)
                                        else
                                        ?by)))))
;------------------------------------------------------------------------------
(defmessage-handler core::hint decrement-reference-count primary
                    "Decrements the reference count of the given hint.  If no parameters are given 
                    then the default is one. If a parameter is given then that is the value 
                    decremented by. If more than one parameter is given then those numbers are 
                    added together and then subtracted from the current reference count"
                    ($?by)
                    (bind ?self:reference-count 
                          (- ?self:reference-count
                             (expand$ (if (= (length$ ?by) 0) then
                                        (create$ 1)
                                        else
                                        ?by)))))
;------------------------------------------------------------------------------
; set functions
;------------------------------------------------------------------------------
(deffunction core::superset 
             "Checks to see if ?a is a superset of ?b"
             (?a ?b)
             (and (>= (length$ ?a) 
                      (length$ ?b))
                  (subsetp ?b 
                           ?a) 
                  (not (subsetp ?a 
                                ?b))))
;------------------------------------------------------------------------------
(deffunction core::equal$ 
             "Checks to see if the two multifields have the same contents"
             (?a ?b)
             (and (= (length$ ?a) 
                     (length$ ?b))
                  (subsetp ?b 
                           ?a) 
                  (subsetp ?a 
                           ?b)))
;------------------------------------------------------------------------------
(deffunction core::has-common-element 
             "Checks to see if the given multifields have an element in common"
             (?a ?b)
             (progn$ (?c ?a)
                     (if (member$ ?c ?b) then 
                       (return TRUE)))
             FALSE)
;------------------------------------------------------------------------------
(deffunction core::disjoint 
             "Checks to see if ?a and ?b have nothing in common"
             (?a ?b)
             (not (has-common-element ?a 
                                      ?b)))
;------------------------------------------------------------------------------
; defmodule functions
;------------------------------------------------------------------------------
(deffunction core::defmodule-exists 
             "Checks to see if the given name is a defined module"
             (?name)
             (member$ ?name
                      (get-defmodule-list))
;------------------------------------------------------------------------------
