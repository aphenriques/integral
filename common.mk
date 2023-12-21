INTEGRAL:=integral

INTEGRAL_LIB_ROOT_DIR:=lib
INTEGRAL_LIB_DIR:=$(INTEGRAL_LIB_ROOT_DIR)/$(INTEGRAL)
INTEGRAL_BIN_DIR:=samples
INTEGRAL_TEST_DIR:=test
INTEGRAL_DEPENDENCIES_DIR:=dependencies
INTEGRAL_STATIC_LIB:=lib$(INTEGRAL).a

# INTEGRAL_ROOT_DIR is defined later. That's why = is used instead of :=
INTEGRAL_EXCEPTION_INCLUDE_DIR=$(INTEGRAL_ROOT_DIR)/$(INTEGRAL_DEPENDENCIES_DIR)/exception/include
INTEGRAL_STATIC_LIB_INCLUDE_DIR=$(INTEGRAL_ROOT_DIR)/$(INTEGRAL_LIB_ROOT_DIR)
INTEGRAL_STATIC_LIB_LDLIB=$(INTEGRAL_ROOT_DIR)/$(INTEGRAL_LIB_DIR)/$(INTEGRAL_STATIC_LIB)

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
SANITIZE_FLAGS:=-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined
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
# INTEGRAL_SHARED_LIB is defined later. That's why = is used instead of :=
INTEGRAL_DARWIN_SHARED_LDFLAGS=-undefined dynamic_lookup -install_name '@rpath/$(INTEGRAL_SHARED_LIB)'
ifdef WITH_LUAJIT
INTEGRAL_DARWIN_LUAJIT_EXECUTABLE_LDFLAGS:=-pagezero_size 10000 -image_base 100000000
endif
else
SHARED_LIB_EXTENSION:=so
endif

INTEGRAL_SHARED_LIB:=lib$(INTEGRAL).$(SHARED_LIB_EXTENSION)

INTEGRAL_CXXFLAGS:=$(EXTRA_CXXFLAGS) -std=c++17 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS) $(FPIC_FLAG)

INTEGRAL_COMMON_LDFLAGS:=$(EXTRA_LDFLAGS) $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS)
INTEGRAL_SHARED_LDFLAGS:=$(INTEGRAL_COMMON_LDFLAGS) -shared $(INTEGRAL_DARWIN_SHARED_LDFLAGS)
INTEGRAL_EXECUTABLE_LDFLAGS:=$(INTEGRAL_COMMON_LDFLAGS) $(INTEGRAL_DARWIN_LUAJIT_EXECUTABLE_LDFLAGS)
