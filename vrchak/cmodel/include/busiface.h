#ifndef BUSIFACE_H
#define BUSIFACE_H

#include <stdint.h>

#include "cpu.h"

//#define FC (-1)

typedef enum {
    BUSOP_BYTE,
    BUSOP_WORD
} OP_SIZE;

typedef enum {
    BUSOP_READ,
    BUSOP_WRITE
} BUS_DIR;

typedef enum {
    ADDR_IO,
    ADDR_MEM
} ADDR_SPACE;

typedef enum {
    NOOP,
    START,
    T1,
    T2,
    T3,
    TWAIT,
    T4,
    FC
} BUS_STATE;

typedef struct busop_t {
    uint32_t   bop_address;
    uint16_t   bop_data;
    OP_SIZE    bop_size;
    BUS_DIR    bop_direction;
    ADDR_SPACE bop_m_io;
    BUS_STATE  bop_state;
} busop_s, *busop_p;

void busop_init(busop_p, uint16_t, uint16_t, int, OP_SIZE, BUS_DIR, ADDR_SPACE);
int busiface_next(busop_p, pins_p);
uint8_t busop_get_byte(busop_p);

#endif
