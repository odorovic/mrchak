#include "intaop.h"

void intaop_init(intaop_p intaop)
{
    intaop->intaop_idx = 0;
    intaop->intaop_iv  = 0;
    intaop->intaop_state = START;
}
bool intaop_next(intaop_p intaop, pins_p pins)
{
    switch(intaop->intaop_state){
        case START:
            //Negedge after initialization
            pins->pin_ale = 1;
            intaop->intaop_state = T1;
            break;
        case T1:
            if(1 == pins->pin_clk){
                pins->pin_ale = 0;
                intaop->intaop_state = T2;
            }
            break;
        case T2:
            if(1 == pins->pin_clk){
                pins->pin_intaN = 0;
                pins->pin_denN =0;
                intaop->intaop_state = T3;
            }
            break;
        case T3:
            if(1 == pins->pin_clk){
                intaop->intaop_state = T4;
            }
            break;
        case T4:
            if(1 == pins->pin_clk){
                pins->pin_intaN = 1;
                pins->pin_denN =1;
                intaop->intaop_iv = pins->pin_adbusin;
                intaop->intaop_idx++;
                intaop->intaop_state = FC;
            }
            break;
        default:
            REPORT_ERROR(INVALID_STATE);
    }
    if(FC == intaop->intaop_state){
        if(2 == intaop->intaop_idx){
            return false;
        }else{
            intaop->intaop_state = START;
            return true;
        }
    }else{
        return true;
    }
}
