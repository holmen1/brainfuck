CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude

SRCDIR = src
BINDIR = bin
INCDIR = include

BIN = $(BINDIR)/bfc
SOURCES = $(SRCDIR)/bfc.c $(SRCDIR)/lexer.c $(SRCDIR)/ast.c $(SRCDIR)/parser.c $(SRCDIR)/ast_optimizer.c $(SRCDIR)/ir.c $(SRCDIR)/codegen_asm.c
OBJECTS = $(SOURCES:.c=.o)

debug: CFLAGS += -g -O0
debug: $(BIN)

release: CFLAGS += -O2
release: $(BIN)

all: release

$(BIN): $(OBJECTS) bin
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin:
	mkdir -p bin

clean:
	rm -f $(OBJECTS) $(BIN)


.PHONY: all clean test debug release
