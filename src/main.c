#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include "options.h"
#include "emulator.h"
#include "emulator_ui4x_api.h"
#include "ui4x/common.h"
#include "display.h"

/* static long long time_in_useconds( void ) */
/* { */
/*     struct timespec ts; */
/*     timespec_get( &ts, TIME_UTC ); */
/*     return ( long long )ts.tv_sec * 1000000000 + ts.tv_nsec; */
/* } */

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

config_t config;

int main( int argc, char* argv[] )
{
    config = *config_init( argc, argv );

    init_emulator( &config );

    /* (G)UI */
    setup_ui( &config );
    ui_start( &config );

    do {
        ui_get_event();

        emulator_run();

        currentTime = time_in_mseconds();
        if ( currentTime > lastTime_gui_update + delay_gui_update ) {
            lastTime_gui_update = currentTime;

            if ( shouldRender )
                ui_update_display();
        }
    } while ( !please_exit );

    close_and_exit();

    return 0;
}
