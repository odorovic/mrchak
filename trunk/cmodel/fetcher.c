
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "errors.h"
#include "prefices.h"
#include "instruction.h"

#include "fetcher.h"

static int s_modrm_array[] = {
    true,  true,  true,  true,  false, false, false, false, true,  true,  true,
    true,  false, false, false, false, true,  true,  true,  true,  false, false,
    false, false, true,  true,  true,  true,  false, false, false, false, true,
    true,  true,  true,  false, false, false, false, true,  true,  true,  true,
    false, false, false, false, true,  true,  true,  true,  false, false, false,
    false, true,  true,  true,  true,  false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, true,  false, true,  false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, true,  true,  false, false, true,  true,
    true,  true,  false, false, false, false, false, false, false, false, true,
    true,  true,  true,  false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, true,  true,  false, false, false, false, false,
    false, true,  true};


static int op_has_imm(inst_p cur){
    uint8_t opcode   = cur->inst_opcode;
    uint8_t modrm    = cur->inst_modrm;

    switch (opcode) {
    case 0x04: //ADD AL, Ib
    case 0x0c: //OR  AL, Ib
    case 0x14: //ADC AL, Ib
    case 0x1c: //SBB AL, Ib
    case 0x24: //AND AL, Ib
    case 0x2c: //SUB AL, Ib
    case 0x34: //XOR AL, Ib
    case 0x3c: //CMP AL, Ib
    case 0x6a: //PUSH Ib
    case 0x6b: //IMUL Gv, Ev, Ib
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
    case 0x80: //IMM G1 Eb, Ib
    case 0x82: //IMM G1 Eb, Ib
    case 0x83: //IMM G1 Ev, Ib
    case 0xa8: //TEST AL, Ib
    case 0xb0: //MOV AL, Ib
    case 0xb1: //MOV CL, Ib
    case 0xb2: //MOV DL, Ib
    case 0xb3: //MOV BL, Ib
    case 0xb4: //MOV AH, Ib
    case 0xb5: //MOV CH, Ib
    case 0xb6: //MOV DH, Ib
    case 0xb7: //MOV BH, Ib
    case 0xc0: //SFT G2 Eb, Ib
    case 0xc1: //SFT G2 Ev, Ib
    case 0xc6: //MOV G11 Eb, Ib
    case 0xcd: //INT Ib
    case 0xd4: //AAM Ib
    case 0xd5: //AAD Ib
    case 0xe0: //LOOPNZ Jb
    case 0xe1: //LOOPZ Jb
    case 0xe2: //LOOP Jb
    case 0xe3: //JCXZ Jb
    case 0xe4: //IN  AL, Ib
    case 0xe5: //IN eAX, Ib
    case 0xe6: //OUT Ib, AL
    case 0xe7: //OUT Ib, eAX
    case 0xeb: //JMP Jb
        return 1;

    case 0xc2: //RET Iw
    case 0xca: //RETF Iw
        return 2;

    case 0x05: //ADD eAX, Iv
    case 0x0d: //OR  eAX, Iv
    case 0x15: //ADC eAX, Iv
    case 0x1d: //SBB eAX, Iv
    case 0x25: //AND eAX, Iv
    case 0x2d: //SUB eAX, Iv
    case 0x35: //XOR eAX, Iv
    case 0x3d: //CMP eAX, Iv
    case 0x68: //PUSH Iv
    case 0x69: //IMUL Gv, Ev, Iv
    case 0x81: //IMM G1 Ev, Iv
    case 0xa9: //TEST eAX, Iv
    case 0xb8: //MOV eAX, Iv
    case 0xb9: //MOV eCX, Iv
    case 0xba: //MOV eDX, Iv
    case 0xbb: //MOV eBX, Iv
    case 0xbc: //MOV eSP, Iv
    case 0xbd: //MOV eBP, Iv
    case 0xbe: //MOV eSI, Iv
    case 0xbf: //MOV eDI, Iv
    case 0xc7: //MOV G11 Ev, Iv
    case 0xe8: //CALL Jv
    case 0xe9: //JMP  Jv
        return 2;

    case 0x9a: //CALLF Ap
    case 0xea: //JMPF Ap
        return 4;

    case 0xf6: //UNA G3 Eb, ?
        switch (modrm & 0x38) {
        case 0x00: //TEST Eb, Ib
            return 1;
        default:
            return 0;
        }

    case 0xf7: //UNA G3 Ev, ?
        switch (modrm & 0x38) {
        case 0x00: //TEST Ev, Iv
            return 2;
        default:
            return 0;
        }
    }
    return 0;
}

