#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

/* TODO: Add includes for other compiler phases */
/* #include "ast.h" */
/* #include "parser.h" */
/* #include "ir.h" */
/* #include "codegen_llvm.h" */

/**
 * Read Brainfuck source file into buffer
 *
 * Returns dynamically allocated string on success, NULL on error
 */
char *read_file(const char *filename, int *length)
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
 * Print all tokens for debugging
 */
void print_tokens(Lexer *lexer)
{
    printf("Tokens:\n");
    Token token;
    do {
        token = lexer_next_token(lexer);
        printf("  [%d] ", token.position);
        
        switch (token.type) {
        case TOKEN_RIGHT:      printf("RIGHT (>)\n"); break;
        case TOKEN_LEFT:       printf("LEFT (<)\n"); break;
        case TOKEN_INC:        printf("INC (+)\n"); break;
        case TOKEN_DEC:        printf("DEC (-)\n"); break;
        case TOKEN_OUTPUT:     printf("OUTPUT (.)\n"); break;
        case TOKEN_INPUT:      printf("INPUT (,)\n"); break;
        case TOKEN_LOOP_START: printf("LOOP_START ([)\n"); break;
        case TOKEN_LOOP_END:   printf("LOOP_END (])\n"); break;
        case TOKEN_EOF:        printf("EOF\n"); break;
        default:               printf("UNKNOWN (%d)\n", token.type); break;
        }
    } while (token.type != TOKEN_EOF);
}

int main(int argc, char *argv[])
{
    /* Parse command line arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.bf> [options]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -o <file>      Output file (default: a.out)\n");
        fprintf(stderr, "  --emit-llvm    Emit LLVM IR instead of executable\n");
        fprintf(stderr, "  --emit-c       Emit C code instead of executable\n");
        fprintf(stderr, "  --print-tokens Print tokens and exit\n");
        fprintf(stderr, "  --print-ast    Print AST and exit\n");
        fprintf(stderr, "  --print-ir     Print IR and exit\n");
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = "a.out";
    int emit_tokens = 0;
    int emit_ast = 0;
    int emit_ir = 0;
    int emit_llvm = 0;
    int emit_c = 0;

    /* Parse remaining arguments */
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "--emit-llvm") == 0) {
            emit_llvm = 1;
        } else if (strcmp(argv[i], "--emit-c") == 0) {
            emit_c = 1;
        } else if (strcmp(argv[i], "--print-tokens") == 0) {
            emit_tokens = 1;
        } else if (strcmp(argv[i], "--print-ast") == 0) {
            emit_ast = 1;
        } else if (strcmp(argv[i], "--print-ir") == 0) {
            emit_ir = 1;
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            return 1;
        }
    }

    /* Phase 1: Read source file */
    printf("Reading source file: %s\n", input_file);
    int source_length;
    char *source = read_file(input_file, &source_length);
    if (!source) {
        return 1;
    }

    /* Phase 2: Lexer - Tokenization */
    printf("Lexing...\n");
    Lexer *lexer = lexer_create(source, source_length);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }

    if (emit_tokens) {
        print_tokens(lexer);
        lexer_free(lexer);
        free(source);
        return 0;
    }

    /* TODO: Phase 3: Parser - Build AST
     * 
     * ASTNode *ast = parser_parse(lexer);
     * if (!ast) {
     *     fprintf(stderr, "Error: Parse failed\n");
     *     lexer_free(lexer);
     *     free(source);
     *     return 1;
     * }
     */
    printf("Parsing...\n");
    fprintf(stderr, "TODO: Parser not implemented\n");
    
    /* TODO: if (emit_ast) {
     *     ast_print(ast);
     *     ast_free(ast);
     *     lexer_free(lexer);
     *     free(source);
     *     return 0;
     * }
     */

    /* TODO: Phase 4: IR Generation
     * 
     * IRProgram *ir = ir_from_ast(ast);
     * if (!ir) {
     *     fprintf(stderr, "Error: IR generation failed\n");
     *     ast_free(ast);
     *     lexer_free(lexer);
     *     free(source);
     *     return 1;
     * }
     */
    printf("Generating IR...\n");
    fprintf(stderr, "TODO: IR generation not implemented\n");

    /* TODO: if (emit_ir) {
     *     ir_print(ir);
     *     ir_free(ir);
     *     ast_free(ast);
     *     lexer_free(lexer);
     *     free(source);
     *     return 0;
     * }
     */

    /* TODO: Phase 5: Optimization (optional)
     * 
     * ir_optimize(ir);
     */
    printf("Optimizing IR...\n");
    fprintf(stderr, "TODO: Optimization not implemented\n");

    /* TODO: Phase 6: Code Generation
     *
     * Choose backend:
     * - LLVM IR (recommended)
     * - C code
     * - x86-64 assembly
     * - ARM64 assembly
     */
    printf("Generating code...\n");
    fprintf(stderr, "TODO: Code generation not implemented\n");

    /* TODO: if (emit_llvm) {
     *     FILE *out = fopen(output_file, "w");
     *     if (!out) {
     *         fprintf(stderr, "Error: Cannot open output file '%s'\n", output_file);
     *         ir_free(ir);
     *         ast_free(ast);
     *         lexer_free(lexer);
     *         free(source);
     *         return 1;
     *     }
     *     codegen_llvm(ir, out);
     *     fclose(out);
     *     printf("Generated LLVM IR: %s\n", output_file);
     *     
     *     // Compile with clang if not just emitting IR
     *     if (!emit_llvm) {
     *         char cmd[512];
     *         snprintf(cmd, sizeof(cmd), "clang -O2 %s -o %s", output_file, output_file);
     *         if (system(cmd) != 0) {
     *             fprintf(stderr, "Error: Failed to compile LLVM IR\n");
     *             return 1;
     *         }
     *     }
     * }
     */

    /* Cleanup */
    /* TODO: ir_free(ir); */
    /* TODO: ast_free(ast); */
    lexer_free(lexer);
    free(source);

    printf("Compilation successful!\n");
    return 0;
}
