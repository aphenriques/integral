.PHONY: all class embedded error_handling hello inheritance luafunction regex showcase synthetic_inheritance type_function clean

all: class embedded error_handling hello inheritance luafunction regex showcase synthetic_inheritance type_function

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

regex:
	cd regex && $(MAKE) all

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
	cd regex && $(MAKE) $@
	cd showcase && $(MAKE) $@
	cd synthetic_inheritance && $(MAKE) $@
	cd type_function && $(MAKE) $@
