# ENV
EXEC_TARGET := main
EXEC_SRC := main.cpp
LIB_TARGET := libstradian.a
UNIT_TEST_TARGET := unit_test
UNIT_TEST_SRC := unit_test.cpp
#SHD_LIB_TARGET := libstradian.so
CC := clang
CXX := c++
NVCC := nvcc
AR := ar
#LD := ld

# DIR_STRUCTURE
BUILD_DIR := build
SRC_DIR := src
TEST_DIR := test
INC_DIR := /opt/homebrew/include /Users/csian/projects/stradian/include
LIB_DIR := /Users/csian/projects/stradian/lib

ifeq ($(shell which nvcc), )
	SRCS := $(shell find $(SRC_DIR) -name "*.cpp" -or -name "*.c" -or -name "*.s")
else
	SRCS := $(shell find $(SRC_DIR) -name "*.cpp" -or -name "*.c" -or -name "*.s" -or -name "*.cu")
endif

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

#INC_DIRS := $(shell find $(INC_DIR) -type d)
INC_DIRS := $(INC_DIR)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Flags
CFLAGS := $(INC_FLAGS) -MMD -MP
CXXFLAGS := -std=c++20 $(INC_FLAGS) -MMD -MP
TESTFLAGS := -std=c++20 -I/opt/homebrew/include
#SHD_CXXFLAGS := -std=c++20 $(INC_FLAGS) -MMD -MP -fPIC
NVCCFLAGS := $(INC_FLAGS)
LDFLAGS := -L/Users/csian/projects/stradian/lib -lstradian -L/opt/homebrew/lib -lssl -lcrypto -lcurl -L/usr/local/lib/mariadb -lmariadbcpp
ARFLAGS := crs
#SHD_LDFLAGS := -shared
DYLD_LIBRARY_PATH := /usr/local/lib/mariadb

IS_NVCC := $(notdir $(shell which $(NVCC) 2> /dev/null))
$(info $(IS_NVCC))

$(LIB_DIR)/$(LIB_TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cu.o: %.cu
	mkdir -p $(dir $@)
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

.PHONY: exec exec_debug test clean clean_lib
exec: $(LIB_DIR)/$(LIB_TARGET)
	$(CXX) -o $(EXEC_TARGET) $(EXEC_SRC) $(CXXFLAGS) $(LDFLAGS)
	rm $(EXEC_TARGET).d

exec_debug: $(LIB_DIR)/$(LIB_TARGT)
	$(CXX) -o $(EXEC_TARGET) $(EXEC_SRC) $(CXXFLAGS) $(LDFLAGS) -g
	rm -r $(EXEC_TARGET).d #$(EXEC_TARGET).dSYM

test:
	$(CXX) -o $(TEST_DIR)/$(UNIT_TEST_TARGET) $(TEST_DIR)/$(UNIT_TEST_SRC) $(TESTFLAGS)

clean:
	rm -r $(BUILD_DIR)/*

clean_lib:
	rm -r $(LIB_DIR)/*

-include $(DEPS)
