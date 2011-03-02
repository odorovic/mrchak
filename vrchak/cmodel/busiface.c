
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "busiface.h"

#define OFFSET(addr)  ((uint16_t)addr)
#define SEGMENT(addr) ((uint16_t)(addr>>(sizeof(uint16_t)*8)))

static int busiface_handle_start(busop_p bop, pins_p pins)
{
    assert(bop);
    assert(pins);
    assert(START == bop->bop_state);
    if(0 == pins->pin_clk){
        if(BUSOP_READ == bop->bop_direction){
            pins->pin_dt_rN = 0;
        }else if(BUSOP_WRITE == bop->bop_direction){
            pins->pin_dt_rN = 1;
        }
        pins->pin_ale = 1;
        bop->bop_state = T1;
    }else{
        printf("ERROR!\n");
        REPORT_ERROR(INVALID_STATE);
    }
    return 0;
}

static int busiface_handle_t1(busop_p bop, pins_p pins)
{
    assert(bop);
    assert(pins);
    assert(T1 == bop->bop_state);
    if(1 == pins->pin_clk){
        pins->pin_m_ioN = bop->bop_m_io;
        if(BUSOP_BYTE == bop->bop_size && (0 == bop->bop_address%2)){
            pins->pin_bheN = 1;
        }else{
            pins->pin_bheN = 0;
        }
        pins->pin_asbus = SEGMENT(bop->bop_address);
        pins->pin_adbus = OFFSET(bop->bop_address);
        bop->bop_state = T1;
    }else if(0 == pins->pin_clk){
        pins->pin_ale = 0;
        bop->bop_state = T2;
    }else{
        REPORT_ERROR(INVALID_VALUE);
    }
    return 0;
}

static int busiface_handle_t2(busop_p bop, pins_p pins)
{
    assert(bop);
    assert(pins);
    assert(T2 == bop->bop_state);
    if(1 == pins->pin_clk){
        if(BUSOP_WRITE == bop->bop_direction){
            pins->pin_adbus = bop->bop_data;
            pins->pin_denN = 0;
            pins->pin_wrN = 0;
        }else{
            pins->pin_adbus = HIGHZ;
            pins->pin_denN = 0;
            pins->pin_rdN = 0;
        }
        bop->bop_state = TWAIT;
    }
    return 0;
}

static int busiface_handle_twait(busop_p bop, pins_p pins)
{
    assert(bop);
    assert(pins);
    assert(TWAIT == bop->bop_state);
    if(1 == pins->pin_clk){
        if(1 == pins->pin_ready){
            bop->bop_state = T4;
            if(BUSOP_READ == bop->bop_direction){
                bop->bop_data = pins->pin_adbusin;
                pins->pin_rdN = 1;
            }else{
                pins->pin_wrN = 1;
            }
        }
    }
    return 0;
}

static int busiface_handle_t4(busop_p bop, pins_p pins)
{
    assert(bop);
    assert(pins);
    assert(T4 == bop->bop_state);
    if(1 == pins->pin_clk){
        pins->pin_adbus = HIGHZ;
        pins->pin_denN = 1;
        bop->bop_state = FC;
    }
    return 0;
}


void busop_init(busop_p bop,
                uint16_t segment,
                uint16_t offset,
                int data,
                OP_SIZE size,
                BUS_DIR dir,
                ADDR_SPACE mio)
{
    uint32_t addr = (uint32_t)offset + ((uint32_t)segment<<4);
    bop->bop_address = addr;
    bop->bop_data = data;
    bop->bop_size =size;
    bop->bop_direction = dir;
    bop->bop_m_io = mio;
    bop->bop_state = START;
}

int busiface_next(busop_p bop, pins_p pins)
{
    assert(bop);
    assert(pins);
    //printf("busiface_next: %d state: ", pins->pin_clk);
    switch(bop->bop_state){
        case NOOP:
            //printf("NOOP\n");
            break;
        case START:
            //printf("START\n");
            //Entered on first negedge after busop initialization. S <= T1.
            CHECK_RC(busiface_handle_start(bop, pins));
            break;
        case T1:
            //printf("T1\n");
            //Entered on posedge and negedge S <= T1 and S <= T2 respectively.
            CHECK_RC(busiface_handle_t1(bop, pins));
            break;
        case T2:
            //printf("T2\n");
            //Entered on posedge and negedge S <= T2 and S <= TWAIT respectively.
            CHECK_RC(busiface_handle_t2(bop, pins));
            break;
        case T3:
        case TWAIT:
            //printf("TWAIT\n");
            //Entered until posedge and READY == 1 then S <= T4
            CHECK_RC(busiface_handle_twait(bop, pins));
            break;
        case T4:
            //printf("T4\n");
            CHECK_RC(busiface_handle_t4(bop, pins));
            break;
        default:
            printf("Invalid bop state %d\n", bop->bop_state);
            REPORT_ERROR(INVALID_STATE);
            break;
    }
    if(FC == bop->bop_state){
        return FC;
    }else{
        return 0;
    }
}

uint8_t busop_get_byte(busop_p bop)
{
    uint16_t data;
    assert(BUSOP_BYTE == bop->bop_size);
    data = bop->bop_data;
    if(bop->bop_address % 2){
        data = data >> 8;
    }
    return (uint8_t)(data & 0xff);
}
