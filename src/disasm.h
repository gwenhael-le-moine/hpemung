#ifndef __DISASM_H
#define __DISASM_H

#include "types.h"

char* disassemble( byte* ptr );
char* nib_to_hex( byte* nib, int n );
char* nib_to_hex_rev( byte* nib, int n );

#endif
