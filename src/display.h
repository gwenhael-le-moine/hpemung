#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdlib.h>
#include <stdbool.h>

#include "types.h"

#define LCD_WIDTH 131
#define LCD_HEIGHT 64

extern address menu_base;
extern address display_base;
extern address display_line_offset;
extern byte display_line_count;
extern byte display_height;
extern byte display_offset;
extern bool display_enable;
extern bool shouldRender;
extern byte lcdScreenGS[];

extern void display_update( void );

#endif
