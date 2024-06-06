# 编译器和标志
CXX = g++
CXXFLAGS = -std=c++11 -I./Inquire/utils  # -Wall -Wextra 

# 目录
SRC_DIR = ./src
UTILS_DIR = ./src/Inquire/utils
INCLUDE_DIR = ./src/Inquire
BUILD_DIR = ./build

TEST_DIR = ./tests
SINGLE_INCLUDE_DIR = ./include

# 源文件
SRCS = $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/inquire.cpp $(SRC_DIR)/test.cpp \
       $(UTILS_DIR)/colorful.cpp $(UTILS_DIR)/console.cpp $(UTILS_DIR)/encode.cpp \
       $(UTILS_DIR)/func.cpp
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/inquire.o \
       $(BUILD_DIR)/colorful.o $(BUILD_DIR)/console.o $(BUILD_DIR)/encode.o \
       $(BUILD_DIR)/func.o

# 目标可执行文件
TARGET = main
TEST_TARGET = test

# 默认目标
all: $(BUILD_DIR) $(TARGET)

# 创建build文件夹
$(BUILD_DIR):
	@mkdir "$(BUILD_DIR)"

# 链接
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^
	@echo ************Build done!*************
	@echo Running ./$(TARGET)......
	./$(TARGET)

# 对象文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(INCLUDE_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(UTILS_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test目标
test: $(TEST_DIR)/test.cpp $(BUILD_DIR)
	g++ $(TEST_DIR)/test.cpp -o $(BUILD_DIR)/test
	$(BUILD_DIR)/test

example: example/example.cpp $(BUILD_DIR)
	g++ example/example.cpp -o $(BUILD_DIR)/example
	$(BUILD_DIR)/example

# Clean目标
clean:
ifeq ($(OS),Windows_NT)
	-@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
	-@if exist "$(TARGET).exe" del /F /Q "$(TARGET).exe"
	-@if exist "$(TEST_TARGET).exe" del /F /Q "$(TEST_TARGET).exe"
else
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf $(BUILD_DIR); fi
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TEST_TARGET)" ]; then rm -f $(TEST_TARGET); fi
endif

# 伪目标
.PHONY: all clean test
