#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

#include "core/display.h"  /* LCD_HEIGHT; LCD_WIDTH; shouldRender; lcdScreenGS[] */
#include "core/emulator.h" /* for please_exit */
#include "core/keyboard.h" /* press_key(); release_key() */

#include "ui4x/src/api.h"

#define KEYBOARD keyboard48

typedef struct hpkey_t {
    int code;
    bool pressed;
} hpkey_t;

static hpkey_t keyboard48[ NB_HP48_KEYS ] = {
    /* From top left to bottom right */
    {0x14,   false},
    {0x84,   false},
    {0x83,   false},
    {0x82,   false},
    {0x81,   false},
    {0x80,   false},

    {0x24,   false},
    {0x74,   false},
    {0x73,   false},
    {0x72,   false},
    {0x71,   false},
    {0x70,   false},

    {0x04,   false},
    {0x64,   false},
    {0x63,   false},
    {0x62,   false},
    {0x61,   false},
    {0x60,   false},

    {0x34,   false},
    {0x54,   false},
    {0x53,   false},
    {0x52,   false},
    {0x51,   false},
    {0x50,   false},

    {0x44,   false},
    {0x43,   false},
    {0x42,   false},
    {0x41,   false},
    {0x40,   false},

    {0x35,   false},
    {0x33,   false},
    {0x32,   false},
    {0x31,   false},
    {0x30,   false},

    {0x25,   false},
    {0x23,   false},
    {0x22,   false},
    {0x21,   false},
    {0x20,   false},

    {0x15,   false},
    {0x13,   false},
    {0x12,   false},
    {0x11,   false},
    {0x10,   false},

    {0x8000, false},
    {0x03,   false},
    {0x02,   false},
    {0x01,   false},
    {0x00,   false},
};

static config_t __config;

void press_key( int hpkey )
{
    if ( hpkey < 0 || hpkey > ui_get_nb_keys() )
        return;
    // Check not already pressed (may be important: avoids a useless do_kbd_int)
    if ( KEYBOARD[ hpkey ].pressed )
        return;

    KEYBOARD[ hpkey ].pressed = true;

    pressKey( hpkey );
}

void release_key( int hpkey )
{
    if ( hpkey < 0 || hpkey > ui_get_nb_keys() )
        return;
    // Check not already released (not critical)
    if ( !KEYBOARD[ hpkey ].pressed )
        return;

    KEYBOARD[ hpkey ].pressed = false;

    releaseKey( hpkey );
}

bool is_key_pressed( int hpkey )
{
    if ( hpkey < 0 || hpkey > ui_get_nb_keys() )
        return false;

    return KEYBOARD[ hpkey ].pressed;
}

unsigned char get_annunciators( void ) { return 0 /* mod_status.hdw.lcd_ann */; }

bool get_display_state( void ) { return display_enable; }

void get_lcd_buffer( int* target )
{
    display_update();

    for ( int xy = 0; xy < ( ui_get_lcd_height() * LCD_WIDTH ); ++xy )
        target[ xy ] = ( int )lcdScreenGS[ xy ];
}

int get_contrast( void ) { return display_contrast; }

void init_emulator( config_t* conf )
{
    __config = *conf;

    emulator_init();
}

void exit_emulator( void ) { emulator_exit(); }

void emulator_stop( void ) { please_exit = true; }
