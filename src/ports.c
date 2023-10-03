#include <stdlib.h>

#include "types.h"
#include "bus.h"
#include "ports.h"

static byte current_bank;
static byte* port2;
static address port2mask;

void ports_init( void )
{
    // ce1 = bank switcher
    bus_info.ce1_data = NULL;
    bus_info.ce1_mask = 0x0007F;
    bus_info.ce1_r_o = true;
    bus_info.ce1_bs = true;

    // ce2 = port1 (plugged)
    bus_info.ce2_data = malloc( PORT1_SIZE );
    bus_info.ce2_mask = PORT1_SIZE - 1;
    bus_info.ce2_r_o = false;

    // nce3 = port2 (plugged)
    port2 = malloc( PORT2_SIZE );
    port2mask = PORT2_SIZE - 1;
    bus_info.nce3_data = port2;
    bus_info.nce3_mask = port2mask & 0x3FFFF;
    bus_info.nce3_r_o = false;

    bus_info.ben = false;
    current_bank = 0;
}

void ports_exit( void ) {}

void ports_switch_bank( address adr )
{
    bool need_remap = false;

    if ( current_bank != ( ( ( byte )adr >> 1 ) & 0x1F ) ) {
        current_bank = ( ( byte )adr >> 1 ) & 0x1F;
        if ( port2 ) {
            bus_info.nce3_data = port2 + ( ( current_bank << 18 ) & port2mask );
            if ( bus_info.nce3_cfg ) {
                need_remap = true;
            }
        }
    }
    if ( !bus_info.ben != !( adr & 0x40 ) ) {
        bus_info.ben = ( adr & 0x40 ) ? true : false;
        if ( bus_info.nce3_cfg ) {
            need_remap = true;
        }
    }
    if ( need_remap ) {
        bus_remap();
    }
}

byte ports_card_detect( void )
{
    byte x = 0;
    if ( bus_info.nce3_data )
        x |= 0x1;
    if ( bus_info.ce2_data )
        x |= 0x2;
    if ( !bus_info.nce3_r_o )
        x |= 0x4;
    if ( !bus_info.ce2_r_o )
        x |= 0x8;
    return x;
}
