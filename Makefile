include common.mk

INSTALL_TOP:=/usr/local
INSTALL_INC:=$(INSTALL_TOP)/include/$(PROJECT)
INSTALL_LIB:=$(INSTALL_TOP)/lib

.PHONY: all static static_release shared shared_release samples test install_exception uninstall_exception install_static install uninstall clean

# Any of the following make rules can be executed with the `-j` option (`make -j`) for parallel compilation 

all:
	cd $(INTEGRAL_LIB_DIR) && $(MAKE) $@

static:
	cd $(INTEGRAL_LIB_DIR) && $(MAKE) $@

# attention! the object files (.o) are generated without -fPIC, so it is necessary to clean the project in order to build shared targets (and most sample/core targets) which require -fPIC
static_release: clean
	cd $(INTEGRAL_LIB_DIR) && $(MAKE) static OPTIMIZED=y WITH_FPIC=n

shared:
	cd $(INTEGRAL_LIB_DIR) && $(MAKE) $@

shared_release:
	cd $(INTEGRAL_LIB_DIR) && $(MAKE) shared OPTIMIZED=y

samples: static
	cd $(INTEGRAL_BIN_DIR) && $(MAKE) all

test: static
	cd $(INTEGRAL_TEST_DIR) && $(MAKE) run

install_exception:
	cd $(INTEGRAL_DEPENDENCIES_DIR)/exception && $(MAKE) install

uninstall_exception:
	cd $(INTEGRAL_DEPENDENCIES_DIR)/exception && $(MAKE) uninstall

install_static: install_exception
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(INTEGRAL_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(INTEGRAL_LIB_DIR)/$(INTEGRAL_STATIC_LIB) $(INSTALL_LIB)

install_shared: install_exception
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(INTEGRAL_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(INTEGRAL_LIB_DIR)/$(INTEGRAL_SHARED_LIB) $(INSTALL_LIB)

install: install_exception
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(INTEGRAL_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(INTEGRAL_LIB_DIR)/$(INTEGRAL_STATIC_LIB) $(INTEGRAL_LIB_DIR)/$(INTEGRAL_SHARED_LIB) $(INSTALL_LIB)

uninstall: uninstall_exception
	$(RM) -R $(INSTALL_INC)
	$(RM) $(INSTALL_LIB)/$(INTEGRAL_STATIC_LIB) $(INSTALL_LIB)/$(INTEGRAL_SHARED_LIB)

clean:
	cd $(INTEGRAL_LIB_DIR) && $(MAKE) $@
	cd $(INTEGRAL_BIN_DIR) && $(MAKE) $@
	cd $(INTEGRAL_TEST_DIR) && $(MAKE) $@
