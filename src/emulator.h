#ifndef __EMULATOR_H
#define __EMULATOR_H

#include <stdbool.h>

#include "types.h"

enum EmulatorStates { EMULATOR_STOP, EMULATOR_STEP, EMULATOR_RUN };

extern volatile bool please_exit;
extern dword emulator_speed;

extern void emulator_init( char* fn_rom, char* fn_ram, char* fn_port1, char* fn_port2 );
extern void emulator_exit( char* fn_rom, char* fn_ram, char* fn_port1, char* fn_port2 );
extern bool emulator_run( void );

extern void emulator_set_state( int state );

#endif
