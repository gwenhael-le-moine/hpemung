#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include "types.h"

extern address menu_base;
extern address display_base;
extern address display_line_offset;
extern byte display_line_count;
extern byte display_height;
extern byte display_offset;
extern boolean display_enable;

void display_init( void );
void display_exit( void );
void display_update( void );
void display_show();

#endif
