PROJECT_NAME:=integral

INSTALL_TOP:=/usr/local
INSTALL_INC:=$(INSTALL_TOP)/include/$(PROJECT_NAME)
INSTALL_LIB:=$(INSTALL_TOP)/lib

SHARED_LIB_FLAGS:=-shared
ifeq ($(shell uname -s),Darwin)
SHARED_LIB_EXTENSION:=dylib
SHARED_LIB_FLAGS+=-undefined dynamic_lookup
else
SHARED_LIB_EXTENSION:=so
endif

PROJECT_STATIC_LIB_NAME:=lib$(PROJECT_NAME).a
PROJECT_SHARED_LIB_NAME:=lib$(PROJECT_NAME).$(SHARED_LIB_EXTENSION)

CXXFLAGS:=-std=c++14 -O3 -I/usr/local/include -I/usr/include/lua5.3
#CXXFLAGS:=-std=c++14 -O3 -I/usr/local/include/luajit-2.0
LDFLAGS:=-L/usr/local/lib -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib

LIB_LUA_FLAG:=-llua
#LIB_LUA_FLAG:=-llua5.3


