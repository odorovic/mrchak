#ifndef PREFICES_H
#define PREFICES_H

#define PREFICES_SG       0x7
#define PREFICES_ES       0x1
#define PREFICES_CS       0x2
#define PREFICES_SS       0x3
#define PREFICES_DS       0x4
#define PREFICES_FS       0x5
#define PREFICES_GS       0x6

#define PREFICES_OPERAND  0x8
#define PREFICES_ADDRESS  0x10

#define PREFICES_REPNE    0x20
#define PREFICES_REPE     0x40

#define PREFICES_REP      (PREFICES_REPE | PREFICES_REPNE)

#define PREFICES_LOCK     0x80

#endif
