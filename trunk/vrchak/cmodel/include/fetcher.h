#ifndef FETCHER_H
#define FETCHER_H

#include <stdbool.h>

#include "instruction.h"

typedef enum fetcher_state_t {
    FETCH_INNITIAL,
    FETCH_MODRM,
    FETCH_ARGS,
    FETCH_DISP,
    FETCH_IMM,
    FETCH_DONE
} FETCHER_STATE;

typedef struct fetcher_t {
    FETCHER_STATE  fet_state;
    inst_s fet_inst;
} fetcher_s, *fetcher_p;

void fetcher_init(fetcher_p f);
bool fetcher_next(fetcher_p f, uint8_t next, int addr);

#endif
