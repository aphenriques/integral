.PHONY: all regex showcase hello embedded clean

all: regex showcase hello embedded

regex:
	cd regex && $(MAKE) all

showcase: regex
	cd showcase && $(MAKE) all

hello:
	cd hello && $(MAKE) all

embedded:
	cd embedded && $(MAKE) all

clean:
	cd regex && $(MAKE) $@
	cd showcase && $(MAKE) $@
	cd hello && $(MAKE) $@
	cd embedded && $(MAKE) $@
