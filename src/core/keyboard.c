#include "../ui4x/api.h"

#include "keyboard.h"
#include "types.h"
#include "cpu.h"

static byte kbd_row[ 9 ];       /* KEYS_BUFFER_SIZE */

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

static void kbd_key_pressed( int row, int col )
{
    bool no_key = !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ];
    kbd_row[ row ] |= 1 << col;
    update_in();
    if ( cpu.shutdown && no_key && ( cpu.in[ 0 ] || cpu.in[ 1 ] || cpu.in[ 3 ] ) )
        cpu.shutdown = false;

    if ( cpu.int_enable && no_key && ( cpu.in[ 0 ] || cpu.in[ 1 ] || cpu.in[ 3 ] ) ) {
        if ( cpu.int_service ) {
            cpu.int_pending = false;
            cpu_interrupt();
        } else
            cpu.int_pending = true;

    } else if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] )
        cpu.int_pending = false;
}

static void kbd_key_released( int row, int col )
{
    kbd_row[ row ] &= ~( 1 << col );
    update_in();
    if ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] )
        cpu.int_pending = false;
}

/**********/
/* public */
/**********/

bool kbd_on;

void kbd_out_changed( void ) { update_in(); }

void pressKey( int hpkey )
{
    switch ( hpkey ) {
    case HP48_KEY_ON:
        {
            bool no_key = !cpu.in[ 3 ];
            kbd_on = true;
            cpu.in[ 3 ] |= 8;
            if ( cpu.shutdown && no_key )
                cpu.shutdown = false;

            if ( cpu.int_service && no_key )
                cpu_interrupt();
        }
        break;
    case HP48_KEY_PLUS:
        kbd_key_pressed( 0, 0 );
        break;
    case HP48_KEY_SPC:
        kbd_key_pressed( 0, 1 );
        break;
    case HP48_KEY_PERIOD:
        kbd_key_pressed( 0, 2 );
        break;
    case HP48_KEY_0:
        kbd_key_pressed( 0, 3 );
        break;
    case HP48_KEY_QUOTE:
        kbd_key_pressed( 0, 4 );
        break;
    case HP48_KEY_MINUS:
        kbd_key_pressed( 1, 0 );
        break;
    case HP48_KEY_3:
        kbd_key_pressed( 1, 1 );
        break;
    case HP48_KEY_2:
        kbd_key_pressed( 1, 2 );
        break;
    case HP48_KEY_1:
        kbd_key_pressed( 1, 3 );
        break;
    case HP48_KEY_A:
        kbd_key_pressed( 1, 4 );
        break;
    case HP48_KEY_SHR:
        kbd_key_pressed( 1, 5 );
        break;
    case HP48_KEY_MUL:
        kbd_key_pressed( 2, 0 );
        break;
    case HP48_KEY_6:
        kbd_key_pressed( 2, 1 );
        break;
    case HP48_KEY_5:
        kbd_key_pressed( 2, 2 );
        break;
    case HP48_KEY_4:
        kbd_key_pressed( 2, 3 );
        break;
    case HP48_KEY_MTH:
        kbd_key_pressed( 2, 4 );
        break;
    case HP48_KEY_SHL:
        kbd_key_pressed( 2, 5 );
        break;
    case HP48_KEY_DIV:
        kbd_key_pressed( 3, 0 );
        break;
    case HP48_KEY_9:
        kbd_key_pressed( 3, 1 );
        break;
    case HP48_KEY_8:
        kbd_key_pressed( 3, 2 );
        break;
    case HP48_KEY_7:
        kbd_key_pressed( 3, 3 );
        break;
    case HP48_KEY_SIN:
        kbd_key_pressed( 3, 4 );
        break;
    case HP48_KEY_ALPHA:
        kbd_key_pressed( 3, 5 );
        break;
    case HP48_KEY_BS:
        kbd_key_pressed( 4, 0 );
        break;
    case HP48_KEY_DEL:
        kbd_key_pressed( 4, 1 );
        break;
    case HP48_KEY_EEX:
        kbd_key_pressed( 4, 2 );
        break;
    case HP48_KEY_NEG:
        kbd_key_pressed( 4, 3 );
        break;
    case HP48_KEY_ENTER:
        kbd_key_pressed( 4, 4 );
        break;
    case HP48_KEY_INV:
        kbd_key_pressed( 5, 0 );
        break;
    case HP48_KEY_POWER:
        kbd_key_pressed( 5, 1 );
        break;
    case HP48_KEY_SQRT:
        kbd_key_pressed( 5, 2 );
        break;
    case HP48_KEY_TAN:
        kbd_key_pressed( 5, 3 );
        break;
    case HP48_KEY_COS:
        kbd_key_pressed( 5, 4 );
        break;
    case HP48_KEY_RIGHT:
        kbd_key_pressed( 6, 0 );
        break;
    case HP48_KEY_DOWN:
        kbd_key_pressed( 6, 1 );
        break;
    case HP48_KEY_LEFT:
        kbd_key_pressed( 6, 2 );
        break;
    case HP48_KEY_EVAL:
        kbd_key_pressed( 6, 3 );
        break;
    case HP48_KEY_STO:
        kbd_key_pressed( 6, 4 );
        break;
    case HP48_KEY_NXT:
        kbd_key_pressed( 7, 0 );
        break;
    case HP48_KEY_UP:
        kbd_key_pressed( 7, 1 );
        break;
    case HP48_KEY_VAR:
        kbd_key_pressed( 7, 2 );
        break;
    case HP48_KEY_CST:
        kbd_key_pressed( 7, 3 );
        break;
    case HP48_KEY_PRG:
        kbd_key_pressed( 7, 4 );
        break;
    case HP48_KEY_F:
        kbd_key_pressed( 8, 0 );
        break;
    case HP48_KEY_E:
        kbd_key_pressed( 8, 1 );
        break;
    case HP48_KEY_D:
        kbd_key_pressed( 8, 2 );
        break;
    case HP48_KEY_C:
        kbd_key_pressed( 8, 3 );
        break;
    case HP48_KEY_B:
        kbd_key_pressed( 8, 4 );
        break;
    }
}

