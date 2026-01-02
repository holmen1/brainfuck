CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -Iinclude

SRCDIR = src
BINDIR = bin
INCDIR = include

# Targets
all: $(BINDIR)/bf

# Interpreter
$(BINDIR)/bf: $(SRCDIR)/bf.c $(INCDIR)/bf.h | $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@

# Create bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Run tests
test: $(BINDIR)/bf
	@echo "Running tests..."
	@./tests/run_tests.sh

# Clean
clean:
	rm -rf $(BINDIR)

.PHONY: all test clean
