#ifndef __PORTS_H
#define __PORTS_H

#include "types.h"

#define PORT1_SIZE ( 256 * 1024 ) /* 128Kio in nibbles */
#define PORT2_SIZE ( 256 * 1024 ) /* 128Kio in nibbles */

void ports_init( void );
void ports_exit( void );
void ports_switch_bank( address adr );
byte ports_card_detect( void );

#endif
