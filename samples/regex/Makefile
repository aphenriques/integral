include ../../common.mk

SRC_DIRS:=.

SAMPLE_SHARED_LIB:=libRegex.so

SRCS=$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS=$(addsuffix .o, $(basename $(SRCS)))
DEPS=$(addsuffix .d, $(basename $(SRCS)))

PROJECT_STATIC_LIB:=../../src/$(PROJECT_STATIC_LIB_NAME)

CXXFLAGS+=-I../../src

.PHONY: all clean

all: $(SAMPLE_SHARED_LIB)

$(SAMPLE_SHARED_LIB): $(OBJS) $(PROJECT_STATIC_LIB)
	$(CXX) $(OBJS) $(PROJECT_STATIC_LIB) $(SHARED_LIB_FLAGS) -o $@

clean:
	$(RM) $(DEPS) $(OBJS) $(SAMPLE_SHARED_LIB)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
