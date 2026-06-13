CC = gcc
TARGET = chaipop

DIR_BUILD = build
DIR_SRC = src

CFLAGS = -Wall -Wextra -pedantic -std=c99
DEBUG ?= 0
ifeq ($(DEBUG),1)
CFLAGS += -g -Og
else
CFLAGS += -O2
endif

SRC = $(wildcard $(DIR_SRC)/*.c)
OBJS = $(patsubst $(DIR_SRC)/%.c,$(DIR_BUILD)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(DIR_BUILD)/%.o: $(DIR_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean

clean:
	rm -rf $(DIR_BUILD) $(TARGET)

