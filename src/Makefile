include ../common.mk

SRC_DIRS:=.

ARFLAGS:=rcs

SRCS:=$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all static shared clean

all: static shared

static: $(PROJECT_STATIC_LIB_NAME)

shared: $(PROJECT_SHARED_LIB_NAME)

$(PROJECT_STATIC_LIB_NAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(PROJECT_SHARED_LIB_NAME): $(OBJS)
	$(CXX) $(SHARED_LIB_FLAGS) $^ -o $@

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

clean:
	$(RM) $(DEPS) $(OBJS) $(PROJECT_STATIC_LIB_NAME) $(PROJECT_SHARED_LIB_NAME)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
