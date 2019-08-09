PROJECT_ROOT_DIR:=../../..

include $(PROJECT_ROOT_DIR)/common.mk

TARGET:=Object
SRC_DIRS:=. $(wildcard */.)
FILTER_OUT:=
INCLUDE_DIRS:=$(PROJECT_STATIC_LIB_INCLUDE_DIR)
SYSTEM_INCLUDE_DIRS:=$(LUA_INCLUDE_DIR) $(PROJECT_EXCEPTION_INCLUDE_DIR)
LIB_DIRS:=
LDLIBS:=$(PROJECT_STATIC_LIB_LDLIB)

# '-isystem <dir>' supress warnings from included headers in <dir>. These headers are also excluded from dependency generation
CXXFLAGS:=$(PROJECT_CXXFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -isystem , $(SYSTEM_INCLUDE_DIRS))
LDFLAGS:=$(PROJECT_SHARED_LDFLAGS) $(addprefix -L, $(LIB_DIRS))

################################################################################

SHARED_LIB:=lib$(TARGET).$(SHARED_LIB_EXTENSION)

SRC_DIRS:=$(subst /.,,$(SRC_DIRS))
SRCS:=$(filter-out $(FILTER_OUT), $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all clean

all:
	cd $(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR) && $(MAKE) static
	$(MAKE) $(SHARED_LIB)

$(SHARED_LIB): $(OBJS) $(PROJECT_ROOT_DIR)/$(PROJECT_LIB_DIR)/$(PROJECT_STATIC_LIB)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(addsuffix /*.d, $(SRC_DIRS)) $(addsuffix /*.o, $(SRC_DIRS)) $(SHARED_LIB)
#	rm -f $(DEPS) $(OBJS) $(SHARED_LIB)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
