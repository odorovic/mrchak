#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "errors.h"
#include "ucodes.h"
#include "decoder.h"
#include "prefices.h"
#include "instruction.h"
#include "operation.h"


void operation_init(op_p op)
{
    op->op_ucodes_idx  = 0;
    op->op_ucodes_size = 0;
    op->op_args_idx    = 0;
    op->op_args_size   = 0;
    op->op_x86_len     = 0;
    op->op_decoded     = false;
    op->op_executed    = false;
}

UCODE op_read_ucode(op_p op)
{
    assert(op->op_ucodes_idx < op->op_ucodes_size);
    return op->op_ucodes[op->op_ucodes_idx++];
}

uint16_t op_read_arg(op_p op)
{
    assert(op->op_args_idx < op->op_args_size);
    return op->op_args[op->op_args_idx++];
}

void op_write_ucode(op_p op, UCODE ucode)
{
    int idx;
    assert(op);
    idx = op->op_ucodes_size++;
    assert(idx < MAX_OPCODES);
    op->op_ucodes[idx] = ucode;
}

void op_write_arg(op_p op, int data)
{
    int* tmp;
    int idx;
    assert(op);
    idx = op->op_args_size++;
    assert(idx < MAX_ARGS);
    op->op_args[idx] = data;
}

static void decodeM(op_p op, int prefices, int modrm, int displacement)
{
    //if (!decodingAddressMode(dec)) return;


    //16 bit address size
    //Segment load
    switch (prefices & PREFICES_SG) {
    default:
        switch (modrm & 0xc7) {
        default:  op_write_ucode(op, LOAD_SEG_DS); break;
        case 0x02:
        case 0x03:
        case 0x42:
        case 0x43:
        case 0x46:
        case 0x82:
        case 0x83:
        case 0x86: op_write_ucode(op, LOAD_SEG_SS); break;
        }
        break;
    case PREFICES_CS: op_write_ucode(op, LOAD_SEG_CS); break;
    case PREFICES_DS: op_write_ucode(op, LOAD_SEG_DS); break;
    case PREFICES_SS: op_write_ucode(op, LOAD_SEG_SS); break;
    case PREFICES_ES: op_write_ucode(op, LOAD_SEG_ES); break;
    }

    switch (modrm & 0x7) {
    case 0x0: op_write_ucode(op, ADDR_BX); op_write_ucode(op, ADDR_SI); break;
    case 0x1: op_write_ucode(op, ADDR_BX); op_write_ucode(op, ADDR_DI); break;
    case 0x2: op_write_ucode(op, ADDR_BP); op_write_ucode(op, ADDR_SI); break;
    case 0x3: op_write_ucode(op, ADDR_BP); op_write_ucode(op, ADDR_DI); break;
    case 0x4: op_write_ucode(op, ADDR_SI); break;
    case 0x5: op_write_ucode(op, ADDR_DI); break;
    case 0x6:
        if ((modrm & 0xc0) == 0x00) {
            op_write_ucode(op, ADDR_IW);
            op_write_arg(op, displacement);
        } else {
            op_write_ucode(op, ADDR_BP);
        }
        break;
    case 0x7: op_write_ucode(op, ADDR_BX); break;
    }

    switch (modrm & 0xc0) {
        case 0x40:
            op_write_ucode(op, ADDR_IB);
            op_write_arg(op, displacement);
            break;
        case 0x80:
            op_write_ucode(op, ADDR_IW);
            op_write_arg(op, displacement);
            break;
    }
}

