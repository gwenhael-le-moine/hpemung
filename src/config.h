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
    bool throttle;
    bool verbose;
    bool allow_shutdn;

    char* fn_rom;
    char* fn_ram;
    char* fn_port1;
    char* fn_port2;
    char* fn_state;
} Config;

extern Config config;

void parse_args( int argc, char* argv[] );

#endif
