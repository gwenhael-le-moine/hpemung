#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "types.h"
#include "bus.h"
#include "gui_buttons.h"
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
// byte prev3_lcdScreen[LCD_WIDTH*LCD_HEIGHT];
byte lcdScreenGS[ LCD_WIDTH * LCD_HEIGHT ];

static address cur_adr;
static bool in_menu;
static byte off_cur_line;
static byte off_line;
static int off_cnt;
/* static bool shouldClear = true; */
static bool shouldRender = false;
static int screen_draw_count = 0;
static bool drawGS = false;

extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern SDL_Texture* texTarget;
extern SDL_Texture* tex2Target;
extern SDL_Texture* faceplateTexture;

static address draw_lcd_line( address adr, int y )
{
    int x = 0;
    int bit = 0;
    byte data = 0; // Initialized to remove warning
    byte* ptr;

    ptr = bus_fast_peek( NULL, adr, NULL );

    // Horisontal pixel offset
    if ( !in_menu ) {
        if ( display_offset > 3 )
            ptr++;

        data = *ptr++;
        data >>= display_offset & 3;
        bit = 4 - ( display_offset & 3 );
    }

    while ( x < LCD_WIDTH ) {
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
        x++;
    }

    return ( adr + 0x22 + ( !in_menu && ( display_offset & 4 ) ? 2 : 0 ) ) & 0xFFFFF;
}

void display_show()
{
    SDL_SetRenderDrawColor( renderer, 48, 68, 90, 0xFF ); // bleu foncé
    SDL_RenderClear( renderer );

    if ( faceplateTexture ) {
        SDL_Rect r3 = { 8, 0, 504, 1124 };
        SDL_RenderCopy( renderer, faceplateTexture, NULL, &r3 );
    }

    if ( shouldRender == true ) {
        shouldRender = false;

        int pitch, w, h;
        Uint32* pixels;
        int access;
        Uint32 format;

        if ( SDL_QueryTexture( texTarget, &format, &access, &w, &h ) != 0 )
            printf( "error\n" );

        if ( SDL_LockTexture( texTarget, NULL, ( void** )&pixels, &pitch ) != 0 )
            printf( "SDL_LockTexture: %s.\n", SDL_GetError() );

        SDL_PixelFormat* pixelFormat = SDL_AllocFormat( format );

        // do stuff
        for ( int y = 0; y < LCD_HEIGHT; y++ ) {
            for ( int x = 0; x < LCD_WIDTH; x++ ) {
                int R = 0;
                int G = 0;
                int B = 0;

                byte hp48pixel = lcdScreenGS[ x + y * LCD_WIDTH ];

                if ( hp48pixel == '\0' ) {
                    R = 119;
                    G = 153;
                    B = 136;
                } else if ( hp48pixel == '\1' ) {
                    R = 71;  // 200;
                    G = 134; // 20;
                    B = 145; // 20;
                } else if ( hp48pixel == '\2' ) {
                    R = 13;  // 20;
                    G = 108; // 200;
                    B = 111; // 20;
                } else if ( hp48pixel == '\3' ) {
                    R = 37;
                    G = 61;
                    B = 84;
                }

                // Now you want to format the color to a correct format that SDL
                // can use. Basically we convert our RGB color to a hex-like BGR
                // color.

                Uint32 color = SDL_MapRGB( pixelFormat, R, G, B );

                // Before setting the color, we need to know where we have to
                // place it.
                Uint32 pixelPosition = y * ( pitch / sizeof( unsigned int ) ) + x;

                pixels[ pixelPosition ] = color;
            }
        }

        SDL_UnlockTexture( texTarget );
    }

    // Show rendered to texture
    SDL_Rect r1 = { 0, 0, LCD_WIDTH, LCD_HEIGHT };
    SDL_Rect r2 = { LCD_X * UI_SCALE, LCD_Y * UI_SCALE, LCD_WIDTH * UI_SCALE, LCD_HEIGHT * UI_SCALE };
    SDL_RenderCopyEx( renderer, texTarget, &r1, &r2, 0, NULL, SDL_FLIP_NONE );

    button_draw_all( calc_buttons );

    SDL_RenderPresent( renderer );
}

void display_update( void )
{
    if ( !display_enable && !off_cnt ) { /* Turn off display */
        off_cnt = 1;
        off_cur_line = off_line = display_line_count;
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
