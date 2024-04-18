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

static void kbd_key_pressed( int row, int col )
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

static void kbd_key_released( int row, int col )
{
    kbd_row[ row ] &= ~( 1 << col );
    update_in();
    if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] )
        cpu.keyintp = false;
}

void press_key( int hpkey )
{
    switch ( hpkey ) {
        case HPKEY_ON:
            {
                bool no_key = !cpu.in[ 3 ];
                kbd_on = true;
                cpu.in[ 3 ] |= 8;
                if ( cpu.shutdown && no_key )
                    cpu.shutdown = false;

                if ( cpu.inte && no_key )
                    cpu_interrupt();
            }
            break;
        case HPKEY_PLUS:
            {
                kbd_key_pressed( 0, 0 );
            }
            break;
        case HPKEY_SPC:
            {
                kbd_key_pressed( 0, 1 );
            }
            break;
        case HPKEY_PERIOD:
            {
                kbd_key_pressed( 0, 2 );
            }
            break;
        case HPKEY_0:
            {
                kbd_key_pressed( 0, 3 );
            }
            break;
        case HPKEY_QUOTE:
            {
                kbd_key_pressed( 0, 4 );
            }
            break;
        case HPKEY_MINUS:
            {
                kbd_key_pressed( 1, 0 );
            }
            break;
        case HPKEY_3:
            {
                kbd_key_pressed( 1, 1 );
            }
            break;
        case HPKEY_2:
            {
                kbd_key_pressed( 1, 2 );
            }
            break;
        case HPKEY_1:
            {
                kbd_key_pressed( 1, 3 );
            }
            break;
        case HPKEY_A:
            {
                kbd_key_pressed( 1, 4 );
            }
            break;
        case HPKEY_SHR:
            {
                kbd_key_pressed( 1, 5 );
            }
            break;
        case HPKEY_MUL:
            {
                kbd_key_pressed( 2, 0 );
            }
            break;
        case HPKEY_6:
            {
                kbd_key_pressed( 2, 1 );
            }
            break;
        case HPKEY_5:
            {
                kbd_key_pressed( 2, 2 );
            }
            break;
        case HPKEY_4:
            {
                kbd_key_pressed( 2, 3 );
            }
            break;
        case HPKEY_MTH:
            {
                kbd_key_pressed( 2, 4 );
            }
            break;
        case HPKEY_SHL:
            {
                kbd_key_pressed( 2, 5 );
            }
            break;
        case HPKEY_DIV:
            {
                kbd_key_pressed( 3, 0 );
            }
            break;
        case HPKEY_9:
            {
                kbd_key_pressed( 3, 1 );
            }
            break;
        case HPKEY_8:
            {
                kbd_key_pressed( 3, 2 );
            }
            break;
        case HPKEY_7:
            {
                kbd_key_pressed( 3, 3 );
            }
            break;
        case HPKEY_SIN:
            {
                kbd_key_pressed( 3, 4 );
            }
            break;
        case HPKEY_ALPHA:
            {
                kbd_key_pressed( 3, 5 );
            }
            break;
        case HPKEY_BS:
            {
                kbd_key_pressed( 4, 0 );
            }
            break;
        case HPKEY_DEL:
            {
                kbd_key_pressed( 4, 1 );
            }
            break;
        case HPKEY_EEX:
            {
                kbd_key_pressed( 4, 2 );
            }
            break;
        case HPKEY_NEG:
            {
                kbd_key_pressed( 4, 3 );
            }
            break;
        case HPKEY_ENTER:
            {
                kbd_key_pressed( 4, 4 );
            }
            break;
        case HPKEY_INV:
            {
                kbd_key_pressed( 5, 0 );
            }
            break;
        case HPKEY_POWER:
            {
                kbd_key_pressed( 5, 1 );
            }
            break;
        case HPKEY_SQRT:
            {
                kbd_key_pressed( 5, 2 );
            }
            break;
        case HPKEY_TAN:
            {
                kbd_key_pressed( 5, 3 );
            }
            break;
        case HPKEY_COS:
            {
                kbd_key_pressed( 5, 4 );
            }
            break;
        case HPKEY_RIGHT:
            {
                kbd_key_pressed( 6, 0 );
            }
            break;
        case HPKEY_DOWN:
            {
                kbd_key_pressed( 6, 1 );
            }
            break;
        case HPKEY_LEFT:
            {
                kbd_key_pressed( 6, 2 );
            }
            break;
        case HPKEY_EVAL:
            {
                kbd_key_pressed( 6, 3 );
            }
            break;
        case HPKEY_STO:
            {
                kbd_key_pressed( 6, 4 );
            }
            break;
        case HPKEY_NXT:
            {
                kbd_key_pressed( 7, 0 );
            }
            break;
        case HPKEY_UP:
            {
                kbd_key_pressed( 7, 1 );
            }
            break;
        case HPKEY_VAR:
            {
                kbd_key_pressed( 7, 2 );
            }
            break;
        case HPKEY_CST:
            {
                kbd_key_pressed( 7, 3 );
            }
            break;
        case HPKEY_PRG:
            {
                kbd_key_pressed( 7, 4 );
            }
            break;
        case HPKEY_F:
            {
                kbd_key_pressed( 8, 0 );
            }
            break;
        case HPKEY_E:
            {
                kbd_key_pressed( 8, 1 );
            }
            break;
        case HPKEY_D:
            {
                kbd_key_pressed( 8, 2 );
            }
            break;
        case HPKEY_C:
            {
                kbd_key_pressed( 8, 3 );
            }
            break;
        case HPKEY_B:
            {
                kbd_key_pressed( 8, 4 );
            }
            break;
    }
}

