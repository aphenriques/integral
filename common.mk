PROJECT:=integral

PROJECT_LIB_ROOT_DIR:=lib
PROJECT_LIB_DIR:=$(PROJECT_LIB_ROOT_DIR)/$(PROJECT)
PROJECT_BIN_DIR:=samples
PROJECT_TEST_DIR:=test
PROJECT_DEPENDENCIES_DIR:=dependencies
PROJECT_STATIC_LIB:=lib$(PROJECT).a

# some variables require PROJECT_ROOT_DIR definition. That's why = is used instead of := for their definition
PROJECT_INCLUDE_DIRS=$(PROJECT_ROOT_DIR)/$(PROJECT_LIB_ROOT_DIR)
PROJECT_LDLIBS=$(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR)/$(PROJECT_STATIC_LIB)

ifdef USE_LUAJIT
LUAJIT_INCLUDE_FOLDER?=/usr/local/include/luajit-2.0
PROJECT_SYSTEM_INCLUDE_DIRS:=$(LUAJIT_INCLUDE_FOLDER) $(PROJECT_ROOT_DIR)/$(PROJECT_DEPENDENCIES_DIR)/exception/include
else
LUA_INCLUDE_FOLDER?=/usr/local/include
PROJECT_SYSTEM_INCLUDE_DIRS:=$(LUA_INCLUDE_FOLDER) $(PROJECT_ROOT_DIR)/$(PROJECT_DEPENDENCIES_DIR)/exception/include
endif

PROJECT_LIB_DIRS:=/usr/local/lib
# '?=' sets the variable if it was not previously set
OPTIMIZATION_FLAGS?=-O0 -g
PROJECT_CXXFLAGS:=-std=c++17 $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS) -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic
PROJECT_LDFLAGS:=$(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS)
# requires PROJECT_LDFLAGS definition. That's why = is used instead of :=
PROJECT_EXECUTABLE_LDFLAGS=$(PROJECT_LDFLAGS)

ifeq ($(shell uname -s),Darwin)
# -Wweak-vtables is a clang feature
SHARED_LIB_EXTENSION:=dylib
PROJECT_LDFLAGS+=-undefined dynamic_lookup
ifdef USE_LUAJIT
PROJECT_EXECUTABLE_LDFLAGS+=-pagezero_size 10000 -image_base 100000000
endif
else
SHARED_LIB_EXTENSION:=so
endif

PROJECT_SHARED_LIB:=lib$(PROJECT).$(SHARED_LIB_EXTENSION)

ifdef USE_LUAJIT
LIB_LUA_FLAG:=-lluajit-5.1
else
LIB_LUA_FLAG:=-llua
endif
