#include "codegen_asm.h"
#include <stdio.h>

#define BF_MEMORY_SIZE 30000

/**
 * Generate x86-64 assembly from IR program
 * 
 * Step 2: Add memory allocation and register setup
 * - Allocate 30000 bytes on stack for BF memory
 * - r12: base pointer to memory array
 * - r13: current cell offset (starts at 0)
 */
int codegen_asm(const IRProgram *program, FILE *output, int debug)
{
    /* Assembly prologue */
    fprintf(output, "    .text\n");
    fprintf(output, "    .globl main\n");
    fprintf(output, "main:\n");
    fprintf(output, "    # Prologue: set up stack frame\n");
    fprintf(output, "    push %%rbp\n");
    fprintf(output, "    mov %%rsp, %%rbp\n");
    fprintf(output, "    \n");
    
    /* Allocate BF memory on stack */
    if (debug) fprintf(output, "    # Allocate %d bytes for BF memory\n", BF_MEMORY_SIZE);
    fprintf(output, "    sub $%d, %%rsp\n", BF_MEMORY_SIZE);
    fprintf(output, "    \n");
    
    /* Set up registers */
    if (debug) {
        fprintf(output, "    # Initialize registers\n");
        fprintf(output, "    # r12 = base of memory array\n");
        fprintf(output, "    # r13 = 0 (cell offset)\n");
    }
    fprintf(output, "    mov %%rsp, %%r12\n");
    fprintf(output, "    xor %%r13, %%r13\n");
    fprintf(output, "    \n");
    
    /* Initialize memory to zero (optional but good practice) */
    if (debug) fprintf(output, "    # Clear memory to zero\n");
    fprintf(output, "    mov %%rsp, %%rdi\n");      /* rdi = destination (memory base) */
    fprintf(output, "    xor %%eax, %%eax\n");      /* eax = 0 (byte to fill) */
    fprintf(output, "    mov $%d, %%ecx\n", BF_MEMORY_SIZE);    /* ecx = count */
    fprintf(output, "    rep stosb\n");             /* Repeat: store al at [rdi], inc rdi */
    fprintf(output, "    \n");
    
    /* Reset registers after memset */
    fprintf(output, "    mov %%rbp, %%rax\n");
    fprintf(output, "    sub $%d, %%rax\n", BF_MEMORY_SIZE);
    fprintf(output, "    mov %%rax, %%r12\n");
    fprintf(output, "    xor %%r13, %%r13\n");
    fprintf(output, "    \n");
    
    /* Emit IR instructions */
    fprintf(output, "bf_program:\n");
    for (int i = 0; i < program->count; i++) {
        IRInstruction instr = program->instructions[i];
        
        switch (instr.opcode) {
            case IR_ADD_PTR:
                /* Move pointer: ptr += operand */
                if (debug) fprintf(output, "    # IR_ADD_PTR %+d\n", instr.operand);
                if (instr.operand > 0) {
                    fprintf(output, "    add $%d, %%r13\n", instr.operand);
                } else if (instr.operand < 0) {
                    fprintf(output, "    sub $%d, %%r13\n", -instr.operand);
                }
                break;
                
            case IR_ADD_CELL:
                /* Modify cell: *ptr += operand */
                if (debug) fprintf(output, "    # IR_ADD_CELL %+d\n", instr.operand);
                /* Add/subtract the operand */
                if (instr.operand > 0) {
                    fprintf(output, "    addb $%d, (%%r12,%%r13)\n", instr.operand);
                } else if (instr.operand < 0) {
                    fprintf(output, "    subb $%d, (%%r12,%%r13)\n", -instr.operand);
                }
                break;
                
            case IR_OUTPUT:
                /* Output: putchar(*ptr) */
                if (debug) fprintf(output, "    # IR_OUTPUT\n");
                fprintf(output, "    movzbl (%%r12,%%r13), %%edi\n");
                fprintf(output, "    call putchar\n");
                fprintf(output, "    \n");
                break;
                
            case IR_INPUT:
                /* Input: *ptr = getchar() */
                if (debug) fprintf(output, "    # IR_INPUT\n");
                fprintf(output, "    call getchar\n");
                fprintf(output, "    movb %%al, (%%r12,%%r13)\n");
                break;
                
            case IR_SET_ZERO:
                /* Optimized clear: *ptr = 0 */
                if (debug) fprintf(output, "    # IR_SET_ZERO\n");
                fprintf(output, "    movb $0, (%%r12,%%r13)\n");
                break;
                
            case IR_LOOP_START:
                /* Loop start: if (*ptr == 0) jump to matching end */
                if (debug) fprintf(output, "    # IR_LOOP_START (label %d)\n", instr.operand);
                fprintf(output, "loop_%d_start:\n", instr.operand);
                fprintf(output, "    movzbl (%%r12,%%r13), %%eax\n");
                fprintf(output, "    test %%eax, %%eax\n");
                fprintf(output, "    jz loop_%d_end\n", instr.operand);
                fprintf(output, "    \n");
                break;
                
            case IR_LOOP_END:
                /* Loop end: if (*ptr != 0) jump back to matching start */
                if (debug) fprintf(output, "    # IR_LOOP_END (label %d)\n", instr.operand);
                fprintf(output, "    movzbl (%%r12,%%r13), %%eax\n");
                fprintf(output, "    test %%eax, %%eax\n");
                fprintf(output, "    jnz loop_%d_start\n", instr.operand);
                fprintf(output, "loop_%d_end:\n", instr.operand);
                fprintf(output, "    \n");
                break;
                
            default:
                /* Not yet implemented */
                if (debug) fprintf(output, "    # TODO: Opcode %d not implemented\n", instr.opcode);
                break;
        }
    }
    
    /* Epilogue: clean up and return */
    fprintf(output, "epilogue:\n");
    if (debug) fprintf(output, "    # restore stack and return 0\n");
    fprintf(output, "    mov %%rbp, %%rsp\n");
    fprintf(output, "    pop %%rbp\n");
    fprintf(output, "    xor %%eax, %%eax\n");
    fprintf(output, "    ret\n");
    
    return 0;
}
