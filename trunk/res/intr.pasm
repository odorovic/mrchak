# 1 "intrtest1.asm"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "intrtest1.asm"

# 1 "macros.inc" 1

MACRO FENTER
    push bp
    mov bp, sp
    push di
    push si
MEND

MACRO FEXIT
    pop si
    pop di
    pop bp
    ret
MEND

MACRO IENTER
    FENTER
    push bx
    push ax
    push cx
    push es
MEND

MACRO IEXIT
    pop es
    pop cx
    pop ax
    pop bx

    pop si
    pop di
    pop bp
    iret
MEND

MACRO PLOCK
    pushf
    cli
MEND

MACRO PUNLOCK
    popf
MEND

MACRO RPUSHA
    push bx
    push cx
    push dx
    push si
    push di
MEND

MACRO RPOPA
    pop di
    pop si
    pop dx
    pop cx
    pop bx
MEND

MACRO CALL_1_ARG
    push ?2
    call ?1
    inc sp
    inc sp
MEND

MACRO CALL_2_ARG
    push ?3
    push ?2
    call ?1
    add sp, #4
MEND

MACRO CALL_3_ARG
    push ?4
    push ?3
    push ?2
    call ?1
    add sp, #6
MEND

MACRO CALL_4_ARG
    push ?5
    push ?4
    push ?3
    push ?2
    call ?1
    add sp, #8
MEND

MACRO CALL_5_ARG
    push ?6
    push ?5
    push ?4
    push ?3
    push ?2
    call ?1
    add sp, #10
MEND

MACRO CALL_LIB_1_ARG
    RPUSHA
    push ?2
    call ?1
    inc sp
    inc sp
    RPOPA
MEND

MACRO CALL_LIB_2_ARG
    RPUSHA
    push ?3
    push ?2
    call ?1
    add sp, #4
    RPOPA
MEND

MACRO CALL_LIB_3_ARG
    RPUSHA
    push ?4
    push ?3
    push ?2
    call ?1
    add sp, #6
    RPOPA
MEND

MACRO CALL_LIB_4_ARG
    RPUSHA
    push ?5
    push ?4
    push ?3
    push ?2
    call ?1
    add sp, #8
    RPOPA
MEND
# 3 "intrtest1.asm" 2

start:



_intr_set_routine:



    FENTER
    pushf
    cli
    push ax
    push bx
    push es
    xor ax, ax
    mov es, ax
    mov bx, 4[bp]
    shl bx, #2
    mov ax, 6[bp]
    seg es
        mov [bx], ax
    mov ax, 8[bp]
    seg es
        mov 2[bx], ax
    pop es
    pop bx
    pop ax
    popf
    FEXIT




_service_intr:
    IENTER
    mov al, #88
    out #0x70, al
    IEXIT

_main:
    mov ax, #0x10
    mov bx, #_service_intr
    mov cx, #0xf000
    CALL_3_ARG(_intr_set_routine, ax, bx, cx)
    int #0x10
    hlt

org 0xfff0
jmpf #0xf000,_main
finish:
nop
