#ifndef __PDEBUG_H
#define __PDEBUG_H

#include "types.h"

void draw_cpu( void );

void pdebug_state_changed( void );
void pdebug_draw_true_speed( dword speed );

// void pdebug_show(BITMAP *bmp);
void pdebug_hide( void );
void pdebug_down( int mx, int my, int mb );
void pdebug_up( int mx, int my, int mb );

#endif
