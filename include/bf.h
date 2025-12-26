/* Brainfuck Interpreter - Header
 * C99 implementation
 * Portable: Works on Linux, FreeBSD, macOS, and any POSIX system
 */

#ifndef BF_H
#define BF_H

/* Memory and program size constants */
#define MEMORY_SIZE 30000
#define PROGRAM_SIZE 100000

/* Function declarations */
int read_program(const char *filename, char *buffer, int max_size);
void execute_program(const char *program, int program_length);
int find_matching_bracket(const char *program, int pos, int direction);

#endif /* BF_H */
