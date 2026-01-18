    .text
    .globl main
main:
    # Prologue: set up stack frame
    push %rbp
    mov %rsp, %rbp
    
    # Allocate 30000 bytes for BF memory
    sub $30000, %rsp
    
    # Initialize registers
    mov %rsp, %r12     # r12 = base of memory array
    xor %r13, %r13     # r13 = 0 (cell offset)
    
    # Clear memory to zero
    mov %rsp, %rdi     # rdi = destination (memory base)
    xor %eax, %eax     # eax = 0 (byte to fill)
    mov $30000, %ecx       # ecx = count
    rep stosb            # Repeat: store al at [rdi], inc rdi
    
    mov %rbp, %rax
    sub $30000, %rax
    mov %rax, %r12     # r12 = base of memory array
    xor %r13, %r13     # r13 = 0 (cell offset)
    
    # Brainfuck program instructions
    # IR_ADD_CELL +8
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $8, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_LOOP_START (label 0)
loop_0_start:
    movzbl (%r12,%r13), %eax  # Load current cell
    test %eax, %eax            # Test if zero
    jz loop_0_end               # Jump to end if zero
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +4
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $4, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_LOOP_START (label 1)
loop_1_start:
    movzbl (%r12,%r13), %eax  # Load current cell
    test %eax, %eax            # Test if zero
    jz loop_1_end               # Jump to end if zero
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +2
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $2, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +3
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $3, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +3
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $3, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR -4
    sub $4, %r13
    
    # IR_ADD_CELL -1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_LOOP_END (label 1)
    movzbl (%r12,%r13), %eax  # Load current cell
    test %eax, %eax            # Test if zero
    jnz loop_1_start            # Jump to start if non-zero
loop_1_end:
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL -1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_ADD_PTR +2
    add $2, %r13
    
    # IR_ADD_CELL +1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_LOOP_START (label 2)
loop_2_start:
    movzbl (%r12,%r13), %eax  # Load current cell
    test %eax, %eax            # Test if zero
    jz loop_2_end               # Jump to end if zero
    
    # IR_ADD_PTR -1
    sub $1, %r13
    
    # IR_LOOP_END (label 2)
    movzbl (%r12,%r13), %eax  # Load current cell
    test %eax, %eax            # Test if zero
    jnz loop_2_start            # Jump to start if non-zero
loop_2_end:
    
    # IR_ADD_PTR -1
    sub $1, %r13
    
    # IR_ADD_CELL -1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_LOOP_END (label 0)
    movzbl (%r12,%r13), %eax  # Load current cell
    test %eax, %eax            # Test if zero
    jnz loop_0_start            # Jump to start if non-zero
loop_0_end:
    
    # IR_ADD_PTR +2
    add $2, %r13
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL -3
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $3, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_CELL +7
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $7, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_CELL +3
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $3, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_PTR +2
    add $2, %r13
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_PTR -1
    sub $1, %r13
    
    # IR_ADD_CELL -1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_PTR -1
    sub $1, %r13
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_CELL +3
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $3, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_CELL -6
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $6, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_CELL -8
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    sub $8, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_PTR +2
    add $2, %r13
    
    # IR_ADD_CELL +1
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $1, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # IR_ADD_PTR +1
    add $1, %r13
    
    # IR_ADD_CELL +2
    movzbl (%r12,%r13), %eax  # Load byte from mem[r12+r13] into eax
    add $2, %eax
    movb %al, (%r12,%r13)    # Store byte back to mem[r12+r13]
    
    # IR_OUTPUT
    movzbl (%r12,%r13), %edi  # Load byte into first argument
    call putchar                 # Print character
    
    # Epilogue: restore stack and return
    mov %rbp, %rsp     # Restore stack pointer
    pop %rbp
    xor %eax, %eax     # Return 0
    ret
