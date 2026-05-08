# Inquire — production C++ interactive CLI library
#
# Common targets:
#   make           # lib + demo
#   make test      # build + run unit tests against the single header
#   make example   # build + run interactive example
#   make asan      # rebuild + test with AddressSanitizer
#   make ubsan     # rebuild + test with UndefinedBehaviorSanitizer
#   make tsan      # rebuild + test with ThreadSanitizer
#   make coverage  # build with --coverage, run tests, summarize gcov
#   make tidy      # run clang-tidy against the modular sources
#   make format    # run clang-format -i across src/ tests/ example/
#   make single-header
#   make clean

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

ALL_FORMATTABLE := $(shell find $(SRC_DIR) $(TEST_DIR) $(EXAMPLE_DIR) \
    -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) 2>/dev/null)

.PHONY: all lib demo test example clean run help single-header \
        asan ubsan tsan coverage tidy format docs

all: lib demo

help:
	@echo "Build targets:"
	@echo "  make all        - build static lib + demo (default)"
	@echo "  make lib        - build $(LIB_AR)"
	@echo "  make demo       - build & link demo binary"
	@echo "  make test       - build & run unit tests (single header)"
	@echo "  make example    - build & run interactive example"
	@echo "  make single-header"
	@echo "Quality targets:"
	@echo "  make asan       - tests under AddressSanitizer"
	@echo "  make ubsan      - tests under UndefinedBehaviorSanitizer"
	@echo "  make tsan       - tests under ThreadSanitizer"
	@echo "  make coverage   - tests with gcov instrumentation"
	@echo "  make tidy       - run clang-tidy"
	@echo "  make format     - run clang-format -i"
	@echo "  make clean"

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

# ---- sanitizer / coverage shortcuts ----
# Each runs in an isolated build directory so they don't poison the
# default `make` artefacts.
SAN_DIR := $(BUILD_DIR)/san

asan:
	@mkdir -p $(SAN_DIR)
	$(CXX) $(CXXSTD) $(WARN) -O1 -g -fsanitize=address -fno-omit-frame-pointer \
	    -I$(INCLUDE_DIR) -o $(SAN_DIR)/test-asan $(TEST_SRC) -fsanitize=address
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 $(SAN_DIR)/test-asan

ubsan:
	@mkdir -p $(SAN_DIR)
	$(CXX) $(CXXSTD) $(WARN) -O1 -g -fsanitize=undefined -fno-omit-frame-pointer \
	    -I$(INCLUDE_DIR) -o $(SAN_DIR)/test-ubsan $(TEST_SRC) -fsanitize=undefined
	UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 $(SAN_DIR)/test-ubsan

tsan:
	@mkdir -p $(SAN_DIR)
	$(CXX) $(CXXSTD) $(WARN) -O1 -g -fsanitize=thread -fno-omit-frame-pointer \
	    -I$(INCLUDE_DIR) -o $(SAN_DIR)/test-tsan $(TEST_SRC) -fsanitize=thread
	$(SAN_DIR)/test-tsan

COV_DIR := $(BUILD_DIR)/cov
coverage:
	@mkdir -p $(COV_DIR)
	$(CXX) $(CXXSTD) $(WARN) -O0 -g --coverage \
	    -I$(INCLUDE_DIR) -o $(COV_DIR)/test-cov $(TEST_SRC) --coverage
	$(COV_DIR)/test-cov
	@if command -v gcov >/dev/null; then \
	    cd $(COV_DIR) && gcov -r test-cov-*.gcno | tail -20 ; \
	else \
	    echo "gcov not found; raw .gcda files in $(COV_DIR)" ; \
	fi

# ---- static analysis & formatting ----
tidy:
	@command -v clang-tidy >/dev/null || { echo "clang-tidy not installed"; exit 1; }
	@command -v cmake >/dev/null || { echo "cmake required for compile_commands.json"; exit 1; }
	cmake -S . -B $(BUILD_DIR)/tidy -DCMAKE_EXPORT_COMPILE_COMMANDS=ON >/dev/null
	clang-tidy -p $(BUILD_DIR)/tidy $(LIB_SRCS)

format:
	@command -v clang-format >/dev/null || { echo "clang-format not installed"; exit 1; }
	clang-format -i $(ALL_FORMATTABLE)

docs:
	@command -v doxygen >/dev/null || { echo "doxygen not installed"; exit 1; }
	doxygen Doxyfile
	@echo "Generated docs at build/doxygen/html/index.html"

clean:
	@rm -rf $(BUILD_DIR)
	@echo cleaned
