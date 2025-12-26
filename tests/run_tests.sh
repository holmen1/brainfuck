#!/bin/sh
# Test runner for Brainfuck interpreter

echo "=== Brainfuck Test Suite ==="
echo ""

# Test 1: Valid file returns success (0)
echo "Test 1: Reading valid file (hello_world.bf)"
./bin/bf examples/hello_world.bf > /dev/null && echo "✓ PASS" || exit 1

# Test 2: Simple output test
echo "Test 2: Output single character (+++++++++.)"
RESULT=$(printf '+++++++++.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
[ "$RESULT" = "09" ] && echo "✓ PASS (output: ASCII 9)" || echo "✗ FAIL (got: $RESULT, expected: 09)"

# Test 3: Pointer movement test
echo "Test 3: Pointer movement (>+.)"
RESULT=$(printf '>+.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
[ "$RESULT" = "01" ] && echo "✓ PASS (output: ASCII 1)" || echo "✗ FAIL (got: $RESULT, expected: 01)"

# Test 4: Multiple increments (output 95, then 96)
echo "Test 4: Multiple increments"
RESULT=$(printf '++++++++++ ++++++++++ ++++++++++ ++++++++++ ++++++++++ ++++++++++ ++++++++++ ++++++++++ ++++++++++ +++++.+.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
# 95 increments = ASCII 95 (0x5f), then +1 = ASCII 96 (0x60)
[ "$RESULT" = "5f60" ] && echo "✓ PASS (output: ASCII 95, 96)" || echo "✗ FAIL (got: $RESULT, expected: 5f60)"

# Test 5: Simple loop
echo "Test 5: Simple loop (++[>+<-]>.)"
RESULT=$(printf '++[>+<-]>.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
# Cell 0 = 2, loop moves to cell 1, output cell 1 = ASCII 2 (0x02)
[ "$RESULT" = "02" ] && echo "✓ PASS (output: ASCII 2)" || echo "✗ FAIL (got: $RESULT, expected: 02)"

# Test 6: Loop with multiplication (+++[>++<-]>.)
echo "Test 6: Loop multiplication (+++[>++<-]>.)"
RESULT=$(printf '+++[>++<-]>.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
# 3 iterations, each adds 2 to cell 1: 3*2 = 6 (0x06)
[ "$RESULT" = "06" ] && echo "✓ PASS (output: ASCII 6)" || echo "✗ FAIL (got: $RESULT, expected: 06)"

# Test 7: Nested loops (++[>++[>++<-]<-]>>.)
echo "Test 7: Nested loops (++[>++[>++<-]<-]>>.)"
RESULT=$(printf '++[>++[>++<-]<-]>>.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
# Outer loop runs 2 times, inner loop runs 2*2=4 times total, each adds 2: 2*2*2 = 8 (0x08)
[ "$RESULT" = "08" ] && echo "✓ PASS (output: ASCII 8)" || echo "✗ FAIL (got: $RESULT, expected: 08)"

echo ""

