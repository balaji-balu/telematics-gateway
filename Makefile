# Compiler and flags
CC ?= gcc
CFLAGS += -Wall -Wextra -I.
LDFLAGS += -lmosquitto -lcoap

# Binary name
TARGET = telematics-gateway

# Source files
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Linking the final binary
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
