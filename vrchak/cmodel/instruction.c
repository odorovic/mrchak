#include "instruction.h"
#include "errors.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define OPCODE_NAME_LEN  32
#define OPERAND_NAME_LEN 32

typedef enum opcode_t {
    AAA,
    AAD,
    AAM,
    AAS,
    ADC,
    ADD,
    AND,
    CALL,
    CALLF,
    CBW,
    CLC,
    CLD,
    CLI,
    CMC,
    CMP,
    CMPS,
    CWD,
    DAA,
    DAS,
    DEC,
    DIV,
    ESC,
    HLT,
    IDIV,
    IMUL,
    IN,
    INC,
    INS,
    INT,
    INTO,
    IRET,
    JA,
    JAE,
    JB,
    JBE,
    JC,
    JCXZ,
    JE,
    JG,
    JGE,
    JL,
    JLE,
    JMP,
    JMPF,
    JNA,
    JNAE,
    JNB,
    JNBE,
    JNC,
    JNE,
    JNG,
    JNGE,
    JNL,
    JNLE,
    JNO,
    JNP,
    JNS,
    JNZ,
    JO,
    JP,
    JPE,
    JPO,
    JS,
    JZ,
    LAHF,
    LDS,
    LEA,
    LEAVE,
    LES,
    LOCK,
    LODS,
    LOOP,
    LOOPE,
    LOOPNE,
    LOOPZ,
    LOOPNZ,
    MOV,
    MOVS,
    MUL,
    NEG,
    NOP,
    NOT,
    OR,
    OUT,
    OUTS,
    POP,
    POPA,
    POPF,
    PUSH,
    PUSHA,
    PUSHF,
    RCL,
    RCR,
    REP,
    REPE,
    REPNE,
    REPZ,
    RET,
    ROL,
    ROR,
    SAHF,
    SAL,
    SAR,
    SBB,
    SCAS,
    SHL,
    SHR,
    STC,
    STD,
    STI,
    STOS,
    SUB,
    TEST,
    WAIT,
    XCHG,
    XLAT,
    XOR,
    OPCODES_LEN
} OPCODE;

static char opcode_names[OPCODES_LEN][OPCODE_NAME_LEN] = {
    "AAA",
    "AAD",
    "AAM",
    "AAS",
    "ADC",
    "ADD",
    "AND",
    "CALL",
    "CALLF",
    "CBW",
    "CLC",
    "CLD",
    "CLI",
    "CMC",
    "CMP",
    "CMPS",
    "CWD",
    "DAA",
    "DAS",
    "DEC",
    "DIV",
    "ESC",
    "HLT",
    "IDIV",
    "IMUL",
    "IN",
    "INC",
    "INS",
    "INT",
    "INTO",
    "IRET",
    "JA",
    "JAE",
    "JB",
    "JBE",
    "JC",
    "JCXZ",
    "JE",
    "JG",
    "JGE",
    "JL",
    "JLE",
    "JMP",
    "JMPF",
    "JNA",
    "JNAE",
    "JNB",
    "JNBE",
    "JNC",
    "JNE",
    "JNG",
    "JNGE",
    "JNL",
    "JNLE",
    "JNO",
    "JNP",
    "JNS",
    "JNZ",
    "JO",
    "JP",
    "JPE",
    "JPO",
    "JS",
    "JZ",
    "LAHF",
    "LDS",
    "LEA",
    "LEAVE",
    "LES",
    "LOCK",
    "LODS",
    "LOOP",
    "LOOPE",
    "LOOPNE",
    "LOOPZ",
    "LOOPNZ",
    "MOV",
    "MOVS",
    "MUL",
    "NEG",
    "NOP",
    "NOT",
    "OR",
    "OUT",
    "OUTS",
    "POP",
    "POPA",
    "POPF",
    "PUSH",
    "PUSHA",
    "PUSHF",
    "RCL",
    "RCR",
    "REP",
    "REPE",
    "REPNE",
    "REPZ",
    "RET",
    "ROL",
    "ROR",
    "SAHF",
    "SAL",
    "SAR",
    "SBB",
    "SCAS",
    "SHL",
    "SHR",
    "STC",
    "STD",
    "STI",
    "STOS",
    "SUB",
    "TEST",
    "WAIT",
    "XCHG",
    "XLAT",
    "XOR"
};

