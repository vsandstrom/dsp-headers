CC = clang++
INCLUDES = -I/usr/local/include 

SRC_DIR := dsp
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

TARGETS = $(BUILD_DIR)/vectest $(BUILD_DIR)/fmtest $(BUILD_DIR)/delaytest $(BUILD_DIR)/envtest $(BUILD_DIR)/demotest

CFLAGS := -std=c++14 -g -O1 -I./$(SRC_DIR) $(INCLUDES)
LDFLAGS := -L/usr/local/lib/ -lportaudio 

.PHONY: all clean

all: $(TARGETS)

# COMPILING EXECUTABLES
$(BUILD_DIR)/fmtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) fmtest.cpp -o  $@
$(BUILD_DIR)/envtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) envtest.cpp -o $@
$(BUILD_DIR)/vectest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) vectest.cpp -o $@
$(BUILD_DIR)/delaytest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) delaytest.cpp -o $@
$(BUILD_DIR)/demotest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) demotest.cpp -o $@

# Compiling dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -r $(BUILD_DIR)/
