#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>

typedef struct {
    char* progname;
    bool real_speed;
    bool verbose;
} Config;

extern Config config;

void parse_args( int argc, char* argv[] );

#endif
