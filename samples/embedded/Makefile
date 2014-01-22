include ../../common.mk

SRC_DIRS:=.

SAMPLE_EXECUTALBE:=embedded

SRCS=$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS=$(addsuffix .o, $(basename $(SRCS)))
DEPS=$(addsuffix .d, $(basename $(SRCS)))

INTEGRAL_STATIC_LIB:=../../src/$(INTEGRAL_STATIC_LIB_NAME)

LDLIBS+=$(LIB_LUA_FLAG)

CPPFLAGS+=-I../../src

.PHONY: all clean

all: $(SAMPLE_EXECUTALBE)

$(SAMPLE_EXECUTALBE): $(OBJS)
	$(CXX) $(OBJS) $(INTEGRAL_STATIC_LIB) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(DEPS) $(OBJS) $(SAMPLE_EXECUTALBE)

%.d: %.cpp
	$(CXX) $(CPPFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
