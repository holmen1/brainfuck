/* Brainfuck Interpreter - Header
 * C99 implementation
 * Portable: Works on Linux, FreeBSD, macOS, and any POSIX system
 */

#ifndef BF_H
#define BF_H

#define MEMORY_SIZE 30000
#define PROGRAM_SIZE 100000

int read_program(const char *filename, char *buffer, int max_size);
int execute_program(const char *program, int program_length, unsigned char *memory);

#endif /* BF_H */