void release_key( int hpkey )
{
    switch ( hpkey ) {
        case HPKEY_ON:
            {
                kbd_on = false;
                cpu.in[ 3 ] &= ~8;
            }
            break;
        case HPKEY_PLUS:
            {
                kbd_key_released( 0, 0 );
            }
            break;
        case HPKEY_SPC:
            {
                kbd_key_released( 0, 1 );
            }
            break;
        case HPKEY_PERIOD:
            {
                kbd_key_released( 0, 2 );
            }
            break;
        case HPKEY_0:
            {
                kbd_key_released( 0, 3 );
            }
            break;
        case HPKEY_QUOTE:
            {
                kbd_key_released( 0, 4 );
            }
            break;
        case HPKEY_MINUS:
            {
                kbd_key_released( 1, 0 );
            }
            break;
        case HPKEY_3:
            {
                kbd_key_released( 1, 1 );
            }
            break;
        case HPKEY_2:
            {
                kbd_key_released( 1, 2 );
            }
            break;
        case HPKEY_1:
            {
                kbd_key_released( 1, 3 );
            }
            break;
        case HPKEY_A:
            {
                kbd_key_released( 1, 4 );
            }
            break;
        case HPKEY_SHR:
            {
                kbd_key_released( 1, 5 );
            }
            break;
        case HPKEY_MUL:
            {
                kbd_key_released( 2, 0 );
            }
            break;
        case HPKEY_6:
            {
                kbd_key_released( 2, 1 );
            }
            break;
        case HPKEY_5:
            {
                kbd_key_released( 2, 2 );
            }
            break;
        case HPKEY_4:
            {
                kbd_key_released( 2, 3 );
            }
            break;
        case HPKEY_MTH:
            {
                kbd_key_released( 2, 4 );
            }
            break;
        case HPKEY_SHL:
            {
                kbd_key_released( 2, 5 );
            }
            break;
        case HPKEY_DIV:
            {
                kbd_key_released( 3, 0 );
            }
            break;
        case HPKEY_9:
            {
                kbd_key_released( 3, 1 );
            }
            break;
        case HPKEY_8:
            {
                kbd_key_released( 3, 2 );
            }
            break;
        case HPKEY_7:
            {
                kbd_key_released( 3, 3 );
            }
            break;
        case HPKEY_SIN:
            {
                kbd_key_released( 3, 4 );
            }
            break;
        case HPKEY_ALPHA:
            {
                kbd_key_released( 3, 5 );
            }
            break;
        case HPKEY_BS:
            {
                kbd_key_released( 4, 0 );
            }
            break;
        case HPKEY_DEL:
            {
                kbd_key_released( 4, 1 );
            }
            break;
        case HPKEY_EEX:
            {
                kbd_key_released( 4, 2 );
            }
            break;
        case HPKEY_NEG:
            {
                kbd_key_released( 4, 3 );
            }
            break;
        case HPKEY_ENTER:
            {
                kbd_key_released( 4, 4 );
            }
            break;
        case HPKEY_INV:
            {
                kbd_key_released( 5, 0 );
            }
            break;
        case HPKEY_POWER:
            {
                kbd_key_released( 5, 1 );
            }
            break;
        case HPKEY_SQRT:
            {
                kbd_key_released( 5, 2 );
            }
            break;
        case HPKEY_TAN:
            {
                kbd_key_released( 5, 3 );
            }
            break;
        case HPKEY_COS:
            {
                kbd_key_released( 5, 4 );
            }
            break;
        case HPKEY_RIGHT:
            {
                kbd_key_released( 6, 0 );
            }
            break;
        case HPKEY_DOWN:
            {
                kbd_key_released( 6, 1 );
            }
            break;
        case HPKEY_LEFT:
            {
                kbd_key_released( 6, 2 );
            }
            break;
        case HPKEY_EVAL:
            {
                kbd_key_released( 6, 3 );
            }
            break;
        case HPKEY_STO:
            {
                kbd_key_released( 6, 4 );
            }
            break;
        case HPKEY_NXT:
            {
                kbd_key_released( 7, 0 );
            }
            break;
        case HPKEY_UP:
            {
                kbd_key_released( 7, 1 );
            }
            break;
        case HPKEY_VAR:
            {
                kbd_key_released( 7, 2 );
            }
            break;
        case HPKEY_CST:
            {
                kbd_key_released( 7, 3 );
            }
            break;
        case HPKEY_PRG:
            {
                kbd_key_released( 7, 4 );
            }
            break;
        case HPKEY_F:
            {
                kbd_key_released( 8, 0 );
            }
            break;
        case HPKEY_E:
            {
                kbd_key_released( 8, 1 );
            }
            break;
        case HPKEY_D:
            {
                kbd_key_released( 8, 2 );
            }
            break;
        case HPKEY_C:
            {
                kbd_key_released( 8, 3 );
            }
            break;
        case HPKEY_B:
            {
                kbd_key_released( 8, 4 );
            }
            break;
    }
}
