
.text; .align 4; .globl SupportsTsx; 
SupportsTsx:
    push %ebp
    mov  %esp, %ebp
    

    .balign 1 ; .byte 0xc7
    .balign 1 ; .byte 0x58
    .balign 1 ; .byte 0x02
    .balign 1 ; .byte 0x00
    .balign 1 ; .byte 0x00
    .balign 1 ; .byte 0x00
    jmp successLabel
abortLabel:
    mov $0, %eax
    jmp returnLabel
successLabel:
    mov $1, %eax
    .balign 1 ; .byte 0x0f
    .balign 1 ; .byte 0x01
    .balign 1 ; .byte 0xd5
returnLabel:
    mov %ebp, %esp
    pop %ebp
    ret
