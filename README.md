12/22/2012
If you're reading this document then you're interested in the different aspects
of the durandal tool set. This document has many different parts to it
including:

1. What is durandal?
2. Motiviation
3. How libExpertSystem works
4. How librampancy works
5. How libindirect works
6. How libpipeline-clips works
7. What's in lib/durandal

# What is durandal?

Durandal is a tool chain that takes the power of an expert system and provides
it to those who work with LLVM. 

The name durandal is a reference to the AI durandal in the video game Marathon
by Bungie Inc. I chose that name because durandal is one of my favorite
villians of all time. 


# Motiviation 

The topic of cs masters thesis was "Implementing Wavefront Scheduling on
Superscalar Processors". While the scheduling algorithm itself is unimportant
to this document the process of implementing taught me a hell of a lot. I had
decided to use LLVM for my thesis because it was very well documented and had a
lot of features that made it super attractive to use. The biggest of these
features is the ease of implementing optimizations. Unfortunately, my C++
skills at the time were very bad. Combined with the fact that I would have to
retain a ton of state I decided that I had to find a way to make the task of
implementing wavefront scheduling easier while still utilizing LLVM. 

During this time I had just finished writing a simple itanium basic block
scheduler as an expert system for my final project in my expert systems class.
I found the use of CLIPS to be so clean and nice because it allowed me to focus
on the problem more than the implementation details. CLIPS also came with a
fantastic integration guide so that CLIPS could be linked into other programs.
This not only prevented me from having to reinvent the wheel but reduced the
amount of work I had to do in C++ to translating the LLVM data structures into
CLIPS' equivalents that I can operate on with rules. So part of my masters
thesis is also a knowledge construction engine that translates a given
LLVM::Function object and all of it's children into a CLIPS knowledge
representation. This knowledge construction engine was an unintended library I
was forced to write to get my thesis off the ground. Combining the knowledge
construction engine with a simple LLVM pass that retrieved the required LLVM
data structures (loops, regions, and the provided function) and use the
knowledge construction engine to interface with CLIPS to get the data into the
CLIPS environment. 

The other task the pass did was the loading of the rules. I came up with a new
clips command "batch-load" which takes in a variable number of strings and
calls BatchStar (batch* in CLIPS) on each of them. So I defined an entry point
to make it so that any new stages or code written in CLIPS wouldn't require a
recompile of the C++ end. 

After finishing my thesis and graduating in September 2012 I've been on a
mystical journey trying to find the illustrious JOB. With everything I learned
from my thesis I decided to bounce around the idea of implementing other
expert system passes to do other things.  The problem was that it required a
lot of boilerplate code to be duplicated each time I wanted to do something. 

During this time I kept on learning new things about CLIPS, realizing that it
was necessary to write a template project that others could use as a starting
point to make using an expert system to write code less of a pain in the ass. I
rewrote the clips code to use the CLIPS module system which then acted as a
catalyst to migrate the project over to the durandal tool set that this
document is a part of. The objective of this new project was

1. To provide a quick way to test new optimization ideas
2. Provide the ability to easily get testing material (pain in the ass during
   my thesis)
3. Provide a way to access the LLVM Pass system in a dynamic fashion. 
4. Make it possible to write passes in languages other than C++

The first objective has been provided through the use of the durandal program
which is a CLIPS REPL with extra functions and abilities. It is also provided
thorough all of the classes and functions found in lib/durandal/

The second objective came about as a way to streamline the process of getting
data into CLIPS for processing. During my thesis I had to use clang externally
to generate llvm bitcode that opt could use. This caused problems for larger
programs because I couldn't properly link them together. The rampancy library
came about as a way to make it easy to get a wide range of testing material. At
first I just wanted clang support but as time went on I realized that I need to
adopt a logcial naming system like what clips does for printing. In fact, my
original design for rampancy was that you would use the printout command to the
compile I/O router which would parse it and then send it to where it needed to
go. I rejected this idea because it would have been a nightmare to debug and
maintain.

So instead, I setup the rampancy library to have a global compiler registry
that was used to dispatch the given command line arguments (or code if you are
asking the compiler to interpret instead) to the appropriate compiler backend
which would then return a module that is converted to knowledge that the
current expert system environment uses. This design allows, any language to be 
used as testing material as long as it can create an LLVM representation of the 
given code. As it stands right now, librampancy was designed to work with clips
but the clips specific code can be refactored off into a separate library if
someone wanted to use it without the knowledge construction.

With this library working I realized that the next step was to reimplement
wavefront scheduling (as a test) within this new tool set. To do this I
realized that I wanted to be able to make it really easy to create passes in
CLIPS (and other languages, I'll get to that in a bit) by not requiring a
single line of C++ yet interact with the pass manager like they are written in
C++. This is where libindirect comes in, it does not provide any clips specific
functionality but instead provides a framework to make creating indirect passes
that are provided their functionality dynamically. This is possible because
the pass system in llvm is very well written. 

The next step is libpipeline which contains the language specific code to allow
the language to interact with libindirect as well as allow libindirect and llvm
to provide the language with the information necessary to run a pass within it.  

# How libExpertSystem works

libExpertSystem is the name I use for the knowledge construction engine I wrote
during my thesis. I've rewritten it for durandal so that it uses classes
instead of global functions and recursion. This makes it possible to support 
multiple functions with a similiar naming scheme. The knowledge construction
engine goes through all the pieces that make up the different llvm data
structures (Module, Function, Region, Loop, BasicBlock, Instruction, Type,
Constant, Operator, etc) and constructs a nearly type exact copy as a CLIPS
class. 

This required 

# How librampancy works

librampancy, as stated above, is a library that provides a logical router for
compilers. It keeps track of compilers that are defined as a subtype of
rampancy::Compiler which contains methods requiring the programmer to handle
the input set from both standard c input arguments (argc, argv) as well as from
CLIPS. The second type occurs whenever the command (rampancy-compile <compiler>
$?args) or (rampancy-intepret ...) is called in CLIPS. Generally speaking, it
is a good idea to implement the code to invoke the target language compiler
within the compile(argc,argv) method and creates the appropriate information
within the clips verison of compile(). 

The Cortex is a class that provides the logical routing capabilities to make
it easy to go from the CLIPS interface function to the compiler without
actually knowing anything about the compiler being compiled. Adding a new
compiler is as easy as defining the compiler implementation and then passing a
reference of it to the global CompilerRegistry combined with a name is all
that's necessary. The cortex interfaces wiht the compiler registry through a
CompilerManager class which does the actual heavy lifiting as well as passing
the appropriate LLVMContext and clips environment. If the cortex is invoked
from a CLIPS call then the Cortex also runs a small LLVM pass that runs the
knowledge conversion pass to convert the module and component pieces to a form
that clips can operate on. Most of librampancy is an internal affair that the 
end user only needs to look at if they want to reimplement different aspects of it. 

# How libindirect works

Lib indirect

# How libpipeline-clips works

## The architecture of a CLIPS pass template

# What's in lib/durandal

lib/durandal contains the CLIPS source code that streamlines the process of
writing LLVM passes within CLIPS. There are two major folders in lib/durandal

* modules - contains all of the objects, functions, and logic that LLVM passes
  written in CLIPS use
* passes - contains the PassRegistry and a series of passes written in CLIPS.

A separate guide details the writing of passes within CLIPS.


