#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "color.h"
#include "pcalc.h"
#include "pfiles.h"
#include "gui.h"

extern SDL_Renderer* renderer;
extern SDL_Texture* faceplateTexture;
extern TTF_Font* ttffont;
extern TTF_Font* ttffont2;

SDL_Surface* surfA[ 49 ];
SDL_Texture* textA[ 49 ];

SDL_Surface* surfB[ 49 ];
SDL_Texture* textB[ 49 ];

SDL_Surface* surfC[ 49 ];
SDL_Texture* textC[ 49 ];

SDL_Surface* surfD[ 49 ];
SDL_Texture* textD[ 49 ];

#define PANEL_FLAG_VISIBLE 0x01

static inline void drawText( int index, int x, int y, int btn_w, int btn_h )
{
    SDL_Surface* letterSurface = surfA[ index ];
    SDL_Texture* letterTexture = textA[ index ];
    if ( letterSurface != NULL && letterTexture != NULL ) {
        int texW = letterSurface->w;
        int texH = letterSurface->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        SDL_RenderCopy( renderer, letterTexture, NULL, &destRect );
    }

    SDL_Surface* letterSurface2 = surfB[ index ];
    SDL_Texture* letterTexture2 = textB[ index ];
    if ( letterSurface2 != NULL && letterTexture2 != NULL ) {
        int texW = letterSurface2->w;
        int texH = letterSurface2->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        destRect.y -= 10;
        destRect.x -= 16;
        SDL_RenderCopy( renderer, letterTexture2, NULL, &destRect );
    }

    SDL_Surface* letterSurface3 = surfC[ index ];
    SDL_Texture* letterTexture3 = textC[ index ];
    if ( letterSurface3 != NULL && letterTexture3 != NULL ) {
        int texW = letterSurface3->w;
        int texH = letterSurface3->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        destRect.y -= 10;
        destRect.x += 16;
        SDL_RenderCopy( renderer, letterTexture3, NULL, &destRect );
    }

    SDL_Surface* letterSurface4 = surfD[ index ];
    SDL_Texture* letterTexture4 = textD[ index ];
    if ( letterSurface4 != NULL && letterTexture4 != NULL ) {
        int texW = letterSurface4->w;
        int texH = letterSurface4->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        destRect.y += 12;
        destRect.x += 22;
        SDL_RenderCopy( renderer, letterTexture4, NULL, &destRect );
    }
}

void gui_initKeyboard( Button* calcbuttons )
{
    printf( "init texts\n" );
    if ( ttffont == NULL ) {
        printf( "init texts error Font NULL\n" );
        return;
    }
    SDL_Color couleurBlanche = { 255, 255, 255, 255 };
    SDL_Color couleurGreen = { 125, 215, 235, 255 };
    SDL_Color couleurPurple = { 191, 192, 236, 255 };
    SDL_Color couleurYellow = { 128, 108, 29, 255 };

    int i = 0;
    Button* buttons = calcbuttons;
    while ( buttons->text ) {
        SDL_Surface* s = NULL;
        SDL_Texture* t = NULL;
        if ( buttons->text && strcmp( buttons->text, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont, buttons->text, couleurBlanche );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }

        surfA[ i ] = s;
        textA[ i ] = t;

        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->textB ) {
        SDL_Surface* s = NULL;
        SDL_Texture* t = NULL;
        if ( buttons->textB && strcmp( buttons->textB, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->textB, couleurPurple );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfB[ i ] = s;
        textB[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->textC ) {
        SDL_Surface* s = NULL;
        SDL_Texture* t = NULL;
        if ( buttons->textC && strcmp( buttons->textC, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->textC, couleurGreen );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfC[ i ] = s;
        textC[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->textC ) {
        SDL_Surface* s = NULL;
        SDL_Texture* t = NULL;
        if ( buttons->textD && strcmp( buttons->textD, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->textD, couleurYellow );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfD[ i ] = s;
        textD[ i ] = t;
        i++;
        buttons++;
    }
}

static inline void button_draw( Button* b )
{
    SDL_Rect rectToDraw = { b->x * 2, b->y * 2, b->w * 2, b->h * 2 };

    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x33 );
    SDL_RenderFillRect( renderer, &rectToDraw );

    drawText( b->index, b->x * 2, 10 + b->y * 2, b->w * 2, b->h * 2 );

    if ( b->flags & BUTTON_PUSHED ) {
        SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0x00, 0xFF );
        SDL_RenderDrawRect( renderer, &rectToDraw );
    } else {
        // debug only
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderDrawRect( renderer, &rectToDraw );
    }
}

void button_draw_all( Button* buttons )
{
    while ( buttons->text ) {
        button_draw( buttons );
        buttons++;
    }
}

static inline Button* find_button( Button* b, int x, int y )
{
    while ( b->text ) {
        if ( x >= b->x * 2 && x < b->x * 2 + b->w * 2 && y >= b->y * 2 && y < b->y * 2 + b->h * 2 )
            return b;

        b++;
    }

    return NULL;
}

int button_mouse_down( Button* buttons, int mx, int my, int mb )
{
    Button* b = find_button( buttons, mx, my );
    if ( !b )
        return 0;

    if ( !( b->flags & BUTTON_DISABLED ) ) {
        if ( ( mb == 2 && ( b->flags & BUTTON_B2TOGGLE ) ) || ( mb == 1 && ( b->flags & BUTTON_B1TOGGLE ) ) ) {

            if ( b->flags & BUTTON_PUSHED ) {
                b->flags &= ~BUTTON_PUSHED;

                if ( b->up )
                    b->up();
            } else {
                b->flags |= BUTTON_PUSHED;

                if ( b->down )
                    b->down();
            }
        } else if ( mb == 1 && !( b->flags & BUTTON_PUSHED ) ) {
            b->flags |= BUTTON_PUSHED;

            if ( b->down )
                b->down();
        }
    }

    return 1;
}

int button_mouse_up( Button* buttons, int mx, int my, int mb )
{
    Button* b = find_button( buttons, mx, my );
    int ret = ( b != NULL );

    if ( b && !( b->flags & BUTTON_DISABLED ) ) {
        if ( mb == 1 && ( b->flags & BUTTON_PUSHED ) && !( b->flags & BUTTON_B1TOGGLE ) ) {
            b->flags &= ~BUTTON_PUSHED;

            if ( b->up )
                b->up();
        }
    }
    if ( mb == 1 ) {
        for ( b = buttons; b->text; b++ ) {
            if ( ( b->flags & ( BUTTON_B1RELEASE | BUTTON_PUSHED ) ) == ( BUTTON_B1RELEASE | BUTTON_PUSHED ) ) {
                b->flags &= ~BUTTON_PUSHED;

                if ( b->up )
                    b->up();
                ret = 1;
            }
        }
    }

    return ret;
}
