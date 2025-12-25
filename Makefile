CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
DEBUG_FLAGS = -g -O0

# Targets
all: bf bfc

# Interpreter
bf: src/bf.c
	$(CC) $(CFLAGS) src/bf.c -o bf

# Compiler
bfc: src/bfc.c
	$(CC) $(CFLAGS) src/bfc.c -o bfc

# Debug builds
debug: CFLAGS = -std=c99 -Wall -Wextra $(DEBUG_FLAGS)
debug: all

# Run tests
test: all
	@echo "Running tests..."
	@./tests/run_tests.sh

# Benchmark
benchmark: all
	@echo "Running benchmarks..."
	@./benchmarks/bench.sh

# Clean
clean:
	rm -f bf bfc *.o *.asm *.out

.PHONY: all debug test benchmark clean
