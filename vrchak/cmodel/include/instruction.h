#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <stdio.h>

#define MAX_X86_SIZE 32

typedef struct inst_t {
    uint8_t  inst_prefices;
    uint8_t  inst_opcode;
    uint8_t  inst_modrm;
    int      inst_disp;
    int      inst_disp_size;
    int      inst_disp_read;
    int      inst_imm;
    int      inst_imm_size;
    int      inst_imm_read;

    uint16_t inst_start_seg;
    uint16_t inst_start_off;
    uint8_t  inst_bytes[MAX_X86_SIZE];
    int      inst_bytes_size;
} inst_s, *inst_p;

void dump_instruction(inst_p inst, FILE* f);

#endif
