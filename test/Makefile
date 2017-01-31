include ../common.mk

SRC_DIRS:=.

TARGET:=integral_test

SRCS:=$(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

PROJECT_STATIC_LIB:=../src/$(PROJECT_STATIC_LIB_NAME)

LDLIBS+=$(LIB_LUA_FLAG) -ldl

CXXFLAGS+=-I../src -I../Catch/single_include/

.PHONY: all test clean

all: $(TARGET)

test: all
	./$(TARGET)

$(TARGET): $(OBJS) $(PROJECT_STATIC_LIB)
	$(CXX) $(OBJS) $(PROJECT_STATIC_LIB) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	$(RM) $(DEPS) $(OBJS) $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
