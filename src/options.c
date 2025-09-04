#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include "options.h"

static config_t __config = {
    .progname = ( char* )"saturn4xxx",

    .model = MODEL_48GX,
    .verbose = false,
    .shiftless = false,
    .big_screen = false,
    .black_lcd = false,

#if defined( HAS_SDL )
    .frontend = FRONTEND_SDL,
#elif defined( HAS_GTK )
    .frontend = FRONTEND_GTK,
#else
    .frontend = FRONTEND_NCURSES,
#endif

    .mono = false,
    .gray = false,

    .chromeless = false,
    .fullscreen = false,
    .scale = 1.0,

    .tiny = false,
    .small = false,

    .throttle = false,
    .allow_shutdn = false,

    .fn_rom = "rom",
    .fn_ram = "ram",
    .fn_port1 = "port1",
    .fn_port2 = "port2",
    .fn_cpu = "cpu",
    .fn_bus = "bus",
};

config_t* config_init( int argc, char* argv[] )
{
    int option_index;
    int c = '?';

    int clopt_model = -1;
    int clopt_verbose = -1;
    int clopt_big_screen = -1;
    int clopt_black_lcd = -1;
    int clopt_shiftless = -1;
    int clopt_frontend = -1;
    int clopt_mono = -1;
    int clopt_gray = -1;
    int clopt_chromeless = -1;
    int clopt_fullscreen = -1;
    int clopt_throttle = -1;
    int clopt_allow_shutdn = -1;
    double clopt_scale = -1.0;

    int clopt_tiny = -1;
    int clopt_small = -1;

    const char* optstring = "h";
    struct option long_options[] = {
        {"help",           no_argument,       NULL,                'h'             },
        {"verbose",        no_argument,       &clopt_verbose,      true            },

        {"throttle",       no_argument,       &clopt_throttle,     true            },
        {"allow-shutdown", no_argument,       &clopt_allow_shutdn, true            },

        /* {"big-screen",     no_argument,       &clopt_big_screen,   true            }, */
        {"black-lcd",      no_argument,       &clopt_black_lcd,    true            },

        /* {"48sx",           no_argument,       &clopt_model,        MODEL_48SX      }, */
        /* {"48gx",           no_argument,       &clopt_model,        MODEL_48GX      }, */
        /* {"40g",            no_argument,       &clopt_model,        MODEL_40G       }, */
        /* {"49g",            no_argument,       &clopt_model,        MODEL_49G       }, */
        /* {"50g",            no_argument,       &clopt_model,        MODEL_50G       }, */

        {"shiftless",      no_argument,       &clopt_shiftless,    true            },

#if defined( HAS_GTK )
        {"gtk",            no_argument,       &clopt_frontend,     FRONTEND_GTK    },
#endif
#if defined( HAS_SDL )
        {"sdl",            no_argument,       &clopt_frontend,     FRONTEND_SDL    },
#endif
        {"chromeless",     no_argument,       &clopt_chromeless,   true            },
        {"fullscreen",     no_argument,       &clopt_fullscreen,   true            },
        {"scale",          required_argument, NULL,                7110            },

        {"tui",            no_argument,       &clopt_frontend,     FRONTEND_NCURSES},
        {"tui-small",      no_argument,       NULL,                6110            },
        {"tui-tiny",       no_argument,       NULL,                6120            },

        {"mono",           no_argument,       &clopt_mono,         true            },
        {"gray",           no_argument,       &clopt_gray,         true            },

        {0,                0,                 0,                   0               }
    };

    const char* help_text = "usage: %s [options]\n"
                            "options:\n"
                            "  -h --help         what you are reading\n"
                            "     --print-config output current configuration to stdout and exit (in config.lua formatting)\n"
                            "     --verbose      display more informations\n"
                            "     --throttle     throttle CPU speed (default: false)\n"
                            "     --allow-shutdown allow shutdown (default: false)\n"
                            /* "     --big-screen   131×80 screen (default: false)\n" */
                            "     --black-lcd    (default: false)\n"
                            /* "     --48gx         emulate a HP 48GX\n" */
                            /* "     --48sx         emulate a HP 48SX\n" */
                            /* "     --40g          emulate a HP 40G\n" */
                            /* "     --49g          emulate a HP 49G\n" */
                            "     --sdl          graphical (SDL) front-end (default: true)\n"
                            "     --gtk          graphical (gtk4) front-end (default: false)\n"
                            "     --tui          text front-end (default: false)\n"
                            "     --tui-small    text small front-end (2×2 pixels per character) (default: "
                            "false)\n"
                            "     --tui-tiny     text tiny front-end (2×4 pixels per character) (default: "
                            "false)\n"
                            "     --chromeless   only show display (default: "
                            "false)\n"
                            "     --fullscreen   make the UI fullscreen "
                            "(default: false)\n"
                            "     --scale=<n>    make the UI scale <n> times "
                            "(default: 1.0)\n"
                            "     --mono         make the UI monochrome (default: "
                            "false)\n"
                            "     --gray         make the UI grayscale (default: "
                            "false)\n"
                            "     --shiftless    don't map the shift keys to let them free for numbers (default: "
                            "false)\n"
                            /* "     --reset        force a reset\n" */
                            /* "     --monitor      start with monitor (default: no)\n" */
                            "\n";

    while ( c != EOF ) {
        c = getopt_long( argc, argv, optstring, long_options, &option_index );

        switch ( c ) {
            case 'h':
                fprintf( stdout, help_text, __config.progname );
                exit( EXIT_SUCCESS );
                break;
            case 6110:
                clopt_frontend = FRONTEND_NCURSES;
                clopt_small = true;
                break;
            case 6120:
                clopt_frontend = FRONTEND_NCURSES;
                clopt_tiny = true;
                break;
            case 7110:
                clopt_scale = atof( optarg );
                break;

            default:
                break;
        }
    }

    /****************************************************/
    /* 2. treat command-line params which have priority */
    /****************************************************/
    if ( clopt_verbose != -1 )
        __config.verbose = clopt_verbose == true;
    /* if ( clopt_model != -1 ) */
    /*     __config.model = clopt_model; */
    /* if ( clopt_big_screen != -1 ) */
    /*     __config.big_screen = clopt_big_screen == true; */
    if ( clopt_black_lcd != -1 )
        __config.black_lcd = clopt_black_lcd == true;
    if ( clopt_frontend != -1 )
        __config.frontend = clopt_frontend;
    if ( clopt_chromeless != -1 )
        __config.chromeless = clopt_chromeless == true;
    if ( clopt_fullscreen != -1 )
        __config.fullscreen = clopt_fullscreen == true;
    if ( clopt_scale > 0.0 )
        __config.scale = clopt_scale;
    if ( clopt_mono != -1 )
        __config.mono = clopt_mono == true;
    if ( clopt_small != -1 )
        __config.small = clopt_small == true;
    if ( clopt_tiny != -1 )
        __config.tiny = clopt_tiny == true;
    if ( clopt_gray != -1 )
        __config.gray = clopt_gray == true;
    if ( clopt_shiftless != -1 )
        __config.shiftless = clopt_shiftless == true;
    if ( clopt_throttle != -1 )
        __config.throttle = clopt_throttle == true;
    if ( clopt_allow_shutdn != -1 )
        __config.allow_shutdn = clopt_allow_shutdn == true;

    /* if ( __config.model == MODEL_49G || __config.model == MODEL_50G ) */
    /*     __config.black_lcd = true; */

    __config.progname = basename( strdup( argv[ 0 ] ) );

    if ( __config.verbose ) {
        if ( optind < argc ) {
            fprintf( stderr, "%i invalid arguments : ", argc - optind );
            while ( optind < argc )
                fprintf( stderr, "%s\n", argv[ optind++ ] );
            fprintf( stderr, "\n" );
        }
    }

    return &__config;
}
