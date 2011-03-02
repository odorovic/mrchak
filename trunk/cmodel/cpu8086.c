#include  <vpi_user.h>
#include  <stdlib.h>
#include  <assert.h>

#include "cpu.h"
#include "busiface.h"
#include "fetcher.h"
#include "executer.h"
#include "intaop.h"
#include "flagcb.h"

#define CPU_START_CS 0xf000
#define CPU_START_IP 0xfff0
#define CPU_START_SS 0x0000
#define CPU_START_SP 0x0000

#define NMI_IV       2

typedef enum {
    CPU_INITIAL,
    CPU_FETCH,
    CPU_EXECUTE,
    CPU_HALT,
    CPU_INTA,
    CPU_RESET,
} CPU_STATE;

typedef struct cpu_t {
    cpu_context_s cpu_regs;
    busop_s       cpu_busop;
    fetcher_s     cpu_fetcher;
    executer_s    cpu_executer;
    intaop_s      cpu_intaop;
    flagcb_s      cpu_flagcb;
    CPU_STATE     cpu_state;
} cpu_s, *cpu_p;

typedef struct vpihandles_t {
    cpu_p       cpu;
    vpiHandle   clk_h;
    vpiHandle   reset_h;
    vpiHandle   ready_h;
    vpiHandle   intr_h;
    vpiHandle   nmi_h;
    vpiHandle   mn_mxN_h;
    vpiHandle   adbusin_h;
    vpiHandle   hlda_h;
    vpiHandle   intaN_h;
    vpiHandle   ale_h;
    vpiHandle   m_ioN_h;
    vpiHandle   dt_rN_h;
    vpiHandle   bheN_h;
    vpiHandle   denN_h;
    vpiHandle   rdN_h;
    vpiHandle   wrN_h;
    vpiHandle   asbus_h;
    vpiHandle   adbus_h;

    vpiHandle   clk_callback_h;
    vpiHandle   end_callback_h;
    p_vpi_time  time_stub;
    p_vpi_value value_stub;

} cpu8086_data_s, *cpu8086_data_p;

uint16_t get_flag(uint16_t flags, int idx)
{
    assert(idx < 16);
    return (flags >> idx) & 0x1;
}

void clear_flag(uint16_t* flags, int idx)
{
    uint16_t mask = ~(1 << idx);
    assert(idx < 16);
    *flags = (*flags) & mask;
}

void set_flag(uint16_t* flags, int idx)
{
    uint16_t mask = (1 << idx);
    assert(idx < 16);
    *flags = (*flags) | mask;
}

void complement_flag(uint16_t* flags, int idx)
{
    uint16_t flag;
    assert(idx < 16);
    flag = get_flag(*flags, idx);
    if(0 == flag){
        set_flag(flags, idx);
    }else{
        clear_flag(flags, idx);
    }
}

static void cpu_init(cpu_p cpu)
{
    fetcher_init(&cpu->cpu_fetcher);
    flagcb_init(&cpu->cpu_flagcb, &cpu->cpu_regs.psw);
    cpu->cpu_state = CPU_INITIAL;
}

static void cpu_reset_regs(cpu_p cpu)
{
    cpu_context_s cc;
    cc.ax  = 0;
    cc.bx  = 0;
    cc.cx  = 0;
    cc.dx  = 0;

        /*Indeksni */
    cc.si  = 0;
    cc.di  = 0;

        /*Pointer*/
    cc.bp  = 0;
    cc.sp  = CPU_START_SP;

        /*Segmentni*/
    cc.cs  = CPU_START_CS;
    cc.ds  = 0;
    cc.es  = 0;
    cc.ss  = CPU_START_SS;

        /*Flag regisri*/
    cc.ip  = CPU_START_IP;
    cc.psw = 0;
    cpu->cpu_regs = cc;
    vpi_printf("%x", cpu->cpu_regs.ip);
}


static void cpu_reset(cpu_p cpu)
{
    cpu->cpu_state = CPU_RESET;
    cpu_reset_regs(cpu);
}

static bool intr_enabled(uint16_t flags)
{
    return flags & (1 << INT_BIT_IDX);
}