void load0_Eb(op_p op, int prefices, int modrm, int displacement)
{
    switch(modrm & 0xc7) {
    default:
        decodeM(op, prefices, modrm, displacement);
        op_write_ucode(op, MEM_READ_BYTE);
        op_write_ucode(op, LOAD0_MEM_BYTE);
        break;

    case 0xc0: op_write_ucode(op, LOAD0_AL); break;
    case 0xc1: op_write_ucode(op, LOAD0_CL); break;
    case 0xc2: op_write_ucode(op, LOAD0_DL); break;
    case 0xc3: op_write_ucode(op, LOAD0_BL); break;
    case 0xc4: op_write_ucode(op, LOAD0_AH); break;
    case 0xc5: op_write_ucode(op, LOAD0_CH); break;
    case 0xc6: op_write_ucode(op, LOAD0_DH); break;
    case 0xc7: op_write_ucode(op, LOAD0_BH); break;
    }
}
void load1_Eb(op_p op, int prefices, int modrm, int displacement)
{
    switch(modrm & 0xc7) {
    default:
        decodeM(op, prefices, modrm, displacement);
        op_write_ucode(op, MEM_READ_BYTE);
        op_write_ucode(op, LOAD1_MEM_BYTE); break;

    case 0xc0: op_write_ucode(op, LOAD1_AL); break;
    case 0xc1: op_write_ucode(op, LOAD1_CL); break;
    case 0xc2: op_write_ucode(op, LOAD1_DL); break;
    case 0xc3: op_write_ucode(op, LOAD1_BL); break;
    case 0xc4: op_write_ucode(op, LOAD1_AH); break;
    case 0xc5: op_write_ucode(op, LOAD1_CH); break;
    case 0xc6: op_write_ucode(op, LOAD1_DH); break;
    case 0xc7: op_write_ucode(op, LOAD1_BH); break;
    }
}
void store0_Eb(op_p op, int prefices, int modrm, int displacement)
{
    switch(modrm & 0xc7) {
    default:
        decodeM(op, prefices, modrm, displacement);
        op_write_ucode(op, STORE0_MEM_BYTE);
        break;

    case 0xc0: op_write_ucode(op, STORE0_AL); break;
    case 0xc1: op_write_ucode(op, STORE0_CL); break;
    case 0xc2: op_write_ucode(op, STORE0_DL); break;
    case 0xc3: op_write_ucode(op, STORE0_BL); break;
    case 0xc4: op_write_ucode(op, STORE0_AH); break;
    case 0xc5: op_write_ucode(op, STORE0_CH); break;
    case 0xc6: op_write_ucode(op, STORE0_DH); break;
    case 0xc7: op_write_ucode(op, STORE0_BH); break;
    }
}
void store1_Eb(op_p op, int prefices, int modrm, int displacement)
{
    switch(modrm & 0xc7) {
    default:
        decodeM(op, prefices, modrm, displacement);
        op_write_ucode(op, STORE1_MEM_BYTE);
        break;

    case 0xc0: op_write_ucode(op, STORE1_AL); break;
    case 0xc1: op_write_ucode(op, STORE1_CL); break;
    case 0xc2: op_write_ucode(op, STORE1_DL); break;
    case 0xc3: op_write_ucode(op, STORE1_BL); break;
    case 0xc4: op_write_ucode(op, STORE1_AH); break;
    case 0xc5: op_write_ucode(op, STORE1_CH); break;
    case 0xc6: op_write_ucode(op, STORE1_DH); break;
    case 0xc7: op_write_ucode(op, STORE1_BH); break;
    }
}

void load0_Ew(op_p op, int prefices, int modrm, int displacement)
{
    switch (modrm & 0xc7) {
    default:
        decodeM(op, prefices, modrm, displacement);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD0_MEM_WORD);
        break;

    case 0xc0: op_write_ucode(op, LOAD0_AX); break;
    case 0xc1: op_write_ucode(op, LOAD0_CX); break;
    case 0xc2: op_write_ucode(op, LOAD0_DX); break;
    case 0xc3: op_write_ucode(op, LOAD0_BX); break;
    case 0xc4: op_write_ucode(op, LOAD0_SP); break;
    case 0xc5: op_write_ucode(op, LOAD0_BP); break;
    case 0xc6: op_write_ucode(op, LOAD0_SI); break;
    case 0xc7: op_write_ucode(op, LOAD0_DI); break;
    }
}
void store0_Ew(op_p op, int prefices, int modrm, int displacement)
{
    switch (modrm & 0xc7) {
    default:
        decodeM(op, prefices, modrm, displacement);
        op_write_ucode(op, STORE0_MEM_WORD);
        break;

    case 0xc0: op_write_ucode(op, STORE0_AX); break;
    case 0xc1: op_write_ucode(op, STORE0_CX); break;
    case 0xc2: op_write_ucode(op, STORE0_DX); break;
    case 0xc3: op_write_ucode(op, STORE0_BX); break;
    case 0xc4: op_write_ucode(op, STORE0_SP); break;
    case 0xc5: op_write_ucode(op, STORE0_BP); break;
    case 0xc6: op_write_ucode(op, STORE0_SI); break;
    case 0xc7: op_write_ucode(op, STORE0_DI); break;
    }
}
void load1_Ew(op_p op, int prefices, int modrm, int displacement)
{
    switch (modrm & 0xc7) {
    default: decodeM(op, prefices, modrm, displacement); op_write_ucode(op, LOAD1_MEM_WORD); break;

    case 0xc0: op_write_ucode(op, LOAD1_AX); break;
    case 0xc1: op_write_ucode(op, LOAD1_CX); break;
    case 0xc2: op_write_ucode(op, LOAD1_DX); break;
    case 0xc3: op_write_ucode(op, LOAD1_BX); break;
    case 0xc4: op_write_ucode(op, LOAD1_SP); break;
    case 0xc5: op_write_ucode(op, LOAD1_BP); break;
    case 0xc6: op_write_ucode(op, LOAD1_SI); break;
    case 0xc7: op_write_ucode(op, LOAD1_DI); break;
    }
}
void store1_Ew(op_p op, int prefices, int modrm, int displacement)
{
    switch (modrm & 0xc7) {
    default: decodeM(op, prefices, modrm, displacement); op_write_ucode(op, STORE1_MEM_WORD); break;

    case 0xc0: op_write_ucode(op, STORE1_AX); break;
    case 0xc1: op_write_ucode(op, STORE1_CX); break;
    case 0xc2: op_write_ucode(op, STORE1_DX); break;
    case 0xc3: op_write_ucode(op, STORE1_BX); break;
    case 0xc4: op_write_ucode(op, STORE1_SP); break;
    case 0xc5: op_write_ucode(op, STORE1_BP); break;
    case 0xc6: op_write_ucode(op, STORE1_SI); break;
    case 0xc7: op_write_ucode(op, STORE1_DI); break;
    }
}

