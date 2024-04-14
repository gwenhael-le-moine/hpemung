#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "display.h"
#include "gui.h"
#include "gui_buttons.h"

void SDL__display_show()
{
    SDL_SetRenderDrawColor( renderer, colors.faceplate.r, colors.faceplate.g, colors.faceplate.b, colors.faceplate.a );
    SDL_RenderClear( renderer );

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
                    R = colors.lcd_pixoff.r;
                    G = colors.lcd_pixoff.g;
                    B = colors.lcd_pixoff.b;
                } else if ( hp48pixel == '\1' ) {
                    R = colors.lcd_pixgray1.r;
                    G = colors.lcd_pixgray1.g;
                    B = colors.lcd_pixgray1.b;
                } else if ( hp48pixel == '\2' ) {
                    R = colors.lcd_pixgray2.r;
                    G = colors.lcd_pixgray2.g;
                    B = colors.lcd_pixgray2.b;
                } else if ( hp48pixel == '\3' ) {
                    R = colors.lcd_pixon.r;
                    G = colors.lcd_pixon.g;
                    B = colors.lcd_pixon.b;
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
