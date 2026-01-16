#include "codegen_asm.h"
#include <stdio.h>

/**
 * Generate x86-64 assembly from IR program
 * 
 * For now: Just emit a minimal "hello" that prints 'H'
 * This proves our assembly generation and build chain works!
 */
int codegen_asm(const IRProgram *program, FILE *output)
{
    /* Suppress unused parameter warning for now */
    (void)program;
    
    /* Emit minimal x86-64 assembly */
    fprintf(output, "    .text\n");
    fprintf(output, "    .globl main\n");
    fprintf(output, "main:\n");
    fprintf(output, "    # Minimal test: print 'H' (ASCII 72)\n");
    fprintf(output, "    mov $72, %%edi      # Load 'H' into first arg\n");
    fprintf(output, "    call putchar        # Call putchar('H')\n");
    fprintf(output, "    \n");
    fprintf(output, "    # Return 0\n");
    fprintf(output, "    xor %%eax, %%eax    # eax = 0 (return value)\n");
    fprintf(output, "    ret\n");
    
    return 0;
}