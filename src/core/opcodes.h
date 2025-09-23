#ifndef __OPCODES_H
#define __OPCODES_H

#include "types.h"

typedef struct Opcode {
    void ( *exec )( byte* );
    struct Opcode* next;        /* `opcode_t` is `struct Opcode` but can't be used here as it's not declared yet */
    char* dissasm;
} opcode_t;

extern opcode_t opcodes[ 16 ];

#endif
