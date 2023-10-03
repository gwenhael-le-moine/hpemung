#ifndef __EMULATOR_H
#define __EMULATOR_H

#include "types.h"

enum EmulatorStates { EMULATOR_STOP, EMULATOR_STEP, EMULATOR_RUN };

extern volatile bool please_exit;
extern dword emulator_speed;

extern void emulator_init( void );
extern void emulator_exit( void );
extern bool emulator_run( void );

extern void emulator_set_state( int state );

#endif
