#ifndef __CPU_H
#define __CPU_H

#include <stdbool.h>

#include "types.h"

typedef struct {
    byte reg[ 4 ][ 16 ];
    byte reg_r[ 5 ][ 16 ];
    address d[ 2 ];

    byte p;
    address pc;
    byte in[ 4 ];
    byte out[ 3 ];
    byte st[ 4 ];

    byte hst;

    address rstk[ 8 ];
    int rstk_ptr;

    int fs_idx_lo[ 16 ];
    int fl_idx_hi[ 16 ];

    bool hexmode : true;
    bool carry : true;
    bool shutdown : true;
    bool int_service : true;
    bool int_enable : true;
    bool int_pending : true;

    dword cycles;
} cpu_t;

#define HST_XM 1
#define HST_SB 2
#define HST_SR 4
#define HST_MP 8

enum RegisterNames { A, B, C, D };

extern cpu_t cpu;

extern void cpu_interrupt( void );
extern void execute_instruction( void );

#endif
