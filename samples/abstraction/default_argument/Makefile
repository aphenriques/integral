INTEGRAL_ROOT_DIR:=../../..

include $(INTEGRAL_ROOT_DIR)/common.mk

TARGET:=default_argument
SRC_DIRS:=. $(wildcard */.)
FILTER_OUT:=
INCLUDE_DIRS:=$(INTEGRAL_STATIC_LIB_INCLUDE_DIR)
SYSTEM_INCLUDE_DIRS:=$(LUA_INCLUDE_DIR) $(INTEGRAL_EXCEPTION_INCLUDE_DIR)
LIB_DIRS:=$(LUA_LIB_DIR)
LDLIBS:=$(INTEGRAL_STATIC_LIB_LDLIB) $(LUA_LDLIB) -ldl

# '-isystem <dir>' supress warnings from included headers in <dir>. These headers are also excluded from dependency generation
CXXFLAGS:=$(INTEGRAL_CXXFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -isystem , $(SYSTEM_INCLUDE_DIRS))
LDFLAGS:=$(INTEGRAL_EXECUTABLE_LDFLAGS) $(addprefix -L, $(LIB_DIRS))

################################################################################

SRC_DIRS:=$(subst /.,,$(SRC_DIRS))
SRCS:=$(filter-out $(FILTER_OUT), $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all run clean

all:
	cd $(INTEGRAL_ROOT_DIR)/$(INTEGRAL_LIB_DIR) && $(MAKE) static
	$(MAKE) $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJS) $(INTEGRAL_ROOT_DIR)/$(INTEGRAL_LIB_DIR)/$(INTEGRAL_STATIC_LIB)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(addsuffix /*.d, $(SRC_DIRS)) $(addsuffix /*.o, $(SRC_DIRS)) $(TARGET)
#	rm -f $(DEPS) $(OBJS) $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
