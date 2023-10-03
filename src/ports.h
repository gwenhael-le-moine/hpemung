#ifndef __PORTS_H
#define __PORTS_H

#include "types.h"

void ports_init( void );
void ports_exit( void );
void ports_switch_bank( address adr );
byte ports_card_detect( void );

#endif
