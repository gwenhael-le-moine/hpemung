#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <getopt.h>

#include "config.h"

Config config = {
    .progname = "hpemung",
    .ui_scale = 3,
    .ui_font1 = "/usr/share/fonts/TTF/unifont.ttf",
    .ui_font2 = "/usr/share/fonts/TTF/unifont.ttf",
    .ui_font_size1 = 6,
    .ui_font_size2 = 4,
    .throttle = false,
    .verbose = false,
    .allow_shutdn = false,
};

void parse_args( int argc, char* argv[] )
{
    int option_index;
    int c = '?';
    char* optstring = "hVs:";
    struct option long_options[] = {
        {"help",           no_argument,       NULL, 'h' },
        {"verbose",        no_argument,       NULL, 'V' },
        {"scale",          required_argument, NULL, 's' },
        {"throttle",       no_argument,       NULL, 1800},
        {"allow-shutdown", no_argument,       NULL, 1900},

        {0,                0,                 0,    0   }
    };

    char* help_text = "usage: %s [options]\n"
                      "options:\n"
                      "  -h --help               what you are reading\n"
                      "  -V --verbose            be verbose (default: false)\n"
                      "  -s --scale=<i>          scale GUI (default: 3)\n"
                      "     --throttle           Throttle speed (default: false)\n"
                      "     --allow-shutdown     Enable SHUTDN instruction (default: false)\n";
    while ( c != EOF ) {
        c = getopt_long( argc, argv, optstring, long_options, &option_index );

        switch ( c ) {
            case 'h':
                fprintf( stderr, help_text, config.progname );
                exit( 0 );
                break;
            case 's':
                config.ui_scale = atoi( optarg );
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
