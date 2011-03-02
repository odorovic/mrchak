[map brief stdout]

org 0x00fff0
start:
mov al, 88
out 0x70, al
jmp start
finish:
hlt
