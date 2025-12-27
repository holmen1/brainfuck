/* Brainfuck Interpreter
 * C99 implementation
 * Portable: Works on Linux, FreeBSD, macOS, and any POSIX system
 */

#include "bf.h"
#include <stdio.h>

/* Internal function declarations */
static int find_matching_bracket(const char *program, int pos, int direction);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.bf>\n", argv[0]);
        return 1;
    }

    char program[PROGRAM_SIZE];
    int program_length = read_program(argv[1], program, PROGRAM_SIZE);

    if (program_length < 0) {
        return 1; // Error message already printed by read_program()
    }

    unsigned char memory[MEMORY_SIZE] = {0};
    return execute_program(program, program_length, memory);
}

/**
 * Read Brainfuck source file into buffer
 *
 * Arguments:
 *   filename - Path to .bf source file
 *   buffer   - Pre-allocated buffer to store program
 *   max_size - Maximum bytes to read
 *
 * Returns:
 *   Number of bytes read, or -1 on error
 */
int read_program(const char *filename, char *buffer, int max_size)
{
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

/*
 * Execute Brainfuck program
 *
 * Arguments:
 *   program        - Brainfuck source code
 *   program_length - Number of instructions
 *   memory         - Pre-allocated memory array
 *
 * Returns:
 *   0 on success
 */
int execute_program(const char *program, int program_length,
                    unsigned char *memory)
{
    unsigned char *cell = memory; /* pointer to current cell */

    /* Main execution loop */
    for (int pc = 0; pc < program_length; pc++) {
        char cmd = program[pc];

        switch (cmd) {
        case '>':
            cell++;
            break;
        case '<':
            cell--;
            break;
        case '+':
            (*cell)++;
            break;
        case '-':
            (*cell)--;
            break;
        case '.':
            putchar(*cell);
            break;
        case ',':
            *cell = getchar();
            break;
        case '[':
            if (*cell == 0) {
                pc = find_matching_bracket(program, pc, +1);
            }
            break;
        case ']':
            if (*cell != 0) {
                pc = find_matching_bracket(program, pc, -1);
            }
            break;
        default:
            /* ignore other characters */
            break;
        }
    }
    return 0;
}

static int find_matching_bracket(const char *program, int pos, int direction)
{
    int depth = 1;
    while (1) {
        pos += direction;
        if (program[pos] == '[') {
            depth += (direction == +1) ? 1 : -1;
        } else if (program[pos] == ']') {
            depth += (direction == +1) ? -1 : 1;
        }

        if (depth == 0) {
            return pos;
        }
    }
}
