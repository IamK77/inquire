# Inquire — production C++ interactive CLI library

CXX      ?= g++
CXXSTD   ?= -std=c++11
WARN     ?= -Wall -Wextra -Wpedantic
OPT      ?= -O2
CXXFLAGS ?= $(CXXSTD) $(WARN) $(OPT)
LDFLAGS  ?=

# ---- layout ----
SRC_DIR        := src
INQUIRE_DIR    := $(SRC_DIR)/Inquire
UTILS_DIR      := $(INQUIRE_DIR)/utils
INCLUDE_DIR    := include
BUILD_DIR      := build
EXAMPLE_DIR    := example
TEST_DIR       := tests

INCLUDES := -I$(INQUIRE_DIR) -I$(UTILS_DIR)

# ---- sources ----
LIB_SRCS := \
    $(INQUIRE_DIR)/inquire.cpp \
    $(UTILS_DIR)/colorful.cpp \
    $(UTILS_DIR)/console.cpp \
    $(UTILS_DIR)/encode.cpp \
    $(UTILS_DIR)/func.cpp \
    $(UTILS_DIR)/renderer.cpp \
    $(UTILS_DIR)/tty_guard.cpp

LIB_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(LIB_SRCS))

MAIN_SRC := $(SRC_DIR)/main.cpp
MAIN_OBJ := $(BUILD_DIR)/$(SRC_DIR)/main.o

TEST_SRC := $(TEST_DIR)/test.cpp
EXAMPLE_SRC := $(EXAMPLE_DIR)/example.cpp

TARGET   := $(BUILD_DIR)/inquire-demo
TEST_BIN := $(BUILD_DIR)/inquire-test
EX_BIN   := $(BUILD_DIR)/inquire-example
LIB_AR   := $(BUILD_DIR)/libinquire.a

.PHONY: all lib demo test example clean run help single-header

all: lib demo

help:
	@echo "Targets: lib | demo | test | example | single-header | clean | run"

single-header:
	bash tools/amalgamate.sh

# Static library
lib: $(LIB_AR)

$(LIB_AR): $(LIB_OBJS)
	@mkdir -p $(@D)
	ar rcs $@ $^

# Demo executable from src/main.cpp using sources directly
demo: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LIB_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run: demo
	$(TARGET)

# Compile object files
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Single-header example/test compile against include/inquire.hpp
example: $(EX_BIN)
	$(EX_BIN)

$(EX_BIN): $(EXAMPLE_SRC) $(INCLUDE_DIR)/inquire.hpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(EXAMPLE_SRC) $(LDFLAGS)

test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(INCLUDE_DIR)/inquire.hpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(TEST_SRC) $(LDFLAGS)

clean:
	@rm -rf $(BUILD_DIR)
	@echo cleaned
