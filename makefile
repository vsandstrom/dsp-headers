CC = clang++
INCLUDES = # -I./portaudio/include

SRC_DIR := dsp
EXAMPLE_DIR := examples
TEST_DIR := tests
BUILD_DIR := build
DFLAG :=

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

TARGETS :=
# vectest
CFLAGS := -std=c++20 -O0 -march=native -msse -mavx -I./$(SRC_DIR) $(INCLUDES) $(DFLAG) -Wall -Wextra #-pedantic -Werror #-fsanitize=address -fsanitize=undefined
LDFLAGS := -L./portaudio/lib/.libs/ -lportaudio

.PHONY: all clean

all: $(EXAMPLE_DIR)/$(TARGETS)

$(EXAMPLE_DIR)/$(TARGETS): $(OBJS) | $(EXAMPLE_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/$(notdir $@).cpp -o $(EXAMPLE_DIR)/$(notdir $@)

# Compiling dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR): $(EXAMPLE_DIR)
	mkdir -p $@

$(EXAMPLE_DIR): 
	mkdir -p $@

clean: 
	rm -r $(BUILD_DIR)/*
	rm -r $(EXAMPLE_DIR)/*
