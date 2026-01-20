    .text
    .globl main
main:
    # Prologue: set up stack frame
    push %rbp
    mov %rsp, %rbp
    
    # Allocate 30000 bytes for BF memory
    sub $30000, %rsp
    
    # Initialize registers
    # r12 = base of memory array
    # r13 = 0 (cell offset)
    mov %rsp, %r12
    xor %r13, %r13
    
    # Clear memory to zero
    mov %rsp, %rdi
    xor %eax, %eax
    mov $30000, %ecx
    rep stosb
    
    mov %rbp, %rax
    sub $30000, %rax
    mov %rax, %r12
    xor %r13, %r13
    
bf_program:
    # IR_ADD_CELL +8
    addb $8, (%r12,%r13)
    # IR_LOOP_START (label 0)
loop_0_start:
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jz loop_0_end
    
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +4
    addb $4, (%r12,%r13)
    # IR_LOOP_START (label 1)
loop_1_start:
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jz loop_1_end
    
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +2
    addb $2, (%r12,%r13)
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +3
    addb $3, (%r12,%r13)
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +3
    addb $3, (%r12,%r13)
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +1
    addb $1, (%r12,%r13)
    # IR_ADD_PTR -4
    sub $4, %r13
    # IR_ADD_CELL -1
    subb $1, (%r12,%r13)
    # IR_LOOP_END (label 1)
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jnz loop_1_start
loop_1_end:
    
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +1
    addb $1, (%r12,%r13)
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +1
    addb $1, (%r12,%r13)
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL -1
    subb $1, (%r12,%r13)
    # IR_ADD_PTR +2
    add $2, %r13
    # IR_ADD_CELL +1
    addb $1, (%r12,%r13)
    # IR_LOOP_START (label 2)
loop_2_start:
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jz loop_2_end
    
    # IR_ADD_PTR -1
    sub $1, %r13
    # IR_LOOP_END (label 2)
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jnz loop_2_start
loop_2_end:
    
    # IR_ADD_PTR -1
    sub $1, %r13
    # IR_ADD_CELL -1
    subb $1, (%r12,%r13)
    # IR_LOOP_END (label 0)
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jnz loop_0_start
loop_0_end:
    
    # IR_ADD_PTR +2
    add $2, %r13
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL -3
    subb $3, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_CELL +7
    addb $7, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_CELL +3
    addb $3, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_PTR +2
    add $2, %r13
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_PTR -1
    sub $1, %r13
    # IR_ADD_CELL -1
    subb $1, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_PTR -1
    sub $1, %r13
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_CELL +3
    addb $3, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_CELL -6
    subb $6, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_CELL -8
    subb $8, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_PTR +2
    add $2, %r13
    # IR_ADD_CELL +1
    addb $1, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
    # IR_ADD_PTR +1
    add $1, %r13
    # IR_ADD_CELL +2
    addb $2, (%r12,%r13)
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi
    call putchar
    
epilogue:
    # restore stack and return 0
    mov %rbp, %rsp
    pop %rbp
    xor %eax, %eax
    ret