void load0_Gb(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, LOAD0_AL); break;
    case 0x08: op_write_ucode(op, LOAD0_CL); break;
    case 0x10: op_write_ucode(op, LOAD0_DL); break;
    case 0x18: op_write_ucode(op, LOAD0_BL); break;
    case 0x20: op_write_ucode(op, LOAD0_AH); break;
    case 0x28: op_write_ucode(op, LOAD0_CH); break;
    case 0x30: op_write_ucode(op, LOAD0_DH); break;
    case 0x38: op_write_ucode(op, LOAD0_BH); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Unknown Byte Register Operand");
    }
}
void store0_Gb(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, STORE0_AL); break;
    case 0x08: op_write_ucode(op, STORE0_CL); break;
    case 0x10: op_write_ucode(op, STORE0_DL); break;
    case 0x18: op_write_ucode(op, STORE0_BL); break;
    case 0x20: op_write_ucode(op, STORE0_AH); break;
    case 0x28: op_write_ucode(op, STORE0_CH); break;
    case 0x30: op_write_ucode(op, STORE0_DH); break;
    case 0x38: op_write_ucode(op, STORE0_BH); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Unknown Byte Register Operand");
    }
}
void load1_Gb(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, LOAD1_AL); break;
    case 0x08: op_write_ucode(op, LOAD1_CL); break;
    case 0x10: op_write_ucode(op, LOAD1_DL); break;
    case 0x18: op_write_ucode(op, LOAD1_BL); break;
    case 0x20: op_write_ucode(op, LOAD1_AH); break;
    case 0x28: op_write_ucode(op, LOAD1_CH); break;
    case 0x30: op_write_ucode(op, LOAD1_DH); break;
    case 0x38: op_write_ucode(op, LOAD1_BH); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Unknown Byte Register Operand");
    }
}
void store1_Gb(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, STORE1_AL); break;
    case 0x08: op_write_ucode(op, STORE1_CL); break;
    case 0x10: op_write_ucode(op, STORE1_DL); break;
    case 0x18: op_write_ucode(op, STORE1_BL); break;
    case 0x20: op_write_ucode(op, STORE1_AH); break;
    case 0x28: op_write_ucode(op, STORE1_CH); break;
    case 0x30: op_write_ucode(op, STORE1_DH); break;
    case 0x38: op_write_ucode(op, STORE1_BH); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Unknown Byte Register Operand");
    }
}

