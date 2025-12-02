#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>

#include "ui4x/src/api.h"

typedef struct {
    /* duplicating ui4x_config_t here so that config_init can return one big struct */
    char* progname;

    ui4x_model_t model;
    bool shiftless;
    bool big_screen;
    bool black_lcd;

    ui4x_frontend_t frontend;
    bool mono;
    bool gray;

    bool chromeless;
    bool fullscreen;
    double scale;

    bool tiny;
    bool small;

    char* wire_name;
    char* ir_name;

    bool verbose;

    /* options below are specific to hpemung */
    bool throttle;
    bool allow_shutdn;

    char* fn_rom;
    char* fn_ram;
    char* fn_port1;
    char* fn_port2;
    char* fn_cpu;
    char* fn_bus;
} config_t;

extern config_t config;

extern config_t* config_init( int argc, char* argv[] );

#endif
