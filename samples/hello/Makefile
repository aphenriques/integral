include ../../common.mk

SRC_DIRS:=.

SAMPLE_SHARED_LIB:=libhello.so

SRCS=$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS=$(addsuffix .o, $(basename $(SRCS)))
DEPS=$(addsuffix .d, $(basename $(SRCS)))

INTEGRAL_STATIC_LIB:=../../src/$(INTEGRAL_STATIC_LIB_NAME)

CPPFLAGS+=-I../../src

.PHONY: all clean

all: $(SAMPLE_SHARED_LIB)

$(SAMPLE_SHARED_LIB): $(OBJS)
	$(CXX) $(OBJS) $(INTEGRAL_STATIC_LIB) $(SHARED_LIB_FLAGS) -o $@

clean:
	rm -f $(DEPS) $(OBJS) $(SAMPLE_SHARED_LIB)

%.d: %.cpp
	$(CXX) $(CPPFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
