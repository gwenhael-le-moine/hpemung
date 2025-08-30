#ifndef __PORTS_H
#define __PORTS_H

#include "types.h"

extern byte current_bank;
extern byte* port2;
extern address port2mask;

extern void ports_switch_bank( address adr );
extern byte ports_card_detect( void );

#endif