typedef enum operand_t {
    NO_OPERAND,
    IMM8,
    IMM16,
    M,
    M8,
    M16,
    M16_16,
    MOFFS8,
    MOFFS16,
    PTR16_16,
    R8,
    R16,
    RM8,
    RM16,
    REL8,
    REL16,
    SREG,
    OPERANDS_LEN
} operand_s, *operand_p;

static char operand_names[OPERANDS_LEN][OPERAND_NAME_LEN]  = {
    "",
    "imm8",
    "imm16",
    "m",
    "m8",
    "m16",
    "m16:16",
    "moffs8",
    "moffs16",
    "ptr16:16",
    "r8",
    "r16",
    "r/m8",
    "r/m16",
    "rel8",
    "rel16",
    "sreg"
};

static OPCODE inst_get_opcode_idx(inst_p inst)
{
    uint8_t  opcode    = inst->inst_opcode;
    uint8_t  modrm     = inst->inst_modrm;
    OPCODE   opcode_idx;
    switch (opcode) {
    case 0x00: //ADD  r/m8,  reg8
    case 0x01: //ADD  r/m16,  reg16
    case 0x02: //ADD  reg8,  r/m8
    case 0x03: //ADD  reg16,  r/m16
    case 0x04: //ADD AL,  imm8
    case 0x05: //ADD AX,  imm16
        opcode_idx = ADD;
        break;

    case 0x08: //OR   r/m8,  reg8
    case 0x09: //OR   r/m16,  reg16
    case 0x0a: //OR   reg8,  r/m8
    case 0x0b: //OR   reg16,  r/m16
    case 0x0c: //OR  AL,  imm8
    case 0x0d:
        opcode_idx = OR; //OR  eAX,  imm16
        break;

    case 0x10: //ADC  r/m8,  reg8
    case 0x11: //ADC  r/m16,  reg16
    case 0x12: //ADC  reg8,  r/m8
    case 0x13: //ADC  reg16,  r/m16
    case 0x14: //ADC AL,  imm8
    case 0x15: //ADC eAX,  imm16
        opcode_idx = ADC;
        break;

    case 0x18: //SBB  r/m8,  reg8
    case 0x19: //SBB  r/m16,  reg16
    case 0x1a: //SBB  reg8,  r/m8
    case 0x1b: //SBB  reg16,  r/m16
    case 0x1c: //SBB AL,  imm8
    case 0x1d: opcode_idx = SBB; break; //SBB eAX,  imm16

    case 0x20: //AND  r/m8,  reg8
    case 0x21: //AND  r/m16,  reg16
    case 0x22: //AND  reg8,  r/m8
    case 0x23: //AND  reg16,  r/m16
    case 0x24: //AND AL,  imm8
    case 0x25: //AND eAX,  imm16
    case 0x84: //TEST  r/m8,  reg8
    case 0x85: //TEST  r/m16,  reg16
    case 0xa8: //TEST AL,  imm8
    case 0xa9: opcode_idx = AND; break; //TEST eAX,  imm16

    case 0x27: opcode_idx = DAA; break; //DAA

    case 0x28: //SUB  r/m8,  reg8
    case 0x29: //SUB  r/m16,  reg16
    case 0x2a: //SUB  reg8,  r/m8
    case 0x2b: //SUB  reg16,  r/m16
    case 0x2c: //SUB AL,  imm8
    case 0x2d: //SUB eAX,  imm16
        opcode_idx = SUB;
        break; //CMP eAX,  imm16
    case 0x38: //CMP  r/m8,  reg8
    case 0x39: //CMP  r/m16,  reg16
    case 0x3a: //CMP  reg8,  r/m8
    case 0x3b: //CMP  reg16,  r/m16
    case 0x3c: //CMP AL,  imm8
    case 0x3d:
        opcode_idx = CMP;
        break; //CMP eAX,  imm16

    case 0x2f: opcode_idx = DAS; break; //DAS

    case 0x30: //XOR  r/m8,  reg8
    case 0x31: //XOR  r/m16,  reg16
    case 0x32: //XOR  reg8,  r/m8
    case 0x33: //XOR  reg16,  r/m16
    case 0x34: //XOR AL,  imm8
    case 0x35: opcode_idx = XOR; break; //XOR eAX,  imm16

    case 0x37: opcode_idx = AAA; break; //AAA
    case 0x3f: opcode_idx = AAS; break; //AAS

    case 0x40: //INC eAX
    case 0x41: //INC eCX
    case 0x42: //INC eDX
    case 0x43: //INC eBX
    case 0x44: //INC eSP
    case 0x45: //INC eBP
    case 0x46: //INC eSI
    case 0x47: opcode_idx = INC; break; //INC eDI

    case 0x48: //DEC eAX
    case 0x49: //DEC eCX
    case 0x4a: //DEC eDX
    case 0x4b: //DEC eBX
    case 0x4c: //DEC eSP
    case 0x4d: //DEC eBP
    case 0x4e: //DEC eSI
    case 0x4f: opcode_idx = DEC; break; //DEC eDI

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
        opcode_idx = PUSH;
        break;

    case 0x9c: //PUSHF
        opcode_idx = PUSHF;
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
        opcode_idx = POP;
        break;

    case 0x9d: //POPF
        opcode_idx = POPF;
        break;

    case 0x60: //PUSHA
        opcode_idx = PUSHA;
        break;

    case 0x61: //POPA
        opcode_idx = POPA;
        break;

    case 0x69: //IMUL  reg16,  r/m16,  imm16
    case 0x6b: //IMUL  reg16,  r/m16,  imm8
        opcode_idx = IMUL;
        break;

    case 0x6c: //INSB
        opcode_idx = INS;
        break;

    case 0x6d: //INSW
        opcode_idx = INS;
        break;

    case 0x6e: //OUTSB
        opcode_idx = OUTS;
        break;

    case 0x6f: //OUTS DX, Xv
        opcode_idx = OUTS;
        break;

    case 0x70: opcode_idx = JO;  break;  //JO  Jb
    case 0x71: opcode_idx = JNO; break; //JNO Jb
    case 0x72: opcode_idx = JC;  break;  //JC  Jb
    case 0x73: opcode_idx = JNC; break; //JNC Jb
    case 0x74: opcode_idx = JZ;  break;  //JZ  Jb
    case 0x75: opcode_idx = JNZ; break; //JNZ Jb
    case 0x76: opcode_idx = JNA; break; //JNA Jb
    case 0x77: opcode_idx = JA;  break;  //JA  Jb
    case 0x78: opcode_idx = JS;  break;  //JS  Jb
    case 0x79: opcode_idx = JNS; break; //JNS Jb
    case 0x7a: opcode_idx = JP;  break;  //JP  Jb
    case 0x7b: opcode_idx = JNP; break; //JNP Jb
    case 0x7c: opcode_idx = JL;  break;  //JL  Jb
    case 0x7d: opcode_idx = JNL; break; //JNL Jb
    case 0x7e: opcode_idx = JNG; break; //JNG Jb
    case 0x7f: opcode_idx = JG;  break;  //JG  Jb

    case 0x80: //IMM GP1  r/m8,  imm8
    case 0x81: //IMM GP1  r/m16,  imm16
    case 0x82: //IMM GP1  r/m8,  imm8
    case 0x83: //IMM GP1  r/m16,  imm8 (will have been sign extended to short/int)
        switch (modrm & 0x38) {
        case 0x00:
            opcode_idx = ADD; break;
        case 0x08:
            opcode_idx = OR; break;
        case 0x10:
            opcode_idx = ADC; break;
        case 0x18:
            opcode_idx = SBB; break;
        case 0x20:
            opcode_idx = AND; break;
        case 0x28:
        case 0x38: //CMP
            opcode_idx = SUB; break;
        case 0x30:
            opcode_idx = XOR; break;
        }
        break;

    case 0x98: //CBW/CWDE
        opcode_idx = CBW;
        break;

    case 0x99:
        opcode_idx = CWD;
        break;

    case 0x9a: //CALLF
        opcode_idx = CALLF;
        break;

    case 0x9b: opcode_idx = WAIT; break; //FWAIT

    case 0x9e: opcode_idx = SAHF; break;
    case 0x9f: opcode_idx = LAHF; break;

    case 0xa4: //MOVSB
        opcode_idx = MOVS;
        break;

    case 0xa5: //MOVSW
        opcode_idx = MOVS;
        break;

    case 0xa6: //CMPSB
        opcode_idx = CMPS;
        break;

    case 0xa7: //CMPSW
        opcode_idx = CMPS;
        break;

    case 0xaa: //STOSB
        opcode_idx = STOS;
        break;

    case 0xab: //STOSW
        opcode_idx = STOS;
        break;

    case 0xac: //LODSB
        opcode_idx = LODS;
        break;

    case 0xad: //LODSW
        opcode_idx = LODS;
        break;

    case 0xae: //SCASB
        opcode_idx = SCAS;
        break;

    case 0xaf: //SCASW
        opcode_idx = SCAS;
        break;

    case 0xc0:
    case 0xd0:
    case 0xd2:
        switch (modrm & 0x38) {
        case 0x00:
            opcode_idx = ROL;
            break;
        case 0x08:
            opcode_idx = ROR;
            break;
        case 0x10:
            opcode_idx = RCL;
            break;
        case 0x18:
            opcode_idx = RCR;
            break;
        case 0x20:
            opcode_idx = SHL;
            break;
        case 0x28:
            opcode_idx = SHR;
            break;
        case 0x30:
            //LOGGING.log(Level.FINE, "invalid SHL encoding");
            opcode_idx = SHL;
            break;
        case 0x38:
            opcode_idx = SAR;
            break;
        }
        break;

    case 0xc1:
    case 0xd1:
    case 0xd3:

        switch (modrm & 0x38) {
        case 0x00:
            opcode_idx = ROL;
            break;
        case 0x08:
            opcode_idx = ROR;
            break;
        case 0x10:
            opcode_idx = RCL;
            break;
        case 0x18:
            opcode_idx = RCR;
            break;
        case 0x20:
            opcode_idx = SHL;
            break;
        case 0x28:
            opcode_idx = SHR;
            break;
        case 0x30:
            //LOGGING.log(Level.FINE, "invalid SHL encoding");
            opcode_idx = SHL;
            break;
        case 0x38:
            opcode_idx = SAR;
            break;
        }
        break;

    case 0xc2: // RET Iw
    case 0xca: // RETF Iw
        opcode_idx = RET;
        break;

    case 0xcc:
        opcode_idx = JMPF;
        break;

    case 0xcd:
        opcode_idx = INT;
        break;

    case 0xce:
        opcode_idx = INTO;
        break;

    case 0xcf:
        opcode_idx = IRET;
        break;

    case 0xd4:
        opcode_idx = AAM;
        break; //AAM
    case 0xd5:
        opcode_idx = AAD;
        break; //AAD

    case 0xd6:
        assert(false);
        break; //SALC

    case 0xe0: //LOOPNZ Jb
        opcode_idx = LOOPNZ;
        break;

    case 0xe1: //LOOPZ Jb
        opcode_idx = LOOPZ;
        break;

    case 0xe2: //LOOP Jb
        opcode_idx = LOOP;
        break;

    case 0xe3: //JCXZ
        opcode_idx = JCXZ;
        break;

    case 0xe4: //IN AL,  imm8
    case 0xec:
        opcode_idx = IN;
        break; //IN AL, DX

    case 0xe5: //IN eAX,  imm8
    case 0xed: //IN eAX, DX
        opcode_idx = IN;
        break;

    case 0xe6: //OUT  imm8, AL
    case 0xee:
        opcode_idx = OUT;
        break; //OUT DX, AL

    case 0xe7: //OUT  imm8, AX
    case 0xef: //OUT DX, AX
        opcode_idx = OUT;
        break;

    case 0xe8: //CALL Jv
        opcode_idx = CALL;
        break;

    case 0xe9: //JMP Jv
        opcode_idx = JMP;
        break;

    case 0xea: //JMPF Ap
        opcode_idx = JMPF;
        break;

    case 0xeb:
        opcode_idx = JMP;
        break; //JMP Jb

    case 0xf4:
        opcode_idx = HLT;
        break; //HLT

    case 0xf5:
        opcode_idx = CMC;
        break; //CMC

    case 0xf6: //UNA GP3  r/m8
        switch (modrm & 0x38) {
        case 0x00:
            opcode_idx = AND;
            break;
        case 0x10:
            opcode_idx = NOT;
            break;
        case 0x18:
            opcode_idx = NEG;
            break;
        case 0x20:
            opcode_idx = MUL;
            break;
        case 0x28:
            opcode_idx = IMUL;
            break;
        case 0x30:
            opcode_idx = DIV;
            break;
        case 0x38:
            opcode_idx = IDIV;
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //throw new IllegalStateException("Invalid Gp 3 Instruction?");
        }
        break;

    case 0xf7: //UNA GP3  r/m16

        switch (modrm & 0x38) {
        case 0x00:
            opcode_idx = AND;
            break;
        case 0x10:
            opcode_idx = NOT;
            break;
        case 0x18:
            opcode_idx = NEG;
            break;
        case 0x20:
            opcode_idx = MUL;
            break;
        case 0x28:
            opcode_idx = IMUL;
            break;
        case 0x30:
            opcode_idx = DIV;
            break;
        case 0x38:
            opcode_idx = IDIV;
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //throw new IllegalStateException("Invalid Gp 3 Instruction?");
        }
        break;

    case 0xf8: opcode_idx = CLC; break; //CLC
    case 0xf9: opcode_idx = STC; break; //STC
    case 0xfa: opcode_idx = CLI; break; //CLI
    case 0xfb: opcode_idx = STI; break; //STI
    case 0xfc: opcode_idx = CLD; break; //CLD
    case 0xfd: opcode_idx = STD; break; //STD

    case 0xfe:
        switch (modrm & 0x38) {
        case 0x00: //INC  r/m8
            opcode_idx = INC; break;
        case 0x08: //DEC  r/m8
            opcode_idx = DEC; break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //throw new IllegalStateException("Invalid Gp 4 Instruction?");
        }
        break;

    case 0xff:
        switch (modrm & 0x38) {
        case 0x00: //INC  r/m16
            opcode_idx = INC;
            break;
        case 0x08: //DEC  r/m16
            opcode_idx = DEC;
            break;
        case 0x10:
            opcode_idx = JMP;
            break;
        case 0x18:
            opcode_idx = JMPF;
            break;
        case 0x20:
            opcode_idx = JMP;
            break;
        case 0x28:
            opcode_idx = JMPF;
            break;
        case 0x30:
            opcode_idx = PUSH;
            break;
        case 0xff:
            assert(false);
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
            //System.out.println("Possibly jumped into unwritten memory...");
            //throw new IllegalStateException("Invalid Gp 5 Instruction? " + modrm);
        }
        break;

    case 0x63:
        assert(false);
    case 0x90:
        opcode_idx = NOP;
        break;
    case 0x86: //XCHG  r/m8,  reg8
    case 0x87: //XCHG  r/m16,  reg16
        opcode_idx = XCHG;
        break;
    case 0x88: //MOV  r/m8,  reg8
    case 0x89: //MOV  r/m16,  reg16
    case 0x8a: //MOV  reg8,  r/m8
    case 0x8b: //MOV  reg16,  r/m16
    case 0x8c: //MOV Ew, Sw
        opcode_idx = MOV;
        break;
    case 0x8d: //LEA  reg16, M
        opcode_idx = LEA;
    case 0x8e: //MOV Sw, Ew
        opcode_idx = MOV;
        break;

    case 0x91: //XCHG AX, CX
    case 0x92: //XCHG AX, CX
    case 0x93: //XCHG AX, CX
    case 0x94: //XCHG AX, CX
    case 0x95: //XCHG AX, CX
    case 0x96: //XCHG AX, CX
    case 0x97: //XCHG AX, CX
        opcode_idx = XCHG;
        break;

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
        opcode_idx = MOV;
        break;

    case 0xc3: //RET
        opcode_idx = RET;
        break;
    case 0xc4: //LES
        opcode_idx = LES;
        break;
    case 0xc5: //LDS
        opcode_idx = LDS;
        break;
    case 0xc6: //MOV GP11  r/m8,  reg8
    case 0xc7: //MOV GP11  r/m16,  reg16
        opcode_idx = MOV;
        break;
    case 0xcb: //RET
        opcode_idx = RET;
        break;

    case 0xd7: //XLAT
        opcode_idx = XLAT;
        break;

    default:
        printf("Missing opcode: %x\n", opcode);
        REPORT_ERROR(INVALID_STATE);
        //throw new IllegalStateException("Missing Operation: 0x" + Integer.toHexString(opcode));

    }
    return opcode_idx;
}

void dump_instruction(inst_p inst, FILE* f)
{
    int i=0;
    OPCODE opc;
    char* opcode;
    assert(inst);
    opc = inst_get_opcode_idx(inst);
    opcode = opcode_names[opc];
    printf("%04x:%04x: %s (", inst->inst_start_seg,
                                  inst->inst_start_off,
                                  opcode);
    printf("%d bytes: ", inst->inst_bytes_size);
    for(i=0;i<inst->inst_bytes_size;i++){
        printf( "%02x", inst->inst_bytes[i]);
        if(i+1 != inst->inst_bytes_size){
            printf( " ");
        }else{
            printf( ")\n");
        }
    }
}
