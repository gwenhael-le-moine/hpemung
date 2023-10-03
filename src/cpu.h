#ifndef __CPU_H
#define __CPU_H

#include "types.h"

typedef struct {
    byte reg[ 4 ][ 16 ];
    byte reg_r[ 5 ][ 16 ];
    byte st[ 4 ];
    byte in[ 4 ];
    byte out[ 3 ];
    byte hst;
    byte p;
    address d[ 2 ];
    address pc;
    address rstk[ 8 ];
    int rstk_ptr;
    boolean carry : 1;
    boolean dec : 1;
    boolean shutdown : 1;
    boolean inte : 1;
    boolean keyscan : 1;
    boolean keyintp : 1;
    dword cycles;
    dword inst_cnt;
} Cpu;

#define HST_XM 1
#define HST_SB 2
#define HST_SR 4
#define HST_MP 8

enum RegisterNames { A, B, C, D };

extern Cpu cpu;

void cpu_interrupt( void );
void execute_instruction( void );

#endif
