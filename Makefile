CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

SRCDIR = src
BINDIR = bin

# Targets
all: $(BINDIR)/bf

# Interpreter
$(BINDIR)/bf: $(SRCDIR)/bf.c | $(BINDIR)
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
