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
    bool carry : true;
    bool dec : true;
    bool shutdown : true;
    bool inte : true;
    bool keyscan : true;
    bool keyintp : true;
    dword cycles;
    dword inst_cnt;
} Cpu;

#define HST_XM 1
#define HST_SB 2
#define HST_SR 4
#define HST_MP 8

enum RegisterNames { A, B, C, D };

extern Cpu cpu;

extern void cpu_interrupt( void );
extern void execute_instruction( void );

#endif
