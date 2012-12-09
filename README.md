This is a generic template that makes it really easy to create expert system
optimization passes that interface with LLVM.

The code is broken up into several modules. 

core - CLIPS base classes and functions
llvm - classes that allow the knowledge construction engine to work correctly
scheduler - code that handles the ordering of modules
MAIN - repository of knowledge asserted by the Knowledge Construction Engine

the rest of the modules to be defined are the responsibility of the programmer
using this project

