PROJECT:=integral

PROJECT_LIB_ROOT_DIR:=lib
PROJECT_LIB_DIR:=$(PROJECT_LIB_ROOT_DIR)/$(PROJECT)
PROJECT_BIN_DIR:=samples
PROJECT_TEST_DIR:=test
PROJECT_DEPENDENCIES_DIR:=dependencies
PROJECT_STATIC_LIB:=lib$(PROJECT).a

# PROJECT_ROOT_DIR is defined later. That's why = is used instead of :=
PROJECT_EXCEPTION_INCLUDE_DIR=$(PROJECT_ROOT_DIR)/$(PROJECT_DEPENDENCIES_DIR)/exception/include
PROJECT_STATIC_LIB_INCLUDE_DIR=$(PROJECT_ROOT_DIR)/$(PROJECT_LIB_ROOT_DIR)
PROJECT_STATIC_LIB_LDLIB=$(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR)/$(PROJECT_STATIC_LIB)

# extra flags may be added here or in make invocation. E.g: make EXTRA_CXXFLAGS=-g
EXTRA_CXXFLAGS:=
EXTRA_LDFLAGS:=

# LUA_LIB_DIR, LUA_INCLUDE_DIR and LUA_LDLIB may be changed here or in make invocation. E.g: make LUA_LIB_DIR=/path/to/lua/lib
LUA_LIB_DIR:=/usr/local/lib

ifeq ($(WITH_LUAJIT), y)
LUA_INCLUDE_DIR:=/usr/local/include/luajit-2.0
LUA_LDLIB:=-lluajit-5.1
else ifeq ($(or $(WITH_LUAJIT), n), n)
LUA_INCLUDE_DIR:=/usr/local/include
LUA_LDLIB:=-llua
else
$(error Invalid parameter value)
endif

ifeq ($(OPTIMIZED), y)
OPTIMIZATION_FLAGS:=-O3 -march=native -flto
else ifeq ($(or $(OPTIMIZED), n), n)
OPTIMIZATION_FLAGS:=-O0 -g
else
$(error Invalid parameter value)
endif

ifeq ($(SANITIZED), y)
ifneq ($(OPTIMIZED), y)
SANITIZE_FLAGS:=-fsanitize=address -fno-omit-frame-pointer
else
$(error Cannot have SANITIZED=y and OPTIMIZED=y)
endif
else ifneq ($(or $(SANITIZED), n), n)
$(error Invalid parameter value)
endif

ifeq ($(WITH_FPIC), n)
FPIC_FLAG:=
else ifeq ($(or $(WITH_FPIC), y), y)
FPIC_FLAG:=-fPIC
else
$(error Invalid parameter value)
endif

ifeq ($(shell uname -s), Darwin)
SHARED_LIB_EXTENSION:=dylib
PROJECT_DARWIN_SHARED_LDFLAGS:=-undefined dynamic_lookup
ifdef WITH_LUAJIT
PROJECT_DARWIN_LUAJIT_EXECUTABLE_LDFLAGS:=-pagezero_size 10000 -image_base 100000000
endif
else
SHARED_LIB_EXTENSION:=so
endif

PROJECT_SHARED_LIB:=lib$(PROJECT).$(SHARED_LIB_EXTENSION)

PROJECT_CXXFLAGS:=$(EXTRA_CXXFLAGS) -std=c++17 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS) $(FPIC_FLAG)

PROJECT_COMMON_LDFLAGS:=$(EXTRA_LDFLAGS) $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS)
PROJECT_SHARED_LDFLAGS:=$(PROJECT_COMMON_LDFLAGS) -shared $(PROJECT_DARWIN_SHARED_LDFLAGS)
PROJECT_EXECUTABLE_LDFLAGS:=$(PROJECT_COMMON_LDFLAGS) $(PROJECT_DARWIN_LUAJIT_EXECUTABLE_LDFLAGS)
