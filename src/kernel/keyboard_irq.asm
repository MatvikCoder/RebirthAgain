section .text
global keyboard_irq
extern keyboard_irq_handler

keyboard_irq:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call keyboard_irq_handler

    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
