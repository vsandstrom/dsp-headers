CC = clang++
INCLUDES = -I/usr/local/include -L/usr/local/lib/ -lportaudio 

SRC_DIR := dsp
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/*.cpp,$(BUILD_DIR)/%.o,$(SRCS))

TARGETS = vectest fmtest delaytest 
# envtest

CFLAGS := -std=c++14 -g -O1 $(INCLUDES)
LDFLAGS :=

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $@.cpp -o $(BUILD_DIR)/$@

# $(BUILD_DIR)/fmtest: $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) fmtest.cpp -o $@
#
# $(BUILD_DIR)/vectest: $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) vectest.cpp -o $@
#
# $(BUILD_DIR)/delaytest: $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) delaytest.cpp -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
 
$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -r $(BUILD_DIR)/*
