#ifndef __PORTS_H
#define __PORTS_H

#include "types.h"

#define PORT1_SIZE ( 256 * 1024 ) /* 128Kio in nibbles */
#define PORT2_SIZE ( 256 * 1024 ) /* 128Kio in nibbles */

extern void ports_init( void );
extern void ports_exit( void );
extern void ports_switch_bank( address adr );
extern byte ports_card_detect( void );

#endif