static void cpu_dump_context(cpu_p cpu)
{
    cpu_context_p cc = &cpu->cpu_regs;
    printf("Dumping CPU context:\n");
    printf("[CPU]\n");
    printf("  AX=%04x\n", cc->ax);
    printf("  BX=%04x\n", cc->bx);
    printf("  CX=%04x\n", cc->cx);
    printf("  DX=%04x\n", cc->dx);
    printf("  SI=%04x\n", cc->si);
    printf("  DI=%04x\n", cc->di);
    printf("  BP=%04x\n", cc->bp);
    printf("  SP=%04x\n", cc->sp);
    printf("  CS=%04x\n", cc->cs);
    printf("  DS=%04x\n", cc->ds);
    printf("  SS=%04x\n", cc->ss);
    printf("  ES=%04x\n", cc->es);
    printf("  IP=%04x\n", cc->ip);
    printf("  FLAGS=%04x\n\n", cc->psw);
}

static void cpu_clear_outputs(pins_p pins)
{
    pins->pin_dt_rN = 1;
    pins->pin_denN  = 1;
    pins->pin_m_ioN  = 1;
    pins->pin_rdN  = 1;
    pins->pin_wrN  = 1;
    pins->pin_intaN  = 1;
    pins->pin_asbus  = 0;
    pins->pin_bheN  = 1;
    pins->pin_ale  = 0;
}


static void cpu_init_inst_read(cpu_p cpu)
{
    busop_init(&cpu->cpu_busop,
                cpu->cpu_regs.cs,
                cpu->cpu_regs.ip,
                0,
                BUSOP_BYTE,
                BUSOP_READ,
                ADDR_MEM);
    cpu->cpu_regs.ip++;
}

void cpu8086_model(cpu_p cpu, pins_p  pins)
{
    if(0 == pins->pin_reset){
//        vpi_printf("reset!\n");
        if(CPU_RESET != cpu->cpu_state){
            cpu_reset(cpu);
            cpu_clear_outputs(pins);
        }
    }else if(1 == pins->pin_clk){
        switch(cpu->cpu_state){
            case CPU_FETCH:
//                vpi_printf("Fetch.\n");
                if(FC == busiface_next(&cpu->cpu_busop, pins)){
                    if(false == fetcher_next(&cpu->cpu_fetcher,
                                busop_get_byte(&cpu->cpu_busop),
                                cpu->cpu_busop.bop_address)){
                        cpu->cpu_busop.bop_state = NOOP;
                        executer_init(&cpu->cpu_executer, &cpu->cpu_fetcher.fet_inst);
                        cpu->cpu_state = CPU_EXECUTE;
                    }else{
                        cpu_init_inst_read(cpu);
                    }
                }
                break;
            case CPU_EXECUTE:
//                vpi_printf("Execute.\n");
                if(NOOP==cpu->cpu_busop.bop_state ||
                   FC==busiface_next(&cpu->cpu_busop, pins)){
                    cpu->cpu_busop.bop_state = NOOP;
                    if(false==executer_next(&cpu->cpu_executer,
                                            &cpu->cpu_regs,
                                            &cpu->cpu_busop,
                                            &cpu->cpu_flagcb)){
                        if(1 == pins->pin_nmi){
                            executer_init_interrupt(&cpu->cpu_executer, NMI_IV);
                        }else if(1 == pins->pin_intr &&
                                 intr_enabled(cpu->cpu_regs.psw)){
                            intaop_init(&cpu->cpu_intaop);
                            cpu->cpu_state = CPU_INTA;
                        }else if(1 == cpu->cpu_executer.exe_halt){
                            cpu->cpu_state = CPU_HALT;
                        }else{
                            cpu_init_inst_read(cpu);
                            cpu->cpu_state = CPU_FETCH;
                            fetcher_init(&cpu->cpu_fetcher);
                        }
                    }
                }
                break;
            case CPU_HALT:
                if(1 == pins->pin_nmi){
                    executer_init_interrupt(&cpu->cpu_executer, NMI_IV);
                }else if(1 == pins->pin_intr &&
                         intr_enabled(cpu->cpu_regs.psw)){
                    printf("Spoljni prekid!\n");
                    intaop_init(&cpu->cpu_intaop);
                    cpu->cpu_state = CPU_INTA;
                }
                //todo provera prekida;
                break;
            case CPU_INTA:
                assert(NOOP == cpu->cpu_busop.bop_state);
                if(false == intaop_next(&cpu->cpu_intaop, pins)){
                    executer_init_interrupt(&cpu->cpu_executer,
                                             cpu->cpu_intaop.intaop_iv);
                    cpu->cpu_state = CPU_EXECUTE;
                }
                break;
            case CPU_INITIAL:
            case CPU_RESET:
//                vpi_printf("Reset.\n");
                cpu_init_inst_read(cpu);
                cpu->cpu_state = CPU_FETCH;
                break;
        }
    }else if(0 == pins->pin_clk){
        switch(cpu->cpu_busop.bop_state){
            case START:
            case T1:
                busiface_next(&cpu->cpu_busop, pins);
                break;
            default:
                break;
        }
    }
}

