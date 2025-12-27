/* Brainfuck Interpreter
 * C99 implementation
 * Portable: Works on Linux, FreeBSD, macOS, and any POSIX system
 */

#include "bf.h"
#include <stdio.h>

/* Internal function declarations */
static int build_jump_table(const char *program, int program_length, int *jump_table);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.bf>\n", argv[0]);
        return 1;
    }

    char program[PROGRAM_SIZE];
    int program_length = read_program(argv[1], program, PROGRAM_SIZE);
    if (program_length < 0) {
        return -1; // Error message already printed by read_program()
    }

    // Build jump table and validate brackets in one pass
    int jump_table[PROGRAM_SIZE];
    int unmatched = build_jump_table(program, program_length, jump_table);
    if (unmatched) {
        return unmatched;
    }

    unsigned char memory[MEMORY_SIZE] = {0};
    return execute_program(program, program_length, memory, jump_table);
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
 *   jump_table     - Pre-computed bracket jump positions
 *
 * Returns:
 *   0 on success
 */
int execute_program(const char *program, int program_length,
                    unsigned char *memory, const int *jump_table)
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
                pc = jump_table[pc];
            }
            break;
        case ']':
            if (*cell != 0) {
                pc = jump_table[pc];
            }
            break;
        default:
            /* ignore other characters */
            break;
        }
    }
    return 0;
}

/**
 * Build jump table for bracket pairs and validate bracket matching
 *
 * This function does two things in one pass:
 * 1. Validates that all brackets are properly matched
 * 2. Builds a jump table for O(1) bracket jumping during execution
 *
 * Arguments:
 *   program        - Brainfuck source code
 *   program_length - Number of instructions
 *   jump_table     - Output array mapping bracket positions to their matches
 *
 * Returns:
 *   0 if all brackets match, otherwise the number of unmatched brackets
 */
static int build_jump_table(const char *program, int program_length, int *jump_table)
{
    int stack[PROGRAM_SIZE]; // Stack to track opening bracket positions
    int stack_ptr = 0;       // Current stack depth
    int unmatched_close = 0; // Count of ']' without matching '['

    for (int i = 0; i < program_length; i++) {
        if (program[i] == '[') {
            // Push opening bracket position onto stack
            stack[stack_ptr++] = i;
        } else if (program[i] == ']') {
            if (stack_ptr > 0) {
                // Found matching pair - record both directions in jump table
                int open_pos = stack[--stack_ptr];
                jump_table[open_pos] = i; // '[' jumps to ']'
                jump_table[i] = open_pos; // ']' jumps to '['
            } else {
                // Closing bracket without opening
                unmatched_close++;
            }
        }
    }

    // stack_ptr now contains count of unmatched opening brackets
    int unmatched_open = stack_ptr;
    int total_unmatched = unmatched_open + unmatched_close;

    if (total_unmatched > 0) {
        fprintf(stderr, "Error: %d unmatched bracket%s\n",
                total_unmatched, total_unmatched == 1 ? "" : "s");
    }

    return total_unmatched;
}
