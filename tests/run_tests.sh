#!/bin/sh
# Test runner for Brainfuck interpreter

echo "=== Brainfuck Test Suite ==="
echo ""

# Test 1: Valid file returns success (0)
echo "Test 1: Reading valid file (hello_world.bf)"
./bin/bf examples/hello_world.bf > /dev/null && echo "✓ PASS" || exit 1

# Test 2: Missing file returns error
echo "Test 2: Missing file returns error"
./bin/bf nonexistent.bf > /dev/null 2>&1 && exit 1 || echo "✓ PASS"

# Test 3: No arguments returns error
echo "Test 3: No arguments returns error"
./bin/bf > /dev/null 2>&1 && exit 1 || echo "✓ PASS"

echo ""
echo "All tests passed!"
