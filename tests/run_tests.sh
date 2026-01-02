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
echo "=== Bracket Mismatch Tests ==="

# Test 8: Opening bracket without closing
echo "Test 8: Missing closing bracket ([++)"
printf '[++' | ./bin/bf /dev/stdin 2>/dev/null
EXIT_CODE=$?
[ "$EXIT_CODE" = "1" ] && echo "✓ PASS (exit code: 1)" || echo "✗ FAIL (got exit code: $EXIT_CODE, expected: 1)"

# Test 9: Closing bracket without opening
echo "Test 9: Missing opening bracket (++])"
printf '++]' | ./bin/bf /dev/stdin 2>/dev/null
EXIT_CODE=$?
[ "$EXIT_CODE" = "1" ] && echo "✓ PASS (exit code: 1)" || echo "✗ FAIL (got exit code: $EXIT_CODE, expected: 1)"

# Test 10: Multiple unmatched opening brackets
echo "Test 10: Multiple missing closing brackets ([[++)"
printf '[[++' | ./bin/bf /dev/stdin 2>/dev/null
EXIT_CODE=$?
[ "$EXIT_CODE" = "2" ] && echo "✓ PASS (exit code: 2)" || echo "✗ FAIL (got exit code: $EXIT_CODE, expected: 2)"

# Test 11: Multiple unmatched closing brackets
echo "Test 11: Multiple missing opening brackets (++]])"
printf '++]]' | ./bin/bf /dev/stdin 2>/dev/null
EXIT_CODE=$?
[ "$EXIT_CODE" = "2" ] && echo "✓ PASS (exit code: 2)" || echo "✗ FAIL (got exit code: $EXIT_CODE, expected: 2)"

# Test 12: Mixed mismatches
echo "Test 12: Mixed mismatches ([[])"
printf '[[]' | ./bin/bf /dev/stdin 2>/dev/null
EXIT_CODE=$?
[ "$EXIT_CODE" = "1" ] && echo "✓ PASS (exit code: 1)" || echo "✗ FAIL (got exit code: $EXIT_CODE, expected: 1)"

# Test 13: Properly balanced brackets should still work
echo "Test 13: Properly balanced brackets ([]++.)"
RESULT=$(printf '[]++.' | ./bin/bf /dev/stdin 2>/dev/null | od -An -tx1 -v | tr -d ' ')
EXIT_CODE=$?
[ "$EXIT_CODE" = "0" ] && [ "$RESULT" = "02" ] && echo "✓ PASS (exit code: 0, output: ASCII 2)" || echo "✗ FAIL (exit code: $EXIT_CODE, output: $RESULT)"

echo ""
echo "=== Addition Program Tests ==="

# Test 14: Addition 2+2=4
echo "Test 14: Addition (2+2=4)"
RESULT=$(echo -n '22' | ./bin/bf examples/add.bf 2>/dev/null | od -An -tx1 | tr -d ' ')
[ "$RESULT" = "34" ] && echo "✓ PASS (output: '4')" || echo "✗ FAIL (got: $RESULT, expected: 34)"

echo ""

