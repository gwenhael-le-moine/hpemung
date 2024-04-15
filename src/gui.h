#ifndef __GUI_H
#define __GUI_H

#include <stdbool.h>

extern void SDL__display_show( void );

extern bool gui_events();
extern bool gui_init( void );
extern bool gui_exit( void );
#endif
