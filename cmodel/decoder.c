
#include <stdio.h>
#include <stdbool.h>

#include "errors.h"
#include "decoder.h"
#include "operation.h"
#include "prefices.h"
#include "ucodes.h"

bool decodingAddressMode(dec_p dec)
{
    if (dec->dec_amode_decoded) {
        return false;
    } else {
        return (dec->dec_amode_decoded = true);
    }
}

static void decodeSegmentPrefix(op_p op, int prefices)
{
    switch (prefices & PREFICES_SG) {
    default:
    case PREFICES_DS: op_write_ucode(op, LOAD_SEG_DS); break;
    case PREFICES_ES: op_write_ucode(op, LOAD_SEG_ES); break;
    case PREFICES_SS: op_write_ucode(op, LOAD_SEG_SS); break;
    case PREFICES_CS: op_write_ucode(op, LOAD_SEG_CS); break;
    }
}

static void decodeComplete(dec_p dec, op_p op, int position)
{
    if (dec->dec_amode_decoded) {
        op_write_ucode(op, MEM_RESET);
        dec->dec_amode_decoded = false;
    }
    op_finish(op, position);
}

static void op_write_preamble(dec_p dec, op_p op, inst_p inst)
{
    uint8_t opcode   = inst->inst_opcode;
    uint8_t modrm    = inst->inst_modrm;
    switch(opcode){
    case 0xcc: //INT 3
    case 0xcd: //INT imm8
    case 0xce: //INTO (unimplemented)
        op_write_ucode(op, LOAD0_FLAGS);
        op_write_ucode(op, PUSH);
        op_write_ucode(op, LOAD0_CS);
        op_write_ucode(op, PUSH);
        op_write_ucode(op, LOAD0_IP);
        op_write_ucode(op, PUSH);
        op_write_ucode(op, CLI);
        op_write_ucode(op, CLT);
        break;
    case 0x9a: //CALL FAR PTR 16:16
        op_write_ucode(op, LOAD0_CS);
        op_write_ucode(op, PUSH);
        op_write_ucode(op, LOAD0_IP);
        op_write_ucode(op, PUSH);
        break;
    case 0xe8: //CALL rel16
        op_write_ucode(op, LOAD0_IP);
        op_write_ucode(op, PUSH);
        break;
    case 0xff:
    switch (modrm & 0x38) {
        case 0x18:  //CALL FAR m16:16
            op_write_ucode(op, LOAD0_CS);
            op_write_ucode(op, PUSH);
        case 0x10: //CALL r/m16
            op_write_ucode(op, LOAD0_IP);
            op_write_ucode(op, PUSH);
            break;
        default:
            break;
    }
    default:
        break;
    }
}

