.PHONY: all class embedded hello inheritance regex showcase clean

all: class embedded hello inheritance regex showcase

class:
	cd class && $(MAKE) all

embedded:
	cd embedded && $(MAKE) all

hello:
	cd hello && $(MAKE) all

inheritance:
	cd inheritance && $(MAKE) all

regex:
	cd regex && $(MAKE) all

showcase: regex
	cd showcase && $(MAKE) all

clean:
	cd class && $(MAKE) $@
	cd embedded && $(MAKE) $@
	cd hello && $(MAKE) $@
	cd inheritance && $(MAKE) $@
	cd regex && $(MAKE) $@
	cd showcase && $(MAKE) $@
