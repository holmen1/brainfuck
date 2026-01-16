#include <stdio.h>
#include <stdlib.h>
#include "ir.h"

#define INITIAL_CAPACITY 128

/* ============================================
 * IR Program Management
 * ============================================ */

/**
 * Create empty IR program with initial capacity
 */
IRProgram *ir_create(void)
{
    IRProgram *program = malloc(sizeof(IRProgram));
    if (!program) {
        return NULL;
    }
    
    program->instructions = malloc(INITIAL_CAPACITY * sizeof(IRInstruction));
    if (!program->instructions) {
        free(program);
        return NULL;
    }
    
    program->count = 0;
    program->capacity = INITIAL_CAPACITY;
    
    return program;
}

/**
 * Append single instruction to IR program
 * Grows array if needed
 */
int ir_append(IRProgram *program, IROpcode opcode, int operand)
{
    if (!program) {
        return -1;
    }
    
    /* Grow array if at capacity */
    if (program->count >= program->capacity) {
        int new_capacity = program->capacity * 2;
        IRInstruction *new_instructions = realloc(
            program->instructions,
            new_capacity * sizeof(IRInstruction)
        );
        if (!new_instructions) {
            return -1;
        }
        program->instructions = new_instructions;
        program->capacity = new_capacity;
    }
    
    /* Add instruction */
    program->instructions[program->count].opcode = opcode;
    program->instructions[program->count].operand = operand;
    program->count++;
    
    return 0;
}

/**
 * Free IR program and all resources
 */
void ir_free(IRProgram *program)
{
    if (!program) {
        return;
    }
    
    free(program->instructions);
    free(program);
}

/**
 * Print IR program in human-readable format
 */
void ir_print(const IRProgram *program)
{
    if (!program) {
        printf("(null IR program)\n");
        return;
    }
    
    printf("IR Program (%d instructions):\n", program->count);
    printf("========================================\n");
    
    for (int i = 0; i < program->count; i++) {
        IRInstruction instr = program->instructions[i];
        printf("%4d: ", i);
        
        switch (instr.opcode) {
            case IR_ADD_PTR:
                printf("ADD_PTR %+d", instr.operand);
                break;
            case IR_ADD_CELL:
                printf("ADD_CELL %+d", instr.operand);
                break;
            case IR_OUTPUT:
                printf("OUTPUT");
                break;
            case IR_INPUT:
                printf("INPUT");
                break;
            case IR_LOOP_START:
                printf("LOOP_START (label %d)", instr.operand);
                break;
            case IR_LOOP_END:
                printf("LOOP_END (label %d)", instr.operand);
                break;
            case IR_SET_ZERO:
                printf("SET_ZERO");
                break;
            default:
                printf("UNKNOWN(%d)", instr.opcode);
                break;
        }
        printf("\n");
    }
    printf("========================================\n");
}

/* ============================================
 * AST to IR Conversion - Coming next!
 * ============================================ */

/**
 * Forward declaration of recursive helper
 */
static int ast_node_to_ir(ASTNode *node, IRProgram *program, int *next_label);

/**
 * Convert AST to IR instruction stream
 */
IRProgram *ir_from_ast(ASTNode *ast)
{
    if (!ast) {
        fprintf(stderr, "Error: NULL AST in ir_from_ast\n");
        return NULL;
    }
    
    IRProgram *program = ir_create();
    if (!program) {
        fprintf(stderr, "Error: Failed to create IR program\n");
        return NULL;
    }
    
    int next_label = 0;  /* Counter for loop labels */
    
    if (ast_node_to_ir(ast, program, &next_label) != 0) {
        fprintf(stderr, "Error: Failed to convert AST to IR\n");
        ir_free(program);
        return NULL;
    }
    
    return program;
}

/**
 * Recursive helper to convert AST nodes to IR instructions
 * Returns 0 on success, -1 on error
 */
static int ast_node_to_ir(ASTNode *node, IRProgram *program, int *next_label)
{
    if (!node) {
        return 0;  /* Empty nodes are OK */
    }
    
    switch (node->type) {
        case AST_SEQUENCE:
            /* Process all children in sequence */
            for (int i = 0; i < node->data.sequence.count; i++) {
                if (ast_node_to_ir(node->data.sequence.children[i], program, next_label) != 0) {
                    return -1;
                }
            }
            break;
            
        case AST_MOVE_RIGHT:
            if (ir_append(program, IR_ADD_PTR, 1) != 0) {
                return -1;
            }
            break;
            
        case AST_MOVE_LEFT:
            if (ir_append(program, IR_ADD_PTR, -1) != 0) {
                return -1;
            }
            break;
            
        case AST_INCREMENT:
            if (ir_append(program, IR_ADD_CELL, 1) != 0) {
                return -1;
            }
            break;
            
        case AST_DECREMENT:
            if (ir_append(program, IR_ADD_CELL, -1) != 0) {
                return -1;
            }
            break;
            
        case AST_OUTPUT:
            if (ir_append(program, IR_OUTPUT, 0) != 0) {
                return -1;
            }
            break;
            
        case AST_INPUT:
            if (ir_append(program, IR_INPUT, 0) != 0) {
                return -1;
            }
            break;
            
        case AST_LOOP: {
            /* Allocate a unique label ID for this loop */
            int label = (*next_label)++;
            
            /* Emit LOOP_START with label */
            if (ir_append(program, IR_LOOP_START, label) != 0) {
                return -1;
            }
            
            /* Recursively emit loop body */
            if (ast_node_to_ir(node->data.loop.body, program, next_label) != 0) {
                return -1;
            }
            
            /* Emit LOOP_END with same label */
            if (ir_append(program, IR_LOOP_END, label) != 0) {
                return -1;
            }
            break;
        }
        
        case AST_MOVE_PTR:
            /* Optimized: multiple moves combined */
            if (ir_append(program, IR_ADD_PTR, node->data.offset) != 0) {
                return -1;
            }
            break;
            
        case AST_MODIFY_CELL:
            /* Optimized: multiple +/- combined */
            if (ir_append(program, IR_ADD_CELL, node->data.delta) != 0) {
                return -1;
            }
            break;
            
        case AST_CLEAR_LOOP:
            /* Optimized: [-] or [+] becomes SET_ZERO */
            if (ir_append(program, IR_SET_ZERO, 0) != 0) {
                return -1;
            }
            break;
            
        default:
            fprintf(stderr, "Error: Unknown AST node type %d\n", node->type);
            return -1;
    }
    
    return 0;
}
