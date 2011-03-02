#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "errors.h"
#include "sigutils.h"


typedef struct pins_t {
    int  pin_clk;
    int  pin_reset;
    int  pin_ready;
    int  pin_intr;
    int  pin_nmi;
    int  pin_mn_mxN;
    int  pin_adbusin;
    int  pin_hlda;
    int  pin_intaN;
    int  pin_ale;
    int  pin_m_ioN;
    int  pin_dt_rN;
    int  pin_bheN;
    int  pin_denN;
    int  pin_rdN;
    int  pin_wrN;
    int  pin_asbus;
    int  pin_adbus;
} pins_s, *pins_p;

typedef struct {
    /*General purpose*/
    uint16_t ax;
    uint16_t bx;
    uint16_t cx;
    uint16_t dx;

    /*Indeksni */
    uint16_t si;
    uint16_t di;

    /*Pointer*/
    uint16_t bp;
    uint16_t sp;

    /*Segmentni*/
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t ss;

    /*Flag regisri*/
    uint16_t ip;
    uint16_t psw;

#define CARRY_BIT_IDX  0
#define PARITY_BIT_IDX 2
#define AUXC_BIT_IDX   4
#define ZERO_BIT_IDX   6
#define SIGN_BIT_IDX   7
#define TRAP_BIT_IDX   8
#define INT_BIT_IDX    9
#define DIR_BIT_IDX    10
#define OFLOW_BIT_IDX  11

} cpu_context_s, *cpu_context_p;

uint16_t get_flag(uint16_t flags, int idx);
void clear_flag(uint16_t* flags, int idx);
void set_flag(uint16_t* flags, int idx);
void complement_flag(uint16_t* flags, int idx);
#endif
