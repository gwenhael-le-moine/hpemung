#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>

extern bool kbd_on;

extern void kbd_out_changed( void );

extern void pressKey( int hpkey );
extern void releaseKey( int hpkey );

#endif
