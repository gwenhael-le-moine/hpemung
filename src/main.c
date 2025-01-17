#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "emulator.h"
#include "gui.h"
#include "display.h"

static long long time_in_useconds( void )
{
    struct timespec ts;
    timespec_get( &ts, TIME_UTC );
    return ( long long )ts.tv_sec * 1000000000 + ts.tv_nsec;
}

long long time_in_mseconds( void )
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return ( ( ( long long )tv.tv_sec ) * 1000 ) + ( tv.tv_usec / 1000 );
}

long long currentTime;

// gui_update
long long lastTime_gui_update = 0;
long long delay_gui_update = 16; // 15625;

int main( int argc, char* argv[] )
{
    config_init( argc, argv );

    if ( !gui_init() )
        exit( EXIT_FAILURE );

    emulator_init();

    do {
        if ( !gui_events() )
            break;

        emulator_run();

        currentTime = time_in_mseconds();
        if ( currentTime > lastTime_gui_update + delay_gui_update ) {
            lastTime_gui_update = currentTime;

            gui_update();
        }
    } while ( !please_exit );

    emulator_exit();
    gui_exit();

    return 0;
}
