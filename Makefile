# Taken from the clang makefile - Following the sample project was preventing 
# compilation due to missing symbols. 

ifndef DURANDAL_LEVEL

IS_TOP_LEVEL := 1
DURANDAL_LEVEL := .
DIRS := lib tools
EXTRA_DIST = include

endif 

LEVEL := $(DURANDAL_LEVEL)/../..

#
# Include the Master Makefile that knows how to build all.
#
include $(LEVEL)/Makefile.common

CFLAGS += -I$(PROJ_SRC_DIR)/$(DURANDAL_LEVEL)/include -I$(PROJ_OBJ_DIR)/$(DURANDAL_LEVEL)/include
CPPFLAGS += -I$(PROJ_SRC_DIR)/$(DURANDAL_LEVEL)/include -I$(PROJ_OBJ_DIR)/$(DURANDAL_LEVEL)/include
ifeq ($(IS_TOP_LEVEL),1)

ifneq ($(PROJ_SRC_ROOT),$(PROJ_OBJ_ROOT))
$(RecursiveTargets)::
	$(Verb) for dir in test unittests; do \
	  if [ -f $(PROJ_SRC_DIR)/$${dir}/Makefile ] && [ ! -f $${dir}/Makefile ]; then \
	    $(MKDIR) $${dir}; \
	    $(CP) $(PROJ_SRC_DIR)/$${dir}/Makefile $${dir}/Makefile; \
	  fi \
	done
endif

endif
