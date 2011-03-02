#ifndef INTAOP_H
#define INTAOP_H

#include <stdbool.h>

#include "cpu.h"
#include "busiface.h"

typedef struct intaop_t {
    int        intaop_idx;
    int        intaop_iv;
    BUS_STATE intaop_state;
} intaop_s, *intaop_p;

void intaop_init(intaop_p intaop);
bool intaop_next(intaop_p intaop, pins_p pins);

#endif