static void op_write_input_args(dec_p dec, op_p op, inst_p inst)
{
    uint8_t prefices = inst->inst_prefices;
    uint8_t opcode   = inst->inst_opcode;
    uint8_t modrm    = inst->inst_modrm;
    int displacement = inst->inst_disp;
    int immediate    = inst->inst_imm;
    switch (opcode) {
    case 0x00: //ADD   r/m8,  reg8
    case 0x08: //OR    r/m8,  reg8
    case 0x10: //ADC   r/m8,  reg8
    case 0x18: //SBB   r/m8,  reg8
    case 0x20: //AND   r/m8,  reg8
    case 0x28: //SUB   r/m8,  reg8
    case 0x30: //XOR   r/m8,  reg8
    case 0x38: //CMP   r/m8,  reg8
    case 0x84: //TEST  r/m8,  reg8
    case 0x86: //XCHG  r/m8,  reg8
        load0_Eb(op, prefices, modrm, displacement);
        load1_Gb(op, modrm);
        break;

    case 0x88: //MOV   r/m8,  reg8
        load0_Gb(op, modrm);
        break;

    case 0x02: //ADD  reg8,  r/m8
    case 0x0a: //OR   reg8,  r/m8
    case 0x12: //ADC  reg8,  r/m8
    case 0x1a: //SBB  reg8,  r/m8
    case 0x22: //AND  reg8,  r/m8
    case 0x2a: //SUB  reg8,  r/m8
    case 0x32: //XOR  reg8,  r/m8
    case 0x3a: //CMP  reg8,  r/m8
        load0_Gb(op, modrm);
        load1_Eb(op, prefices, modrm, displacement);
        break;

    case 0x8a:  //MOV  reg8,  r/m8
        load0_Eb(op, prefices, modrm, displacement);
        break;

    case 0x01: //ADD  r/m16,  reg16
    case 0x09: //OR   r/m16,  reg16
    case 0x11: //ADC  r/m16,  reg16
    case 0x19: //SBB  r/m16,  reg16
    case 0x21: //AND  r/m16,  reg16
    case 0x29: //SUB  r/m16,  reg16
    case 0x31: //XOR  r/m16,  reg16
    case 0x39: //CMP   r/m16,  reg16
    case 0x85: //TEST  r/m16,  reg16
    case 0x87: //XCHG  r/m16,  reg16
        load0_Ew(op, prefices, modrm, displacement);
        load1_Gw(op, modrm);
        break;

    case 0x89: //MOV   r/m16,  reg16
        load0_Gw(op, modrm);
        break;

    case 0x03: //ADD  reg16,  r/m16
    case 0x0b: //OR   reg16,  r/m16
    case 0x13: //ADC  reg16,  r/m16
    case 0x1b: //SBB  reg16,  r/m16
    case 0x23: //AND  reg16,  r/m16
    case 0x2b: //SUB  reg16,  r/m16
    case 0x33: //XOR  reg16,  r/m16
    case 0x3b: //CMP  reg16,  r/m16
        load0_Gw(op, modrm);
        load1_Ew(op, prefices, modrm, displacement);
        break;

    case 0x8b: //MOV  reg16,  r/m16
        load0_Ew(op, prefices, modrm, displacement);
        break;

    case 0x8d: //LEA  reg16, M
        load0_M(op, prefices, modrm, displacement);
        break;


    case 0x80: //IMM G1  r/m8,  imm8
    case 0x82: //IMM G1  r/m8,  imm8
    case 0xc0: //SFT G2  r/m8,  imm8
        load0_Eb(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IB);
        op_write_arg(op, immediate);
        break;

    case 0xc6: //MOV G11  r/m8,  imm8
    case 0xb0: //MOV AL,  imm8
    case 0xb1: //MOV CL,  imm8
    case 0xb2: //MOV DL,  imm8
    case 0xb3: //MOV BL,  imm8
    case 0xb4: //MOV AH,  imm8
    case 0xb5: //MOV CH,  imm8
    case 0xb6: //MOV DH,  imm8
    case 0xb7: //MOV BH,  imm8
    case 0xe4: //IN  AL,  imm8
    case 0x70: //Jcc Jb
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
    case 0x7f:
    case 0xd4: //AAM  imm8
    case 0xd5: //AAD  imm8
    case 0xe0: //LOOPNZ Jb
    case 0xe1: //LOOPZ Jb
    case 0xe2: //LOOP Jb
    case 0xe3: //JCXZ Jb
    case 0xeb: //JMP Jb
    case 0xe5: //IN eAX,  imm8
        op_write_ucode(op, LOAD0_IB);
        op_write_arg(op, immediate);
        break;

    case 0x81: //IMM G1  r/m16,  imm16
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IW);
        op_write_arg(op, immediate);
        break;

    case 0xc7: //MOV G11  r/m16,  imm16
    case 0x68: //PUSH  imm16
    case 0x6a: //PUSH  imm8
    case 0xe8: //CALL Jv
    case 0xe9: //JMP  Jv
        op_write_ucode(op, LOAD0_IW);
        op_write_arg(op, immediate);
        break;

    case 0xc1: //SFT G2  r/m16,  imm8
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IB);
        op_write_arg(op, immediate);
        break;

    case 0x83: //IMM G1  r/m16,  imm8 sign extend the byte to 16/32 bits
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IW);
        op_write_arg(op, immediate);
        break;

    case 0x8f: //POP  r/m16
    case 0x58: //POP eAX
    case 0x59: //POP eCX
    case 0x5a: //POP eDX
    case 0x5b: //POP eBX
    case 0x5c: //POP eSP
    case 0x5d: //POP eBP
    case 0x5e: //POP eSI
    case 0x5f: //POP eDI
    case 0x07: //POP ES
    case 0x17: //POP SS
    case 0x1f: //POP DS
        break;

    case 0xc2: // RET Iw
    case 0xc3: // RET
        op_write_ucode(op, POP);
        op_write_ucode(op, LOAD0_MEM_WORD);
        op_write_ucode(op, STORE0_IP);
        break;
    case 0xca: // RETF Iw
    case 0xcb: // RETF
        //TODO popovanje segmenta
        op_write_ucode(op, POP);
        op_write_ucode(op, STORE0_CS);
        break;

    case 0x9a: //CALLF Ap
    case 0xea: //JMPF Ap
        op_write_ucode(op, LOAD0_IW);
        op_write_arg(op, (0xffff & immediate));
        op_write_ucode(op, LOAD1_IW);
        op_write_arg(op, (immediate >> 16));
        break;

    case 0x9c:
        op_write_ucode(op, LOAD0_FLAGS); break;

    case 0xec: //IN AL, DX
    case 0xed: //IN eAX, DX
        op_write_ucode(op, LOAD0_DX);
        break;

    case 0xee: //OUT DX, AL
        op_write_ucode(op, LOAD0_DX);
        op_write_ucode(op, LOAD1_AL);
        break;

    case 0xef: //OUT DX, eAX
        op_write_ucode(op, LOAD0_DX);
        op_write_ucode(op, LOAD1_AX);
        break;

    case 0x04: //ADD AL,  imm8
    case 0x0c: //OR  AL,  imm8
    case 0x14: //ADC AL,  imm8
    case 0x1c: //SBB AL,  imm8
    case 0x24: //AND AL,  imm8
    case 0x2c: //SUB AL,  imm8
    case 0x34: //XOR AL,  imm8
    case 0x3c: //CMP AL,  imm8
    case 0xa8: //TEST AL,  imm8
        op_write_ucode(op, LOAD0_AL);
        op_write_ucode(op, LOAD1_IB);
        op_write_arg(op, immediate);
        break;

    case 0xc8: //ENTER Iw,  imm8
        op_write_ucode(op, LOAD0_IW);
        op_write_arg(op, (0xffffl & (immediate >> 16)));
        op_write_ucode(op, LOAD1_IB);
        op_write_arg(op, (0xffl & immediate));
        break;

    case 0x69: //IMUL  reg16,  r/m16,  imm16
    case 0x6b: //IMUL  reg16,  r/m16,  imm8
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IW);
        op_write_arg(op, immediate);
        break;

    case 0xe6: //OUT  imm8, AL
        op_write_ucode(op, LOAD0_UB);
        op_write_arg(op, immediate);
        op_write_ucode(op, LOAD1_AL);
        break;

    case 0x05: //ADD eAX,  imm16
    case 0x0d: //OR  eAX,  imm16
    case 0x15: //ADC eAX,  imm16
    case 0x1d: //SBB eAX,  imm16
    case 0x25: //AND eAX,  imm16
    case 0x2d: //SUB eAX,  imm16
    case 0x35: //XOR eAX,  imm16
    case 0x3d: //CMP eAX,  imm16
    case 0xa9: //TEST eAX,  imm16
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_IW);
        op_write_arg(op, immediate);
        break;

    case 0xb8: //MOV eAX,  imm16
    case 0xb9: //MOV eCX,  imm16
    case 0xba: //MOV eDX,  imm16
    case 0xbb: //MOV eBX,  imm16
    case 0xbc: //MOV eSP,  imm16
    case 0xbd: //MOV eBP,  imm16
    case 0xbe: //MOV eSI,  imm16
    case 0xbf: //MOV eDI,  imm16
        op_write_ucode(op, LOAD0_IW);
        op_write_arg(op, immediate);
        break;

    case 0xe7: //OUT  imm8, eAX
        op_write_ucode(op, LOAD0_IB);
        op_write_arg(op, immediate);
        op_write_ucode(op, LOAD1_AX);
        break;

    case 0x40: //INC eAX
    case 0x48: //DEC eAX
    case 0x50: //PUSH eAX
        op_write_ucode(op, LOAD0_AX);
        break;

    case 0x41: //INC eCX
    case 0x49: //DEC eCX
    case 0x51: //PUSH eCX
        op_write_ucode(op, LOAD0_CX);
        break;

    case 0x42: //INC eDX
    case 0x4a: //DEC eDX
    case 0x52: //PUSH eDX
        op_write_ucode(op, LOAD0_DX);
        break;

    case 0x43: //INC eBX
    case 0x4b: //DEC eBX
    case 0x53: //PUSH eBX
        op_write_ucode(op, LOAD0_BX);
        break;

    case 0x44: //INC eSP
    case 0x4c: //DEC eSP
    case 0x54: //PUSH eSP
        op_write_ucode(op, LOAD0_SP);
        break;

    case 0x45: //INC eBP
    case 0x4d: //DEC eBP
    case 0x55: //PUSH eBP
        op_write_ucode(op, LOAD0_BP);
        break;

    case 0x46: //INC eSI
    case 0x4e: //DEC eSI
    case 0x56: //PUSH eSI
        op_write_ucode(op, LOAD0_SI);
        break;

    case 0x47: //INC eDI
    case 0x4f: //DEC eDI
    case 0x57: //PUSH eDI
        op_write_ucode(op, LOAD0_DI);
        break;

    case 0x91: //XCHG eAX, eCX
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_CX);
        break;

    case 0x92: //XCHG eAX, eDX
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_DX);
        break;

    case 0x93: //XCHG eAX, eBX
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_BX);
        break;

    case 0x94: //XCHG eAX, eSP
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_SP);
        break;

    case 0x95: //XCHG eAX, eBP
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_BP);
        break;

    case 0x96: //XCHG eAX, eSI
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_SI);
        break;

    case 0x97: //XCHG eAX, eDI
        op_write_ucode(op, LOAD0_AX);
        op_write_ucode(op, LOAD1_DI);
        break;

    case 0xd0: //SFT G2  r/m8, 1
        load0_Eb(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IB);
        op_write_ucode(op, 1);
        break;

    case 0xd2: //SFT G2  r/m8, CL
        load0_Eb(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_CL);
        break;

    case 0xd1: //SFT G2  r/m16, 1
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_IB);
        op_write_ucode(op, 1);
        break;

    case 0xd3: //SFT G2  r/m16, CL
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, LOAD1_CL);
        break;

    case 0xf6: //UNA G3  r/m8, ?
        switch (modrm & 0x38) {
        case 0x00: //TEST  r/m8,  imm8
            load0_Eb(op, prefices, modrm, displacement);
            op_write_ucode(op, LOAD1_IB);
            op_write_arg(op, immediate);
            break;
        case 0x10:
        case 0x18:
            load0_Eb(op, prefices, modrm, displacement);
            break;
        case 0x20:
        case 0x28:
            load0_Eb(op, prefices, modrm, displacement);
            break;
        case 0x30:
        case 0x38:
            load0_Eb(op, prefices, modrm, displacement);
            break;
        }
        break;

    case 0xf7: //UNA G3  r/m16, ?

        switch (modrm & 0x38) {
        case 0x00: //TEST Ew, Iw
            load0_Ew(op, prefices, modrm, displacement);
            op_write_ucode(op, LOAD1_IW);
            op_write_arg(op, immediate);
            break;
        case 0x10:
        case 0x18:
            load0_Ew(op, prefices, modrm, displacement);
            break;
        case 0x20:
        case 0x28:
            load0_Ew(op, prefices, modrm, displacement);
            break;
        case 0x30:
        case 0x38:
            load0_Ew(op, prefices, modrm, displacement);
        }
        break;

    case 0xfe: //INC/DEC G4  r/m8
        load0_Eb(op, prefices, modrm, displacement);
        break;

    case 0x06: //PUSH ES
        op_write_ucode(op, LOAD0_ES);
        break;

    case 0x0e: //PUSH CS
        op_write_ucode(op, LOAD0_CS);
        break;

    case 0x16: //PUSH SS
        op_write_ucode(op, LOAD0_SS);
        break;

    case 0x1e: //PUSH DS
        op_write_ucode(op, LOAD0_DS);
        break;

    case 0x8c: //MOV Ew, Sw
        load0_Sw(op, modrm);
        break;

    case 0x8e: //MOV Sw, Ew
        load0_Ew(op, prefices, modrm, displacement);
        break;

    case 0xa0: //MOV AL, Ob
        load0_Ob(op, prefices, displacement);
        break;

    case 0xa2: //MOV Ob, AL
        op_write_ucode(op, LOAD0_AL);
        break;

    case 0xa1: //MOV eAX, Ov
        load0_Ow(op, prefices, displacement);
        break;

    case 0xa3: //MOV Ov, eAX
        op_write_ucode(op, LOAD0_AX);
        break;

    case 0x6c: //INS Yb, DX (prefices do not override segment)
    case 0x6d: //INS Yv, DX (prefices do not override segment)
        op_write_ucode(op, LOAD0_DX);
        break;

    case 0x6e: //OUTS DX, Xb
        op_write_ucode(op, LOAD1_DX);
        op_write_ucode(op, LOAD_SEG_DS);
        op_write_ucode(op, ADDR_SI);
        op_write_ucode(op, MEM_READ_BYTE);
        op_write_ucode(op, LOAD0_MEM_BYTE);
        break;
    case 0x6f: //OUTS DX, Xv
        op_write_ucode(op, LOAD1_DX);
        op_write_ucode(op, LOAD_SEG_DS);
        op_write_ucode(op, ADDR_SI);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD0_MEM_WORD);
        break;

    case 0xa4: //MOVS Yb, Xb
    case 0xa5: //MOVS Yv, Xv
    case 0xa6: //CMPS Yb, Xb
    case 0xa7: //CMPS Xv, Yv
    case 0xac: //LODS AL, Xb
    case 0xad: //LODS eAX, Xv
        decodeSegmentPrefix(op, prefices);
        break;

    case 0xaa: //STOS Yb, AL (prefices do not override segment)
        op_write_ucode(op, LOAD0_AL);
        break;

    case 0xab: //STOS Yv, eAX
        op_write_ucode(op, LOAD0_AX);
        break;


    case 0xae: //SCAS AL, Yb (prefices do not override segment)
        op_write_ucode(op, LOAD0_AL);
        break;

    case 0xaf: //SCAS eAX, Yv
        op_write_ucode(op, LOAD0_AX);
        break;

    case 0xff: //INC/DEC G5
        switch (modrm & 0x38) {
        case 0x00:
        case 0x08:
        case 0x10:
        case 0x20:
        case 0x30:
            load0_Ew(op, prefices, modrm, displacement);
            break;
        case 0x18:
        case 0x28:
            load0_Ew(op, prefices, modrm, displacement);
            op_write_ucode(op, ADDR_IB);
            op_write_arg(op, 2);
            op_write_ucode(op, LOAD1_MEM_WORD);
        }
        break;

    case 0xc4: //LES  reg16, Mp
    case 0xc5: //LDS  reg16, Mp
        load0_Ew(op, prefices, modrm, displacement);
        op_write_ucode(op, ADDR_IB);
        op_write_ucode(op, 2);
        op_write_ucode(op, LOAD1_MEM_WORD);
        break;

    case 0xcc: // INT 3
        op_write_ucode(op, ADDR_INT);
        op_write_arg(op, 3);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD1_MEM_WORD);
        op_write_ucode(op, ADDR_IW);
        op_write_arg(op, 2);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD0_MEM_WORD);
        break;

    case 0xcd: // INT imm8
        op_write_ucode(op, ADDR_INT);
        op_write_arg(op, immediate);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD1_MEM_WORD);
        op_write_ucode(op, ADDR_IW);
        op_write_arg(op, 2);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD0_MEM_WORD);
        break;

    case 0xce: // INTO (uniimplemented)
        op_write_ucode(op, ADDR_INT);
        op_write_arg(op, 4);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD1_MEM_WORD);
        op_write_ucode(op, ADDR_IW);
        op_write_arg(op, 2);
        op_write_ucode(op, MEM_READ_WORD);
        op_write_ucode(op, LOAD0_MEM_WORD);
        break;

    case 0xcf: // IRET
        op_write_ucode(op, POP);
        op_write_ucode(op, LOAD0_MEM_WORD);
        op_write_ucode(op, STORE0_IP);
        op_write_ucode(op, POP);
        op_write_ucode(op, LOAD0_MEM_WORD);
        op_write_ucode(op, STORE0_CS);
        op_write_ucode(op, POP);
        op_write_ucode(op, LOAD0_MEM_WORD);
        op_write_ucode(op, STORE0_FLAGS);
        break;

    case 0xd7: // XLAT
        switch (prefices & PREFICES_SG) {
        case PREFICES_ES: op_write_ucode(op, LOAD_SEG_ES); break;
        case PREFICES_CS: op_write_ucode(op, LOAD_SEG_CS); break;
        case PREFICES_SS: op_write_ucode(op, LOAD_SEG_SS); break;
        default:
        case PREFICES_DS: op_write_ucode(op, LOAD_SEG_DS); break;
        }

        if (decodingAddressMode(dec)) {
            op_write_ucode(op, ADDR_BX);
            op_write_ucode(op, ADDR_UAL);
        }
        op_write_ucode(op, MEM_READ_BYTE);
        op_write_ucode(op, LOAD0_MEM_BYTE);
        break;
    }
}

