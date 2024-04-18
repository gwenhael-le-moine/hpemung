#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "config.h"
#include "emulator.h"
#include "gui.h"
#include "display.h"

Uint64 currentTime;

// display_update
Uint64 lastTime_timer1 = 0;
unsigned int delay_timer1 = 16384;

// display show
Uint64 lastTime_timer5 = 0;
unsigned int delay_timer5 = 64; // fps

int main( int argc, char* argv[] )
{
    config_init( argc, argv );

    if ( !gui_init() )
        exit( EXIT_FAILURE );

    emulator_init();

    while ( !please_exit ) {
        if ( please_exit )
            break;

        currentTime = SDL_GetTicks64();

        emulator_run();

        if ( currentTime > lastTime_timer1 + delay_timer1 ) {
            lastTime_timer1 = currentTime;

            display_update();
        }

        if ( currentTime > lastTime_timer5 + delay_timer5 ) {
            lastTime_timer5 = currentTime;

            gui_update();
        }

        if ( !gui_events() )
            break;
    }

    emulator_exit();
    gui_exit();

    return 0;
}
