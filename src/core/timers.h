#ifndef __TIMERS_H
#define __TIMERS_H

#include "types.h"

extern byte timer1_control;
extern byte timer2_control;
extern byte timer1_value;
extern dword timer2_value;

extern void timer1_update( void );
extern void timer2_update( void );

#endif
