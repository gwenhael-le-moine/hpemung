#include "config.h"

Config config = {
    .progname = "hpemung",
    .ui_scale = 3,
    .ui_font1 = "/usr/share/fonts/TTF/unifont.ttf",
    .ui_font2 = "/usr/share/fonts/TTF/unifont.ttf",
    .ui_font_size1 = 6,
    .ui_font_size2 = 4,
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
