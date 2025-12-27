# brainfuck

A minimal, suckless Brainfuck interpreter written in C99.

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

## Usage (so far)

### Hello World
```bash
$ ./bin/bf ./examples/hello_world.bf
Hello World!
```

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
```
