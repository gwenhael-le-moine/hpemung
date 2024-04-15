#include "config.h"

Config config = {
    .progname = "hpemung",
    .ui_font = "/usr/share/fonts/TTF/unifont.ttf",
    .real_speed = true,
    .verbose = true,
};

void parse_args( int argc, char* argv[] )
{
    while ( --argc ) {
        argv++;
        if ( argv[ 0 ][ 0 ] == '-' ) {
            switch ( argv[ 0 ][ 1 ] ) {
                case 't':
                    config.real_speed = true;
                    break;
                case 'r':
                    config.real_speed = false;
                    break;
            }
        }
    }
}