void releaseKey( int hpkey )
{
    switch ( hpkey ) {
    case HP48_KEY_ON:
        kbd_on = false;
        cpu.in[ 3 ] &= ~8;
        break;
    case HP48_KEY_PLUS:
        kbd_key_released( 0, 0 );
        break;
    case HP48_KEY_SPC:
        kbd_key_released( 0, 1 );
        break;
    case HP48_KEY_PERIOD:
        kbd_key_released( 0, 2 );
        break;
    case HP48_KEY_0:
        kbd_key_released( 0, 3 );
        break;
    case HP48_KEY_QUOTE:
        kbd_key_released( 0, 4 );
        break;
    case HP48_KEY_MINUS:
        kbd_key_released( 1, 0 );
        break;
    case HP48_KEY_3:
        kbd_key_released( 1, 1 );
        break;
    case HP48_KEY_2:
        kbd_key_released( 1, 2 );
        break;
    case HP48_KEY_1:
        kbd_key_released( 1, 3 );
        break;
    case HP48_KEY_A:
        kbd_key_released( 1, 4 );
        break;
    case HP48_KEY_SHR:
        kbd_key_released( 1, 5 );
        break;
    case HP48_KEY_MUL:
        kbd_key_released( 2, 0 );
        break;
    case HP48_KEY_6:
        kbd_key_released( 2, 1 );
        break;
    case HP48_KEY_5:
        kbd_key_released( 2, 2 );
        break;
    case HP48_KEY_4:
        kbd_key_released( 2, 3 );
        break;
    case HP48_KEY_MTH:
        kbd_key_released( 2, 4 );
        break;
    case HP48_KEY_SHL:
        kbd_key_released( 2, 5 );
        break;
    case HP48_KEY_DIV:
        kbd_key_released( 3, 0 );
        break;
    case HP48_KEY_9:
        kbd_key_released( 3, 1 );
        break;
    case HP48_KEY_8:
        kbd_key_released( 3, 2 );
        break;
    case HP48_KEY_7:
        kbd_key_released( 3, 3 );
        break;
    case HP48_KEY_SIN:
        kbd_key_released( 3, 4 );
        break;
    case HP48_KEY_ALPHA:
        kbd_key_released( 3, 5 );
        break;
    case HP48_KEY_BS:
        kbd_key_released( 4, 0 );
        break;
    case HP48_KEY_DEL:
        kbd_key_released( 4, 1 );
        break;
    case HP48_KEY_EEX:
        kbd_key_released( 4, 2 );
        break;
    case HP48_KEY_NEG:
        kbd_key_released( 4, 3 );
        break;
    case HP48_KEY_ENTER:
        kbd_key_released( 4, 4 );
        break;
    case HP48_KEY_INV:
        kbd_key_released( 5, 0 );
        break;
    case HP48_KEY_POWER:
        kbd_key_released( 5, 1 );
        break;
    case HP48_KEY_SQRT:
        kbd_key_released( 5, 2 );
        break;
    case HP48_KEY_TAN:
        kbd_key_released( 5, 3 );
        break;
    case HP48_KEY_COS:
        kbd_key_released( 5, 4 );
        break;
    case HP48_KEY_RIGHT:
        kbd_key_released( 6, 0 );
        break;
    case HP48_KEY_DOWN:
        kbd_key_released( 6, 1 );
        break;
    case HP48_KEY_LEFT:
        kbd_key_released( 6, 2 );
        break;
    case HP48_KEY_EVAL:
        kbd_key_released( 6, 3 );
        break;
    case HP48_KEY_STO:
        kbd_key_released( 6, 4 );
        break;
    case HP48_KEY_NXT:
        kbd_key_released( 7, 0 );
        break;
    case HP48_KEY_UP:
        kbd_key_released( 7, 1 );
        break;
    case HP48_KEY_VAR:
        kbd_key_released( 7, 2 );
        break;
    case HP48_KEY_CST:
        kbd_key_released( 7, 3 );
        break;
    case HP48_KEY_PRG:
        kbd_key_released( 7, 4 );
        break;
    case HP48_KEY_F:
        kbd_key_released( 8, 0 );
        break;
    case HP48_KEY_E:
        kbd_key_released( 8, 1 );
        break;
    case HP48_KEY_D:
        kbd_key_released( 8, 2 );
        break;
    case HP48_KEY_C:
        kbd_key_released( 8, 3 );
        break;
    case HP48_KEY_B:
        kbd_key_released( 8, 4 );
        break;
    }
}
