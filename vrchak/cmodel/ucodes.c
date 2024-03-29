
#include "ucodes.h"

char ucode_names[UCODES_SIZE][UCODE_NAME_LEN] = {
    "AAA",
    "AAD",
    "AAM",
    "AAS",
    "ADC",
    "ADD",
    "ADD_TO_SP",
    "ADDR_BP",
    "ADDR_BX",
    "ADDR_DI",
    "ADDR_IB",
    "ADDR_INT",
    "ADDR_IW",
    "ADDR_SI",
    "ADDR_UAL",
    "AND",
    "BREAKPOINT",
    "CLC",
    "CLD",
    "CLI",
    "CLT",
    "CMC",
    "CMPSB_A16",
    "CMPSW_A16",
    "CWD",
    "DAA",
    "DAS",
    "DEC",
    "DIV_O16",
    "DIV_O8",
    "FWAIT",
    "HALT",
    "IDIV_O16",
    "IDIV_O8",
    "IMUL",
    "IMULA_O16",
    "IMULA_O8",
    "INC",
    "IN_O16",
    "IN_O8",
    "INSB_A16",
    "INSW_A16",
    "INTO_O16_A16",
    "JA_O8",
    "JC_O8",
    "JCXZ",
    "JG_O8",
    "JL_O8",
    "JNA_O8",
    "JNC_O8",
    "JNG_O8",
    "JNL_O8",
    "JNO_O8",
    "JNP_O8",
    "JNS_O8",
    "JNZ_O8",
    "JO_O8",
    "JP_O8",
    "JS_O8",
    "JUMP_ABS_O16",
    "JUMP_FAR_O16",
    "JUMP_O16",
    "JUMP_O8",
    "JZ_O8",
    "LAHF",
    "LOAD0_ADDR",
    "LOAD0_AH",
    "LOAD0_AL",
    "LOAD0_AX",
    "LOAD0_BH",
    "LOAD0_BL",
    "LOAD0_BP",
    "LOAD0_BX",
    "LOAD0_CH",
    "LOAD0_CL",
    "LOAD0_CS",
    "LOAD0_CX",
    "LOAD0_DH",
    "LOAD0_DI",
    "LOAD0_DL",
    "LOAD0_DS",
    "LOAD0_DX",
    "LOAD0_FLAGS",
    "LOAD0_ES",
    "LOAD0_IB",
    "LOAD0_IP",
    "LOAD0_IW",
    "LOAD0_MEM_BYTE",
    "LOAD0_MEM_WORD",
    "LOAD0_SI",
    "LOAD0_SP",
    "LOAD0_SS",
    "LOAD0_UB",
    "LOAD1_AH",
    "LOAD1_AL",
    "LOAD1_AX",
    "LOAD1_BH",
    "LOAD1_BL",
    "LOAD1_BP",
    "LOAD1_BX",
    "LOAD1_CH",
    "LOAD1_CL",
    "LOAD1_CX",
    "LOAD1_DH",
    "LOAD1_DI",
    "LOAD1_DL",
    "LOAD1_DX",
    "LOAD1_IB",
    "LOAD1_IW",
    "LOAD1_MEM_BYTE",
    "LOAD1_MEM_WORD",
    "LOAD1_SI",
    "LOAD1_SP",
    "LOAD_SEG_CS",
    "LOAD_SEG_DS",
    "LOAD_SEG_ES",
    "LOAD_SEG_SS",
    "LODSB_A16",
    "LODSW_A16",
    "LOOP_CX",
    "LOOPNZ_CX",
    "LOOPZ_CX",
    "MEM_READ_BYTE",
    "MEM_READ_WORD",
    "MEM_RESET",
    "MOVSB_A16",
    "MOVSW_A16",
    "MUL_O16",
    "MUL_O8",
    "NEG",
    "NOT",
    "OR",
    "OUT_O16",
    "OUT_O8",
    "OUTSB_A16",
    "OUTSW_A16",
    "POP",
    "POPF",
    "PUSH",
    "PUSHF",
    "RCL_O16",
    "RCL_O8",
    "RCR_O16",
    "RCR_O8",
    "REP",
    "REPE",
    "REPNE",
    "RET_FAR_IW_O16_A16",
    "RET_FAR_O16_A16",
    "RET_IW_O16_A16",
    "RET_O16_A16",
    "ROL_O16",
    "ROL_O8",
    "ROR_O16",
    "ROR_O8",
    "SAHF",
    "SALC",
    "SAR_O16",
    "SAR_O8",
    "SBB",
    "SCASB_A16",
    "SCASW_A16",
    "SHL",
    "SHR",
    "SIGN_EXTEND_8_16",
    "STC",
    "STD",
    "STI",
    "STORE0_AH",
    "STORE0_AL",
    "STORE0_AX",
    "STORE0_BH",
    "STORE0_BL",
    "STORE0_BP",
    "STORE0_BX",
    "STORE0_CH",
    "STORE0_CL",
    "STORE0_CS",
    "STORE0_CX",
    "STORE0_DH",
    "STORE0_DI",
    "STORE0_DL",
    "STORE0_DS",
    "STORE0_DX",
    "STORE0_ES",
    "STORE0_FLAGS",
    "STORE0_IP",
    "STORE0_MEM_BYTE",
    "STORE0_MEM_WORD",
    "STORE0_SI",
    "STORE0_SP",
    "STORE0_SS",
    "STORE1_AH",
    "STORE1_AL",
    "STORE1_AX",
    "STORE1_BH",
    "STORE1_BL",
    "STORE1_BP",
    "STORE1_BX",
    "STORE1_CH",
    "STORE1_CL",
    "STORE1_CS",
    "STORE1_CX",
    "STORE1_DH",
    "STORE1_DI",
    "STORE1_DL",
    "STORE1_DS",
    "STORE1_DX",
    "STORE1_ES",
    "STORE1_MEM_BYTE",
    "STORE1_MEM_WORD",
    "STORE1_SI",
    "STORE1_SP",
    "STORE1_SS",
    "STOSB_A16",
    "STOSW_A16",
    "SUB",
    "UNDEFINED",
    "XOR"
};

