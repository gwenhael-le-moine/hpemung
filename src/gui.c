#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "gui_color.h"
#include "gui_buttons.h"
#include "files.h"
#include "gui.h"
#include "emulator.h"
#include "display.h"

#define PANEL_FLAG_VISIBLE 0x01

extern SDL_Renderer* renderer;
extern SDL_Texture* faceplateTexture;
extern TTF_Font* ttffont;
extern TTF_Font* ttffont2;

SDL_Surface* surfA[ 49 ];
SDL_Texture* textA[ 49 ];

SDL_Surface* surfB[ 49 ];
SDL_Texture* label_Lshift[ 49 ];

SDL_Surface* surfC[ 49 ];
SDL_Texture* label_Rshift[ 49 ];

SDL_Surface* surfD[ 49 ];
SDL_Texture* label_below[ 49 ];

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
    SDL_Texture* letterTexture2 = label_Lshift[ index ];
    if ( letterSurface2 != NULL && letterTexture2 != NULL ) {
        int texW = letterSurface2->w;
        int texH = letterSurface2->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        destRect.y -= 10;
        destRect.x -= 16;
        SDL_RenderCopy( renderer, letterTexture2, NULL, &destRect );
    }

    SDL_Surface* letterSurface3 = surfC[ index ];
    SDL_Texture* letterTexture3 = label_Rshift[ index ];
    if ( letterSurface3 != NULL && letterTexture3 != NULL ) {
        int texW = letterSurface3->w;
        int texH = letterSurface3->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        destRect.y -= 10;
        destRect.x += 16;
        SDL_RenderCopy( renderer, letterTexture3, NULL, &destRect );
    }

    SDL_Surface* letterSurface4 = surfD[ index ];
    SDL_Texture* letterTexture4 = label_below[ index ];
    if ( letterSurface4 != NULL && letterTexture4 != NULL ) {
        int texW = letterSurface4->w;
        int texH = letterSurface4->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y, texW, texH };
        destRect.y += 12;
        destRect.x += 22;
        SDL_RenderCopy( renderer, letterTexture4, NULL, &destRect );
    }
}

