[map brief stdout]

START equ 0xff00
BIOS_SEG equ 0xf000

org START

_hello_str:
db 'Hello world!', 0

_main:
    mov si, _hello_str
    mov ax, BIOS_SEG
    mov ds, ax
    mov dx, 0x70
    mov cx, 11
    rep outsb
    hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp BIOS_SEG:_main
finish:
nop

