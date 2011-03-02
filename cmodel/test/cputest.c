
#include "../cputest.c"

static int test_read_cycle()
{
    bus_op_s bop;
    pins_s   pins;

    bus_op_init(&bop,
                DEFAULT_ADDR,
                0,
                OP_WORD,
                BUSOP_READ,
                ADDR_MEM);
    pins_init(&pins);

}
