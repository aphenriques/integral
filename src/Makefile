include ../common.mk

SRC_DIRS:=.

ARFLAGS:=rcs

SRCS:=$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all static shared clean

all: static shared

static: $(INTEGRAL_STATIC_LIB_NAME)

shared: $(INTEGRAL_SHARED_LIB_NAME)

$(INTEGRAL_STATIC_LIB_NAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(INTEGRAL_SHARED_LIB_NAME): $(OBJS)
	$(CXX) $(SHARED_LIB_FLAGS) $^ -o $@

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

clean:
	$(RM) $(DEPS) $(OBJS) $(INTEGRAL_STATIC_LIB_NAME) $(INTEGRAL_SHARED_LIB_NAME)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
