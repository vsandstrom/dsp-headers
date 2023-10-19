CC = clang++
INCLUDES = 
# -I/usr/local/include 

SRC_DIR := dsp
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

TARGETS = $(BUILD_DIR)/vectest $(BUILD_DIR)/combtest $(BUILD_DIR)/fmtest $(BUILD_DIR)/delaytest $(BUILD_DIR)/envtest $(BUILD_DIR)/demotest $(BUILD_DIR)/complextest $(BUILD_DIR)/osctest $(BUILD_DIR)/verbtest

CFLAGS := -std=c++14 -O0 -march=native -msse -mavx -I./$(SRC_DIR) $(INCLUDES)
#-march=native -msse -msse2 -mssse3 -msse4.1 -msse4.2 -mavx512vl -I./$(SRC_DIR) $(INCLUDES)
#-I./$(SRC_DIR) $(INCLUDES)
LDFLAGS := -L/usr/local/lib/ -lportaudio 

.PHONY: all clean

all: $(TARGETS)

# COMPILING EXECUTABLES
$(BUILD_DIR)/fmtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) fmtest.cpp -o  $@
$(BUILD_DIR)/complextest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) complextest.cpp -o  $@
$(BUILD_DIR)/envtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) envtest.cpp -o $@
$(BUILD_DIR)/vectest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) vectest.cpp -o $@
$(BUILD_DIR)/delaytest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) delaytest.cpp -o $@
$(BUILD_DIR)/demotest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) demotest.cpp -o $@
$(BUILD_DIR)/osctest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) osctest.cpp -o $@
$(BUILD_DIR)/combtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) combtest.cpp -o $@
$(BUILD_DIR)/verbtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) verbtest.cpp -o $@

# Compiling dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@


clean:
	rm -r $(BUILD_DIR)/