PLI_INT32 cpu8086_end_sim(p_cb_data cb_data)
{

    cpu8086_data_p cpu_data;
    cpu_data = (cpu8086_data_p)cb_data->user_data;


    vpi_free_object(cpu_data->clk_callback_h);
    vpi_free_object(cpu_data->end_callback_h);
    free(cpu_data->time_stub);
    free(cpu_data->value_stub);
    cpu_dump_context(cpu_data->cpu);
    return 0;
}

PLI_INT32 cpu8086_interface(p_cb_data cb_data)
{
    s_vpi_value  value_s;
    cpu8086_data_p cpu_data;
    s_vpi_vecval vecval;

    pins_s pins;
    pins.pin_clk       = 0;
    pins.pin_reset     = 0;
    pins.pin_ready     = 0;
    pins.pin_intr      = 0;
    pins.pin_nmi       = 0;
    pins.pin_adbusin   = 0;
    pins.pin_mn_mxN    = NO_CHANGE;
    pins.pin_hlda      = NO_CHANGE;
    pins.pin_intaN     = NO_CHANGE;
    pins.pin_ale       = NO_CHANGE;
    pins.pin_m_ioN     = NO_CHANGE;
    pins.pin_dt_rN     = NO_CHANGE;
    pins.pin_bheN      = NO_CHANGE;
    pins.pin_denN      = NO_CHANGE;
    pins.pin_rdN       = NO_CHANGE;
    pins.pin_wrN       = NO_CHANGE;
    pins.pin_asbus     = NO_CHANGE;
    pins.pin_adbus     = NO_CHANGE;

    cpu_data = (cpu8086_data_p)cb_data->user_data;
    value_s.format = vpiScalarVal;
    vpi_get_value(cpu_data->clk_h, &value_s);
    pins.pin_clk = (int)value_s.value.integer;
    value_s.format = vpiIntVal;
    vpi_get_value(cpu_data->reset_h, &value_s);
    pins.pin_reset = (int)value_s.value.integer;
    vpi_get_value(cpu_data->ready_h, &value_s);
    pins.pin_ready = (int)value_s.value.integer;
    vpi_get_value(cpu_data->intr_h, &value_s);
    pins.pin_intr = (int)value_s.value.integer;
    vpi_get_value(cpu_data->nmi_h, &value_s);
    pins.pin_nmi = (int)value_s.value.integer;
    vpi_get_value(cpu_data->adbusin_h, &value_s);
    pins.pin_adbusin = (int)value_s.value.integer;
    cpu8086_model(cpu_data->cpu, &pins);
    UPDATE_IF_CHANGE(pins.pin_mn_mxN, cpu_data->mn_mxN_h, value_s);
    UPDATE_IF_CHANGE(pins.pin_hlda,   cpu_data->hlda_h,   value_s);
    UPDATE_IF_CHANGE(pins.pin_intaN,  cpu_data->intaN_h,  value_s);
    UPDATE_IF_CHANGE(pins.pin_ale,    cpu_data->ale_h,    value_s);
    UPDATE_IF_CHANGE(pins.pin_m_ioN,  cpu_data->m_ioN_h,  value_s);
    UPDATE_IF_CHANGE(pins.pin_dt_rN,  cpu_data->dt_rN_h,  value_s);
    UPDATE_IF_CHANGE(pins.pin_bheN,   cpu_data->bheN_h,   value_s);
    UPDATE_IF_CHANGE(pins.pin_denN,   cpu_data->denN_h,   value_s);
    UPDATE_IF_CHANGE(pins.pin_rdN,    cpu_data->rdN_h,    value_s);
    UPDATE_IF_CHANGE(pins.pin_wrN,    cpu_data->wrN_h,    value_s);
    UPDATE_IF_CHANGE(pins.pin_asbus,  cpu_data->asbus_h,  value_s);
    UPDATE_IF_CHANGE(pins.pin_adbus,  cpu_data->adbus_h,  value_s);
    return 0;
}

