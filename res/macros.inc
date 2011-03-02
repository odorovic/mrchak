
%macro FENTER 0
    push bp
    mov bp, sp
    push di
    push si
%endmacro

%macro FEXIT 0
    pop si
    pop di
    pop bp
    ret
%endmacro

%macro IENTER 0
    FENTER
    push bx
    push ax
    push cx
    push es
%endmacro

%macro IEXIT 0
    pop es
    pop cx
    pop ax
    pop bx

    pop si
    pop di
    pop bp
    iret
%endmacro

%macro PLOCK 0
    pushf
    cli
%endmacro

%macro PUNLOCK 0
    popf
%endmacro

%macro RPUSHA 0
    push bx
    push cx
    push dx
    push si
    push di
%endmacro

%macro RPOPA 0
    pop  di
    pop  si
    pop  dx
    pop  cx
    pop  bx
%endmacro

%macro CALL_1_ARG 2
    push ?2
    call ?1
    inc  sp
    inc  sp
%endmacro

%macro CALL_2_ARG 3
    push ?3
    push ?2
    call ?1
    add  sp, 4
%endmacro

%macro CALL_3_ARG 4
    push ?4
    push ?3
    push ?2
    call ?1
    add  sp, 6
%endmacro

%macro CALL_4_ARG 5
    push ?5
    push ?4
    push ?3
    push ?2
    call ?1
    add  sp, 8
%endmacro

%macro CALL_5_ARG 6
    push ?6
    push ?5
    push ?4
    push ?3
    push ?2
    call ?1
    add  sp, 10
%endmacro

%macro CALL_LIB_1_ARG 2
    RPUSHA
    push ?2
    call ?1
    inc  sp
    inc  sp
    RPOPA
%endmacro

%macro CALL_LIB_2_ARG 3
    RPUSHA
    push ?3
    push ?2
    call ?1
    add  sp, 4
    RPOPA
%endmacro

%macro CALL_LIB_3_ARG 4
    RPUSHA
    push ?4
    push ?3
    push ?2
    call ?1
    add  sp, 6
    RPOPA
%endmacro

%macro CALL_LIB_4_ARG 5
    RPUSHA
    push ?5
    push ?4
    push ?3
    push ?2
    call ?1
    add  sp, 8
    RPOPA
%endmacro

