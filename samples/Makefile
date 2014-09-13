.PHONY: all class embedded error_handling hello inheritance luafunction reference regex rtti_polymorphism shared_ptr showcase synthetic_inheritance type_function clean

all: class embedded error_handling hello inheritance luafunction reference regex rtti_polymorphism shared_ptr showcase synthetic_inheritance type_function

class:
	cd class && $(MAKE) all

embedded:
	cd embedded && $(MAKE) all

error_handling:
	cd error_handling && $(MAKE) all

hello:
	cd hello && $(MAKE) all

inheritance:
	cd inheritance && $(MAKE) all

luafunction:
	cd luafunction && $(MAKE) all

reference:
	cd reference && $(MAKE) all

regex:
	cd regex && $(MAKE) all

rtti_polymorphism:
	cd rtti_polymorphism && $(MAKE) all

shared_ptr:
	cd shared_ptr && $(MAKE) all

showcase: regex
	cd showcase && $(MAKE) all

synthetic_inheritance:
	cd synthetic_inheritance && $(MAKE) all

type_function:
	cd type_function && $(MAKE) all

clean:
	cd class && $(MAKE) $@
	cd embedded && $(MAKE) $@
	cd error_handling && $(MAKE) $@
	cd hello && $(MAKE) $@
	cd inheritance && $(MAKE) $@
	cd luafunction && $(MAKE) $@
	cd reference && $(MAKE) $@
	cd regex && $(MAKE) $@
	cd rtti_polymorphism && $(MAKE) $@
	cd shared_ptr && $(MAKE) $@
	cd showcase && $(MAKE) $@
	cd synthetic_inheritance && $(MAKE) $@
	cd type_function && $(MAKE) $@
