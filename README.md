This program is a generic tool that allows one to use the power of an expert
system to do code analysis with LLVM and, eventually, other programming
languages. This tool requires that it is compiled in the projects folder of an
llvm source tree. 

The expert system used is the CLIPS programming language. 

There are several base modules stored in the modules folder
core - CLIPS base classes and functions
llvm - allows llvm data to be treated as knowledge in an expert system
llvm/passes - contains many different rule sets each of them being their own
module
scheduler - code that handles the ordering of modules

The lib folder contains source code for LibExpertSystem, the interface between
LLVM and CLIPS


The name durandal is a homage to the AI Durandal from the video game Marathon
by Bungie. 
