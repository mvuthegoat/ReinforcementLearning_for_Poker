# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build

# Files to compile
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
MAIN = $(BUILD_DIR)/test

# Targets
all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