PLI_INT32 cpu8086_calltf(PLI_BYTE8* arg)
{
    vpiHandle   instance_h, arg_itr, cb_h;
    p_vpi_value value_p;
    p_vpi_time  time_p;
    s_cb_data   cb_data_s;

    cpu8086_data_p cpu_data;
    cpu_data = malloc(sizeof(cpu8086_data_s));
    cpu_data->cpu = malloc(sizeof(cpu_s));
    cpu_init(cpu_data->cpu);

    /* obtain a handle to the system task instance */
    instance_h = vpi_handle(vpiSysTfCall, NULL);
    //tmp_h = vpi_handle(vpiScope, instance_h);
    //c = vpi_get_str(vpiFullName, tmp_h);
    //printf("Scope name %s\n", c);
    //tmp_h = vpi_handle(vpiModule, tmp_h);
    //c = vpi_get_str(vpiFullName, tmp_h);
    //printf("Scope name %s\n", c);
    /* obtain handles to system task arguments */
    /* compiletf has already verified arguments are correct */
    arg_itr = vpi_iterate(vpiArgument, instance_h);
    //TODO: More sanity checking.
    cpu_data->clk_h    = vpi_scan(arg_itr);
    cpu_data->reset_h  = vpi_scan(arg_itr);
    cpu_data->ready_h  = vpi_scan(arg_itr);
    cpu_data->intr_h   = vpi_scan(arg_itr);
    cpu_data->nmi_h    = vpi_scan(arg_itr);
    cpu_data->mn_mxN_h = vpi_scan(arg_itr);
    cpu_data->adbusin_h= vpi_scan(arg_itr);
    cpu_data->hlda_h   = vpi_scan(arg_itr);
    cpu_data->intaN_h  = vpi_scan(arg_itr);
    cpu_data->ale_h    = vpi_scan(arg_itr);
    cpu_data->m_ioN_h  = vpi_scan(arg_itr);
    cpu_data->dt_rN_h  = vpi_scan(arg_itr);
    cpu_data->bheN_h   = vpi_scan(arg_itr);
    cpu_data->denN_h   = vpi_scan(arg_itr);
    cpu_data->rdN_h    = vpi_scan(arg_itr);
    cpu_data->wrN_h    = vpi_scan(arg_itr);
    cpu_data->asbus_h  = vpi_scan(arg_itr);
    cpu_data->adbus_h  = vpi_scan(arg_itr);
    vpi_free_object (arg_itr); /* free iterator--did not scan to null */

    time_p  = malloc(sizeof(s_vpi_time));
    value_p = malloc(sizeof(s_vpi_value));
    time_p->type         = vpiSuppressTime;
    value_p->format      = vpiSuppressVal;
    cb_data_s.reason     = cbValueChange;
    cb_data_s.cb_rtn     = cpu8086_interface;
    cb_data_s.time       = time_p;
    cb_data_s.value      = value_p;
    /* add value change callbacks to clock input to the C model, */
    /* pass pointer to storage for handles as user_data value */
    cb_data_s.user_data = (PLI_UBYTE8 *)cpu_data;
    cb_data_s.obj       = cpu_data->clk_h;
    cb_h = vpi_register_cb(&cb_data_s);
    cpu_data->clk_callback_h = cb_h;

    /* add end of simulation callback. */
    cb_data_s.reason    = cbEndOfSimulation;
    cb_data_s.cb_rtn    = cpu8086_end_sim;
    cb_h = vpi_register_cb(&cb_data_s);
    cpu_data->end_callback_h = cb_h;
    cpu_data->time_stub = time_p;
    cpu_data->value_stub = value_p;

    return(0);
}

void cpu8086_register()
{
      s_vpi_systf_data tf_data;
      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$cpu8086";
      tf_data.calltf    = cpu8086_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = 0;
      vpi_register_systf(&tf_data);
}

PLI_INT32 charprinter_calltf(PLI_BYTE8*);

void charprinter_register()
{
      s_vpi_systf_data tf_data;
      tf_data.type      = vpiSysTask;
      tf_data.tfname    = "$charprinter";
      tf_data.calltf    = charprinter_calltf;
      tf_data.compiletf = 0;
      tf_data.sizetf    = 0;
      tf_data.user_data = 0;
      vpi_register_systf(&tf_data);
}

DLLEXPORT void (*vlog_startup_routines[])() = {
    cpu8086_register,
    charprinter_register,
    0
};

