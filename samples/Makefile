.PHONY: abstraction core all clean

abstraction:
	cd abstraction && $(MAKE) all

core:
	cd core && $(MAKE) all

all: abstraction core

clean:
	cd abstraction && $(MAKE) $@
	cd core && $(MAKE) $@
