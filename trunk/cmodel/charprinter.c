#include  <vpi_user.h>
#include  <stdlib.h>

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>

#include "sigutils.h"

#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 4000

typedef struct  pins_t {
    int pin_csN;
    int pin_wrN;
    int pin_data;
} pins_s, *pins_p;

typedef struct charprinter_data_t {
    vpiHandle   csN_h;
    vpiHandle   wrN_h;
    vpiHandle   data_h;

    vpiHandle   wr_callback_h;
    vpiHandle   end_callback_h;
    p_vpi_time  time_stub;
    p_vpi_value value_stub;

#ifndef _MSC_VER
    struct sockaddr_in addr;
    int    socket;
#endif
} charprinter_data_s, *charprinter_data_p;

PLI_INT32 charprinter_end_sim(p_cb_data cb_data)
{
    charprinter_data_p cp_data;
    cp_data = (charprinter_data_p)cb_data->user_data;

    vpi_free_object(cp_data->wr_callback_h);
    vpi_free_object(cp_data->end_callback_h);
    free(cp_data->time_stub);
    free(cp_data->value_stub);
    return 0;
}

PLI_INT32 charprinter_interface(p_cb_data cb_data)
{
    pins_s pins;

    char buf;
    s_vpi_value  value_s;
    //s_vpi_vecval vecval;
    charprinter_data_p cp_data;

    pins.pin_csN  = 0;
    pins.pin_wrN  = 0;
    pins.pin_data = 0;

    cp_data = (charprinter_data_p)cb_data->user_data;
    value_s.format = vpiScalarVal;
    vpi_get_value(cp_data->csN_h, &value_s);
    pins.pin_csN = (int)value_s.value.integer;
    vpi_get_value(cp_data->wrN_h, &value_s);
    pins.pin_wrN = (int)value_s.value.integer;
    value_s.format = vpiIntVal;
    vpi_get_value(cp_data->data_h, &value_s);
    pins.pin_data = (int)value_s.value.integer;

#ifndef _MSC_VER
    if(0 == pins.pin_csN && 1 == pins.pin_wrN){
        buf = (char)pins.pin_data;
        if(-1 == sendto(cp_data->socket,
                &buf,
                1,
                0, //flags
                (const struct sockaddr *)&cp_data->addr,
                sizeof(struct sockaddr_in))){
            printf("error!");
        }
    }
#endif
    return 0;
}

PLI_INT32 charprinter_calltf(PLI_BYTE8* arg)
{
    vpiHandle   instance_h, arg_itr, cb_h ;
    p_vpi_value value_p;
    p_vpi_time  time_p;
    s_cb_data   cb_data_s;

    charprinter_data_p cp_data;
    cp_data = malloc(sizeof(charprinter_data_s));

    /* obtain a handle to the system task instance */
    instance_h = vpi_handle(vpiSysTfCall, NULL);

    /* obtain handles to system task arguments */
    /* compiletf has already verified arguments are correct */
    arg_itr = vpi_iterate(vpiArgument, instance_h);
    //TODO: More sanity checking.
    cp_data->csN_h    = vpi_scan(arg_itr);
    cp_data->wrN_h    = vpi_scan(arg_itr);
    cp_data->data_h   = vpi_scan(arg_itr);
    vpi_free_object (arg_itr); /* free iterator--did not scan to null */

    time_p  = malloc(sizeof(s_vpi_time));
    value_p = malloc(sizeof(s_vpi_value));
    time_p->type         = vpiSuppressTime;
    value_p->format      = vpiSuppressVal;
    cb_data_s.reason     = cbValueChange;
    cb_data_s.cb_rtn     = charprinter_interface;
    cb_data_s.time       = time_p;
    cb_data_s.value      = value_p;
    /* add value change callbacks to clock input to the C model, */
    /* pass pointer to storage for handles as user_data value */
    cb_data_s.user_data = (PLI_UBYTE8 *)cp_data;
    cb_data_s.obj       = cp_data->wrN_h;
    cb_h = vpi_register_cb(&cb_data_s);
    cp_data->wr_callback_h = cb_h;

    /* add end of simulation callback. */
    cb_data_s.reason    = cbEndOfSimulation;
    cb_data_s.cb_rtn    = charprinter_end_sim;
    cb_h = vpi_register_cb(&cb_data_s);
    cp_data->end_callback_h = cb_h;
    cp_data->time_stub = time_p;
    cp_data->value_stub = value_p;

#ifndef _MSC_VER
    /*Socket stuff*/
    if(-1 == (cp_data->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))){
        exit(1);
    }
    memset((char*)&cp_data->addr, 0, sizeof(cp_data->addr));
    cp_data->addr.sin_family = AF_INET;
    cp_data->addr.sin_port = htons(SERV_PORT);
    if(0==inet_aton(SERV_ADDR, &cp_data->addr.sin_addr)){
        printf("inet_aton() error\n");
        exit(1);
    }
#endif
    return(0);
}

