    .text
    .globl main
main:
    # Prologue: set up stack frame
    push %rbp
    mov %rsp, %rbp
    
    sub $30000, %rsp
    
    mov %rsp, %r12
    xor %r13, %r13
    
    mov %rsp, %rdi
    xor %eax, %eax
    mov $30000, %ecx
    rep stosb
    
    mov %rbp, %rax
    sub $30000, %rax
    mov %rax, %r12
    xor %r13, %r13
    
bf_program:
    call getchar
    movb %al, (%r12,%r13)
    add $1, %r13
    call getchar
    movb %al, (%r12,%r13)
    sub $1, %r13
loop_0_start:
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jz loop_0_end
    
    subb $1, (%r12,%r13)
    add $1, %r13
    addb $1, (%r12,%r13)
    sub $1, %r13
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jnz loop_0_start
loop_0_end:
    
    add $2, %r13
    addb $6, (%r12,%r13)
loop_1_start:
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jz loop_1_end
    
    subb $1, (%r12,%r13)
    add $1, %r13
    addb $8, (%r12,%r13)
    sub $1, %r13
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jnz loop_1_start
loop_1_end:
    
    add $1, %r13
loop_2_start:
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jz loop_2_end
    
    subb $1, (%r12,%r13)
    sub $2, %r13
    subb $1, (%r12,%r13)
    add $2, %r13
    movzbl (%r12,%r13), %eax
    test %eax, %eax
    jnz loop_2_start
loop_2_end:
    
    sub $2, %r13
    movzbl (%r12,%r13), %edi
    call putchar
    
epilogue:
    mov %rbp, %rsp
    pop %rbp
    xor %eax, %eax
    ret
