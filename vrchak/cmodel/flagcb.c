#include "flagcb.h"
#include "cpu.h"

void flagcb_init(flagcb_p fcb, uint16_t* flags)
{
    fcb->fcb_eval = 0;
    fcb->fcb_flags = flags;
}

void flagcb_set_cb(flagcb_p fcb, fcb_fp eval, uint16_t res, int carry)
{
    fcb->fcb_eval = eval;
    fcb->fcb_result = res;
    fcb->fcb_carry = carry;
}

void flagcb_eval(flagcb_p fcb)
{
    assert(fcb);
    if(fcb->fcb_eval){
        *(fcb->fcb_flags) = fcb->fcb_eval(fcb);
    }
}

static int calc_parity(uint16_t val)
{
    return 0;
}

uint16_t flags_sfzfcf(flagcb_p fcb)
{
    return 0;
}

uint16_t flags_sfzfpf(flagcb_p fcb)
{
    return 0;
}

uint16_t flags_add(flagcb_p fcb)
{
    uint16_t res = fcb->fcb_result;
    int parity = calc_parity(res);
    int carry = fcb->fcb_carry;
    uint16_t flags = *(fcb->fcb_flags);
    if(1 == carry){
        set_flag(&flags, CARRY_BIT_IDX);
    }else if(0 == carry){
        clear_flag(&flags, CARRY_BIT_IDX);
    }
    if(1 == parity){
        set_flag(&flags, PARITY_BIT_IDX);
    }else{
        set_flag(&flags, PARITY_BIT_IDX);
    }
    if(0 == res){
        set_flag(&flags, ZERO_BIT_IDX);
    }else{
        clear_flag(&flags, ZERO_BIT_IDX);
    }
    if(res >= 0){
        set_flag(&flags, SIGN_BIT_IDX);
    }else{
        clear_flag(&flags, SIGN_BIT_IDX);
    }
    return flags;

}

uint16_t flags_log(flagcb_p fcb)
{
    return *(fcb->fcb_flags);
}
