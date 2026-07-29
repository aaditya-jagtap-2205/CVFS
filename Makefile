CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude
SRC     = src/cvfs.c
HDR     = include/cvfs.h
TARGET  = cvfs

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