static void op_write_operation(dec_p dec, op_p op, inst_p inst)
{
    uint8_t  prefices  = inst->inst_prefices;
    uint8_t  opcode    = inst->inst_opcode;
    uint8_t  modrm     = inst->inst_modrm;
    uint16_t immediate = inst->inst_imm;
    switch (opcode) {
    case 0x00: //ADD  r/m8,  reg8
    case 0x01: //ADD  r/m16,  reg16
    case 0x02: //ADD  reg8,  r/m8
    case 0x03: //ADD  reg16,  r/m16
    case 0x04: //ADD AL,  imm8
    case 0x05: //ADD AX,  imm16
        op_write_ucode(op, ADD);
        break;

    case 0x08: //OR   r/m8,  reg8
    case 0x09: //OR   r/m16,  reg16
    case 0x0a: //OR   reg8,  r/m8
    case 0x0b: //OR   reg16,  r/m16
    case 0x0c: //OR  AL,  imm8
    case 0x0d:
        op_write_ucode(op, OR); //OR  eAX,  imm16
        break;

    case 0x10: //ADC  r/m8,  reg8
    case 0x11: //ADC  r/m16,  reg16
    case 0x12: //ADC  reg8,  r/m8
    case 0x13: //ADC  reg16,  r/m16
    case 0x14: //ADC AL,  imm8
    case 0x15: //ADC eAX,  imm16
        op_write_ucode(op, ADC);
        break;

    case 0x18: //SBB  r/m8,  reg8
    case 0x19: //SBB  r/m16,  reg16
    case 0x1a: //SBB  reg8,  r/m8
    case 0x1b: //SBB  reg16,  r/m16
    case 0x1c: //SBB AL,  imm8
    case 0x1d: op_write_ucode(op, SBB); break; //SBB eAX,  imm16

    case 0x20: //AND  r/m8,  reg8
    case 0x21: //AND  r/m16,  reg16
    case 0x22: //AND  reg8,  r/m8
    case 0x23: //AND  reg16,  r/m16
    case 0x24: //AND AL,  imm8
    case 0x25: //AND eAX,  imm16
    case 0x84: //TEST  r/m8,  reg8
    case 0x85: //TEST  r/m16,  reg16
    case 0xa8: //TEST AL,  imm8
    case 0xa9: op_write_ucode(op, AND); break; //TEST eAX,  imm16

    case 0x27: op_write_ucode(op, DAA); break; //DAA

    case 0x28: //SUB  r/m8,  reg8
    case 0x29: //SUB  r/m16,  reg16
    case 0x2a: //SUB  reg8,  r/m8
    case 0x2b: //SUB  reg16,  r/m16
    case 0x2c: //SUB AL,  imm8
    case 0x2d: //SUB eAX,  imm16
    case 0x38: //CMP  r/m8,  reg8
    case 0x39: //CMP  r/m16,  reg16
    case 0x3a: //CMP  reg8,  r/m8
    case 0x3b: //CMP  reg16,  r/m16
    case 0x3c: //CMP AL,  imm8
    case 0x3d: op_write_ucode(op, SUB); break; //CMP eAX,  imm16

    case 0x2f: op_write_ucode(op, DAS); break; //DAS

    case 0x30: //XOR  r/m8,  reg8
    case 0x31: //XOR  r/m16,  reg16
    case 0x32: //XOR  reg8,  r/m8
    case 0x33: //XOR  reg16,  r/m16
    case 0x34: //XOR AL,  imm8
    case 0x35: op_write_ucode(op, XOR); break; //XOR eAX,  imm16

    case 0x37: op_write_ucode(op, AAA); break; //AAA
    case 0x3f: op_write_ucode(op, AAS); break; //AAS

    case 0x40: //INC eAX
    case 0x41: //INC eCX
    case 0x42: //INC eDX
    case 0x43: //INC eBX
    case 0x44: //INC eSP
    case 0x45: //INC eBP
    case 0x46: //INC eSI
    case 0x47: op_write_ucode(op, INC); break; //INC eDI

    case 0x48: //DEC eAX
    case 0x49: //DEC eCX
    case 0x4a: //DEC eDX
    case 0x4b: //DEC eBX
    case 0x4c: //DEC eSP
    case 0x4d: //DEC eBP
    case 0x4e: //DEC eSI
    case 0x4f: op_write_ucode(op, DEC); break; //DEC eDI

    case 0x06: //PUSH ES
    case 0x0e: //PUSH CS
    case 0x16: //PUSH SS
    case 0x1e: //PUSH DS
    case 0x50: //PUSH AX
    case 0x51: //PUSH CX
    case 0x52: //PUSH DX
    case 0x53: //PUSH BX
    case 0x54: //PUSH SP
    case 0x55: //PUSH BP
    case 0x56: //PUSH SI
    case 0x57: //PUSH DI
    case 0x68: //PUSH  imm16
    case 0x6a: //PUSH  imm8
        op_write_ucode(op, PUSH);
        break;

    case 0x9c: //PUSHF
        op_write_ucode(op, PUSHF);
        break;

    case 0x07: //POP ES
    case 0x17: //POP SS
    case 0x1f: //POP DS
    case 0x58: //POP AX
    case 0x59: //POP CX
    case 0x5a: //POP DX
    case 0x5b: //POP BX
    case 0x5c: //POP SP
    case 0x5d: //POP BP
    case 0x5e: //POP SI
    case 0x5f: //POP DI
    case 0x8f: //POP  r/m16
        op_write_ucode(op, POP);
        break;

    case 0x9d: //POPF
        op_write_ucode(op, POPF);
        break;

    case 0x60: //PUSHA
        //op_write_ucode(op, PUSHA);
        assert(false);
        break;

    case 0x61: //POPA
        //op_write_ucode(op, POPA);
        assert(false);
        break;

    case 0x69: //IMUL  reg16,  r/m16,  imm16
    case 0x6b: //IMUL  reg16,  r/m16,  imm8
        op_write_ucode(op, IMUL);
        break;

    case 0x6c: //INSB
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_INSB_A16);
        } else {
            op_write_ucode(op, INSB_A16);
        }
        break;

    case 0x6d: //INSW
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_INSW_A16);
        } else {
            op_write_ucode(op, INSW_A16);
        }
        break;

    case 0x6e: //OUTSB
        op_write_ucode(op, OUTSB_A16);
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP);
        }
        break;

    case 0x6f: //OUTS DX, Xv

        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_OUTSW_A16);
        } else {
            op_write_ucode(op, OUTSW_A16);
        }
        break;

    case 0x70: op_write_ucode(op, JO_O8); break;  //JO  Jb
    case 0x71: op_write_ucode(op, JNO_O8); break; //JNO Jb
    case 0x72: op_write_ucode(op, JC_O8); break;  //JC  Jb
    case 0x73: op_write_ucode(op, JNC_O8); break; //JNC Jb
    case 0x74: op_write_ucode(op, JZ_O8); break;  //JZ  Jb
    case 0x75: op_write_ucode(op, JNZ_O8); break; //JNZ Jb
    case 0x76: op_write_ucode(op, JNA_O8); break; //JNA Jb
    case 0x77: op_write_ucode(op, JA_O8); break;  //JA  Jb
    case 0x78: op_write_ucode(op, JS_O8); break;  //JS  Jb
    case 0x79: op_write_ucode(op, JNS_O8); break; //JNS Jb
    case 0x7a: op_write_ucode(op, JP_O8); break;  //JP  Jb
    case 0x7b: op_write_ucode(op, JNP_O8); break; //JNP Jb
    case 0x7c: op_write_ucode(op, JL_O8); break;  //JL  Jb
    case 0x7d: op_write_ucode(op, JNL_O8); break; //JNL Jb
    case 0x7e: op_write_ucode(op, JNG_O8); break; //JNG Jb
    case 0x7f: op_write_ucode(op, JG_O8); break;  //JG  Jb

    case 0x80: //IMM GP1  r/m8,  imm8
    case 0x81: //IMM GP1  r/m16,  imm16
    case 0x82: //IMM GP1  r/m8,  imm8
    case 0x83: //IMM GP1  r/m16,  imm8 (will have been sign extended to short/int)
        switch (modrm & 0x38) {
        case 0x00:
            op_write_ucode(op, ADD); break;
        case 0x08:
            op_write_ucode(op, OR); break;
        case 0x10:
            op_write_ucode(op, ADC); break;
        case 0x18:
            op_write_ucode(op, SBB); break;
        case 0x20:
            op_write_ucode(op, AND); break;
        case 0x28:
        case 0x38: //CMP
            op_write_ucode(op, SUB); break;
        case 0x30:
            op_write_ucode(op, XOR); break;
        }
        break;

    case 0x98: //CBW/CWDE
        op_write_ucode(op, LOAD0_AL);
        op_write_ucode(op, SIGN_EXTEND_8_16);
        op_write_ucode(op, STORE0_AX);
        break;

    case 0x99:
        op_write_ucode(op, CWD);
        break;

    case 0x9a: //CALLF
        op_write_ucode(op, JUMP_FAR_O16); break;

    case 0x9b: op_write_ucode(op, FWAIT); break; //FWAIT

    case 0x9e: op_write_ucode(op, SAHF); break;
    case 0x9f: op_write_ucode(op, LAHF); break;

    case 0xa4: //MOVSB
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_MOVSB_A16);
        } else {
            op_write_ucode(op, MOVSB_A16);
        }
        break;

    case 0xa5: //MOVSW/D
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_MOVSW_A16);
        } else {
            op_write_ucode(op, MOVSW_A16);
        }
        break;

    case 0xa6: //CMPSB
        if ((prefices & PREFICES_REPE) != 0) {
            op_write_ucode(op, REPE_CMPSB_A16);
        } else if ((prefices & PREFICES_REPNE) != 0) {
            op_write_ucode(op, REPNE_CMPSB_A16);
        } else {
            op_write_ucode(op, CMPSB_A16);
        }
        break;

    case 0xa7: //CMPSW
        if ((prefices & PREFICES_REPE) != 0) {
            op_write_ucode(op, REPE_CMPSW_A16);
        } else if ((prefices & PREFICES_REPNE) != 0) {
            op_write_ucode(op, REPNE_CMPSW_A16);
        } else {
            op_write_ucode(op, CMPSW_A16);
        }
        break;

    case 0xaa: //STOSB
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_STOSB_A16);
        } else {
            op_write_ucode(op, STOSB_A16);
        }
        break;

    case 0xab: //STOSW
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_STOSW_A16);
        } else {
            op_write_ucode(op, STOSW_A16);
        }
        break;

    case 0xac: //LODSB
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_LODSB_A16);
        } else {
            op_write_ucode(op, LODSB_A16);
        }
        break;

    case 0xad: //LODSW
        if ((prefices & PREFICES_REP) != 0) {
            op_write_ucode(op, REP_LODSW_A16);
        } else {
            op_write_ucode(op, LODSW_A16);
        }
        break;

    case 0xae: //SCASB
        if ((prefices & PREFICES_REPE) != 0) {
            op_write_ucode(op, REPE_SCASB_A16);
        } else if ((prefices & PREFICES_REPNE) != 0) {
            op_write_ucode(op, REPNE_SCASB_A16);
        } else {
            op_write_ucode(op, SCASB_A16);
        }
        break;

    case 0xaf: //SCASW
        if ((prefices & PREFICES_REPE) != 0) {
            op_write_ucode(op, REPE_SCASW_A16);
        } else if ((prefices & PREFICES_REPNE) != 0) {
            op_write_ucode(op, REPNE_SCASW_A16);
        } else {
            op_write_ucode(op, SCASW_A16);
        }
        break;

    case 0xc0:
    case 0xd0:
    case 0xd2:
        switch (modrm & 0x38) {
        case 0x00:
            op_write_ucode(op, ROL_O8); break;
        case 0x08:
            op_write_ucode(op, ROR_O8); break;
        case 0x10:
            op_write_ucode(op, RCL_O8); break;
        case 0x18:
            op_write_ucode(op, RCR_O8); break;
        case 0x20:
            op_write_ucode(op, SHL); break;
        case 0x28:
            op_write_ucode(op, SHR); break;
        case 0x30:
            //LOGGING.log(Level.FINE, "invalid SHL encoding");
            op_write_ucode(op, SHL); break;
        case 0x38:
            op_write_ucode(op, SAR_O8); break;
        }
        break;

    case 0xc1:
    case 0xd1:
    case 0xd3:

        switch (modrm & 0x38) {
        case 0x00:
            op_write_ucode(op, ROL_O16);
            break;
        case 0x08:
            op_write_ucode(op, ROR_O16);
            break;
        case 0x10:
            op_write_ucode(op, RCL_O16);
            break;
        case 0x18:
            op_write_ucode(op, RCR_O16);
            break;
        case 0x20:
            op_write_ucode(op, SHL);
            break;
        case 0x28:
            op_write_ucode(op, SHR);
            break;
        case 0x30:
            //LOGGING.log(Level.FINE, "invalid SHL encoding");
            op_write_ucode(op, SHL);
            break;
        case 0x38:
            op_write_ucode(op, SAR_O16);
            break;
        }
        break;

    case 0xc2: // RET Iw
    case 0xca: // RETF Iw
        op_write_ucode(op, ADD_TO_SP);
        op_write_arg(op, immediate);
        break;

    case 0xcc:
        op_write_ucode(op, JUMP_FAR_O16);
        break;

    case 0xcd:
        op_write_ucode(op, JUMP_FAR_O16);
        break;

    case 0xce:
        op_write_ucode(op, INTO_O16_A16);
        break;

    case 0xcf:
        //op_write_ucode(op, IRET_O16_A16);
        break;

    case 0xd4:
        op_write_ucode(op, AAM);
        break; //AAM
    case 0xd5:
        op_write_ucode(op, AAD);
        break; //AAD

    case 0xd6:
        op_write_ucode(op, SALC);
        break; //SALC

    case 0xe0: //LOOPNZ Jb
        op_write_ucode(op, LOOPNZ_CX);
        break;

    case 0xe1: //LOOPZ Jb
        op_write_ucode(op, LOOPZ_CX);
        break;

    case 0xe2: //LOOP Jb
        op_write_ucode(op, LOOP_CX);
        break;

    case 0xe3: //JCXZ
        op_write_ucode(op, JCXZ);
        break;

    case 0xe4: //IN AL,  imm8
    case 0xec:
        op_write_ucode(op, IN_O8);
        break; //IN AL, DX

    case 0xe5: //IN eAX,  imm8
    case 0xed: //IN eAX, DX
        op_write_ucode(op, IN_O16);
        break;

    case 0xe6: //OUT  imm8, AL
    case 0xee:
        op_write_ucode(op, OUT_O8);
        break; //OUT DX, AL

    case 0xe7: //OUT  imm8, AX
    case 0xef: //OUT DX, AX
        op_write_ucode(op, OUT_O16);
        break;

    case 0xe8: //CALL Jv
        op_write_ucode(op, JUMP_O16);
        break;

    case 0xe9: //JMP Jv
        op_write_ucode(op, JUMP_O16);
        break;

    case 0xea: //JMPF Ap
        op_write_ucode(op, JUMP_FAR_O16);
        break;

    case 0xeb:
        op_write_ucode(op, JUMP_O8);
        break; //JMP Jb

    case 0xf4:
        op_write_ucode(op, HALT);
        break; //HLT

    case 0xf5:
        op_write_ucode(op, CMC);
        break; //CMC

    case 0xf6: //UNA GP3  r/m8
        switch (modrm & 0x38) {
        case 0x00:
            op_write_ucode(op, AND);
            break;
        case 0x10:
            op_write_ucode(op, NOT);
            break;
        case 0x18:
            op_write_ucode(op, NEG);
            break;
        case 0x20:
            op_write_ucode(op, MUL_O8);
            break;
        case 0x28:
            op_write_ucode(op, IMULA_O8);
            break;
        case 0x30:
            op_write_ucode(op, DIV_O8);
            break;
        case 0x38:
            op_write_ucode(op, IDIV_O8);
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //throw new IllegalStateException("Invalid Gp 3 Instruction?");
        }
        break;

    case 0xf7: //UNA GP3  r/m16

        switch (modrm & 0x38) {
        case 0x00:
            op_write_ucode(op, AND);
            break;
        case 0x10:
            op_write_ucode(op, NOT);
            break;
        case 0x18:
            op_write_ucode(op, NEG);
            break;
        case 0x20:
            op_write_ucode(op, MUL_O16);
            break;
        case 0x28:
            op_write_ucode(op, IMULA_O16);
            break;
        case 0x30:
            op_write_ucode(op, DIV_O16);
            break;
        case 0x38:
            op_write_ucode(op, IDIV_O16);
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //throw new IllegalStateException("Invalid Gp 3 Instruction?");
        }
        break;

    case 0xf8: op_write_ucode(op, CLC); break; //CLC
    case 0xf9: op_write_ucode(op, STC); break; //STC
    case 0xfa: op_write_ucode(op, CLI); break; //CLI
    case 0xfb: op_write_ucode(op, STI); break; //STI
    case 0xfc: op_write_ucode(op, CLD); break; //CLD
    case 0xfd: op_write_ucode(op, STD); break; //STD

    case 0xfe:
        switch (modrm & 0x38) {
        case 0x00: //INC  r/m8
            op_write_ucode(op, INC); break;
        case 0x08: //DEC  r/m8
            op_write_ucode(op, DEC); break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //throw new IllegalStateException("Invalid Gp 4 Instruction?");
        }
        break;

    case 0xff:
        switch (modrm & 0x38) {
        case 0x00: //INC  r/m16
            op_write_ucode(op, INC);
            break;
        case 0x08: //DEC  r/m16
            op_write_ucode(op, DEC);
            break;
        case 0x10:
            op_write_ucode(op, JUMP_ABS_O16);
            break;
        case 0x18:
            op_write_ucode(op, JUMP_FAR_O16);
            break;
        case 0x20:
            op_write_ucode(op, JUMP_ABS_O16);
            break;
        case 0x28:
            op_write_ucode(op, JUMP_FAR_O16);
            break;
        case 0x30:
            op_write_ucode(op, PUSH);
            break;
        case 0xff:
            op_write_ucode(op, UNDEFINED);
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //System.out.println("Possibly jumped into unwritten memory...");
            //throw new IllegalStateException("Invalid Gp 5 Instruction? " + modrm);
        }
        break;

    case 0x63: op_write_ucode(op, UNDEFINED); break; //ARPL
    case 0x90:
        op_write_ucode(op, MEM_RESET); //use mem_reset as Nop
        break;
    case 0x86: //XCHG  r/m8,  reg8
    case 0x87: //XCHG  r/m16,  reg16
    case 0x88: //MOV  r/m8,  reg8
    case 0x89: //MOV  r/m16,  reg16
    case 0x8a: //MOV  reg8,  r/m8
    case 0x8b: //MOV  reg16,  r/m16
    case 0x8c: //MOV Ew, Sw
    case 0x8d: //LEA  reg16, M
    case 0x8e: //MOV Sw, Ew

    case 0x91: //XCHG AX, CX
    case 0x92: //XCHG AX, CX
    case 0x93: //XCHG AX, CX
    case 0x94: //XCHG AX, CX
    case 0x95: //XCHG AX, CX
    case 0x96: //XCHG AX, CX
    case 0x97: //XCHG AX, CX

    case 0xa0: //MOV AL, Ob
    case 0xa1: //MOV AX, Ov
    case 0xa2: //MOV Ob, AL
    case 0xa3: //MOV Ov, AX

    case 0xb0: //MOV AL,  imm8
    case 0xb1: //MOV CL,  imm8
    case 0xb2: //MOV DL,  imm8
    case 0xb3: //MOV BL,  imm8
    case 0xb4: //MOV AH,  imm8
    case 0xb5: //MOV CH,  imm8
    case 0xb6: //MOV DH,  imm8
    case 0xb7: //MOV BH,  imm8

    case 0xb8: //MOV eAX,  imm16
    case 0xb9: //MOV eCX,  imm16
    case 0xba: //MOV eDX,  imm16
    case 0xbb: //MOV eBX,  imm16
    case 0xbc: //MOV eSP,  imm16
    case 0xbd: //MOV eBP,  imm16
    case 0xbe: //MOV eSI,  imm16
    case 0xbf: //MOV eDI,  imm16

    case 0xc3: //RET
    case 0xc4: //LES
    case 0xc5: //LDS
    case 0xc6: //MOV GP11  r/m8,  reg8
    case 0xc7: //MOV GP11  r/m16,  reg16
    case 0xcb: //RET

    case 0xd7: //XLAT
        break;

    default:
        printf("Missing opcode: %x\n", opcode);
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Missing Operation: 0x" + Integer.toHexString(opcode));

    }
}

