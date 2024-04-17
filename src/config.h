#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>

typedef struct {
    char* progname;
    int ui_scale;
    char* ui_font1;
    char* ui_font2;
    int ui_font_size1;
    int ui_font_size2;
    bool real_speed;
    bool verbose;
} Config;

extern Config config;

void parse_args( int argc, char* argv[] );

#endif
