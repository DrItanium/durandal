##===- projects/durandal/tools/libindirecttest/Makefile ---------------*- Makefile -*-===##

#
# Indicate where we are relative to the top of the source tree.
#
DURANDAL_LEVEL=../..

#
# Give the name of the tool.
#
TOOLNAME=libindirecttest

#
# List libraries that we'll need
# We use LIBS because sample is a dynamic library.
#
USEDLIBS = core.a indirect.a rampancy.a ExpertSystem.a clips.a clangFrontend.a \
			  clangSerialization.a clangDriver.a clangCodeGen.a \
           clangParse.a clangSema.a clangStaticAnalyzerFrontend.a \
           clangStaticAnalyzerCheckers.a clangStaticAnalyzerCore.a \
           clangAnalysis.a clangRewrite.a \
           clangEdit.a clangAST.a clangLex.a clangBasic.a clang.a 
LINK_COMPONENTS = all
ifneq ($(HOST_OS),DARWIN)
  LDFLAGS += -lrt
endif
#
# Include Makefile.common so we know what to do.
#
include $(DURANDAL_LEVEL)/Makefile

