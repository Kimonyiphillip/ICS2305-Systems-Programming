
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

TARGET = msh
SOURCE = msh.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
