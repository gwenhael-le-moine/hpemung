#include <SDL2/SDL.h>
#include <stdlib.h>
#include "types.h"
#include "cpu.h"
#include "bus.h"
#include "timers.h"
#include "display.h"
#include "gui.h"
#include "pdebug.h"
#include "emulator.h"

/* Define true_TIMER2 to make timer2 run in true speed (8192 hz).
 * If it is not defined timer2 is syncronized to the cpu speed.
 */
// #define true_TIMER2

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

static TimerEvent timer_events[] = {
    {0,  1000000 / 20 /*BPS_TO_TIMER(20)*/,     false, gui_update     },
    { 0, 1000000 /*BPS_TO_TIMER(1)*/,           false, true_speed_proc},
#ifdef true_TIMER2
    { 0, 1000000 / 8192 /*BPS_TO_TIMER(8192)*/, false, timer2_update  },
#endif
    { 0, 0,                                     false, NULL           }
};

volatile bool please_exit = false;
dword emulator_speed = 4000000;
static int emulator_state = EMULATOR_RUN; // EMULATOR_STOP;

void true_speed_proc( void )
{
    static dword last_cycles;

    pdebug_draw_true_speed( cpu.cycles - last_cycles );
    last_cycles = cpu.cycles;
}

/* static void timer_event_proc( void* what ) { ( ( TimerEvent* )what
 * )->value++; } */

static void start_timer_proc( void ( *proc )( void ) )
{
    TimerEvent* ptr = timer_events;

    while ( ptr->proc && ptr->proc != proc ) {
        ptr++;
    }
    if ( ptr->proc && !ptr->running ) {
        ptr->value = 0;
        ptr->running = true;

        // printf("ptr->speed = %d\n", ptr->speed);
        // install_param_int_ex(timer_event_proc, (void *)ptr, ptr->speed);
    }
}

static void stop_timer_proc( void ( *proc )( void ) )
{
    TimerEvent* ptr = timer_events;

    while ( ptr->proc && ptr->proc != proc ) {
        ptr++;
    }
    if ( ptr->proc && ptr->running ) {
        ptr->value = 0;
        ptr->running = false;

        // remove_param_int(timer_event_proc, (void *)ptr);
    }
}

void emulator_set_state( int state )
{
    printf( "emulator_set_state\n" );
#ifdef true_TIMER2
    if ( state != EMULATOR_STOP ) {
        start_timer_proc( timer2_update );
    } else {
        stop_timer_proc( timer2_update );
    }
#endif
    emulator_state = state;
    pdebug_state_changed();
}

int emulator_get_state( void ) { return emulator_state; }

void emulator_init( void )
{
    static bool locked = false;

    bus_init();
    display_init();

    if ( !locked ) {
        //	LOCK_VARIABLE(timer_events);
        //	LOCK_FUNCTION(timer_event_proc);
        locked = true;
    }
}

void emulator_exit( void )
{
    display_exit();
    bus_exit();
}

bool emulator_run( void )
{
    CycleEvent* cep;
    TimerEvent* tep;
    dword delta;

    static bool first_run = false;
    if ( first_run == false && emulator_state == EMULATOR_RUN ) {
        first_run = true;
        start_timer_proc( gui_update );
        start_timer_proc( true_speed_proc );
    }

    if ( please_exit ) {
        return false;
    }

    // while (!please_exit)
    {
        if ( emulator_state != EMULATOR_STOP ) {

            // draw_cpu();

            if ( !cpu.shutdown ) {
                execute_instruction();

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

        for ( tep = timer_events; tep->proc; tep++ ) {
            if ( tep->running && tep->value ) {
                tep->value--;
                tep->proc();
            }
        }

        // printf("emulator_state = %d\n", emulator_state);

        if ( emulator_state == EMULATOR_STOP ) {
            // rest(10);
            SDL_Delay( 10 );
        } else {
            // yield_timeslice ();
            // SDL_Delay(1);
        }
    }

    //
    // #ifdef true_TIMER2
    //    if (emulator_state != EMULATOR_STOP) {
    //		stop_timer_proc(timer2_update);
    //    }
    // #endif
    //

    if ( first_run == true && emulator_state == EMULATOR_STOP ) {
#ifdef true_TIMER2
        stop_timer_proc( timer2_update ); //
#endif

        stop_timer_proc( true_speed_proc );
        stop_timer_proc( gui_update );
    }

    return true;
}
