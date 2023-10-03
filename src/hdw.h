#ifndef __HDW_H
#define __HDW_H

#include "types.h"

extern byte hdw_read_nibble( address adr );
extern void hdw_write_nibble( byte data, address adr );

#endif
