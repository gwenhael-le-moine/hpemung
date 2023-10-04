#include <time.h>
#include <errno.h>

#include <stdlib.h>
#include <sys/time.h>

#include "types.h"
#include "cpu.h"
#include "bus.h"
#include "timers.h"
#include "display.h"
#include "gui.h"
#include "emulator.h"

#define MAX_DELTA 4000

typedef struct {
    dword next;
    dword freq;
    void ( *proc )( void );
} CycleEvent;

typedef struct {
    volatile int value;
    int speed;
    bool running;
    void ( *proc )( void );
} TimerEvent;

static CycleEvent cycle_events[] = {
    {0,  16,   timer1_update },
#ifndef true_TIMER2
    { 0, 8192, timer2_update },
#endif
    { 0, 4096, display_update},
    { 0, 0,    NULL          }
};

volatile bool please_exit = false;
dword emulator_speed = 4000000;
static int emulator_state = EMULATOR_RUN;

void emulator_set_state( int state ) { emulator_state = state; }

void emulator_init( void )
{
    static bool locked = false;

    bus_init();

    if ( !locked )
        locked = true;
}

void emulator_exit( void ) { bus_exit(); }

static inline void throttle( bool is_needed )
{
    if ( !is_needed )
        return;

    struct timeval tv;
    struct timeval tv2;
    struct timezone tz;

    /* Throttling speed */
    gettimeofday( &tv, &tz );
    gettimeofday( &tv2, &tz );
    while ( ( tv.tv_sec == tv2.tv_sec ) && ( ( tv.tv_usec - tv2.tv_usec ) < 2 ) )
        gettimeofday( &tv, &tz );

    tv2.tv_usec = tv.tv_usec;
    tv2.tv_sec = tv.tv_sec;
}

int msleep( long msec )
{
    struct timespec ts;
    int res;

    if ( msec < 0 ) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = ( msec % 1000 ) * 1000000;

    do {
        res = nanosleep( &ts, &ts );
    } while ( res && errno == EINTR );

    return res;
}

bool emulator_run( void )
{
    CycleEvent* cep;
    dword delta;

    static bool first_run = false;
    if ( first_run == false && emulator_state == EMULATOR_RUN )
        first_run = true;

    if ( please_exit )
        return false;

    if ( emulator_state != EMULATOR_STOP ) {
        if ( !cpu.shutdown ) {
            execute_instruction();

            throttle( true );

            if ( emulator_state == EMULATOR_STEP ) {
                emulator_set_state( EMULATOR_STOP );
            }
        } else {
            delta = MAX_DELTA;
            for ( cep = cycle_events; cep->proc; cep++ ) {
                delta = MIN( delta, cep->next - cpu.cycles + 1 );
            }
            cpu.cycles += delta;
        }

        for ( cep = cycle_events; cep->proc; cep++ ) {
            if ( ( cep->next - cpu.cycles ) & 0x80000000 ) {
                cep->next += emulator_speed / cep->freq;
                cep->proc();
            }
        }
    }

    if ( emulator_state == EMULATOR_STOP )
        msleep( 10 );

    return true;
}
