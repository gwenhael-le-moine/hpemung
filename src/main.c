#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "emulator.h"
#include "gui.h"
#include "display.h"
#include "gui_display.h"

unsigned int currentTime;

// display_update
unsigned int lastTime_timer1 = 0;
unsigned int delay_timer1 = 16384;

// display show
unsigned int lastTime_timer5 = 0;
unsigned int delay_timer5 = 64; // fps

static int fullscreen = false;

static void parse_args( int argc, char* argv[] )
{
    while ( --argc ) {
        argv++;
        if ( argv[ 0 ][ 0 ] == '-' ) {
            switch ( argv[ 0 ][ 1 ] ) {
                case 'f':
                    fullscreen = true;
                    break;
                case 'w':
                    fullscreen = false;
                    break;
            }
        }
    }
}

static inline void mainloop()
{
    if ( please_exit || !SDL_ready )
        return;

    currentTime = SDL_GetTicks();

    emulator_run();

    if ( currentTime > lastTime_timer1 + delay_timer1 ) {
        lastTime_timer1 = currentTime;
        display_update();
    }

    if ( currentTime > lastTime_timer5 + delay_timer5 ) {
        lastTime_timer5 = currentTime;
        SDL__display_show();
    }

    if ( !gui_refresh() )
        return;
}

int main( int argc, char* argv[] )
{
    parse_args( argc, argv );

    gui_init();
    emulator_init();

    while ( !please_exit )
        mainloop();

    emulator_exit();
    gui_exit();

    return 0;
}
