#include "types.h"
#include "cpu.h"
#include "persistence.h"

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
    if ( cpu.shutdown && no_key && ( cpu.in[ 0 ] || cpu.in[ 1 ] || cpu.in[ 3 ] ) )
        cpu.shutdown = false;

    if ( cpu.keyscan && no_key && ( cpu.in[ 0 ] || cpu.in[ 1 ] || cpu.in[ 3 ] ) ) {
        if ( cpu.inte ) {
            cpu.keyintp = false;
            cpu_interrupt();
        } else
            cpu.keyintp = true;

    } else if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] )
        cpu.keyintp = false;
}

void kbd_key_released( int row, int col )
{
    kbd_row[ row ] &= ~( 1 << col );
    update_in();
    if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] )
        cpu.keyintp = false;
}

void press_PLUS( void ) { kbd_key_pressed( 0, 0 ); }
void release_PLUS( void ) { kbd_key_released( 0, 0 ); }
void press_SPC( void ) { kbd_key_pressed( 0, 1 ); }
void release_SPC( void ) { kbd_key_released( 0, 1 ); }
void press_PERIOD( void ) { kbd_key_pressed( 0, 2 ); }
void release_PERIOD( void ) { kbd_key_released( 0, 2 ); }
void press_0( void ) { kbd_key_pressed( 0, 3 ); }
void release_0( void ) { kbd_key_released( 0, 3 ); }
void press_QUOTE( void ) { kbd_key_pressed( 0, 4 ); }
void release_QUOTE( void ) { kbd_key_released( 0, 4 ); }
void press_MINUS( void ) { kbd_key_pressed( 1, 0 ); }
void release_MINUS( void ) { kbd_key_released( 1, 0 ); }
void press_3( void ) { kbd_key_pressed( 1, 1 ); }
void release_3( void ) { kbd_key_released( 1, 1 ); }
void press_2( void ) { kbd_key_pressed( 1, 2 ); }
void release_2( void ) { kbd_key_released( 1, 2 ); }
void press_1( void ) { kbd_key_pressed( 1, 3 ); }
void release_1( void ) { kbd_key_released( 1, 3 ); }
void press_A( void ) { kbd_key_pressed( 1, 4 ); }
void release_A( void ) { kbd_key_released( 1, 4 ); }
void press_RSHIFT( void ) { kbd_key_pressed( 1, 5 ); }
void release_RSHIFT( void ) { kbd_key_released( 1, 5 ); }
void press_MULT( void ) { kbd_key_pressed( 2, 0 ); }
void release_MULT( void ) { kbd_key_released( 2, 0 ); }
void press_6( void ) { kbd_key_pressed( 2, 1 ); }
void release_6( void ) { kbd_key_released( 2, 1 ); }
void press_5( void ) { kbd_key_pressed( 2, 2 ); }
void release_5( void ) { kbd_key_released( 2, 2 ); }
void press_4( void ) { kbd_key_pressed( 2, 3 ); }
void release_4( void ) { kbd_key_released( 2, 3 ); }
void press_MTH( void ) { kbd_key_pressed( 2, 4 ); }
void release_MTH( void ) { kbd_key_released( 2, 4 ); }
void press_LSHIFT( void ) { kbd_key_pressed( 2, 5 ); }
void release_LSHIFT( void ) { kbd_key_released( 2, 5 ); }
void press_DIV( void ) { kbd_key_pressed( 3, 0 ); }
void release_DIV( void ) { kbd_key_released( 3, 0 ); }
void press_9( void ) { kbd_key_pressed( 3, 1 ); }
void release_9( void ) { kbd_key_released( 3, 1 ); }
void press_8( void ) { kbd_key_pressed( 3, 2 ); }
void release_8( void ) { kbd_key_released( 3, 2 ); }
void press_7( void ) { kbd_key_pressed( 3, 3 ); }
void release_7( void ) { kbd_key_released( 3, 3 ); }
void press_SIN( void ) { kbd_key_pressed( 3, 4 ); }
void release_SIN( void ) { kbd_key_released( 3, 4 ); }
void press_ALPHA( void ) { kbd_key_pressed( 3, 5 ); }
void release_ALPHA( void ) { kbd_key_released( 3, 5 ); }
void press_BKSP( void ) { kbd_key_pressed( 4, 0 ); }
void release_BKSP( void ) { kbd_key_released( 4, 0 ); }
void press_DEL( void ) { kbd_key_pressed( 4, 1 ); }
void release_DEL( void ) { kbd_key_released( 4, 1 ); }
void press_EEX( void ) { kbd_key_pressed( 4, 2 ); }
void release_EEX( void ) { kbd_key_released( 4, 2 ); }
void press_NEG( void ) { kbd_key_pressed( 4, 3 ); }
void release_NEG( void ) { kbd_key_released( 4, 3 ); }
void press_ENTER( void ) { kbd_key_pressed( 4, 4 ); }
void release_ENTER( void ) { kbd_key_released( 4, 4 ); }
void press_INV( void ) { kbd_key_pressed( 5, 0 ); }
void release_INV( void ) { kbd_key_released( 5, 0 ); }
void press_POW( void ) { kbd_key_pressed( 5, 1 ); }
void release_POW( void ) { kbd_key_released( 5, 1 ); }
void press_SQRT( void ) { kbd_key_pressed( 5, 2 ); }
void release_SQRT( void ) { kbd_key_released( 5, 2 ); }
void press_TAN( void ) { kbd_key_pressed( 5, 3 ); }
void release_TAN( void ) { kbd_key_released( 5, 3 ); }
void press_COS( void ) { kbd_key_pressed( 5, 4 ); }
void release_COS( void ) { kbd_key_released( 5, 4 ); }
void press_RIGHT( void ) { kbd_key_pressed( 6, 0 ); }
void release_RIGHT( void ) { kbd_key_released( 6, 0 ); }
void press_DOWN( void ) { kbd_key_pressed( 6, 1 ); }
void release_DOWN( void ) { kbd_key_released( 6, 1 ); }
void press_LEFT( void ) { kbd_key_pressed( 6, 2 ); }
void release_LEFT( void ) { kbd_key_released( 6, 2 ); }
void press_EVAL( void ) { kbd_key_pressed( 6, 3 ); }
void release_EVAL( void ) { kbd_key_released( 6, 3 ); }
void press_STO( void ) { kbd_key_pressed( 6, 4 ); }
void release_STO( void ) { kbd_key_released( 6, 4 ); }
void press_NXT( void ) { kbd_key_pressed( 7, 0 ); }
void release_NXT( void ) { kbd_key_released( 7, 0 ); }
void press_UP( void ) { kbd_key_pressed( 7, 1 ); }
void release_UP( void ) { kbd_key_released( 7, 1 ); }
void press_VAR( void ) { kbd_key_pressed( 7, 2 ); }
void release_VAR( void ) { kbd_key_released( 7, 2 ); }
void press_CST( void ) { kbd_key_pressed( 7, 3 ); }
void release_CST( void ) { kbd_key_released( 7, 3 ); }
void press_PRG( void ) { kbd_key_pressed( 7, 4 ); }
void release_PRG( void ) { kbd_key_released( 7, 4 ); }
void press_F( void ) { kbd_key_pressed( 8, 0 ); }
void release_F( void ) { kbd_key_released( 8, 0 ); }
void press_E( void ) { kbd_key_pressed( 8, 1 ); }
void release_E( void ) { kbd_key_released( 8, 1 ); }
void press_D( void ) { kbd_key_pressed( 8, 2 ); }
void release_D( void ) { kbd_key_released( 8, 2 ); }
void press_C( void ) { kbd_key_pressed( 8, 3 ); }
void release_C( void ) { kbd_key_released( 8, 3 ); }
void press_B( void ) { kbd_key_pressed( 8, 4 ); }
void release_B( void ) { kbd_key_released( 8, 4 ); }

void press_ON( void )
{
    bool no_key = !cpu.in[ 3 ];
    kbd_on = true;
    cpu.in[ 3 ] |= 8;
    if ( cpu.shutdown && no_key )
        cpu.shutdown = false;

    if ( cpu.inte && no_key )
        cpu_interrupt();
}

void release_ON( void )
{
    kbd_on = false;
    cpu.in[ 3 ] &= ~8;
}

void press_LoadFile( void ) {}
void release_LoadFile( void ) { load_file_on_stack( "zeldahp.dir" ); }
