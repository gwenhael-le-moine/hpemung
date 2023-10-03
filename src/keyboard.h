#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"

extern bool kbd_on;

extern void kbd_out_changed( void );
extern void kbd_key_pressed( int row, int col );
extern void kbd_key_released( int row, int col );
extern void kbd_on_pressed( void );
extern void kbd_on_released( void );

#endif
