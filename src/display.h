#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdlib.h>

#include "types.h"

extern address menu_base;
extern address display_base;
extern address display_line_offset;
extern byte display_line_count;
extern byte display_height;
extern byte display_offset;
extern bool display_enable;

extern void display_init( void );
extern void display_exit( void );
extern void display_update( void );
extern void display_show( void );

#endif
