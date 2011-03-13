[map brief stdout]

START    equ 0xff00
BIOS_SEG equ 0xf000
RAM_SEG  equ 0x0000
STRLEN   equ 8

org START

_hello_str:
times STRLEN db 'X'

_error:
    into ; implemented in tested model as a breakpoint.
    hlt
_main:
    mov si, _hello_str
    mov di, _hello_str
    mov ax, BIOS_SEG
    mov ds, ax
    mov ax, RAM_SEG
    mov es, ax
    mov cx, STRLEN
    rep movsb
    ;Verify stuff is properly copied. 
    mov ax, RAM_SEG
    mov dx, ax
    mov cx, STRLEN
    mov dl, 0x58
    mov si, _hello_str
_loop:
    mov dh, [si]
    cmp dl, dh
    jne _error
    inc si
    dec cx
    cmp cx, 0
    jne _loop
    hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp BIOS_SEG:_main
finish:
nop

