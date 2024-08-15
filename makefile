# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -I./include

# Linker flags
LDFLAGS := -L/usr/local/lib -lgtest -pthread

# GTest flags
GTEST_FLAGS := --gtest_color=yes --gtest_output=xml:test_results.xml --gtest_repeat=1 --gtest_random_seed=0

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := tests
INCLUDE_DIR := include

# Source files
SRCS := $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Test files
TEST_SRCS := $(wildcard $(TEST_DIR)/*/*.cpp)
TEST_OBJS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_BINS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(BIN_DIR)/%)

# Main targets
.PHONY: all clean test

all: $(OBJS)

test: $(TEST_BINS)
	@for test in $(TEST_BINS); do ./$$test $(GTEST_FLAGS); done

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) test_results.xml

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -c $< -o $@

# Link test executables
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Include dependencies
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

# Generate dependency files
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -MM -MT '$(@:.d=.o)' $< > $@

$(OBJ_DIR)/%.d: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -MM -MT '$(@:.d=.o)' $< > $@
