#include "codegen_arm64.h"
#include <stdio.h>

#define BF_MEMORY_SIZE 30000

/**
 * Generate AArch64 assembly from IR program
 * 
 * Step 2: Add memory allocation and register setup
 * - Allocate 30000 bytes on stack for BF memory
 * - x19: base pointer to memory array
 * - x20: current cell offset (starts at 0)
 */
int codegen_arm64(const IRProgram *program, FILE *output, int debug)
{
    /* Assembly prologue */
    fprintf(output, "    .text\n");
    fprintf(output, "    .globl main\n");
    fprintf(output, "    .align 2\n");
    fprintf(output, "main:\n");
    fprintf(output, "    // Prologue: set up stack frame\n");
    fprintf(output, "    stp x29, x30, [sp, #-16]!\n");
    fprintf(output, "    mov x29, sp\n");
    fprintf(output, "    stp x19, x20, [sp, #-16]!\n");
    fprintf(output, "    \n");
    
    /* Allocate BF memory on stack */
    if (debug) fprintf(output, "    // Allocate %d bytes for BF memory\n", BF_MEMORY_SIZE);
    fprintf(output, "    sub sp, sp, #%d\n", BF_MEMORY_SIZE);
    fprintf(output, "    \n");
    
    /* Set up registers */
    if (debug) {
        fprintf(output, "    // Initialize registers\n");
        fprintf(output, "    // x19 = base of memory array\n");
        fprintf(output, "    // x20 = 0 (cell offset)\n");
    }
    fprintf(output, "    mov x19, sp\n");
    fprintf(output, "    mov x20, #0\n");
    fprintf(output, "    \n");
    
    /* Initialize memory to zero (optional but good practice) */
    if (debug) fprintf(output, "    // Clear memory to zero\n");
    fprintf(output, "    mov x0, sp\n");            /* x0 = destination (memory base) */
    fprintf(output, "    mov x1, #0\n");            /* x1 = 0 (byte to fill) */
    fprintf(output, "    mov x2, #%d\n", BF_MEMORY_SIZE);     /* x2 = count */
    fprintf(output, "    bl memset\n");             /* Call memset (x19, x20 preserved) */
    fprintf(output, "    \n");
    
    /* Emit IR instructions */
    fprintf(output, "bf_program:\n");
    for (int i = 0; i < program->count; i++) {
        IRInstruction instr = program->instructions[i];
        
        switch (instr.opcode) {
            case IR_ADD_PTR:
                /* Move pointer: ptr += operand */
                if (debug) fprintf(output, "    // IR_ADD_PTR %+d\n", instr.operand);
                if (instr.operand > 0) {
                    fprintf(output, "    add x20, x20, #%d\n", instr.operand);
                } else if (instr.operand < 0) {
                    fprintf(output, "    sub x20, x20, #%d\n", -instr.operand);
                }
                break;
                
            case IR_ADD_CELL:
                /* Modify cell: *ptr += operand */
                if (debug) fprintf(output, "    // IR_ADD_CELL %+d\n", instr.operand);
                /* Add/subtract the operand */
                fprintf(output, "    ldrb w0, [x19, x20]\n");
                if (instr.operand > 0) {
                    fprintf(output, "    add w0, w0, #%d\n", instr.operand);
                } else if (instr.operand < 0) {
                    fprintf(output, "    sub w0, w0, #%d\n", -instr.operand);
                }
                fprintf(output, "    strb w0, [x19, x20]\n");
                break;
                
            case IR_OUTPUT:
                /* Output: putchar(*ptr) */
                if (debug) fprintf(output, "    // IR_OUTPUT\n");
                fprintf(output, "    ldrb w0, [x19, x20]\n");
                fprintf(output, "    bl putchar\n");
                fprintf(output, "    \n");
                break;
                
            case IR_INPUT:
                /* Input: *ptr = getchar() */
                if (debug) fprintf(output, "    // IR_INPUT\n");
                fprintf(output, "    bl getchar\n");
                fprintf(output, "    strb w0, [x19, x20]\n");
                break;
                
            case IR_SET_ZERO:
                /* Optimized clear: *ptr = 0 */
                if (debug) fprintf(output, "    // IR_SET_ZERO\n");
                fprintf(output, "    strb wzr, [x19, x20]\n");
                break;
                
            case IR_LOOP_START:
                /* Loop start: if (*ptr == 0) jump to matching end */
                if (debug) fprintf(output, "    // IR_LOOP_START (label %d)\n", instr.operand);
                fprintf(output, "loop_%d_start:\n", instr.operand);
                fprintf(output, "    ldrb w0, [x19, x20]\n");
                fprintf(output, "    cbz w0, loop_%d_end\n", instr.operand);
                fprintf(output, "    \n");
                break;
                
            case IR_LOOP_END:
                /* Loop end: if (*ptr != 0) jump back to matching start */
                if (debug) fprintf(output, "    // IR_LOOP_END (label %d)\n", instr.operand);
                fprintf(output, "    ldrb w0, [x19, x20]\n");
                fprintf(output, "    cbnz w0, loop_%d_start\n", instr.operand);
                fprintf(output, "loop_%d_end:\n", instr.operand);
                fprintf(output, "    \n");
                break;
                
            default:
                /* Not yet implemented */
                if (debug) fprintf(output, "    // TODO: Opcode %d not implemented\n", instr.opcode);
                break;
        }
    }
    
    /* Epilogue: clean up and return */
    fprintf(output, "epilogue:\n");
    if (debug) fprintf(output, "    // restore stack and return 0\n");
    fprintf(output, "    add sp, sp, #%d\n", BF_MEMORY_SIZE);
    fprintf(output, "    ldp x19, x20, [sp], #16\n");
    fprintf(output, "    ldp x29, x30, [sp], #16\n");
    fprintf(output, "    mov w0, #0\n");
    fprintf(output, "    ret\n");
    
    return 0;
}
