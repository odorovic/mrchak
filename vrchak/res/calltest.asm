[map brief stdout]

START equ 0xff00
BIOS_SEG equ 0xf000

org START
_func:
    ret

_main:
    mov  ax, 0xabc0
    mov  sp, ax
    call _func
    hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp BIOS_SEG:_main
finish:
nop

