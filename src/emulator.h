#ifndef __EMULATOR_H
#define __EMULATOR_H

#include <stdbool.h>

#include "types.h"

enum EmulatorStates { EMULATOR_STOP, EMULATOR_STEP, EMULATOR_RUN };

extern volatile bool please_exit;

extern void emulator_init();
extern void emulator_exit();
extern bool emulator_run();

extern void emulator_set_state( int state );

#endif
