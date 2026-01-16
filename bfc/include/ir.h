#ifndef IR_H
#define IR_H

#include "ast.h"

/*
 * Intermediate Representation (IR) for Brainfuck
 * 
 * The IR sits between the AST and code generation. It provides:
 * 1. A linear instruction stream (easier to emit code from than tree)
 * 2. A platform-independent representation
 * 3. Opportunities for additional optimizations
 * 
 * Design: Keep it simple and close to BF semantics
 */

/* IR Instruction Opcodes */
typedef enum {
    /* Basic operations */
    IR_ADD_PTR,       /* Add to data pointer: ptr += operand */
    IR_ADD_CELL,      /* Add to current cell: *ptr += operand */
    IR_OUTPUT,        /* Output current cell: putchar(*ptr) */
    IR_INPUT,         /* Input to current cell: *ptr = getchar() */
    
    /* Control flow */
    IR_LOOP_START,    /* Begin loop: if (*ptr == 0) jump to matching LOOP_END */
    IR_LOOP_END,      /* End loop: if (*ptr != 0) jump back to matching LOOP_START */
    
    /* Optimized operations (future) */
    IR_SET_ZERO,      /* Set current cell to 0: *ptr = 0 (optimized [-] pattern) */
} IROpcode;

/* Single IR Instruction */
typedef struct {
    IROpcode opcode;
    int operand;      /* Used by ADD_PTR and ADD_CELL, label ID for loops */
} IRInstruction;

/* Complete IR Program - linear instruction array */
typedef struct {
    IRInstruction *instructions;  /* Dynamic array of instructions */
    int count;                     /* Number of instructions */
    int capacity;                  /* Allocated capacity */
} IRProgram;

/* ============================================
 * Public API Functions
 * ============================================ */

/**
 * Create empty IR program with initial capacity
 */
IRProgram *ir_create(void);

/**
 * Convert AST to IR instruction stream
 * Returns NULL on error
 */
IRProgram *ir_from_ast(ASTNode *ast);

/**
 * Append single instruction to IR program
 * Returns 0 on success, -1 on error
 */
int ir_append(IRProgram *program, IROpcode opcode, int operand);

/**
 * Print IR program in human-readable format
 */
void ir_print(const IRProgram *program);

/**
 * Free IR program and all resources
 */
void ir_free(IRProgram *program);

#endif /* IR_H */
