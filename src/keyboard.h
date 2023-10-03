#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"

extern bool kbd_on;

void kbd_out_changed( void );
void kbd_key_pressed( int row, int col );
void kbd_key_released( int row, int col );
void kbd_on_pressed( void );
void kbd_on_released( void );

#endif
