#ifndef CODEGEN_ARM64_H
#define CODEGEN_ARM64_H

#include "ir.h"
#include <stdio.h>

/**
 * Generate AArch64 assembly from IR program
 * 
 * Outputs to the given file pointer (typically a .s file)
 * Returns 0 on success, -1 on error
 */
int codegen_arm64(const IRProgram *program, FILE *output, int debug);

#endif /* CODEGEN_ARM64_H */
