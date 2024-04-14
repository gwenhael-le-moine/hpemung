#include <stdlib.h>
#include <stdbool.h>

#include "types.h"
#include "bus.h"

byte current_bank;
byte* port2;
address port2mask;

void ports_switch_bank( address adr )
{
    bool need_remap = false;

    if ( current_bank != ( ( ( byte )adr >> 1 ) & 0x1F ) ) {
        current_bank = ( ( byte )adr >> 1 ) & 0x1F;
        if ( port2 ) {
            bus_info.nce3_data = port2 + ( ( current_bank << 18 ) & port2mask );

            if ( bus_info.nce3_cfg )
                need_remap = true;
        }
    }
    if ( !bus_info.ben != !( adr & 0x40 ) ) {
        bus_info.ben = ( adr & 0x40 ) ? true : false;

        if ( bus_info.nce3_cfg )
            need_remap = true;
    }
    if ( need_remap )
        bus_remap();
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
