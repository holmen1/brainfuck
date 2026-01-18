#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "ir.h"
#include "codegen_asm.h"

static char *read_file(const char *filename, int *length);
static int compile_assembly(const char *asm_file, const char *output_file);

int main(int argc, char *argv[])
{
    /* Parse command line arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.bf> [options]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  --print-ast    Print AST and exit\n");
        fprintf(stderr, "  --print-ir     Print IR and exit\n");
        return 1;
    }

    const char *input_file = argv[1];
    int print_ast = 0;
    int print_ir = 0;

    /* Parse remaining arguments */
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            print_ast = 1;
        } else if (strcmp(argv[i], "--print-ir") == 0) {
            print_ir = 1;
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            return 1;
        }
    }

    printf("Reading source file: %s\n", input_file);
    int source_length;
    char *source = read_file(input_file, &source_length);
    if (!source) {
        return 1;
    }

    printf("Lexing...\n");
    Lexer *lexer = lexer_create(source, source_length);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }

    printf("Parsing...\n");
    ASTNode *ast = parser_parse(lexer);
    if (!ast) {
        fprintf(stderr, "Error: Parse failed\n");
        lexer_free(lexer);
        free(source);
        return 1;
    }

    if (print_ast) {
        ast_print(ast);
        ast_free(ast);
        lexer_free(lexer);
        free(source);
        return 0;
    }

    printf("Optimizing AST...\n");
    ast = ast_optimize(ast);
    if (!ast) {
        fprintf(stderr, "Error: Optimization failed\n");
        lexer_free(lexer);
        free(source);
        return 1;
    }

    /* Phase 4: IR Generation */
    printf("Generating IR...\n");
    IRProgram *ir = ir_from_ast(ast);
    if (!ir) {
        fprintf(stderr, "Error: IR generation failed\n");
        ast_free(ast);
        lexer_free(lexer);
        free(source);
        return 1;
    }

    if (print_ir) {
        ir_print(ir);
        ir_free(ir);
        ast_free(ast);
        lexer_free(lexer);
        free(source);
        return 0;
    }

    /* Determine output paths from input filename */
    char asm_file[64];
    char exe_file[64];
    char name[32];
    
    /* Extract basename from input file */
    const char *last_slash = strrchr(input_file, '/');
    const char *base = last_slash ? last_slash + 1 : input_file;
    
    /* Copy and remove .bf extension if present */
    strncpy(name, base, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    
    char *dot = strrchr(name, '.');
    if (dot && strcmp(dot, ".bf") == 0) {
        *dot = '\0';
    }
    
    snprintf(asm_file, sizeof(asm_file), "build/%s.s", name);
    snprintf(exe_file, sizeof(exe_file), "bin/%s", name);

    /* Phase 5: Code Generation */
    printf("Generating x86-64 assembly...\n");
    
    FILE *out = fopen(asm_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", asm_file);
        ir_free(ir);
        ast_free(ast);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    if (codegen_asm(ir, out) != 0) {
        fprintf(stderr, "Error: Code generation failed\n");
        fclose(out);
        ir_free(ir);
        ast_free(ast);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    fclose(out);
    printf("Assembly written to %s\n", asm_file);
    
    /* Phase 6: Compile to executable */
    printf("Compiling to executable...\n");
    if (compile_assembly(asm_file, exe_file) != 0) {
        fprintf(stderr, "Error: Compilation failed\n");
        ir_free(ir);
        ast_free(ast);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    printf("Success! Executable: %s\n", exe_file);

    /* Cleanup */
    ir_free(ir);
    ast_free(ast);
    lexer_free(lexer);
    free(source);

    return 0;
}

/**
 * Read Brainfuck source file into buffer
 *
 * Returns dynamically allocated string on success, NULL on error
 */
static char *read_file(const char *filename, int *length)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }

    /* Determine file size */
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Allocate buffer */
    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Out of memory\n");
        fclose(file);
        return NULL;
    }

    /* Read file */
    int read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) {
        fprintf(stderr, "Error: Failed to read file\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';
    *length = file_size;
    fclose(file);
    return buffer;
}

/**
 * Compile assembly file to executable using gcc
 */
static int compile_assembly(const char *asm_file, const char *output_file)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        /* Child process */
        execl("/usr/bin/gcc", "gcc", "-o", output_file, asm_file, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error: gcc command failed\n");
            return -1;
        }
    }
    
    return 0;
}