static int modrm_has_disp(uint8_t prefices, uint8_t modrm)
{
    //16 bit address size
    switch(modrm & 0xc0) {
    case 0x00:
        if ((modrm & 0x7) == 0x6)
            return 2;
        else
            return 0;
    case 0x40: return 1; //IB
    case 0x80: return 2; //IW
    }

    return 0;
}

static int op_has_disp(inst_p cur){
    uint8_t prefices = cur->inst_prefices;
    uint8_t opcode   = cur->inst_opcode;
    uint8_t modrm    = cur->inst_modrm;

    switch (opcode) {
        //modrm things
    case 0x00: //ADD  Eb, Gb
    case 0x01: //ADD Ev, Gv
    case 0x02: //ADD Gb, Eb
    case 0x03: //ADD Gv, Ev
    case 0x08: //OR   Eb, Gb
    case 0x09: //OR  Ev, Gv
    case 0x0a: //OR  Gb, Eb
    case 0x0b: //OR  Gv, Ev
    case 0x10: //ADC  Eb, Gb
    case 0x11: //ADC Ev, Gv
    case 0x12: //ADC Gb, Eb
    case 0x13: //ADC Gv, Ev
    case 0x18: //SBB  Eb, Gb
    case 0x19: //SBB Ev, Gv
    case 0x1a: //SBB Gb, Eb
    case 0x1b: //SBB Gv, Ev
    case 0x20: //AND  Eb, Gb
    case 0x21: //AND Ev, Gv
    case 0x22: //AND Gb, Eb
    case 0x23: //AND Gv, Ev
    case 0x28: //SUB  Eb, Gb
    case 0x29: //SUB Ev, Gv
    case 0x2a: //SUB Gb, Eb
    case 0x2b: //SUB Gv, Ev
    case 0x30: //XOR  Eb, Gb
    case 0x31: //XOR Ev, Gv
    case 0x32: //XOR Gb, Eb
    case 0x33: //XOR Gv, Ev
    case 0x38: //CMP  Eb, Gb
    case 0x39: //CMP  Ev, Gv
    case 0x3a: //CMP Gb, Eb
    case 0x3b: //CMP Gv, Ev
    case 0x69: //IMUL Gv, Ev, Iv
    case 0x6b: //IMUL Gv, Ev, Ib
    case 0x80: //IMM G1 Eb, Ib
    case 0x81: //IMM G1 Ev, Iv
    case 0x82: //IMM G1 Eb, Ib
    case 0x83: //IMM G1 Ev, Ib
    case 0x84: //TEST Eb, Gb
    case 0x85: //TEST Ev, Gv
    case 0x86: //XCHG Eb, Gb
    case 0x87: //XCHG Ev, Gv
    case 0x88: //MOV  Eb, Gb
    case 0x89: //MOV  Ev, Gv
    case 0x8a: //MOV Gb, Eb
    case 0x8b: //MOV Gv, Ev
    case 0x8c: //MOV Ew, Sw
    case 0x8d: //LEA Gv, M
    case 0x8e: //MOV Sw, Ew
    case 0x8f: //POP Ev
    case 0xc0: //SFT G2 Eb, Ib
    case 0xc1: //SFT G2 Ev, Ib
    case 0xc4: //LES Gv, Mp
    case 0xc5: //LDS Gv, Mp
    case 0xc6: //MOV G11 Eb, Ib
    case 0xc7: //MOV G11 Ev, Iv
    case 0xd0: //SFT G2 Eb, 1
    case 0xd1: //SFT G2 Ev, 1
    case 0xd2: //SFT G2 Eb, CL
    case 0xd3: //SFT G2 Ev, CL
    case 0xf6: //UNA G3 Eb, ?
    case 0xf7: //UNA G3 Ev, ?
    case 0xfe: //INC/DEC G4 Eb
    case 0xff: //INC/DEC G5
        return modrm_has_disp(prefices, modrm);

    //special cases
    case 0xa0: //MOV AL, Ob
    case 0xa2: //MOV Ob, AL
    case 0xa1: //MOV AX, Ov
    case 0xa3: //MOV Ov, AX
        return 2;

    default: return 0;
    }

}

static FETCHER_STATE decode_arg_lens(fetcher_p f){
    int disp_size, imm_size;
    f->fet_inst.inst_disp_size = op_has_disp(&f->fet_inst);
    f->fet_inst.inst_imm_size  = op_has_imm(&f->fet_inst);
    disp_size = f->fet_inst.inst_disp_size;
    imm_size = f->fet_inst.inst_imm_size;
    if(disp_size > 0){
        return FETCH_DISP;
    }else if(imm_size > 0){
        return FETCH_IMM;
    }else{
        return FETCH_DONE;
    }
}


