#ifndef OPERATION_H
#define OPERATION_H

#include <stdbool.h>

#include "ucodes.h"
#include "instruction.h"

#define MAX_OPCODES 20
#define MAX_ARGS    20

typedef struct op_t {
    UCODE   op_ucodes[MAX_OPCODES];
    int     op_ucodes_idx;
    int     op_ucodes_size;
    int     op_args[MAX_ARGS];
    int     op_args_idx;
    int     op_args_size;
    int     op_x86_len;
    bool    op_decoded;
    bool    op_executed;
} op_s, *op_p;

void operation_init(op_p op);

UCODE    op_read_ucode(op_p op);
uint16_t op_read_arg(op_p op);
void op_write_ucode(op_p op, UCODE ucode);
void op_write_arg(op_p op, int data);
void op_finish(op_p op, int pos);
void load0_Eb(op_p op, int prefices, int modrm, int displacement);
void load1_Eb(op_p op, int prefices, int modrm, int displacement);
void store0_Eb(op_p op, int prefices, int modrm, int displacement);
void store1_Eb(op_p op, int prefices, int modrm, int displacement);
void load0_Ew(op_p op, int prefices, int modrm, int displacement);
void store0_Ew(op_p op, int prefices, int modrm, int displacement);
void load1_Ew(op_p op, int prefices, int modrm, int displacement);
void store1_Ew(op_p op, int prefices, int modrm, int displacement);
void load0_Gb(op_p op, int modrm);
void store0_Gb(op_p op, int modrm);
void load1_Gb(op_p op, int modrm);
void store1_Gb(op_p op, int modrm);
void load0_Gw(op_p op, int modrm);
void store0_Gw(op_p op, int modrm);
void load1_Gw(op_p op, int modrm);
void store1_Gw(op_p op, int modrm);
void load0_Sw(op_p op, int modrm);
void store0_Sw(op_p op, int modrm);
void store1_Sw(op_p op, int modrm);
void load0_Ob(op_p op, int prefices, int displacement);
void store0_Ob(op_p op, int prefices, int displacement);
void load0_Ow(op_p op, int prefices, int displacement);
void store0_Ow(op_p op, int prefices, int displacement);
void load1_Ob(op_p op, int prefices, int displacement);
void load1_Ow(op_p op, int prefices, int displacement);
void load0_M(op_p op, int prefices, int modrm, int displacement);

#endif
