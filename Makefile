CC := g++
CFLAGS := -c -Wall
LDFLAGS := 
SRC_DIR := src/
BUILD_DIR := build/
TARGET := $(BUILD_DIR)/managerd.bin
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:.cpp:.o)

all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

# Pattern rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
