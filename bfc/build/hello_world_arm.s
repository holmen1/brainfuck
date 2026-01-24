    .text
    .globl main
    .align 2
main:
    // Prologue: set up stack frame
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    stp x19, x20, [sp, #-16]!
    
    sub sp, sp, #30000
    
    mov x19, sp
    mov x20, #0
    
    mov x0, sp
    mov x1, #0
    mov x2, #30000
    bl memset
    
bf_program:
    ldrb w0, [x19, x20]
    add w0, w0, #8
    strb w0, [x19, x20]
loop_0_start:
    ldrb w0, [x19, x20]
    cbz w0, loop_0_end
    
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #4
    strb w0, [x19, x20]
loop_1_start:
    ldrb w0, [x19, x20]
    cbz w0, loop_1_end
    
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #2
    strb w0, [x19, x20]
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #3
    strb w0, [x19, x20]
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #3
    strb w0, [x19, x20]
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #1
    strb w0, [x19, x20]
    sub x20, x20, #4
    ldrb w0, [x19, x20]
    sub w0, w0, #1
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    cbnz w0, loop_1_start
loop_1_end:
    
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #1
    strb w0, [x19, x20]
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #1
    strb w0, [x19, x20]
    add x20, x20, #1
    ldrb w0, [x19, x20]
    sub w0, w0, #1
    strb w0, [x19, x20]
    add x20, x20, #2
    ldrb w0, [x19, x20]
    add w0, w0, #1
    strb w0, [x19, x20]
loop_2_start:
    ldrb w0, [x19, x20]
    cbz w0, loop_2_end
    
    sub x20, x20, #1
    ldrb w0, [x19, x20]
    cbnz w0, loop_2_start
loop_2_end:
    
    sub x20, x20, #1
    ldrb w0, [x19, x20]
    sub w0, w0, #1
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    cbnz w0, loop_0_start
loop_0_end:
    
    add x20, x20, #2
    ldrb w0, [x19, x20]
    bl putchar
    
    add x20, x20, #1
    ldrb w0, [x19, x20]
    sub w0, w0, #3
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    ldrb w0, [x19, x20]
    add w0, w0, #7
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    ldrb w0, [x19, x20]
    bl putchar
    
    ldrb w0, [x19, x20]
    add w0, w0, #3
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    add x20, x20, #2
    ldrb w0, [x19, x20]
    bl putchar
    
    sub x20, x20, #1
    ldrb w0, [x19, x20]
    sub w0, w0, #1
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    sub x20, x20, #1
    ldrb w0, [x19, x20]
    bl putchar
    
    ldrb w0, [x19, x20]
    add w0, w0, #3
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    ldrb w0, [x19, x20]
    sub w0, w0, #6
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    ldrb w0, [x19, x20]
    sub w0, w0, #8
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    add x20, x20, #2
    ldrb w0, [x19, x20]
    add w0, w0, #1
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
    add x20, x20, #1
    ldrb w0, [x19, x20]
    add w0, w0, #2
    strb w0, [x19, x20]
    ldrb w0, [x19, x20]
    bl putchar
    
epilogue:
    add sp, sp, #30000
    ldp x19, x20, [sp], #16
    ldp x29, x30, [sp], #16
    mov w0, #0
    ret
