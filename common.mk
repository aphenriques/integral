INSTALL_TOP= /usr/local
INSTALL_INC= $(INSTALL_TOP)/include/integral
INSTALL_LIB= $(INSTALL_TOP)/lib

INTEGRAL_STATIC_LIB_NAME:=libintegral.a
INTEGRAL_SHARED_LIB_NAME:=libintegral.so

CPPFLAGS:=-std=c++11 -O3 -I/usr/local/include -I/usr/include/lua5.2
LDFLAGS:=-L/usr/local/lib -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib

LIB_LUA_FLAG:=-llua
#LIB_LUA_FLAG:=-llua5.2

SHARED_LIB_FLAGS:=-shared

UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
SHARED_LIB_FLAGS+=-undefined dynamic_lookup
else
CPPFLAGS+=-fPIC
endif
