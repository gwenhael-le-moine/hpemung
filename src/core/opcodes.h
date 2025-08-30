#ifndef __OPCODES_H
#define __OPCODES_H

#include "types.h"

typedef struct Opcode Opcode;

struct Opcode {
    void ( *exec )( byte* );
    Opcode* next;
    char* dissasm;
};

extern Opcode opcodes[ 16 ];

#endif