static FETCHER_STATE fetch_prefix(fetcher_p f, uint8_t next){
    uint8_t ir = next;
    FETCHER_STATE next_state;
    switch(ir){
        case 0x2e:
            f->fet_inst.inst_prefices &= ~PREFICES_SG;
            f->fet_inst.inst_prefices |= PREFICES_CS;
            break;
        case 0x3e:
            f->fet_inst.inst_prefices &= ~PREFICES_SG;
            f->fet_inst.inst_prefices |= PREFICES_DS;
            break;
        case 0x26:
            f->fet_inst.inst_prefices &= ~PREFICES_SG;
            f->fet_inst.inst_prefices |= PREFICES_ES;
            break;
        case 0x36:
            f->fet_inst.inst_prefices &= ~PREFICES_SG;
            f->fet_inst.inst_prefices |= PREFICES_SS;
            break;
        case 0x64:
            f->fet_inst.inst_prefices &= ~PREFICES_SG;
            f->fet_inst.inst_prefices |= PREFICES_FS;
            break;
        case 0x65:
            f->fet_inst.inst_prefices &= ~PREFICES_SG;
            f->fet_inst.inst_prefices |= PREFICES_GS;
            break;
        case 0xf2:
            f->fet_inst.inst_prefices |= PREFICES_REPNE;
            break;
        case 0xf3:
            f->fet_inst.inst_prefices |= PREFICES_REPE;
            break;
        case 0xf0:
            f->fet_inst.inst_prefices |= PREFICES_LOCK;
            break;
        default:
            f->fet_inst.inst_opcode = ir;
            break;
    }

    switch(ir){
        //Any prefix gets silently recorded.
        case 0x2e:
        case 0x3e:
        case 0x26:
        case 0x36:
        case 0x64:
        case 0x65:
        case 0xf2:
        case 0xf3:
        case 0xf0:
            next_state  = FETCH_INNITIAL;
            break;
        default:
            if(true == s_modrm_array[ir]){
                next_state = FETCH_MODRM;
            } else{
                next_state = decode_arg_lens(f);
            }
    }
    return next_state;
}

static FETCHER_STATE fetch_modrm(fetcher_p f, uint8_t next){
    f->fet_inst.inst_modrm = next;
    return decode_arg_lens(f);
}

static FETCHER_STATE fetch_disp(fetcher_p f, uint8_t next){
    int idx = f->fet_inst.inst_disp_read++;
    int read = f->fet_inst.inst_disp_read;
    int size = f->fet_inst.inst_disp_size;
    f->fet_inst.inst_disp |= (next << 8*idx) & (0xff << 8*idx);
    if(read == size){
        if(f->fet_inst.inst_imm_size > 0){
            return FETCH_IMM;
        }else{
            return FETCH_DONE;
        }
    }else{
        return FETCH_DISP;
    }
}

static FETCHER_STATE fetch_imm(fetcher_p f, uint8_t next){
    int8_t  byte;
    int16_t word;
    int idx = f->fet_inst.inst_imm_read++;
    int read = f->fet_inst.inst_imm_read;
    int size = f->fet_inst.inst_imm_size;
    f->fet_inst.inst_imm |= (next << 8*idx) & (0xff << 8*idx);
    if(read == size){
        return FETCH_DONE;
    }else{
        return FETCH_IMM;
    }
}

bool fetcher_next(fetcher_p f, uint8_t next, int addr)
{
    int i=0;
    f->fet_inst.inst_bytes[f->fet_inst.inst_bytes_size++] = next;
    switch(f->fet_state){
        case FETCH_INNITIAL:
            f->fet_inst.inst_start_off = (uint16_t)addr;
            f->fet_inst.inst_start_seg = (uint16_t)(addr>>16);
            f->fet_state = fetch_prefix(f, next);
            break;
        case FETCH_MODRM:
            f->fet_state = fetch_modrm(f, next);
            break;
        case FETCH_DISP:
            f->fet_state = fetch_disp(f, next);
            break;
        case FETCH_IMM:
            f->fet_state = fetch_imm(f, next);
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
    }
    if(f->fet_state == FETCH_DONE){
        dump_instruction(&f->fet_inst, stdout);
        return false;
    }else{
        return true;
    }
}

void fetcher_init(fetcher_p f)
{
    inst_s inst;
    inst.inst_prefices   = 0;
    inst.inst_opcode     = 0;
    inst.inst_modrm      = 0;
    inst.inst_disp       = 0;
    inst.inst_disp_size  = 0;
    inst.inst_disp_read  = 0;
    inst.inst_imm        = 0;
    inst.inst_imm_size   = 0;
    inst.inst_imm_read   = 0;

    inst.inst_start_off  = 0;
    inst.inst_start_seg  = 0;
    inst.inst_bytes_size = 0;

    f->fet_state = FETCH_INNITIAL;
    f->fet_inst = inst;

}