void SDL__gui_initKeyboard( Button* calcbuttons )
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
    SDL_Surface* s = NULL;
    SDL_Texture* t = NULL;

    int i = 0;
    Button* buttons = calcbuttons;
    while ( buttons->label ) {
        s = NULL;
        t = NULL;
        if ( buttons->label && strcmp( buttons->label, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont, buttons->label, couleurBlanche );
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
    while ( buttons->label_Lshift ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_Lshift && strcmp( buttons->label_Lshift, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_Lshift, couleurPurple );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfB[ i ] = s;
        label_Lshift[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->label_Rshift ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_Rshift && strcmp( buttons->label_Rshift, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_Rshift, couleurGreen );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfC[ i ] = s;
        label_Rshift[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->label_Rshift ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_below && strcmp( buttons->label_below, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_below, couleurYellow );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfD[ i ] = s;
        label_below[ i ] = t;
        i++;
        buttons++;
    }
}

static inline void SDL__button_draw( Button* b )
{
    SDL_Rect rectToDraw = { b->x * UI_SCALE, b->y * UI_SCALE, b->w * UI_SCALE, b->h * UI_SCALE };

    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x33 );
    SDL_RenderFillRect( renderer, &rectToDraw );

    drawText( b->index, b->x * UI_SCALE, 10 + b->y * UI_SCALE, b->w * UI_SCALE, b->h * UI_SCALE );

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
    while ( buttons->label ) {
        SDL__button_draw( buttons );
        buttons++;
    }
}

static inline Button* find_button( Button* b, int x, int y )
{
    while ( b->label ) {
        if ( x >= b->x * UI_SCALE && x < b->x * UI_SCALE + b->w * UI_SCALE && y >= b->y * UI_SCALE &&
             y < b->y * UI_SCALE + b->h * UI_SCALE )
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
        for ( b = buttons; b->label; b++ ) {
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

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texTarget = NULL;
SDL_Texture* tex2Target = NULL;
SDL_Texture* faceplateTexture = NULL;

TTF_Font* ttffont = NULL;
TTF_Font* ttffont2 = NULL;

bool SDL_ready = false;

bool gui_refresh()
{
    SDL_Event event;

    while ( SDL_PollEvent( &event ) ) {
        switch ( event.type ) {
            case SDL_MOUSEBUTTONUP:
                button_mouse_up( calc_buttons, event.button.x, event.button.y, 1 );
                break;

            case SDL_MOUSEBUTTONDOWN:
                button_mouse_down( calc_buttons, event.button.x, event.button.y, 1 );
                break;

            case SDL_KEYDOWN:
                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_KP_0:
                        press_0();
                        break;
                    case SDL_SCANCODE_KP_1:
                        press_1();
                        break;
                    case SDL_SCANCODE_KP_2:
                        press_2();
                        break;
                    case SDL_SCANCODE_KP_3:
                        press_3();
                        break;
                    case SDL_SCANCODE_KP_4:
                        press_4();
                        break;
                    case SDL_SCANCODE_KP_5:
                        press_5();
                        break;
                    case SDL_SCANCODE_KP_6:
                        press_6();
                        break;
                    case SDL_SCANCODE_KP_7:
                        press_7();
                        break;
                    case SDL_SCANCODE_KP_8:
                        press_8();
                        break;
                    case SDL_SCANCODE_KP_9:
                        press_9();
                        break;
                    case SDL_SCANCODE_KP_PERIOD:
                        press_PERIOD();
                        break;
                    case SDL_SCANCODE_SPACE:
                        press_SPC();
                        break;
                    case SDL_SCANCODE_ESCAPE:
                    case SDL_SCANCODE_F5:
                        press_ON();
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                    case SDL_SCANCODE_F1:
                        press_ENTER();
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        press_BKSP();
                        break;
                    case SDL_SCANCODE_LEFT:
                        press_LEFT();
                        break;
                    case SDL_SCANCODE_RIGHT:
                        press_RIGHT();
                        break;
                    case SDL_SCANCODE_UP:
                        press_UP();
                        break;
                    case SDL_SCANCODE_DOWN:
                        press_DOWN();
                        break;
                    case SDL_SCANCODE_KP_PLUS:
                        press_PLUS();
                        break;
                    case SDL_SCANCODE_KP_MINUS:
                        press_MINUS();
                        break;
                    case SDL_SCANCODE_KP_MULTIPLY:
                        press_MULT();
                        break;
                    case SDL_SCANCODE_KP_DIVIDE:
                        press_DIV();
                        break;
                    case SDL_SCANCODE_A:
                        press_A();
                        break;
                    case SDL_SCANCODE_B:
                        press_B();
                        break;
                    case SDL_SCANCODE_C:
                        press_C();
                        break;
                    case SDL_SCANCODE_D:
                        press_D();
                        break;
                    case SDL_SCANCODE_E:
                        press_E();
                        break;
                    case SDL_SCANCODE_F:
                        press_F();
                        break;
                    case SDL_SCANCODE_G:
                        press_MTH();
                        break;
                    case SDL_SCANCODE_H:
                        press_PRG();
                        break;
                    case SDL_SCANCODE_I:
                        press_CST();
                        break;
                    case SDL_SCANCODE_J:
                        press_VAR();
                        break;
                    case SDL_SCANCODE_K:
                        press_UP();
                        break;
                    case SDL_SCANCODE_L:
                        press_NXT();
                        break;
                    case SDL_SCANCODE_M:
                        press_QUOTE();
                        break;
                    case SDL_SCANCODE_N:
                        press_STO();
                        break;
                    case SDL_SCANCODE_O:
                        press_EVAL();
                        break;
                    case SDL_SCANCODE_P:
                        press_LEFT();
                        break;
                    case SDL_SCANCODE_Q:
                        press_DOWN();
                        break;
                    case SDL_SCANCODE_R:
                        press_RIGHT();
                        break;
                    case SDL_SCANCODE_S:
                        press_SIN();
                        break;
                    case SDL_SCANCODE_T:
                        press_COS();
                        break;
                    case SDL_SCANCODE_U:
                        press_TAN();
                        break;
                    case SDL_SCANCODE_V:
                        press_SQRT();
                        break;
                    case SDL_SCANCODE_W:
                        press_POW();
                        break;
                    case SDL_SCANCODE_X:
                        press_INV();
                        break;
                    case SDL_SCANCODE_Y:
                        press_NEG();
                        break;
                    case SDL_SCANCODE_Z:
                        press_EEX();
                        break;
                    case SDL_SCANCODE_F2:
                        press_LSHIFT();
                        break;
                    case SDL_SCANCODE_F3:
                        press_RSHIFT();
                        break;
                    case SDL_SCANCODE_F4:
                        press_ALPHA();
                        break;
                    case SDL_SCANCODE_F7:
                        please_exit = true;
                        break;
                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_KP_0:
                        release_0();
                        break;
                    case SDL_SCANCODE_KP_1:
                        release_1();
                        break;
                    case SDL_SCANCODE_KP_2:
                        release_2();
                        break;
                    case SDL_SCANCODE_KP_3:
                        release_3();
                        break;
                    case SDL_SCANCODE_KP_4:
                        release_4();
                        break;
                    case SDL_SCANCODE_KP_5:
                        release_5();
                        break;
                    case SDL_SCANCODE_KP_6:
                        release_6();
                        break;
                    case SDL_SCANCODE_KP_7:
                        release_7();
                        break;
                    case SDL_SCANCODE_KP_8:
                        release_8();
                        break;
                    case SDL_SCANCODE_KP_9:
                        release_9();
                        break;
                    case SDL_SCANCODE_KP_PERIOD:
                        release_PERIOD();
                        break;
                    case SDL_SCANCODE_SPACE:
                        release_SPC();
                        break;
                    case SDL_SCANCODE_ESCAPE:
                    case SDL_SCANCODE_F5:
                        release_ON();
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                    case SDL_SCANCODE_F1:
                        release_ENTER();
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        release_BKSP();
                        break;
                    case SDL_SCANCODE_LEFT:
                        release_LEFT();
                        break;
                    case SDL_SCANCODE_RIGHT:
                        release_RIGHT();
                        break;
                    case SDL_SCANCODE_UP:
                        release_UP();
                        break;
                    case SDL_SCANCODE_DOWN:
                        release_DOWN();
                        break;
                    case SDL_SCANCODE_KP_PLUS:
                        release_PLUS();
                        break;
                    case SDL_SCANCODE_KP_MINUS:
                        release_MINUS();
                        break;
                    case SDL_SCANCODE_KP_MULTIPLY:
                        release_MULT();
                        break;
                    case SDL_SCANCODE_KP_DIVIDE:
                        release_DIV();
                        break;
                    case SDL_SCANCODE_A:
                        release_A();
                        break;
                    case SDL_SCANCODE_B:
                        release_B();
                        break;
                    case SDL_SCANCODE_C:
                        release_C();
                        break;
                    case SDL_SCANCODE_D:
                        release_D();
                        break;
                    case SDL_SCANCODE_E:
                        release_E();
                        break;
                    case SDL_SCANCODE_F:
                        release_F();
                        break;
                    case SDL_SCANCODE_G:
                        release_MTH();
                        break;
                    case SDL_SCANCODE_H:
                        release_PRG();
                        break;
                    case SDL_SCANCODE_I:
                        release_CST();
                        break;
                    case SDL_SCANCODE_J:
                        release_VAR();
                        break;
                    case SDL_SCANCODE_K:
                        release_UP();
                        break;
                    case SDL_SCANCODE_L:
                        release_NXT();
                        break;
                    case SDL_SCANCODE_M:
                        release_QUOTE();
                        break;
                    case SDL_SCANCODE_N:
                        release_STO();
                        break;
                    case SDL_SCANCODE_O:
                        release_EVAL();
                        break;
                    case SDL_SCANCODE_P:
                        release_LEFT();
                        break;
                    case SDL_SCANCODE_Q:
                        release_DOWN();
                        break;
                    case SDL_SCANCODE_R:
                        release_RIGHT();
                        break;
                    case SDL_SCANCODE_S:
                        release_SIN();
                        break;
                    case SDL_SCANCODE_T:
                        release_COS();
                        break;
                    case SDL_SCANCODE_U:
                        release_TAN();
                        break;
                    case SDL_SCANCODE_V:
                        release_SQRT();
                        break;
                    case SDL_SCANCODE_W:
                        release_POW();
                        break;
                    case SDL_SCANCODE_X:
                        release_INV();
                        break;
                    case SDL_SCANCODE_Y:
                        release_NEG();
                        break;
                    case SDL_SCANCODE_Z:
                        release_EEX();
                        break;
                    case SDL_SCANCODE_F2:
                        release_LSHIFT();
                        break;
                    case SDL_SCANCODE_F3:
                        release_RSHIFT();
                        break;
                    case SDL_SCANCODE_F4:
                        release_ALPHA();
                        break;
                    default:
                        break;
                }
                break;

            case SDL_USEREVENT:
                break;

            case SDL_QUIT:
                {
                    please_exit = true;
                    return false;
                }
        }
    }
    return true;
}

void gui_init( void )
{
    if ( SDL_Init( SDL_INIT_VIDEO | IMG_INIT_PNG | SDL_INIT_TIMER ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return;
    }

    if ( TTF_Init() == -1 ) {
        fprintf( stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError() );
        exit( EXIT_FAILURE );
    }

    ttffont = TTF_OpenFont( FONT_FILENAME, 6 * UI_SCALE );
    ttffont2 = TTF_OpenFont( FONT_FILENAME, 4 * UI_SCALE );

    int window_width = ( LCD_WIDTH + ( 2 * UI_PADDING ) ) * UI_SCALE;
    int window_height = ( UI_KB_OFFSET_Y + UI_KB_HEIGHT ) + 2 * UI_PADDING;

    window = SDL_CreateWindow( "hpemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return;
    }

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if ( renderer == NULL ) {
        printf( "Erreur lors de la creation d'un renderer : %s", SDL_GetError() );
        return;
    }

    tex2Target = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 504, 1124 );
    texTarget = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 504, 1124 );

    SDL_UpdateWindowSurface( window );

    SDL__gui_initKeyboard( calc_buttons );

    printf( "init done\n" );

    color_init();

    SDL_ready = true;
}

void gui_exit( void )
{
    TTF_CloseFont( ttffont );
    TTF_CloseFont( ttffont2 );
    TTF_Quit();

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();
}
