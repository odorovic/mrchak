[map brief stdout]

START    equ 0xff00
BIOS_SEG equ 0xf000
RAM_SEG  equ 0x0000
STRLEN   equ 8

org START
_main:
;Enable breakpoint
pushf
pop ax
mov dx, 1
shl dx, 15
or  ax, dx
push ax
popf

mov ax, 1
mov ax, 2
mov ax, 3
mov ax, 4
mov ax, 5
hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp BIOS_SEG:_main
finish:
nop