static void op_write_output_args(dec_p dec, op_p op, inst_p inst)
{

    int prefices     = inst->inst_prefices;
    int opcode       = inst->inst_opcode;
    int modrm        = inst->inst_modrm;
    int displacement = inst->inst_disp;
    //Normal One Byte Operation
    switch (opcode) {
    case 0x00: //ADD  r/m8,  reg8
    case 0x08: //OR   r/m8,  reg8
    case 0x10: //ADC  r/m8,  reg8
    case 0x18: //SBB  r/m8,  reg8
    case 0x20: //AND  r/m8,  reg8
    case 0x28: //SUB  r/m8,  reg8
    case 0x30: //XOR  r/m8,  reg8
    case 0x88: //MOV   r/m8,  reg8
    case 0xc0: //SFT G2  r/m8,  imm8
    case 0xc6: //MOV G11  r/m8,  imm8
    case 0xfe: //INC/DEC G4  r/m8
        store0_Eb(op, prefices, modrm, displacement);
        break;

    case 0x80: //IMM G1  r/m8,  imm8
    case 0x82: //IMM G1  r/m8,  imm8
        if ((modrm & 0x38) == 0x38)
            break;
        store0_Eb(op, prefices, modrm, displacement); break;

    case 0x86: //XCHG  r/m8,  reg8
        store0_Gb(op, modrm);
        store1_Eb(op, prefices, modrm, displacement);
        break;

    case 0x02: //ADD  reg8,  r/m8
    case 0x0a: //OR   reg8,  r/m8
    case 0x12: //ADC  reg8,  r/m8
    case 0x1a: //SBB  reg8,  r/m8
    case 0x22: //AND  reg8,  r/m8
    case 0x2a: //SUB  reg8,  r/m8
    case 0x32: //XOR  reg8,  r/m8
    case 0x8a: //MOV  reg8,  r/m8
        store0_Gb(op, modrm);
        break;

    case 0x01: //ADD  r/m16,  reg16
    case 0x09: //OR   r/m16,  reg16
    case 0x11: //ADC  r/m16,  reg16
    case 0x19: //SBB  r/m16,  reg16
    case 0x21: //AND  r/m16,  reg16
    case 0x29: //SUB  r/m16,  reg16
    case 0x31: //XOR  r/m16,  reg16
    case 0x89: //MOV   r/m16,  reg16
    case 0xc7: //MOV G11  r/m16,  imm16
    case 0xc1: //SFT G2  r/m16,  imm8
    case 0x8f: //POP  r/m16
    case 0xd1: //SFT G2  r/m16, 1
    case 0xd3: //SFT G2  r/m16, CL
        store0_Ew(op, prefices, modrm, displacement);
        break;

    case 0x81: //IMM G1  r/m16,  imm16
    case 0x83: //IMM G1  r/m16,  imm8
        if ((modrm & 0x38) == 0x38) //CMP G1  r/m16, I?
            break;
        store0_Ew(op, prefices, modrm, displacement);
        break;

    case 0x87: //XCHG  r/m16,  reg16
        store0_Gw(op, modrm);
        store1_Ew(op, prefices, modrm, displacement);
        break;

    case 0x03: //ADD  reg16,  r/m16
    case 0x0b: //OR   reg16,  r/m16
    case 0x13: //ADC  reg16,  r/m16
    case 0x1b: //SBB  reg16,  r/m16
    case 0x23: //AND  reg16,  r/m16
    case 0x2b: //SUB  reg16,  r/m16
    case 0x33: //XOR  reg16,  r/m16
    case 0x69: //IMUL  reg16,  r/m16,  imm16
    case 0x6b: //IMUL  reg16,  r/m16,  imm8
    case 0x8b: //MOV  reg16,  r/m16
    case 0x8d: //LEA  reg16, M
        store0_Gw(op, modrm);
        break;
    case 0xec: //IN AL, DX
    case 0x04: //ADD AL,  imm8
    case 0x0c: //OR  AL,  imm8
    case 0x14: //ADC AL,  imm8
    case 0x1c: //SBB AL,  imm8
    case 0x24: //AND AL,  imm8
    case 0x2c: //SUB AL,  imm8
    case 0x34: //XOR AL,  imm8
    case 0xe4: //IN  AL,  imm8
    case 0xb0: //MOV AL,  imm8
        op_write_ucode(op, STORE0_AL);
        break;

    case 0xb1: //MOV CL,  imm8
        op_write_ucode(op, STORE0_CL);
        break;

    case 0xb2: //MOV DL,  imm8
        op_write_ucode(op, STORE0_DL);
        break;

    case 0xb3: //MOV BL,  imm8
        op_write_ucode(op, STORE0_BL);
        break;

    case 0xb4: //MOV AH,  imm8
        op_write_ucode(op, STORE0_AH);
        break;

    case 0xb5: //MOV CH,  imm8
        op_write_ucode(op, STORE0_CH);
        break;

    case 0xb6: //MOV DH,  imm8
        op_write_ucode(op, STORE0_DH);
        break;

    case 0xb7: //MOV BH,  imm8
        op_write_ucode(op, STORE0_BH);
        break;



    case 0x05: //ADD eAX,  imm16
    case 0x0d: //OR  eAX,  imm16
    case 0x15: //ADC eAX,  imm16
    case 0x1d: //SBB eAX,  imm16
    case 0x25: //AND eAX,  imm16
    case 0x2d: //SUB eAX,  imm16
    case 0x35: //XOR eAX,  imm16
    case 0xb8: //MOV eAX,  imm16
    case 0xe5: //IN eAX,  imm8
    case 0x40: //INC eAX
    case 0x48: //DEC eAX
    case 0x58: //POP eAX
    case 0xed: //IN eAX, DX
        op_write_ucode(op, STORE0_AX);
        break;

    case 0x41: //INC eCX
    case 0x49: //DEC eCX
    case 0x59: //POP eCX
    case 0xb9: //MOV eCX,  imm16
        op_write_ucode(op, STORE0_CX);
        break;

    case 0x42: //INC eDX
    case 0x4a: //DEC eDX
    case 0x5a: //POP eDX
    case 0xba: //MOV eDX,  imm16
        op_write_ucode(op, STORE0_DX);
        break;

    case 0x43: //INC eBX
    case 0x4b: //DEC eBX
    case 0x5b: //POP eBX
    case 0xbb: //MOV eBX,  imm16
        op_write_ucode(op, STORE0_BX);
        break;

    case 0x44: //INC eSP
    case 0x4c: //DEC eSP
    case 0x5c: //POP eSP
    case 0xbc: //MOV eSP,  imm16
        op_write_ucode(op, STORE0_SP);
        break;

    case 0x45: //INC eBP
    case 0x4d: //DEC eBP
    case 0x5d: //POP eBP
    case 0xbd: //MOV eBP,  imm16
        op_write_ucode(op, STORE0_BP);
        break;

    case 0x46: //INC eSI
    case 0x4e: //DEC eSI
    case 0x5e: //POP eSI
    case 0xbe: //MOV eSI,  imm16
        op_write_ucode(op, STORE0_SI);
        break;

    case 0x47: //INC eDI
    case 0x4f: //DEC eDI
    case 0x5f: //POP eDI
    case 0xbf: //MOV eDI,  imm16
        op_write_ucode(op, STORE0_DI);
        break;


    case 0x91: //XCHG eAX, eCX
        op_write_ucode(op, STORE0_CX);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x92: //XCHG eAX, eDX
        op_write_ucode(op, STORE0_DX);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x93: //XCHG eAX, eBX
        op_write_ucode(op, STORE0_BX);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x94: //XCHG eAX, eSP
        op_write_ucode(op, STORE0_SP);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x95: //XCHG eAX, eBP
        op_write_ucode(op, STORE0_BP);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x96: //XCHG eAX, eSI
        op_write_ucode(op, STORE0_SI);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x97: //XCHG eAX, eDI
        op_write_ucode(op, STORE0_DI);
        op_write_ucode(op, STORE1_AX);
        break;

    case 0x9d: //POPF
        op_write_ucode(op, STORE0_FLAGS); break;

    case 0xd0: //SFT G2  r/m8, 1
    case 0xd2: //SFT G2  r/m8, CL
        store0_Eb(op, prefices, modrm, displacement);
        break;



    case 0xf6: //UNA G3  r/m8, ?
        switch (modrm & 0x38) {
        case 0x10:
        case 0x18:
            store0_Eb(op, prefices, modrm, displacement);
            break;
        }
        break;

    case 0xf7: //UNA G3  r/m16, ?
        switch (modrm & 0x38) {
        case 0x10:
        case 0x18:
            store0_Ew(op, prefices, modrm, displacement);
            break;
        }
        break;


    case 0x07: //POP ES
        op_write_ucode(op, STORE0_ES);
        break;

    case 0x17: //POP SS
        op_write_ucode(op, STORE0_SS);
        break;

    case 0x1f: //POP DS
        op_write_ucode(op, STORE0_DS);
        break;

    case 0x8c: //MOV Ew, Sw
        store0_Ew(op, prefices, modrm, displacement); 
        break;

    case 0x8e: //MOV Sw, Ew
        store0_Sw(op, modrm);
        break;

    case 0xa0: //MOV AL, Ob
        op_write_ucode(op, STORE0_AL);
        break;

    case 0xa2: //MOV Ob, AL
        store0_Ob(op, prefices, displacement);
        break;

    case 0xa1: //MOV eAX, Ov
        op_write_ucode(op, STORE0_AX);
        break;

    case 0xa3: //MOV Ov, eAX
        store0_Ow(op, prefices, displacement);
        break;

    case 0xff: //INC/DEC G5
        switch (modrm & 0x38) {
        case 0x00:
        case 0x08:
            store0_Ew(op, prefices, modrm, displacement);
        }
        break;

    case 0xc4: //LES  reg16, Mp
        store0_Gw(op, modrm);
        op_write_ucode(op, STORE1_ES);
        break;
    }
}

int operation_decode_inst(op_p op, inst_p in)
{
    dec_s dec;
    dec.dec_amode_decoded = false;

    op_write_preamble(&dec, op, in);
    op_write_input_args(&dec, op, in);
    op_write_operation(&dec, op, in);
    op_write_output_args(&dec, op, in);
    return 0;
}

