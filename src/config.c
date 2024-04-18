#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <getopt.h>

#include "config.h"

Config config = {
    .progname = "hpemung",

    .throttle = false,
    .verbose = false,
    .allow_shutdn = false,

    .ui_scale = 3,
    .ui_font1 = "/usr/share/fonts/TTF/unifont.ttf",
    .ui_font2 = "/usr/share/fonts/TTF/unifont.ttf",
    .ui_font_size1 = 6,
    .ui_font_size2 = 4,

    .fn_rom = "rom",
    .fn_ram = "ram",
    .fn_port1 = "port1",
    .fn_port2 = "port2",
    .fn_state = "state",
};

void config_init( int argc, char* argv[] )
{
    int option_index;
    int c = '?';
    char* optstring = "hVs:f:";
    struct option long_options[] = {
        {"help",           no_argument,       NULL, 'h' },
        {"verbose",        no_argument,       NULL, 'V' },
        {"scale",          required_argument, NULL, 's' },
        {"font",           required_argument, NULL, 'f' },
        {"throttle",       no_argument,       NULL, 1800},
        {"allow-shutdown", no_argument,       NULL, 1900},

        {0,                0,                 0,    0   }
    };

    char* help_text = "usage: %s [options]\n"
                      "options:\n"
                      "  -h --help               what you are reading\n"
                      "  -V --verbose            be verbose (default: false)\n"
                      "  -s --scale=<i>          scale GUI (default: %i)\n"
                      "  -f --font=<path>        set GUI font (default: %s)\n"
                      "     --throttle           Throttle speed (default: false)\n"
                      "     --allow-shutdown     Enable SHUTDN instruction (default: false)\n";
    while ( c != EOF ) {
        c = getopt_long( argc, argv, optstring, long_options, &option_index );

        switch ( c ) {
            case 'h':
                fprintf( stderr, help_text, config.progname, config.ui_scale, config.ui_font1 );
                exit( 0 );
                break;
            case 's':
                config.ui_scale = atoi( optarg );
                break;
            case 'f':
                config.ui_font1 = optarg;
                config.ui_font2 = optarg;
                break;
            case 'V':
                config.verbose = true;
                break;
            case 1800:
                config.throttle = true;
                break;
            case 1900:
                config.allow_shutdn = true;
                break;

            case '?':
            case ':':
                exit( 0 );
                break;
            default:
                break;
        }
    }

    if ( optind < argc ) {
        fprintf( stderr, "Invalid arguments : " );
        while ( optind < argc )
            fprintf( stderr, "%s\n", argv[ optind++ ] );
        fprintf( stderr, "\n" );
    }
}
