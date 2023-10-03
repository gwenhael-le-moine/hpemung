#include "types.h"
#include "cpu.h"
#include "keyboard.h"

bool kbd_on;

static byte kbd_row[ 9 ];

static void update_in( void )
{
    byte in = 0;

    // TODO: Emulate real HP48 keyboard circuit

    if ( cpu.out[ 0 ] & 1 )
        in |= kbd_row[ 0 ];
    if ( cpu.out[ 0 ] & 2 )
        in |= kbd_row[ 1 ];
    if ( cpu.out[ 0 ] & 4 )
        in |= kbd_row[ 2 ];
    if ( cpu.out[ 0 ] & 8 )
        in |= kbd_row[ 3 ];
    if ( cpu.out[ 1 ] & 1 )
        in |= kbd_row[ 4 ];
    if ( cpu.out[ 1 ] & 2 )
        in |= kbd_row[ 5 ];
    if ( cpu.out[ 1 ] & 4 )
        in |= kbd_row[ 6 ];
    if ( cpu.out[ 1 ] & 8 )
        in |= kbd_row[ 7 ];
    if ( cpu.out[ 2 ] & 1 )
        in |= kbd_row[ 8 ];

    cpu.in[ 0 ] = in & 0xF;
    cpu.in[ 1 ] = ( in >> 4 ) & 0xF;
    cpu.in[ 2 ] = 0;
    cpu.in[ 3 ] = kbd_on ? 8 : 0;
}

void kbd_out_changed( void ) { update_in(); }

void kbd_key_pressed( int row, int col )
{
    bool no_key = !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ];
    kbd_row[ row ] |= 1 << col;
    update_in();
    if ( cpu.shutdown && no_key && ( cpu.in[ 0 ] || cpu.in[ 1 ] || cpu.in[ 3 ] ) ) {
        cpu.shutdown = false;
    }
    if ( cpu.keyscan && no_key && ( cpu.in[ 0 ] || cpu.in[ 1 ] || cpu.in[ 3 ] ) ) {
        if ( cpu.inte ) {
            cpu.keyintp = false;
            cpu_interrupt();
        } else {
            cpu.keyintp = true;
        }
    } else if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] ) {
        cpu.keyintp = false;
    }
}

void kbd_key_released( int row, int col )
{
    kbd_row[ row ] &= ~( 1 << col );
    update_in();
    if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] ) {
        cpu.keyintp = false;
    }
}

void kbd_on_pressed( void )
{
    bool no_key = !cpu.in[ 3 ];
    kbd_on = true;
    cpu.in[ 3 ] |= 8;
    if ( cpu.shutdown && no_key ) {
        cpu.shutdown = false;
    }
    if ( cpu.inte && no_key ) {
        cpu_interrupt();
    }
}

void kbd_on_released( void )
{
    kbd_on = false;
    cpu.in[ 3 ] &= ~8;
}
