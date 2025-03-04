CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/main.c src/greyscale.c src/mango-maths.c src/gaussian.c src/sobel-operate.c src/suppression.c src/hysteresis.c src/canny.c src/shi-tomasi.c src/DoG.c src/blob-detection.c src/corner-detection.c
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
