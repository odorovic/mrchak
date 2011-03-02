[map brief stdout]

START    equ 0xff00
BIOS_SEG equ 0xf000
org START

_hello_str:
db 'Hello world!', 0

_main:
    mov bx, _hello_str
    mov di, 0
_print_loop:
    mov al, [cs:bx+di]
    cmp al, 0
    je _end_loop
    out 0x70, al
    inc di
    jmp _print_loop
_end_loop:
    hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp word BIOS_SEG:_main
finish:
nop

