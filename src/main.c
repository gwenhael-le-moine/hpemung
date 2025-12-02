#include <unistd.h>

#include <sys/time.h>
#include <time.h>

#include "emulator_api.h"
#include "options.h"

#include "core/display.h"
#include "core/emulator.h"

#include "ui4x/src/api.h"

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
    ui4x_config_t config_ui = {
        .model = config.model,
        .shiftless = config.shiftless,
        .black_lcd = config.black_lcd,

        .frontend = config.frontend,

        .mono = config.mono,
        .gray = config.gray,

        .chromeless = config.chromeless,
        .fullscreen = config.fullscreen,
        .zoom = config.scale,

        .tiny = config.tiny,
        .small = config.small,

        .verbose = config.verbose,

        .name = config.progname,
        .progname = config.progname,
        .wire_name = config.wire_name,
        .ir_name = config.ir_name,

        .style_filename = NULL /* FIXME */
    };

    ui4x_emulator_api_t emulator_api = { .press_key = press_key,
                                         .release_key = release_key,
                                         .is_key_pressed = is_key_pressed,
                                         .is_display_on = get_display_state,
                                         .get_annunciators = get_annunciators,
                                         .get_lcd_buffer = get_lcd_buffer,
                                         .get_contrast = get_contrast,
                                         .do_stop = emulator_stop };
    init_ui( &config_ui, &emulator_api );

    do {
        ui_handle_pending_inputs();

        emulator_run();

        currentTime = time_in_mseconds();
        if ( currentTime > lastTime_gui_update + delay_gui_update ) {
            lastTime_gui_update = currentTime;

            if ( shouldRender )
                ui_refresh_output();
        }
    } while ( !please_exit );

    return 0;
}
