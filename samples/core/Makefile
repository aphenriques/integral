PROJECT_ROOT_DIR:=../..

include $(PROJECT_ROOT_DIR)/common.mk

# http://stackoverflow.com/a/11206700

SUBDIRS:=$(wildcard */.)
TARGETS:=all clean  # must not contain '/'

SUBDIRS_TARGETS:=$(foreach t,$(TARGETS),$(addsuffix $t,$(SUBDIRS)))

.PHONY: $(TARGETS) $(SUBDIRS_TARGETS) lib

$(TARGETS): % : $(addsuffix %,$(SUBDIRS))
	@echo 'Done "$*" target'

$(SUBDIRS_TARGETS):
	$(MAKE) -C $(@D) $(@F:.%=%)

# lib as prerequisite so that make -j all won't invoke make lib in parallel in each subdir
$(addsuffix all,$(SUBDIRS)): lib

lib:
	cd $(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR) && $(MAKE) all