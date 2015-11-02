##===- projects/durandal/tools/clips/Makefile ---------------*- Makefile -*-===##

#
# Indicate where we are relative to the top of the source tree.
#
DURANDAL_LEVEL=../..

#
# Give the name of the tool.
#
TOOLNAME=gen-clips-classes

#
# List libraries that we'll need
# We use LIBS because sample is a dynamic library.
#
USEDLIBS = knowledge.a electron.a maya.a
#USEDLIBS = pipeline_clips.a indirect.a core.a rampancy.a ExpertSystem.a clips.a 
LINK_COMPONENTS = all
#
# Include Makefile.common so we know what to do.
#
include $(DURANDAL_LEVEL)/Makefile

ifneq ($(HOST_OS),Darwin)
  LDFLAGS += -lrt
endif

