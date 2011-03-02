#ifndef EXECUTER_H
#define EXECUTER_H

#include <stdint.h>

#include "instruction.h"
#include "operation.h"
#include "busiface.h"
#include "flagcb.h"
#include "cpu.h"

typedef struct executer_t {
    op_s      exe_op;
    int       exe_halt;
    uint16_t  exe_op0;
    uint16_t  exe_op1;
    uint16_t  exe_dst;
    uint16_t  exe_addr;
    uint16_t  exe_seg;
} executer_s, *executer_p;

void executer_init(executer_p exe, inst_p i);
void executer_init_interrupt(executer_p exe, int iv);
int executer_next(executer_p exe, cpu_context_p, busop_p bop, flagcb_p fcb);

#endif
