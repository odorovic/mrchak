[map brief stdout]

%include "macros.inc"

INTERNAL_IRQ  equ 0x0f
EXTERNAL_IRQ  equ 0x10
BIOS_SEG      equ 0xf000
START         equ 0xff00

org START
_external_msg:
db 'Triggered external interrupt!\n', 0

_internal_msg:
db 'Triggered internal interrupt!\n', 0

_charprint:
    mov di, 0
_print_loop:
    mov al, [cs:bx+di]
    cmp al, 0
    je _end_loop
    out 0x70, al
    inc di
    jmp _print_loop
_end_loop:
    ret

_service_external_intr:
    IENTER
    mov bx, _external_msg
    call _charprint
    IEXIT
    iret

_service_internal_intr:
    IENTER
    mov bx, _internal_msg
    call _charprint
    IEXIT
    iret

_main:
    mov ax, 0xabc0
    mov sp, ax
    xor ax, ax
    mov es, ax
    mov bx, INTERNAL_IRQ
    shl bx, 2
    mov ax, _service_internal_intr
    mov [es:bx], ax
    mov ax, BIOS_SEG
    mov [es:bx+2], ax
    mov bx, EXTERNAL_IRQ
    shl bx, 2
    mov ax, _service_external_intr
    mov [es:bx], ax
    mov ax, BIOS_SEG
    mov [es:bx+2], ax
    int INTERNAL_IRQ
    sti
    hlt

times 0x0fff0 - ($ - $$) - START db 0
start:
jmp BIOS_SEG:_main
finish:
nop

