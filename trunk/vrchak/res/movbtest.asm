
[map brief stdout]

START    equ 0xff00
BIOS_SEG equ 0xf000
RAM_SEG  equ 0x0000
STRLEN   equ 64

org START

_hello_str:
times STRLEN db 0x57

_main:
    mov ax, BIOS_SEG
    mov ds, ax
    mov dl, [_hello_str]
    mov dh, 0x58
    hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp BIOS_SEG:_main
finish:
nop

