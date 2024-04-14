#ifndef __GUI_H
#define __GUI_H

#include <stdbool.h>

extern bool SDL_ready;

extern void SDL__display_show( void );

extern bool gui_events();
extern void gui_init( void );
extern void gui_exit( void );
#endif
