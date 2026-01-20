CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2 -std=c99

TARGET=chaipop

$(TARGET):
	$(CC) $(CFLAGS) src/main.c -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)

