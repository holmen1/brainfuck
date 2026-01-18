CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Iinclude
BIN     = bin/bfc
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c,build/%.o,$(SOURCES))

all: CFLAGS += -O2
all: $(BIN)

debug: CFLAGS += -g -O0
debug: $(BIN)

$(BIN): $(OBJECTS) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c -o $@ $<

bin build:
	@mkdir -p $@

clean:
	rm -f $(OBJECTS) $(BIN)

.PHONY: all debug clean
