#include <time.h>
#include <errno.h>

#include <stdlib.h>
#include <sys/time.h>

#include "types.h"
#include "cpu.h"
#include "bus.h"
#include "timers.h"
#include "display.h"
#include "emulator.h"
#include "persistence.h"
#include "ports.h"
#include "config.h"

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
    {.next = 0, .freq = 16,   .proc = timer1_update },
    {.next = 0, .freq = 8192, .proc = timer2_update },
    {.next = 0, .freq = 4096, .proc = display_update},
};

volatile bool please_exit = false;
dword emulator_speed = 4000000;
static int emulator_state = EMULATOR_RUN;

void emulator_set_state( int state ) { emulator_state = state; }

void emulator_init()
{
    get_absolute_working_dir_path();

    rom_init( config.fn_rom );
    ram_init( config.fn_ram );
    ports_init( config.fn_port1, config.fn_port2 );

    cpu_bus_init( config.fn_state );
}

void emulator_exit()
{
    ports_exit( config.fn_port1, config.fn_port2 );
    ram_exit( config.fn_ram );
    rom_exit();
    cpu_bus_exit( config.fn_state );
}

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

    if ( emulator_state != EMULATOR_STOP ) {
        if ( !cpu.shutdown ) {
            execute_instruction();

            throttle( cpu.keyintp || config.throttle );

            if ( emulator_state == EMULATOR_STEP )
                emulator_set_state( EMULATOR_STOP );

        } else {
            delta = MAX_DELTA;
            for ( cep = cycle_events; cep->proc; cep++ )
                delta = MIN( delta, cep->next - cpu.cycles + 1 );

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
