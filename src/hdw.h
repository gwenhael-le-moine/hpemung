#ifndef __HDW_H
#define __HDW_H

#include "types.h"

void hdw_init( void );
void hdw_exit( void );

byte hdw_read_nibble( address adr );
void hdw_write_nibble( byte data, address adr );

#endif
