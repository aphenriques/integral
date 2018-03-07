PROJECT_ROOT_DIR:=../../..

include $(PROJECT_ROOT_DIR)/common.mk

TARGET:=function_call
SRC_DIRS:=. $(wildcard */.)
FILTER_OUT:=
INCLUDE_DIRS:=$(PROJECT_INCLUDE_DIRS)
SYSTEM_INCLUDE_DIRS:=$(PROJECT_SYSTEM_INCLUDE_DIRS)
LIB_DIRS:=$(PROJECT_LIB_DIRS)
LDLIBS:=$(PROJECT_LDLIBS) $(LIB_LUA_FLAG) -ldl

# '-isystem <dir>' supress warnings from included headers in <dir>. These headers are also excluded from dependency generation
CXXFLAGS:=$(PROJECT_CXXFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -isystem , $(SYSTEM_INCLUDE_DIRS))
LDFLAGS:=$(PROJECT_LDFLAGS) $(addprefix -L, $(LIB_DIRS))

################################################################################

SRC_DIRS:=$(subst /.,,$(SRC_DIRS))
SRCS:=$(filter-out $(FILTER_OUT), $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all run clean

all:
	cd $(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR) && $(MAKE) all
	$(MAKE) $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJS) $(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR)/$(PROJECT_STATIC_LIB)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(addsuffix /*.d, $(SRC_DIRS)) $(addsuffix /*.o, $(SRC_DIRS)) $(TARGET)
#	rm -f $(DEPS) $(OBJS) $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
