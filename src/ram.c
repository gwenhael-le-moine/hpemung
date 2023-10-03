#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "bus.h"
#include "ram.h"

static address ram_size = 256 * 1024; // in nibbles, not bytes!

void ram_init( void )
{
    byte* buf = malloc( ram_size );
    if ( !buf )
        exit( 0x20 );

    memset( buf, 0, ram_size );
    bus_info.ram_data = buf;
    bus_info.ram_mask = ram_size - 1;
}

void ram_exit( void )
{
    free( bus_info.ram_data );
    bus_info.ram_data = NULL;
    bus_info.ram_mask = 0x00000;
}
