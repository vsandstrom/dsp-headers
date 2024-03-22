CC = clang++
INCLUDES = # -I./portaudio/include

SRC_DIR := dsp
EXAMPLE_DIR := examples
TEST_DIR := tests
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

TARGETS = $(EXAMPLE_DIR)/vectest $(EXAMPLE_DIR)/combtest $(EXAMPLE_DIR)/fmtest $(EXAMPLE_DIR)/delaytest $(EXAMPLE_DIR)/envtest $(EXAMPLE_DIR)/demotest $(EXAMPLE_DIR)/complextest $(EXAMPLE_DIR)/osctest $(EXAMPLE_DIR)/verbtest $(EXAMPLE_DIR)/verbtest2 $(EXAMPLE_DIR)/verbtest3 $(EXAMPLE_DIR)/graintest $(EXAMPLE_DIR)/envelope_test

CFLAGS := -std=c++14 -O0 -march=native -msse -mavx -I./$(SRC_DIR) $(INCLUDES)
LDFLAGS := -L./portaudio/lib/.libs/ -lportaudio

.PHONY: all clean

all: $(TARGETS)

# COMPILING EXECUTABLES
$(EXAMPLE_DIR)/fmtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/fmtest.cpp -o  $@
$(EXAMPLE_DIR)/complextest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/complextest.cpp -o  $@
$(EXAMPLE_DIR)/envtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/envtest.cpp -o $@
$(EXAMPLE_DIR)/vectest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/vectest.cpp -o $@
$(EXAMPLE_DIR)/delaytest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/delaytest.cpp -o $@
$(EXAMPLE_DIR)/demotest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/demotest.cpp -o $@
$(EXAMPLE_DIR)/osctest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/osctest.cpp -o $@
$(EXAMPLE_DIR)/combtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/combtest.cpp -o $@
$(EXAMPLE_DIR)/verbtest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/verbtest.cpp -o $@
$(EXAMPLE_DIR)/verbtest2: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/verbtest2.cpp -o $@
$(EXAMPLE_DIR)/verbtest3: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/verbtest3.cpp -o $@
$(EXAMPLE_DIR)/graintest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/graintest.cpp -o $@
$(EXAMPLE_DIR)/envelope_test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(TEST_DIR)/envelope_test.cpp -o $@
# Compiling dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLE_DIR):
	mkdir -p $@


$(BUILD_DIR):
	mkdir -p $@

clean: 
	rm -r $(BUILD_DIR)/
