
#include <stdio.h>
#include <assert.h>

#include "cpu.h"
#include "decoder.h"
#include "executer.h"
#include "flagcb.h"

//#define get_low_byte(x)  ((uint8_t)x)
//#define get_high_byte(x) ((uint8_t)(x>>8))

extern char ucode_names[UCODES_SIZE][UCODE_NAME_LEN];

static uint8_t get_high_byte(uint16_t w)
{
    return (uint8_t)(w>>8);
}

static uint8_t get_low_byte(uint16_t w)
{
    return (uint8_t)w;
}

static void set_high_byte(uint16_t* w, uint8_t b)
{
    uint16_t highmask = ((uint16_t)b) << 8;
    uint16_t lowmask = (*w) & 0xff;
    *w = lowmask | highmask;
}

static void set_low_byte(uint16_t* w, uint8_t b)
{
    uint16_t lowmask  = (uint16_t)b;
    uint16_t highmask = ((*w) & 0xff00);
    *w = lowmask | highmask;
}

static bool execute_ucode(op_p op,
                          executer_p exe,
                          cpu_context_p cpu,
                          busop_p bop,
                          flagcb_p fcb)
{
    bool go_on = true;
    uint8_t al, ah;
    uint16_t zero, sign, oflow, carry, parity;
    uint16_t res16;
    uint32_t res32;
    UCODE ucode = op_read_ucode(op);

    switch(ucode){
        case AAA:
            al = get_low_byte(cpu->ax);
            ah = get_high_byte(cpu->ax);
            if((al & 0x0f) > 9 || (1 == get_flag(cpu->psw, AUXC_BIT_IDX))){
                al = (al+6) & 0x0f;
                ah = ah+1;
                set_flag(&cpu->psw, CARRY_BIT_IDX);
                set_flag(&cpu->psw, AUXC_BIT_IDX);
            }else{
                clear_flag(&cpu->psw, CARRY_BIT_IDX);
                clear_flag(&cpu->psw, AUXC_BIT_IDX);
            }
            set_low_byte(&cpu->ax, al);
            set_high_byte(&cpu->ax, ah);
            break;
        case AAD:
            al = get_low_byte(cpu->ax);
            ah = get_high_byte(cpu->ax);
            al = (ah*10) + al;
            ah = 0;
            set_low_byte(&cpu->ax, al);
            set_high_byte(&cpu->ax, ah);
            flagcb_set_cb(fcb, &flags_sfzfcf, cpu->ax, 0);
            break;
        case AAM:
            al = get_low_byte(cpu->ax);
            ah = get_high_byte(cpu->ax);
            ah = al /10;
            al = al % 10;
            set_low_byte(&cpu->ax, al);
            set_high_byte(&cpu->ax, ah);
            flagcb_set_cb(fcb, &flags_sfzfcf, cpu->ax, 0);
            break;
        case AAS:
            al = get_low_byte(cpu->ax);
            ah = get_high_byte(cpu->ax);
            if((al & 0x0f) > 9 || (get_flag(cpu->psw, AUXC_BIT_IDX))){
                al = (al+6) & 0x0f;
                ah = ah+1;
                set_flag(&cpu->psw, CARRY_BIT_IDX);
                set_flag(&cpu->psw, AUXC_BIT_IDX);
            }else{
                clear_flag(&cpu->psw, CARRY_BIT_IDX);
                clear_flag(&cpu->psw, AUXC_BIT_IDX);
            }
            set_low_byte(&cpu->ax, al);
            set_high_byte(&cpu->ax, ah);
            break;
        case ADC:
            carry = get_flag(cpu->psw, CARRY_BIT_IDX);
            res32 = exe->exe_op0 + exe->exe_op1 + carry;
            carry = (res32 >> 16) & 0x01;
            flagcb_set_cb(fcb, &flags_add, res32, carry);
            break;
        case ADD:
            res32 = exe->exe_op0 + exe->exe_op1;
            exe->exe_op0 = res32;
            carry = (res32 >> 16) & 0x01;
            flagcb_set_cb(fcb, &flags_add, res32, carry);
            break;
        case ADDR_BP:
            exe->exe_addr += cpu->bp;
            break;
        case ADDR_BX:
            exe->exe_addr += cpu->bx;
            break;
        case ADDR_DI:
            exe->exe_addr += cpu->di;
            break;
        case ADDR_IB:
            exe->exe_addr += (int16_t)((int8_t)op_read_arg(op));
            break;
        case ADDR_INT:
            exe->exe_addr = op_read_arg(op) * 4;
            break;
        case ADDR_IW:
            exe->exe_addr += op_read_arg(op);
            break;
        case ADDR_SI:
            exe->exe_addr += cpu->si;
            break;
        case ADDR_UAL:
            exe->exe_addr += get_low_byte(cpu->ax);
            break;
        case AND:
            res16 = exe->exe_op0 & exe->exe_op1;
            flagcb_set_cb(fcb, &flags_log, res16, 0);
            exe->exe_op0 = res16;
            break;
        case CLC:
            clear_flag(&cpu->psw, CARRY_BIT_IDX);
            break;
        case CLD:
            clear_flag(&cpu->psw, DIR_BIT_IDX);
            break;
        case CLI:
            clear_flag(&cpu->psw, INT_BIT_IDX);
            break;
        case CLT:
            clear_flag(&cpu->psw, TRAP_BIT_IDX);
            break;
        case CMC:
            complement_flag(&cpu->psw, CARRY_BIT_IDX);
            break;
        case CMPSB_A16:
        case CMPSW_A16:
            assert(false);
        case CWD:
            if((int16_t)cpu->ax < 0){
                cpu->dx = 0xffff;
            }else{
                cpu->dx = 0x0;
            }
            break;
        case DAA:
            al = get_low_byte(cpu->ax);
            if(((al & 0x0f) > 9) || (1 == get_flag(cpu->psw, AUXC_BIT_IDX))){
                al += 6;
                set_flag(&cpu->psw, AUXC_BIT_IDX);
            }else{
                clear_flag(&cpu->psw, AUXC_BIT_IDX);
            }
            if((al > 0x9f) || (1 == get_flag(cpu->psw, CARRY_BIT_IDX))){
                al += 0x60;
                set_flag(&cpu->psw, CARRY_BIT_IDX);
            }else{
                clear_flag(&cpu->psw, CARRY_BIT_IDX);
            }
            flagcb_set_cb(fcb, &flags_sfzfpf, al, 0);
            break;
        case DAS:
            al = get_low_byte(cpu->ax);
            if(((al & 0x0f) > 9) || (1 == get_flag(cpu->psw, AUXC_BIT_IDX))){
                al -= 6;
                set_flag(&cpu->psw, AUXC_BIT_IDX);
            }else{
                clear_flag(&cpu->psw, AUXC_BIT_IDX);
            }
            if((al > 0x9f) || (1 == get_flag(cpu->psw, CARRY_BIT_IDX))){

                al -= 0x60;
                set_flag(&cpu->psw, CARRY_BIT_IDX);
            }else{
                clear_flag(&cpu->psw, CARRY_BIT_IDX);
            }
            flagcb_set_cb(fcb, &flags_sfzfpf, al, 0);
            break;
        case DEC:
            exe->exe_op0 = exe->exe_op0 - 1;
            flagcb_set_cb(fcb, &flags_add, exe->exe_op0, -1);
            break;
        case DIV_O16:
        case DIV_O8:
        case FWAIT:
            assert(false);
            break;
        case HALT:
            exe->exe_halt = 1;
            cpu->ip--;
            go_on = false;
            break;
        case IDIV_O16:
        case IDIV_O8:
        case IMUL:
        case IMULA_O16:
        case IMULA_O8:
            assert(false);
            break;
        case INC:
            exe->exe_op0 = exe->exe_op0 + 1;
            flagcb_set_cb(fcb, &flags_add, exe->exe_op0, -1);
            break;
        case IN_O16:
            assert(NOOP == bop->bop_state);
            busop_init(bop,
                       0,
                       exe->exe_op0,
                       0,
                       BUSOP_READ,
                       BUSOP_WORD,
                       ADDR_IO);
            go_on = false;
            break;
        case IN_O8:
            assert(NOOP == bop->bop_state);
            busop_init(bop,
                       0,
                       exe->exe_op0,
                       0,
                       BUSOP_READ,
                       BUSOP_BYTE,
                       ADDR_IO);
            go_on = false;
            break;
        case INTO_O16_A16:
        case INSB_A16:
        case INSW_A16:
            assert(false);
        case JA_O8:
            flagcb_eval(fcb);
            zero  = get_flag(cpu->psw, ZERO_BIT_IDX);
            carry = get_flag(cpu->psw, CARRY_BIT_IDX);
            if (0 == zero && 0 == carry){
                cpu->ip += exe->exe_op0;
            }
            break;
        case JC_O8:
            flagcb_eval(fcb);
            carry = get_flag(cpu->psw, CARRY_BIT_IDX);
            if (0 == carry){
                cpu->ip += exe->exe_op0;
            }
            break;
        case JCXZ:
            if(0 == cpu->cx){
                cpu->ip += exe->exe_op0;
            }
            break;
        case JG_O8:
            flagcb_eval(fcb);
            zero  = get_flag(cpu->psw, ZERO_BIT_IDX);
            sign  = get_flag(cpu->psw, SIGN_BIT_IDX);
            oflow = get_flag(cpu->psw, OFLOW_BIT_IDX);
            if(zero == 0 && sign==oflow){
                cpu->ip += exe->exe_op0;
            }
            break;
        case JL_O8:
            flagcb_eval(fcb);
            sign  = get_flag(cpu->psw, SIGN_BIT_IDX);
            oflow = get_flag(cpu->psw, OFLOW_BIT_IDX);
            if(sign != oflow){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNA_O8:
            flagcb_eval(fcb);
            carry  = get_flag(cpu->psw, CARRY_BIT_IDX);
            zero   = get_flag(cpu->psw, ZERO_BIT_IDX);
            if(carry == zero){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNC_O8:
            flagcb_eval(fcb);
            carry  = get_flag(cpu->psw, CARRY_BIT_IDX);
            if(0 == carry){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNG_O8:
            flagcb_eval(fcb);
            zero  = get_flag(cpu->psw, ZERO_BIT_IDX);
            sign  = get_flag(cpu->psw, SIGN_BIT_IDX);
            oflow = get_flag(cpu->psw, OFLOW_BIT_IDX);
            if(zero!=0 || sign!=oflow){
                cpu->ip += exe->exe_op0;
            }
            break;
        case JNL_O8:
            flagcb_eval(fcb);
            sign  = get_flag(cpu->psw, SIGN_BIT_IDX);
            oflow = get_flag(cpu->psw, OFLOW_BIT_IDX);
            if(sign == oflow){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNO_O8:
            flagcb_eval(fcb);
            oflow = get_flag(cpu->psw, OFLOW_BIT_IDX);
            if(0 == oflow){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNP_O8:
            flagcb_eval(fcb);
            parity = get_flag(cpu->psw, PARITY_BIT_IDX);
            if(0 == parity){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNS_O8:
            flagcb_eval(fcb);
            sign  = get_flag(cpu->psw, SIGN_BIT_IDX);
            if(0 == sign){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JNZ_O8:
            flagcb_eval(fcb);
            zero  = get_flag(cpu->psw, ZERO_BIT_IDX);
            if(0 == zero){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JO_O8:
            flagcb_eval(fcb);
            oflow = get_flag(cpu->psw, OFLOW_BIT_IDX);
            if(1 == oflow){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JP_O8:
            flagcb_eval(fcb);
            parity = get_flag(cpu->psw, PARITY_BIT_IDX);
            if(1 == parity){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JS_O8:
            flagcb_eval(fcb);
            sign  = get_flag(cpu->psw, SIGN_BIT_IDX);
            if(1 == sign){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case JUMP_ABS_O16:
            cpu->ip = exe->exe_op0;
            break;
        case JUMP_FAR_O16:
            cpu->cs = exe->exe_op1;
            cpu->ip = exe->exe_op0;
            break;
        case JUMP_O16:
        case JUMP_O8:
            cpu->ip += exe->exe_op0;
            break;
        case JZ_O8:
            flagcb_eval(fcb);
            zero  = get_flag(cpu->psw, ZERO_BIT_IDX);
            if(1 == zero){
                cpu->ip+= exe->exe_op0;
            }
            break;
        case LAHF:
            assert(false);
        case LOAD0_ADDR:
            exe->exe_op0 = exe->exe_addr;
            break;
        case LOAD0_AH:
            exe->exe_op0 = get_high_byte(cpu->ax);
            break;
        case LOAD0_AL:
            exe->exe_op0 = get_low_byte(cpu->ax);
            break;
        case LOAD0_AX:
            exe->exe_op0 = cpu->ax;
            break;
        case LOAD0_BH:
            exe->exe_op0 = get_high_byte(cpu->bx);
            break;
        case LOAD0_BL:
            exe->exe_op0 = get_low_byte(cpu->bx);
            break;
        case LOAD0_BP:
            exe->exe_op0 = cpu->bp;
            break;
        case LOAD0_BX:
            exe->exe_op0 = cpu->bx;
            break;
        case LOAD0_CH:
            exe->exe_op0 = get_high_byte(cpu->cx);
            break;
        case LOAD0_CL:
            exe->exe_op0 = get_low_byte(cpu->cx);
            break;
        case LOAD0_CS:
            exe->exe_op0 = cpu->cs;
            break;
        case LOAD0_CX:
            exe->exe_op0 = cpu->cx;
            break;
        case LOAD0_DH:
            exe->exe_op0 = get_high_byte(cpu->dx);
            break;
        case LOAD0_DI:
            exe->exe_op0 = cpu->di;
            break;
        case LOAD0_DL:
            exe->exe_op0 = get_low_byte(cpu->dx);
            break;
        case LOAD0_DS:
            exe->exe_op0 = cpu->ds;
            break;
        case LOAD0_DX:
            exe->exe_op0 = cpu->dx;
            break;
        case LOAD0_FLAGS:
            exe->exe_op0 = cpu->psw;
            break;
        case LOAD0_ES:
            exe->exe_op0 = cpu->es;
            break;
        case LOAD0_IP:
            exe->exe_op0 = cpu->ip;
            break;
        case LOAD0_IB:
            exe->exe_op0 = (int16_t)(int8_t)op_read_arg(op);
            printf("op = %04x\n", exe->exe_op0);
            break;
        case LOAD0_IW:
            exe->exe_op0 = op_read_arg(op);
            printf("op = %04x\n", exe->exe_op0);
            break;
        case LOAD0_MEM_BYTE:
            exe->exe_op0 = busop_get_byte(bop);
            break;
        case LOAD0_MEM_WORD:
            printf("MDR = %04x\n", bop->bop_data);
            exe->exe_op0 = bop->bop_data;
            break;
            //Init a bus transaction.
        case LOAD0_SI:
            exe->exe_op0 = cpu->si;
        case LOAD0_SP:
            exe->exe_op0 = cpu->sp;
            break;
        case LOAD0_SS:
            exe->exe_op0 = cpu->ss;
        case LOAD0_UB:
            exe->exe_op0 = op_read_arg(op);
            printf("op = %04x\n", exe->exe_op0);
            break;
        case LOAD1_AH:
            exe->exe_op1 = get_high_byte(cpu->ax);
            break;
        case LOAD1_AL:
            exe->exe_op1 = get_low_byte(cpu->ax);
            break;
        case LOAD1_AX:
            exe->exe_op1 = cpu->ss;
            break;
        case LOAD1_BH:
            exe->exe_op1 = get_high_byte(cpu->bx);
            break;
        case LOAD1_BL:
            exe->exe_op1 = get_low_byte(cpu->bx);
            break;
        case LOAD1_BP:
            exe->exe_op1 = cpu->bp;
            break;
        case LOAD1_BX:
            exe->exe_op1 = cpu->bx;
            break;
        case LOAD1_CH:
            exe->exe_op1 = get_high_byte(cpu->cx);
            break;
        case LOAD1_CL:
            exe->exe_op1 = get_low_byte(cpu->cx);
            break;
        case LOAD1_CX:
            exe->exe_op1 = cpu->cx;
            break;
        case LOAD1_DH:
            exe->exe_op1 = get_high_byte(cpu->dx);
            break;
        case LOAD1_DI:
            exe->exe_op1 = cpu->di;
            break;
        case LOAD1_DL:
            exe->exe_op1 = get_low_byte(cpu->dx);
            break;
        case LOAD1_DX:
            exe->exe_op1 = cpu->dx;
            break;
        case LOAD1_IB:
        case LOAD1_IW:
            exe->exe_op1 = op_read_arg(op);
            //printf("%04x\n", exe->exe_op1);
            break;
        case LOAD1_MEM_BYTE:
            exe->exe_op1 = busop_get_byte(bop);
            break;
        case LOAD1_MEM_WORD:
            printf("MDR = %04x\n", bop->bop_data);
            exe->exe_op1 = bop->bop_data;
            break;
        case LOAD1_SI:
            exe->exe_op0 = cpu->si;
            break;
        case LOAD1_SP:
            exe->exe_op0 = cpu->sp;
            break;
        case LOAD_SEG_CS:
            exe->exe_seg = cpu->cs;
            break;
        case LOAD_SEG_DS:
            exe->exe_seg = cpu->ds;
            break;
        case LOAD_SEG_ES:
            exe->exe_seg = cpu->es;
            break;
        case LOAD_SEG_SS:
            exe->exe_seg = cpu->ss;
            break;
        case LODSB_A16:
        case LODSW_A16:
        case LOOP_CX:
        case LOOPNZ_CX:
        case LOOPZ_CX:
            assert(false);
            break;
        case MEM_READ_BYTE:
            busop_init(bop,
                       exe->exe_seg,
                       exe->exe_addr,
                       0,
                       BUSOP_BYTE,
                       BUSOP_READ,
                       ADDR_MEM);
            go_on = false;
            break;
        case MEM_READ_WORD:
            busop_init(bop,
                       exe->exe_seg,
                       exe->exe_addr,
                       0,
                       BUSOP_WORD,
                       BUSOP_READ,
                       ADDR_MEM);
            go_on = false;
            break;
        case MEM_RESET:
            break;
        case MOVSB_A16:
        case MOVSW_A16:
        case MUL_O16:
        case MUL_O8:
            assert(false);
            break;
        case NEG:
        case NOT:
        case OR:
        case OUT_O16:
            assert(NOOP == bop->bop_state);
            busop_init(bop,
                       0,
                       exe->exe_op0,
                       exe->exe_op1,
                       BUSOP_WORD,
                       BUSOP_WRITE,
                       ADDR_IO);
            go_on = false;
            break;
        case OUT_O8:
            assert(NOOP == bop->bop_state);
            printf("addr: %x, data: %x\n",exe->exe_op0, exe->exe_op1);
            busop_init(bop,
                       0,
                       exe->exe_op0,
                       exe->exe_op1,
                       BUSOP_BYTE,
                       BUSOP_WRITE,
                       ADDR_IO);
            go_on = false;
            break;
        case OUTSB_A16:
            assert(NOOP == bop->bop_state);
            printf("addr: %x, data: %x\n",exe->exe_op1, exe->exe_op0);
            busop_init(bop,
                       0,
                       exe->exe_op1,
                       exe->exe_op0,
                       BUSOP_BYTE,
                       BUSOP_WRITE,
                       ADDR_IO);
            if(0 == get_flag(cpu->psw, DIR_BIT_IDX)){
                cpu->si++;
            }else{
                cpu->si--;
            }
            break;
        case OUTSW_A16:
            assert(false);
            break;
        case POP:
            assert(NOOP == bop->bop_state);
            busop_init(bop,
                       cpu->ss,
                       cpu->sp,
                       0,
                       BUSOP_WORD,
                       BUSOP_READ,
                       ADDR_MEM);
            cpu->sp += 2;
            go_on = false;
            break;
        case POPF:
            assert(NOOP == bop->bop_state);
            busop_init(bop,
                       cpu->ss,
                       cpu->sp,
                       0,
                       BUSOP_WORD,
                       BUSOP_READ,
                       ADDR_MEM);
            cpu->sp += 2;
            go_on = false;
            break;
        case PUSH:
            assert(NOOP == bop->bop_state);
            cpu->sp -= 2;
            printf("store word ptr[%04x:%04x] = %04x\n",
                    cpu->ss,
                    cpu->sp,
                    exe->exe_op0);
            busop_init(bop,
                       cpu->ss,
                       cpu->sp,
                       exe->exe_op0,
                       BUSOP_WORD,
                       BUSOP_WRITE,
                       ADDR_MEM);
            go_on = false;
            break;
        case PUSHF:
            assert(NOOP == bop->bop_state);
            cpu->sp -= 2;
            busop_init(bop,
                       cpu->ss,
                       cpu->sp,
                       cpu->psw,
                       BUSOP_WORD,
                       BUSOP_WRITE,
                       ADDR_MEM);
            go_on = false;
            break;
        case RCL_O16:
        case RCL_O8:
        case RCR_O16:
        case RCR_O8:
            assert(false);
            break;
        case REPE_CMPSB_A16:
        case REP:
            cpu->cx--;
            if(0 != cpu->cx){
                cpu->ip -= 2;
            }
            break;
        case REPE_CMPSW_A16:
        case REPE_SCASB_A16:
        case REPE_SCASW_A16:
        case REP_INSB_A16:
        case REP_INSW_A16:
        case REP_LODSB_A16:
        case REP_LODSW_A16:
        case REP_MOVSB_A16:
        case REP_MOVSW_A16:
        case REPNE_CMPSB_A16:
        case REPNE_CMPSW_A16:
        case REPNE_SCASB_A16:
        case REPNE_SCASW_A16:
        case REP_OUTSB_A16:
        case REP_OUTSW_A16:
        case REP_STOSB_A16:
        case REP_STOSW_A16:
        case RET_FAR_IW_O16_A16:
        case RET_FAR_O16_A16:
        case RET_IW_O16_A16:
        case RET_O16_A16:
        case ROL_O16:
        case ROL_O8:
        case ROR_O16:
        case ROR_O8:
        case SAHF:
        case SALC:
        case SAR_O16:
        case SAR_O8:
        case SBB:
        case SCASB_A16:
        case SCASW_A16:
            assert(false);
        case SHL:
            printf("%04x\n", exe->exe_op0);
            exe->exe_op0 = exe->exe_op0 << exe->exe_op1;
            break;
        case SHR:
            exe->exe_op0 = exe->exe_op0 >> exe->exe_op1;
            break;
        case SIGN_EXTEND_8_16:
            exe->exe_op0 = (int16_t)((int8_t)exe->exe_op0);
        case STC:
            set_flag(&cpu->psw, CARRY_BIT_IDX);
        case STD:
            set_flag(&cpu->psw, DIR_BIT_IDX);
            break;
        case STI:
            set_flag(&cpu->psw, INT_BIT_IDX);
            break;
        case STORE0_AH:
            set_high_byte(&cpu->ax, exe->exe_op0);
            break;
        case STORE0_AL:
            set_low_byte(&cpu->ax, exe->exe_op0);
            break;
        case STORE0_AX:
            cpu->ax = exe->exe_op0;
            break;
        case STORE0_BH:
            set_high_byte(&cpu->bx, exe->exe_op0);
            break;
        case STORE0_BL:
            set_low_byte(&cpu->bx, exe->exe_op0);
            break;
        case STORE0_BP:
            cpu->bp = exe->exe_op0;
            break;
        case STORE0_BX:
            cpu->bx = exe->exe_op0;
            break;
        case STORE0_CH:
            set_high_byte(&cpu->cx, exe->exe_op0);
            break;
        case STORE0_CL:
            set_low_byte(&cpu->cx, exe->exe_op0);
            break;
        case STORE0_CS:
            cpu->cs = exe->exe_op0;
            break;
        case STORE0_CX:
            cpu->cx = exe->exe_op0;
            break;
        case STORE0_DH:
            set_high_byte(&cpu->dx, exe->exe_op0);
            break;
        case STORE0_DI:
            cpu->di = exe->exe_op0;
            break;
        case STORE0_DL:
            set_low_byte(&cpu->dx, exe->exe_op0);
            break;
        case STORE0_DS:
            cpu->ds = exe->exe_op0;
            break;
        case STORE0_DX:
            cpu->dx = exe->exe_op0;
            break;
        case STORE0_ES:
            cpu->es = exe->exe_op0;
            break;
        case STORE0_FLAGS:
            cpu->psw = exe->exe_op0;
            break;
        case STORE0_IP:
            cpu->ip = exe->exe_op0;
            break;
        case STORE0_MEM_BYTE:
            printf("store byte ptr[%04x:%04x] = %02x\n",
                    exe->exe_seg,
                    exe->exe_addr,
                    exe->exe_op0);
            busop_init(bop,
                       exe->exe_seg,
                       exe->exe_addr,
                       exe->exe_op0,
                       BUSOP_WRITE,
                       BUSOP_BYTE,
                       ADDR_MEM);
            go_on = false;
            break;
        case STORE0_MEM_WORD:
            printf("store word ptr[%04x:%04x] = %02x\n",
                    exe->exe_seg,
                    exe->exe_addr,
                    exe->exe_op0);
            busop_init(bop,
                       exe->exe_seg,
                       exe->exe_addr,
                       exe->exe_op0,
                       BUSOP_WRITE,
                       BUSOP_WORD,
                       ADDR_MEM);
            go_on = false;
            break;
        case STORE0_SI:
            cpu->si = exe->exe_op0;
            break;
        case STORE0_SP:
            cpu->sp = exe->exe_op0;
            break;
        case STORE0_SS:
            cpu->ss = exe->exe_op0;
            break;

        case STORE1_AH:
            set_high_byte(&cpu->ax, exe->exe_op1);
            break;
        case STORE1_AL:
            set_low_byte(&cpu->ax, exe->exe_op1);
            break;
        case STORE1_AX:
            cpu->ax = exe->exe_op1;
            break;
        case STORE1_BH:
            set_high_byte(&cpu->bx, exe->exe_op1);
            break;
        case STORE1_BL:
            set_low_byte(&cpu->bx, exe->exe_op1);
            break;
        case STORE1_BP:
            cpu->bp = exe->exe_op1;
            break;
        case STORE1_BX:
            cpu->bx = exe->exe_op1;
            break;
        case STORE1_CH:
            set_high_byte(&cpu->cx, exe->exe_op1);
            break;
        case STORE1_CL:
            set_low_byte(&cpu->cx, exe->exe_op1);
            break;
        case STORE1_CS:
            cpu->cs = exe->exe_op1;
            break;
        case STORE1_CX:
            cpu->cx = exe->exe_op1;
            break;
        case STORE1_DH:
            set_high_byte(&cpu->dx, exe->exe_op1);
            break;
        case STORE1_DI:
            cpu->di = exe->exe_op1;
            break;
        case STORE1_DL:
            set_low_byte(&cpu->dx, exe->exe_op1);
            break;
        case STORE1_DS:
            cpu->di = exe->exe_op1;
            break;
        case STORE1_DX:
            cpu->di = exe->exe_op1;
            break;
        case STORE1_ES:
            cpu->es = exe->exe_op1;
            break;
        case STORE1_MEM_BYTE:
            busop_init(bop,
                       exe->exe_seg,
                       exe->exe_addr,
                       exe->exe_op1,
                       BUSOP_WRITE,
                       BUSOP_BYTE,
                       ADDR_MEM);
            go_on = false;
            break;
        case STORE1_MEM_WORD:
            busop_init(bop,
                       exe->exe_seg,
                       exe->exe_addr,
                       exe->exe_op1,
                       BUSOP_WRITE,
                       BUSOP_WORD,
                       ADDR_MEM);
            go_on = false;
            break;
        case STORE1_SI:
            cpu->si = exe->exe_op1;
            break;
        case STORE1_SP:
            cpu->sp = exe->exe_op1;
            break;
        case STORE1_SS:
            cpu->ss = exe->exe_op1;
            break;

        case STOSB_A16:
        case STOSW_A16:
            break;
        case SUB:
            res32 = exe->exe_op0 - exe->exe_op1;
            exe->exe_op0 = (uint16_t)res32;
            carry = (res32 >> 16) & 0x01;
            flagcb_set_cb(fcb, &flags_add, res32, carry);
            break;
        case UNDEFINED:
        case UCODES_SIZE:
            assert(false);
        case XOR:
            res16 = exe->exe_op0 & exe->exe_op1;
            flagcb_set_cb(fcb, &flags_log, res16, 0);
            exe->exe_op0 = res16;
            break;
    }
    //printf("Kraj izvrsavanja ukoda.\n");
    if(op->op_ucodes_idx == op->op_ucodes_size){
        go_on = false;
        op->op_executed = true;
    }
    return go_on;
}


void executer_init(executer_p exe, inst_p i)
{
    exe->exe_halt = 0;
    exe->exe_op0  = 0;
    exe->exe_op1  = 0;
    exe->exe_addr = 0;
    operation_init(&exe->exe_op);
    operation_decode_inst(&exe->exe_op, i);
}

void executer_init_interrupt(executer_p exe, int iv)
{
    op_p op = &exe->exe_op;
    exe->exe_halt = 0;
    exe->exe_seg  = 0;
    exe->exe_op0  = 0;
    exe->exe_op1  = 0;
    exe->exe_addr = 0;
    operation_init(op);
    op_write_ucode(op, LOAD0_FLAGS);
    op_write_ucode(op, PUSH);
    op_write_ucode(op, LOAD0_CS);
    op_write_ucode(op, PUSH);
    op_write_ucode(op, LOAD0_IP);
    op_write_ucode(op, PUSH);
    op_write_ucode(op, CLI);
    op_write_ucode(op, CLT);

    op_write_ucode(op, ADDR_INT);
    op_write_arg(op, iv);
    op_write_ucode(op, MEM_READ_WORD);
    op_write_ucode(op, LOAD1_MEM_WORD);
    op_write_ucode(op, ADDR_IW);
    op_write_arg(op, 2);
    op_write_ucode(op, MEM_READ_WORD);
    op_write_ucode(op, LOAD0_MEM_WORD);

    op_write_ucode(op, JUMP_FAR_O16);
}

int executer_next(executer_p exe, cpu_context_p cpu, busop_p bop, flagcb_p fcb)
{
    bool go_on;
    int i = exe->exe_op.op_ucodes_idx;
    if(exe->exe_op.op_executed){
        return false;
    }

    while(true){
        go_on = execute_ucode(&exe->exe_op, exe, cpu, bop, fcb);
        i = exe->exe_op.op_ucodes[exe->exe_op.op_ucodes_idx-1];
        printf("%s\n", ucode_names[i]);
        if(false == go_on){
            break;
        }
    }
    if(exe->exe_op.op_executed && bop->bop_state == NOOP){
        return false;
    }else{
        return true;
    }
}
