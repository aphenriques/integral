PROJECT_NAME:=integral

INSTALL_TOP:=/usr/local
INSTALL_INC:=$(INSTALL_TOP)/include/$(PROJECT_NAME)
INSTALL_LIB:=$(INSTALL_TOP)/lib

# TODO with c++17: remove -Wno-unused-parameter and -Wno-unused-but-set-parameter and use [[maybe_unused]] attribute
CXXFLAGS:=-std=c++14 -O3 -I/usr/local/include -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wno-missing-braces -Wno-unused-parameter -pedantic
LDFLAGS:=-L/usr/local/lib -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib

LIB_LUA_FLAG:=-llua

SHARED_LIB_FLAGS:=-shared
ifeq ($(shell uname -s),Darwin)
# -Wweak-vtables is a clang feature
CXXFLAGS+=-Wweak-vtables
SHARED_LIB_EXTENSION:=dylib
SHARED_LIB_FLAGS+=-undefined dynamic_lookup
else
CXXFLAGS+=-Wno-unused-but-set-parameter
SHARED_LIB_EXTENSION:=so
endif

PROJECT_STATIC_LIB_NAME:=lib$(PROJECT_NAME).a
PROJECT_SHARED_LIB_NAME:=lib$(PROJECT_NAME).$(SHARED_LIB_EXTENSION)