void load0_Gw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, LOAD0_AX); break;
    case 0x08: op_write_ucode(op, LOAD0_CX); break;
    case 0x10: op_write_ucode(op, LOAD0_DX); break;
    case 0x18: op_write_ucode(op, LOAD0_BX); break;
    case 0x20: op_write_ucode(op, LOAD0_SP); break;
    case 0x28: op_write_ucode(op, LOAD0_BP); break;
    case 0x30: op_write_ucode(op, LOAD0_SI); break;
    case 0x38: op_write_ucode(op, LOAD0_DI); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Unknown Word Register Operand");
    }
}
void store0_Gw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, STORE0_AX); break;
    case 0x08: op_write_ucode(op, STORE0_CX); break;
    case 0x10: op_write_ucode(op, STORE0_DX); break;
    case 0x18: op_write_ucode(op, STORE0_BX); break;
    case 0x20: op_write_ucode(op, STORE0_SP); break;
    case 0x28: op_write_ucode(op, STORE0_BP); break;
    case 0x30: op_write_ucode(op, STORE0_SI); break;
    case 0x38: op_write_ucode(op, STORE0_DI); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Unknown Word Register Operand");
    }
}
void load1_Gw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, LOAD1_AX); break;
    case 0x08: op_write_ucode(op, LOAD1_CX); break;
    case 0x10: op_write_ucode(op, LOAD1_DX); break;
    case 0x18: op_write_ucode(op, LOAD1_BX); break;
    case 0x20: op_write_ucode(op, LOAD1_SP); break;
    case 0x28: op_write_ucode(op, LOAD1_BP); break;
    case 0x30: op_write_ucode(op, LOAD1_SI); break;
    case 0x38: op_write_ucode(op, LOAD1_DI); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        // throw new IllegalStateException("Unknown Word Register Operand");
    }
}
void store1_Gw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, STORE1_AX); break;
    case 0x08: op_write_ucode(op, STORE1_CX); break;
    case 0x10: op_write_ucode(op, STORE1_DX); break;
    case 0x18: op_write_ucode(op, STORE1_BX); break;
    case 0x20: op_write_ucode(op, STORE1_SP); break;
    case 0x28: op_write_ucode(op, STORE1_BP); break;
    case 0x30: op_write_ucode(op, STORE1_SI); break;
    case 0x38: op_write_ucode(op, STORE1_DI); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        // throw new IllegalStateException("Unknown Word Register Operand");
    }
}

void load0_Sw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, LOAD0_ES); break;
    case 0x08: op_write_ucode(op, LOAD0_CS); break;
    case 0x10: op_write_ucode(op, LOAD0_SS); break;
    case 0x18: op_write_ucode(op, LOAD0_DS); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        // throw new IllegalStateException("Unknown Segment Register Operand");
    }
}
void store0_Sw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, STORE0_ES); break;
    case 0x08: op_write_ucode(op, STORE0_CS); break;
    case 0x10: op_write_ucode(op, STORE0_SS); break;
    case 0x18: op_write_ucode(op, STORE0_DS); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        // throw new IllegalStateException("Unknown Segment Register Operand");
    }
}
void store1_Sw(op_p op, int modrm)
{
    switch(modrm & 0x38) {
    case 0x00: op_write_ucode(op, STORE1_ES); break;
    case 0x08: op_write_ucode(op, STORE1_CS); break;
    case 0x10: op_write_ucode(op, STORE1_SS); break;
    case 0x18: op_write_ucode(op, STORE1_DS); break;
    default:
        REPORT_ERROR(INVALID_STATE);
        // throw new IllegalStateException("Unknown Segment Register Operand");
    }
}

static void decodeO(op_p op, int prefices, int displacement)
{
    switch (prefices & PREFICES_SG) {
    default:
    case PREFICES_DS: op_write_ucode(op, LOAD_SEG_DS); break;
    case PREFICES_ES: op_write_ucode(op, LOAD_SEG_ES); break;
    case PREFICES_SS: op_write_ucode(op, LOAD_SEG_SS); break;
    case PREFICES_CS: op_write_ucode(op, LOAD_SEG_CS); break;
    }

    //if (decodingAddressMode()) {
        op_write_ucode(op, ADDR_IW);
        op_write_arg(op, displacement);
    //}
}

void load0_Ob(op_p op, int prefices, int displacement)
{
    decodeO(op, prefices, displacement);
    op_write_ucode(op, MEM_READ_BYTE);
    op_write_ucode(op, LOAD0_MEM_BYTE);
}
void store0_Ob(op_p op, int prefices, int displacement)
{
    decodeO(op, prefices, displacement);
    op_write_ucode(op, STORE0_MEM_BYTE);
}

void load0_Ow(op_p op, int prefices, int displacement)
{
    decodeO(op, prefices, displacement);
    op_write_ucode(op, MEM_READ_WORD);
    op_write_ucode(op, LOAD0_MEM_WORD);
}
void store0_Ow(op_p op, int prefices, int displacement)
{
    decodeO(op, prefices, displacement);
    op_write_ucode(op, STORE0_MEM_WORD);
}

void load1_Ob(op_p op, int prefices, int displacement)
{
    decodeO(op, prefices, displacement);
    op_write_ucode(op, MEM_READ_BYTE);
    op_write_ucode(op, LOAD1_MEM_BYTE);
}

void load1_Ow(op_p op, int prefices, int displacement)
{
    decodeO(op, prefices, displacement);
    op_write_ucode(op, MEM_READ_WORD);
    op_write_ucode(op, LOAD1_MEM_WORD);
}

void load0_M(op_p op, int prefices, int modrm, int displacement)
{
    decodeM(op, prefices, modrm, displacement);
    op_write_ucode(op, LOAD0_ADDR);
}

