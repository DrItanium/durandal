(batch* "Init.clp")
(defrule MAIN::GotPasses
 ?f <- (passes $?p)
 =>
 (printout t "Got passes " ?p crlf)
 (retract ?f)
 (instances))
(send [::test] register)
(send [::paths] register)
(rampancy-compile clang (create$ ../../../flops.c))
(send [::mod5] optimize 2 (create$ test))
