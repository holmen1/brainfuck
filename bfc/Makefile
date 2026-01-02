CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -Iinclude

SRCDIR = src
BINDIR = bin
INCDIR = include

BIN = $(BINDIR)/bfc
SOURCES = $(SRCDIR)/bfc.c $(SRCDIR)/lexer.c
OBJECTS = $(SOURCES:.c=.o)

all: $(BIN)

$(BIN): $(OBJECTS) bin
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin:
	mkdir -p bin

clean:
	rm -f $(OBJECTS) $(BIN)


.PHONY: all clean test
