# 编译器和标志
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I./utils

# 目录
SRC_DIR = .
UTILS_DIR = ./utils
BUILD_DIR = ./build

# 源文件
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/inquire.cpp $(SRC_DIR)/test.cpp \
       $(UTILS_DIR)/colorful.cpp $(UTILS_DIR)/console.cpp $(UTILS_DIR)/encode.cpp \
       $(UTILS_DIR)/func.cpp
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/inquire.o $(BUILD_DIR)/test.o \
       $(BUILD_DIR)/colorful.o $(BUILD_DIR)/console.o $(BUILD_DIR)/encode.o \
       $(BUILD_DIR)/func.o

# 目标可执行文件
TARGET = main.exe
TEST_TARGET = test.exe

# 默认目标
all: $(BUILD_DIR) $(TARGET)

# 创建build文件夹
$(BUILD_DIR):
	@mkdir "$(BUILD_DIR)"

# 链接
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^
	@echo "Build finished!"
	@echo "Running \"./$(TARGET)\"......
	./$(TARGET)

# 对象文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(UTILS_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@



# Test目标
test: $(BUILD_DIR)/test.o
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/test.cpp -o $(TEST_TARGET)

# # Clean目标
# clean:
# 	-@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
# 	-@if exist "$(TARGET)" del /F /Q "$(TARGET)"
# 	-@if exist "$(TEST_TARGET)" del /F /Q "$(TEST_TARGET)"
# Clean目标
clean:
ifeq ($(OS),Windows_NT)
	-@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
	-@if exist "$(TARGET)" del /F /Q "$(TARGET)"
	-@if exist "$(TEST_TARGET)" del /F /Q "$(TEST_TARGET)"
else
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf $(BUILD_DIR); fi
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TEST_TARGET)" ]; then rm -f $(TEST_TARGET); fi
endif

# 伪目标
.PHONY: all clean test
