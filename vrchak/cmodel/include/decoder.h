#ifndef DECODER_H
#define DECODER_H

#include "operation.h"
#include "instruction.h"

typedef struct dec_t {
    bool dec_amode_decoded;
} dec_s, *dec_p;

bool decodingAddressMode(dec_p dec);
int  operation_decode_inst(op_p op, inst_p in);

#endif

