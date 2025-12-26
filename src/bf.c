/* Brainfuck Interpreter
 * C99 implementation
 * Portable: Works on Linux, FreeBSD, macOS, and any POSIX system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 30000
#define PROGRAM_SIZE 100000

/**
 * Read Brainfuck source file into buffer
 * 
 * @param filename Path to .bf source file
 * @param buffer   Pre-allocated buffer to store program
 * @param max_size Maximum bytes to read
 * @return Number of bytes read, or -1 on error
 */
int read_program(const char *filename, char *buffer, int max_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    int length = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF && length < max_size) {
        buffer[length++] = ch;
    }
    
    fclose(file);
    
    // Check if file was too large
    if (ch != EOF && length == max_size) {
        fprintf(stderr, "Error: Program too large (max %d bytes)\n", max_size);
        return -1;
    }
    
    return length;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.bf>\n", argv[0]);
        return 1;
    }

    // Read Brainfuck source file
    char program[PROGRAM_SIZE];
    int program_length = read_program(argv[1], program, PROGRAM_SIZE);
    
    if (program_length < 0) {
        return 1;  // Error message already printed by read_program()
    }
    
    return (program_length > 0) ? 0 : 1;
}
