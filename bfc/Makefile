CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Iinclude -D_POSIX_C_SOURCE=200809L
BIN     = bin/bfc
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

all: CFLAGS += -O2
all: $(BIN)

debug: CFLAGS += -g -O0
debug: $(BIN)

$(BIN): $(OBJECTS) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin:
	@mkdir -p $@

clean:
	rm -f $(OBJECTS) $(BIN)

.PHONY: all debug clean
