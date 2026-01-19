#ifndef CODEGEN_ASM_H
#define CODEGEN_ASM_H

#include "ir.h"
#include <stdio.h>

/**
 * Generate x86-64 assembly from IR program
 * 
 * Outputs to the given file pointer (typically a .s file)
 * Returns 0 on success, -1 on error
 */
int codegen_asm(const IRProgram *program, FILE *output, int debug);

#endif /* CODEGEN_ASM_H */
