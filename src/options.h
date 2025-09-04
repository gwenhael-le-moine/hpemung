#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>

typedef enum { FRONTEND_SDL, FRONTEND_NCURSES, FRONTEND_GTK } frontend_t;

typedef enum { MODEL_48SX = 485, MODEL_48GX = 486, MODEL_40G = 406, MODEL_49G = 496, MODEL_50G = 506 } model_t;

typedef struct {
    char* progname;

    model_t model;
    bool verbose;
    bool shiftless;
    bool big_screen;
    bool black_lcd;

    frontend_t frontend;
    bool mono;
    bool gray;

    bool chromeless;
    bool fullscreen;
    double scale;

    bool tiny;
    bool small;

    char* wire_name;
    char* ir_name;

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
