# brainfuck

A minimal Brainfuck interpreter and compiler written in C99.

Brainfuck is an esoteric programming language created by Urban Müller in 1993. See the [language specification](https://esolangs.org/wiki/Brainfuck) for details.

## Language Reference

The brainfuck interpreter knows the following instructions:

|Cmd|Effect|                               
|---|------|                               
|+|Increases element under pointer|      
|-|Decrases element under pointer|       
|>|Increases pointer|      
|<|Decreases pointer|                    
|[|Starts loop, flag under pointer|      
|]|Indicates end of loop|             
|.|Outputs ASCII code under pointer|     
|,|Reads char and stores ASCII under ptr|

## Inerpreter

### Hello World
```bash
$ cat examples/hello_world.bf
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```
```bash
$ ./bin/bf ./examples/hello_world.bf
Hello World!
```

### 2 + 2 = 4
```bash
$ cat examples/add.bf
,>,<[->+<]>>++++++[ - >++++++++< ]>[ - << - >> ]<< .
$ ./bin/bf examples/add.bf
22
4
```
Program purpose: Reads two numbers as ASCII characters, adds them (interpreting them as digit characters), and outputs the sum as an ASCII character.

Breakdown:

`,>,<` - Read two input characters into cells 0 and 1, then move back to cell 0

`[->+<]` - Loop that transfers the value in cell 0 to cell 1:

Decrement cell 0  
Move right and increment cell 1  
Move left and repeat until cell 0 is zero  
Result: cell 0 = 0, cell 1 contains the sum  
`>>` - Move to cell 2

`++++++[ - >++++++++< ]` - Multiply 6 × 8 = 48 in cell 2 (this is the ASCII offset for character '0'):

Set cell 2 to 6  
Loop: decrement cell 2, move right, add 8 to cell 3, move left  
After loop: cell 2 = 0, cell 3 = 48  
`>[ - << - >> ]` - Subtract 48 from cell 1 (the sum):

Move to cell 3 (which holds 48)  
While cell 3 > 0: decrement cell 3, move left twice, decrement cell 1, move right twice  
Result: cell 1 now contains the numeric difference  
`<< .` - Move back to cell 1 and output the character

Example: If you input '2' and '2' (ASCII 50), it calculates (50 + 50) - 48 = 52, which is ASCII '4'


### Build and test

```bash
$ make
gcc -std=c99 -Wall -Wextra -O2 -Iinclude src/bf.c -o bin/bf
$ make test
Running tests...
=== Brainfuck Test Suite ===

Test 1: Reading valid file (hello_world.bf)
✓ PASS
Test 2: Output single character (+++++++++.)
✓ PASS (output: ASCII 9)
Test 3: Pointer movement (>+.)
✓ PASS (output: ASCII 1)
Test 4: Multiple increments
✓ PASS (output: ASCII 95, 96)
Test 5: Simple loop (++[>+<-]>.)
✓ PASS (output: ASCII 2)
Test 6: Loop multiplication (+++[>++<-]>.)
✓ PASS (output: ASCII 6)
Test 7: Nested loops (++[>++[>++<-]<-]>>.)
✓ PASS (output: ASCII 8)

=== Bracket Mismatch Tests ===
Test 8: Missing closing bracket ([++)
✓ PASS (exit code: 1)
Test 9: Missing opening bracket (++])
✓ PASS (exit code: 1)
Test 10: Multiple missing closing brackets ([[++)
✓ PASS (exit code: 2)
Test 11: Multiple missing opening brackets (++]])
✓ PASS (exit code: 2)
Test 12: Mixed mismatches ([[])
✓ PASS (exit code: 1)
Test 13: Properly balanced brackets ([]++.)
✓ PASS (exit code: 0, output: ASCII 2)

=== Addition Program Tests ===
Test 14: Addition (2+2=4)
✓ PASS (output: '4')
```

## Compiler (work in progress)

**Print AST**

```bash
$ ./bin/bfc ../bf/examples/add.bf --print-ast
Reading source file: ../bf/examples/add.bf
Lexing...
Parsing...
SEQUENCE(12 children)
  INPUT
  MOVE_PTR(1)
  INPUT
  MOVE_PTR(-1)
  LOOP
    SEQUENCE(4 children)
      MODIFY_CELL(-1)
      MOVE_PTR(1)
      MODIFY_CELL(1)
      MOVE_PTR(-1)
  MOVE_PTR(2)
  MODIFY_CELL(6)
  LOOP
    SEQUENCE(4 children)
      MODIFY_CELL(-1)
      MOVE_PTR(1)
      MODIFY_CELL(8)
      MOVE_PTR(-1)
  MOVE_PTR(1)
  LOOP
    SEQUENCE(4 children)
      MODIFY_CELL(-1)
      MOVE_PTR(-2)
      MODIFY_CELL(-1)
      MOVE_PTR(2)
  MOVE_PTR(-2)
  OUTPUT
```
