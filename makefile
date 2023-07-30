CC = clang++
INCLUDES = -I/usr/local/include -L/usr/local/lib/ -lportaudio 

SRC_DIR := dsp
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/*.cpp,$(BUILD_DIR)/%.o,$(SRCS))

TARGETS = $(BUILD_DIR)/vectest $(BUILD_DIR)/fmtest $(BUILD_DIR)/delaytest

CFLAGS := -std=c++14 -g -O1 -I$(SRC_DIR) $(INCLUDES)
LDFLAGS :=

.PHONY: all clean

all: $(TARGETS)

$(BUILD_DIR)/fmtest: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) fmtest.cpp -o $@

$(BUILD_DIR)/vectest: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) vectest.cpp -o $@

$(BUILD_DIR)/delaytest: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) delaytest.cpp -o $@
 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
 
$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -r $(BUILD_DIR)/*
