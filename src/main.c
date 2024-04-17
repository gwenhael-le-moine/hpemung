#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "config.h"
#include "emulator.h"
#include "gui.h"
#include "display.h"

unsigned int currentTime;

// display_update
unsigned int lastTime_timer1 = 0;
unsigned int delay_timer1 = 16384;

// display show
unsigned int lastTime_timer5 = 0;
unsigned int delay_timer5 = 64; // fps

int main( int argc, char* argv[] )
{
    parse_args( argc, argv );

    if ( !gui_init() )
        exit( EXIT_FAILURE );

    emulator_init( "rom", "ram", "port1", "port2", "bus", "cpu" );

    while ( !please_exit ) {
        if ( please_exit )
            break;

        currentTime = SDL_GetTicks();

        emulator_run();

        if ( currentTime > lastTime_timer1 + delay_timer1 ) {
            lastTime_timer1 = currentTime;
            display_update();
        }

        if ( currentTime > lastTime_timer5 + delay_timer5 ) {
            lastTime_timer5 = currentTime;
            gui_refresh();
        }

        if ( !gui_events() )
            break;
    }

    emulator_exit( "rom", "ram", "port1", "port2", "bus", "cpu" );
    gui_exit();

    return 0;
}
