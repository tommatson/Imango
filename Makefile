CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/main.c src/greyscale.c src/mango-maths.c src/gaussian.c
OBJ = $(SRC:.c=.o)
TARGET = build/main

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p build
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build *.o

.PHONY: all clean
