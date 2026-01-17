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
int codegen_asm(const IRProgram *program, FILE *output)
{
    /* Suppress unused parameter warning for now */
    (void)program;
    
    /* Assembly prologue */
    fprintf(output, "    .text\n");
    fprintf(output, "    .globl main\n");
    fprintf(output, "main:\n");
    fprintf(output, "    # Prologue: set up stack frame\n");
    fprintf(output, "    push %%rbp\n");
    fprintf(output, "    mov %%rsp, %%rbp\n");
    fprintf(output, "    \n");
    
    /* Allocate BF memory on stack */
    fprintf(output, "    # Allocate %d bytes for BF memory\n", BF_MEMORY_SIZE);
    fprintf(output, "    sub $%d, %%rsp\n", BF_MEMORY_SIZE);
    fprintf(output, "    \n");
    
    /* Set up registers */
    fprintf(output, "    # Initialize registers\n");
    fprintf(output, "    mov %%rsp, %%r12     # r12 = base of memory array\n");
    fprintf(output, "    xor %%r13, %%r13     # r13 = 0 (cell offset)\n");
    fprintf(output, "    \n");
    
    /* Initialize memory to zero (optional but good practice) */
    fprintf(output, "    # Clear memory to zero\n");
    fprintf(output, "    mov %%rsp, %%rdi     # rdi = destination (memory base)\n");
    fprintf(output, "    xor %%eax, %%eax     # eax = 0 (byte to fill)\n");
    fprintf(output, "    mov $%d, %%ecx       # ecx = count\n", BF_MEMORY_SIZE);
    fprintf(output, "    rep stosb            # Repeat: store al at [rdi], inc rdi\n");
    fprintf(output, "    \n");
    
    /* Reset registers after memset */
    fprintf(output, "    mov %%rbp, %%rax\n");
    fprintf(output, "    sub $%d, %%rax\n", BF_MEMORY_SIZE);
    fprintf(output, "    mov %%rax, %%r12     # r12 = base of memory array\n");
    fprintf(output, "    xor %%r13, %%r13     # r13 = 0 (cell offset)\n");
    fprintf(output, "    \n");
    
    /* Emit IR instructions */
    fprintf(output, "    # Brainfuck program instructions\n");
    for (int i = 0; i < program->count; i++) {
        IRInstruction instr = program->instructions[i];
        
        switch (instr.opcode) {
            case IR_ADD_PTR:
                /* Move pointer: ptr += operand */
                fprintf(output, "    # IR_ADD_PTR %+d\n", instr.operand);
                if (instr.operand > 0) {
                    fprintf(output, "    add $%d, %%r13\n", instr.operand);
                } else if (instr.operand < 0) {
                    fprintf(output, "    sub $%d, %%r13\n", -instr.operand);
                }
                fprintf(output, "    \n");
                break;
                
            case IR_ADD_CELL:
                /* Modify cell: *ptr += operand */
                fprintf(output, "    # IR_ADD_CELL %+d\n", instr.operand);
                /* Load current cell value */
                fprintf(output, "    movzbl (%%r12,%%r13), %%eax  # Load byte from mem[r12+r13] into eax\n");
                /* Add/subtract the operand */
                if (instr.operand > 0) {
                    fprintf(output, "    add $%d, %%eax\n", instr.operand);
                } else if (instr.operand < 0) {
                    fprintf(output, "    sub $%d, %%eax\n", -instr.operand);
                }
                /* Store back (only low byte) */
                fprintf(output, "    movb %%al, (%%r12,%%r13)    # Store byte back to mem[r12+r13]\n");
                fprintf(output, "    \n");
                break;
                
            case IR_OUTPUT:
                /* Output: putchar(*ptr) */
                fprintf(output, "    # IR_OUTPUT\n");
                fprintf(output, "    movzbl (%%r12,%%r13), %%edi  # Load byte into first argument\n");
                fprintf(output, "    call putchar                 # Print character\n");
                fprintf(output, "    \n");
                break;
                
            case IR_LOOP_START:
                /* Loop start: if (*ptr == 0) jump to matching end */
                fprintf(output, "    # IR_LOOP_START (label %d)\n", instr.operand);
                fprintf(output, "loop_%d_start:\n", instr.operand);
                fprintf(output, "    movzbl (%%r12,%%r13), %%eax  # Load current cell\n");
                fprintf(output, "    test %%eax, %%eax            # Test if zero\n");
                fprintf(output, "    jz loop_%d_end               # Jump to end if zero\n", instr.operand);
                fprintf(output, "    \n");
                break;
                
            case IR_LOOP_END:
                /* Loop end: if (*ptr != 0) jump back to matching start */
                fprintf(output, "    # IR_LOOP_END (label %d)\n", instr.operand);
                fprintf(output, "    movzbl (%%r12,%%r13), %%eax  # Load current cell\n");
                fprintf(output, "    test %%eax, %%eax            # Test if zero\n");
                fprintf(output, "    jnz loop_%d_start            # Jump to start if non-zero\n", instr.operand);
                fprintf(output, "loop_%d_end:\n", instr.operand);
                fprintf(output, "    \n");
                break;
                
            default:
                /* Not yet implemented */
                fprintf(output, "    # TODO: Opcode %d not implemented\n", instr.opcode);
                break;
        }
    }
    
    /* Epilogue: clean up and return */
    fprintf(output, "    # Epilogue: restore stack and return\n");
    fprintf(output, "    mov %%rbp, %%rsp     # Restore stack pointer\n");
    fprintf(output, "    pop %%rbp\n");
    fprintf(output, "    xor %%eax, %%eax     # Return 0\n");
    fprintf(output, "    ret\n");
    
    return 0;
}