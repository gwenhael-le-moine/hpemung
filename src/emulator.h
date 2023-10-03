#ifndef __EMULATOR_H
#define __EMULATOR_H

#include "types.h"

enum EmulatorStates { EMULATOR_STOP, EMULATOR_STEP, EMULATOR_RUN };

void true_speed_proc( void );

extern volatile bool please_exit;
extern dword emulator_speed;

void emulator_init( void );
void emulator_exit( void );
bool emulator_run( void );

void emulator_set_state( int state );
int emulator_get_state( void );

#endif
