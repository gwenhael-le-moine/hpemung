#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include "options.h"
#include "emulator.h"
// #include "gui.h"
#include "emulator_ui4x_api.h"
#include "ui4x/common.h"

#define UI_REFRESH_RATE_Hz 64

void signal_handler( int sig )
{
    switch ( sig ) {
        case SIGALRM:
            ui_get_event();
            ui_update_display();
            break;
        case SIGPIPE:
            close_and_exit();
            exit( 0 );
        default:
            break;
    }
}

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

config_t config;

int main( int argc, char* argv[] )
{
    config = *config_init( argc, argv );

    init_emulator( &config );

    /* (G)UI */
    setup_ui( &config );
    ui_start( &config );

    sigset_t set;
    struct sigaction sa;
    sigemptyset( &set );
    sigaddset( &set, SIGALRM );
    sa.sa_handler = signal_handler;
    sa.sa_mask = set;
#ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
#endif
    sigaction( SIGALRM, &sa, ( struct sigaction* )0 );

    /************************************/
    /* set the real time interval timer */
    /************************************/
    /*
      Every <interval>µs setitimer will trigger a SIGALRM
      which will getUI events and refresh UI in signal_handler
     */
    struct itimerval it;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 1000000 / UI_REFRESH_RATE_Hz;
    it.it_value.tv_sec = it.it_interval.tv_sec;
    it.it_value.tv_usec = it.it_interval.tv_usec;
    setitimer( ITIMER_REAL, &it, ( struct itimerval* )0 );

    do {
        /* ui_get_event(); */

        emulator_run();

        /* currentTime = time_in_mseconds(); */
        /* if ( currentTime > lastTime_gui_update + delay_gui_update ) { */
        /*     lastTime_gui_update = currentTime; */

        /*     ui_update_display(); */
        /* } */
    } while ( !please_exit );

    close_and_exit();

    return 0;
}
