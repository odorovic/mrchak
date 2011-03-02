#ifndef SIGUTILS_H
#define SIGUTILS_H

#define NO_CHANGE -1
#define HIGHZ     -2

#define UPDATE_IF_CHANGE(val, handle, val_s)                         \
    do{                                                              \
        PLI_INT32 oldfmt;                                            \
        s_vpi_time delay;                                            \
        delay.type = vpiSimTime;                                     \
        delay.low =  0;                                              \
        delay.high = 0;                                              \
        if(NO_CHANGE != (val)){                                      \
            oldfmt = (val_s).format;                                 \
            if(HIGHZ != (val)){                                      \
                (val_s).format = vpiIntVal;                          \
                (val_s).value.integer = (PLI_INT32)(val);            \
            }else{                                                   \
                (val_s).format = vpiVectorVal;                       \
                vecval.aval = 0;                                     \
                vecval.bval = ~0;                                    \
                (val_s).value.vector = &vecval;                      \
            }                                                        \
            vpi_put_value((handle), &(val_s), &delay, vpiInertialDelay);     \
            (val_s).format = oldfmt ;                                \
        }                                                            \
    }while(0)                                                        \

#endif
