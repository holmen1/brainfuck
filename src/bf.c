/* Brainfuck Interpreter
 * C99 implementation
 * Portable: Works on Linux, FreeBSD, macOS, and any POSIX system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 30000
#define PROGRAM_SIZE 100000

/* TODO: Implement the interpreter
 * 
 * Components needed:
 * 1. Memory array (30,000 bytes, initialized to 0)
 * 2. Data pointer (starts at 0)
 * 3. Program storage
 * 4. Program counter
 * 5. Jump table for brackets
 * 
 * Steps:
 * 1. Read Brainfuck source file
 * 2. Build bracket jump table (match [ with ])
 * 3. Execute commands in loop:
 *    > : increment data pointer
 *    < : decrement data pointer
 *    + : increment byte at data pointer
 *    - : decrement byte at data pointer
 *    . : output byte at data pointer
 *    , : input byte to data pointer
 *    [ : if byte is 0, jump to matching ]
 *    ] : if byte is non-zero, jump back to matching [
 * 4. Add error checking (mismatched brackets, bounds, etc.)
 */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.bf>\n", argv[0]);
        return 1;
    }

    // Read Brainfuck source file
    char program[PROGRAM_SIZE];
    int program_length = 0;
    
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    int ch;
    while ((ch = fgetc(file)) != EOF && program_length < PROGRAM_SIZE) {
        program[program_length++] = ch;
    }
    
    fclose(file);
    
    return (program_length > 0) ? 0 : 1;
}
