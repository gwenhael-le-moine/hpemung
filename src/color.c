#include <stdlib.h>

#include "color.h"

#define RESERVED_LCD 128

enum LCD_Modes { LCD_MODE_SIMPLE, LCD_MODE_GRAY4, LCD_MODE_GRAY8, LCD_MODE_EXP };

int color[ C_COUNT ];

typedef struct RGB {
    unsigned char r, g, b;
} RGB;

static int lcd_0_r, lcd_0_g, lcd_0_b;
static int lcd_1_r, lcd_1_g, lcd_1_b;
static int lcd_mode;

static int bit_count( unsigned int i )
{
    int n = 0;

    while ( i ) {
        n += i & 1;
        i >>= 1;
    }
    return n;
}

static int simple_color( int i ) { return ( i & 0x40 ) ? 255 : 0; }

static int gray4_color( int i ) { return bit_count( i & 0x70 ) * 85; }

static int gray8_color( int i ) { return bit_count( i ) * 255 / 7; }

static int exp_color( int i ) { return i * 255 / 127; }

typedef int ( *lcd_color_func )( int i );

lcd_color_func lcd_color_functions[] = { simple_color, gray4_color, gray8_color, exp_color };

void build_lcd_palette( void ) {}

void color_lcd( int r0, int g0, int b0, int r1, int g1, int b1 )
{
    lcd_0_r = r0 >> 2;
    lcd_0_g = g0 >> 2;
    lcd_0_b = b0 >> 2;
    lcd_1_r = r1 >> 2;
    lcd_1_g = g1 >> 2;
    lcd_1_b = b1 >> 2;
    build_lcd_palette();
}

void color_lcd_mode( int mode )
{
    lcd_mode = mode;
    build_lcd_palette();
}

void color_set_emu( int i, int r, int g, int b ) {}

void color_init( void )
{
    for ( int i = 0; i < C_COUNT; i++ )
        color[ i ] = RESERVED_LCD + i;

    color_set_emu( C_BACKGROUND, 0, 0, 0 );
    color_set_emu( C_PANEL_BACK, 64, 64, 64 );
    color_set_emu( C_PANEL_BORDER, 128, 128, 128 );
    color_set_emu( C_PANEL_TEXT, 255, 255, 255 );
    color_set_emu( C_PANEL_DISABLED, 128, 128, 128 );
    color_set_emu( C_BUTTON_BACK, 64, 64, 64 );
    color_set_emu( C_BUTTON_BORDER, 128, 128, 128 );
    color_set_emu( C_BUTTON_PUSHED, 128, 128, 128 );
    color_set_emu( C_BUTTON_TEXT, 255, 255, 255 );
    color_set_emu( C_BUTTON_DISABLED, 128, 128, 128 );

    color_lcd( 128, 192, 128, 0, 0, 64 );
    color_lcd_mode( LCD_MODE_GRAY4 );
}
