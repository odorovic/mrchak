#ifndef FLAGCB_H
#define FLAGCB_H

#include <stdint.h>

typedef struct flag_callback_t {
    uint16_t* fcb_flags;
    uint16_t  fcb_result;
    int       fcb_carry;
    uint16_t  (*fcb_eval)(struct flag_callback_t*);
} flagcb_s, *flagcb_p;

typedef uint16_t (*fcb_fp)(flagcb_p);

void flagcb_init(flagcb_p fcb, uint16_t* flags);

void flagcb_set_cb(flagcb_p, fcb_fp, uint16_t res, int carry);
void flagcb_eval(flagcb_p fcb);

uint16_t flags_sfzfcf(flagcb_p);
uint16_t flags_sfzfpf(flagcb_p);
uint16_t flags_add(flagcb_p);
uint16_t flags_log(flagcb_p);

#endif
