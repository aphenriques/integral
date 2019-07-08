include common.mk

INSTALL_TOP?=/usr/local
INSTALL_INC:=$(INSTALL_TOP)/include/$(PROJECT)
INSTALL_LIB:=$(INSTALL_TOP)/lib

.PHONY: all static shared samples install_exception uninstall_exception static_release install_static install uninstall clean

# Any of the following make rules can be executed with the `-j` option (`make -j`) for parallel compilation 

all:
	cd $(PROJECT_LIB_DIR) && $(MAKE) $@

static:
	cd $(PROJECT_LIB_DIR) && $(MAKE) $@

shared:
	cd $(PROJECT_LIB_DIR) && $(MAKE) $@

samples: static
	cd $(PROJECT_BIN_DIR) && $(MAKE) all

test: static
	cd $(PROJECT_TEST_DIR) && $(MAKE) run

install_exception:
	cd $(PROJECT_DEPENDENCIES_DIR)/exception && $(MAKE) install

uninstall_exception:
	cd $(PROJECT_DEPENDENCIES_DIR)/exception && $(MAKE) uninstall

static_release: clean
	cd $(PROJECT_LIB_DIR) && $(MAKE) static OPTIMIZATION_FLAGS=-O3 FPIC_FLAG=

install_static:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(PROJECT_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(PROJECT_LIB_DIR)/$(PROJECT_STATIC_LIB) $(INSTALL_LIB)

install: install_exception
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(PROJECT_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(PROJECT_LIB_DIR)/$(PROJECT_STATIC_LIB) $(PROJECT_LIB_DIR)/$(PROJECT_SHARED_LIB) $(INSTALL_LIB)

uninstall: uninstall_exception
	$(RM) -R $(INSTALL_INC)
	$(RM) $(INSTALL_LIB)/$(PROJECT_STATIC_LIB) $(INSTALL_LIB)/$(PROJECT_SHARED_LIB)

clean:
	cd $(PROJECT_LIB_DIR) && $(MAKE) $@
	cd $(PROJECT_BIN_DIR) && $(MAKE) $@
	cd $(PROJECT_TEST_DIR) && $(MAKE) $@
