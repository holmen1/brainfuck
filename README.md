# brainfuck
The brainfuck interpreter knows the following instructions:

|Cmd|Effect|                               
|---|------|                               
|+|Increases element under pointer|      
|-|Decrases element under pointer|       
|>|Increases pointer|      
|<|Decreases pointer|                    
|[|Starts loop, flag under pointer|      
|]|Indicates end of loop<                
|.|Outputs ASCII code under pointer|     
|,|Reads char and stores ASCII under ptr|

## Usage (so far)

### Build and test

```bash
$ make
mkdir -p bin
gcc -std=c99 -Wall -Wextra -O2 src/bf.c -o bin/bf
src/bf.c: In function 'main':
src/bf.c:44:10: warning: variable 'program' set but not used [-Wunused-but-set-variable]
   44 |     char program[PROGRAM_SIZE];
      |          ^~~~~~~

$ make test
Running tests...
=== Brainfuck Test Suite ===

Test 1: Reading valid file (hello_world.bf)
✓ PASS
Test 2: Missing file returns error
✓ PASS
Test 3: No arguments returns error
✓ PASS

All tests passed!
```
