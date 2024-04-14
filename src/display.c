#include <stdlib.h>

#include "types.h"
#include "bus.h"
#include "display.h"

address menu_base;
address display_base;
address display_line_offset;
byte display_line_count;
byte display_height;
byte display_offset;
bool display_enable;

byte lcdScreen[ LCD_WIDTH * LCD_HEIGHT ];
byte prev_lcdScreen[ LCD_WIDTH * LCD_HEIGHT ];
byte prev2_lcdScreen[ LCD_WIDTH * LCD_HEIGHT ];
byte lcdScreenGS[ LCD_WIDTH * LCD_HEIGHT ];

static address cur_adr;
static bool in_menu;
static byte off_line;
static int off_cnt;
bool shouldRender = false;
static int screen_draw_count = 0;
static bool drawGS = false;

static address draw_lcd_line( address adr, int y )
{
    int bit = 0;
    byte data = 0; // Initialized to remove warning
    byte* ptr = bus_fast_peek( NULL, adr, NULL );

    // Horisontal pixel offset
    if ( !in_menu ) {
        if ( display_offset > 3 )
            ptr++;

        data = *ptr++;
        data >>= display_offset & 3;
        bit = 4 - ( display_offset & 3 );
    }

    for ( int x = 0; x < LCD_WIDTH; ++x ) {
        if ( bit == 0 ) {
            data = *ptr++;
            bit = 4;
        }

        byte pixel = ( ( data & 1 ) << 6 );
        if ( pixel != '\0' )
            pixel = '\3';

        byte pixelGS = lcdScreenGS[ x + y * LCD_WIDTH ];

        prev2_lcdScreen[ x + y * LCD_WIDTH ] = prev_lcdScreen[ x + y * LCD_WIDTH ];
        prev_lcdScreen[ x + y * LCD_WIDTH ] = lcdScreen[ x + y * LCD_WIDTH ];
        lcdScreen[ x + y * LCD_WIDTH ] = pixel;

        byte prev_pixel = prev_lcdScreen[ x + y * LCD_WIDTH ];
        byte prev2_pixel = prev2_lcdScreen[ x + y * LCD_WIDTH ];

        if ( drawGS == true ) {
            if ( prev2_pixel == '\0' && prev_pixel == '\0' && pixel == '\0' )
                pixelGS = '\0';

            if ( prev2_pixel == '\0' && prev_pixel == '\0' && pixel == '\3' )
                pixelGS = '\1';

            if ( prev2_pixel == '\0' && prev_pixel == '\3' && pixel == '\0' )
                pixelGS = '\1';

            if ( prev2_pixel == '\3' && prev_pixel == '\0' && pixel == '\0' )
                pixelGS = '\1';

            if ( prev2_pixel == '\0' && prev_pixel == '\3' && pixel == '\3' )
                pixelGS = '\2';

            if ( prev2_pixel == '\3' && prev_pixel == '\0' && pixel == '\3' )
                pixelGS = '\2';

            if ( prev2_pixel == '\3' && prev_pixel == '\3' && pixel == '\0' )
                pixelGS = '\2';

            if ( prev2_pixel == '\3' && prev_pixel == '\3' && pixel == '\3' )
                pixelGS = '\3';

            lcdScreenGS[ x + y * LCD_WIDTH ] = pixelGS;
        }

        data >>= 1;
        bit--;
    }

    return ( adr + 0x22 + ( !in_menu && ( display_offset & 4 ) ? 2 : 0 ) ) & 0xFFFFF;
}

void display_update( void )
{
    if ( !display_enable && !off_cnt ) { /* Turn off display */
        off_cnt = 1;
        off_line = display_line_count;
        display_line_count = 0;
    } else if ( display_enable && off_cnt ) { /* Turn on display */
        off_cnt = 0;
        display_line_count = 0;
        in_menu = 0;
        cur_adr = display_base;
    }

    if ( !off_cnt ) { /* Display is on */
        cur_adr = draw_lcd_line( cur_adr, display_line_count );

        if ( !in_menu )
            cur_adr += display_line_offset;

        if ( display_line_count == display_height ) {
            in_menu = 1;
            cur_adr = menu_base;
        }

        display_line_count++;

        if ( display_line_count == LCD_HEIGHT ) {
            display_line_count = 0;
            in_menu = 0;
            cur_adr = display_base;

            shouldRender = true;

            screen_draw_count++;
            if ( screen_draw_count == 3 )
                screen_draw_count = 0;
        }

        drawGS = screen_draw_count == 0;

    } else if ( off_cnt <= 7 ) /* Display is off and still fading */
        off_cnt = 8;
}
