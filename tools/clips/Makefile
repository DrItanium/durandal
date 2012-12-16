##===- projects/durandal/tools/clips/Makefile ---------------*- Makefile -*-===##

#
# Indicate where we are relative to the top of the source tree.
#
DURANDAL_LEVEL=../..

#
# Give the name of the tool.
#
TOOLNAME=clips

#
# List libraries that we'll need
# We use LIBS because sample is a dynamic library.
#
USEDLIBS = rampancy.a ExpertSystem.a clips.a clangFrontend.a \
			  clangSerialization.a clangDriver.a clangCodeGen.a \
           clangParse.a clangSema.a clangStaticAnalyzerFrontend.a \
           clangStaticAnalyzerCheckers.a clangStaticAnalyzerCore.a \
           clangAnalysis.a clangRewrite.a \
           clangEdit.a clangAST.a clangLex.a clangBasic.a clang.a 
LINK_COMPONENTS = all
#
# Include Makefile.common so we know what to do.
#
include $(DURANDAL_LEVEL)/Makefile

