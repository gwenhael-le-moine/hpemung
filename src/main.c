#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "emulator.h"
#include "gui.h"
#include "color.h"
#include "display.h"
#include "gui.h"
#include "timers.h"
#include "keyboard.h"
#include "pcalc.h"
#include "pfiles.h"

const int SCREEN_WIDTH = 524;
const int SCREEN_HEIGHT = 750;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texTarget = NULL;
SDL_Texture* tex2Target = NULL;
SDL_Texture* faceplateTexture = NULL;

TTF_Font* ttffont = NULL;
TTF_Font* ttffont2 = NULL;

bool SDL_ready = false;

unsigned int currentTime;

// display_update
unsigned int lastTime_timer1 = 0;
unsigned int delay_timer1 = 16384;

// display show
unsigned int lastTime_timer5 = 0;
unsigned int delay_timer5 = 64; // fps

static int fullscreen = false;

static void parse_args( int argc, char* argv[] )
{
    while ( --argc ) {
        argv++;
        if ( argv[ 0 ][ 0 ] == '-' ) {
            switch ( argv[ 0 ][ 1 ] ) {
                case 'f':
                    fullscreen = true;
                    break;
                case 'w':
                    fullscreen = false;
                    break;
            }
        }
    }
}

static void program_init( void )
{
    if ( SDL_Init( SDL_INIT_VIDEO | IMG_INIT_PNG | SDL_INIT_TIMER ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return;
    }

    if ( TTF_Init() == -1 ) {
        fprintf( stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError() );
        exit( EXIT_FAILURE );
    }

    ttffont = TTF_OpenFont( FONT_FILENAME, 14 );
    ttffont2 = TTF_OpenFont( FONT_FILENAME, 10 );

    window = SDL_CreateWindow( "jsEmu48", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

    pcalc_init();

    printf( "init done\n" );

    color_init();

    SDL_ready = true;
}

static void program_exit( void )
{
    TTF_CloseFont( ttffont );
    TTF_CloseFont( ttffont2 );
    TTF_Quit();

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

bool refreshSDL()
{
    SDL_Event event;

    while ( SDL_PollEvent( &event ) ) {
        switch ( event.type ) {
            case SDL_MOUSEBUTTONUP:
                {
                    // printf("mouse up %d %d\n", event.button.x, event.button.y);

                    pcalc_up( event.button.x, event.button.y, 1 );
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                {
                    // printf("mouse down %d %d\n", event.button.x, event.button.y);

                    pcalc_down( event.button.x, event.button.y, 1 );
                }
                break;

            case SDL_KEYDOWN:
                pcalc_kb_down( event.key.keysym.scancode );

                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_ESCAPE:
                        kbd_on_pressed();
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                        kbd_key_pressed( 4, 4 );
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        kbd_key_pressed( 4, 0 );
                        break;
                    case SDL_SCANCODE_LEFT:
                        kbd_key_pressed( 6, 2 );
                        break;
                    case SDL_SCANCODE_RIGHT:
                        kbd_key_pressed( 6, 0 );
                        break;
                    case SDL_SCANCODE_UP:
                        kbd_key_pressed( 7, 1 );
                        break;
                    case SDL_SCANCODE_DOWN:
                        kbd_key_pressed( 6, 1 );
                        break;
                    case SDL_SCANCODE_KP_PLUS:
                        kbd_key_pressed( 0, 0 );
                        break;
                    case SDL_SCANCODE_KP_MINUS:
                        kbd_key_pressed( 1, 0 );
                        break;
                    case SDL_SCANCODE_KP_MULTIPLY:
                        kbd_key_pressed( 2, 0 );
                        break;
                    case SDL_SCANCODE_KP_DIVIDE:
                        kbd_key_pressed( 3, 0 );
                        break;
                    case SDL_SCANCODE_A:
                        kbd_key_pressed( 1, 4 );
                        break;
                    case SDL_SCANCODE_B:
                        kbd_key_pressed( 8, 4 );
                        break;
                    case SDL_SCANCODE_C:
                        kbd_key_pressed( 8, 3 );
                        break;
                    case SDL_SCANCODE_D:
                        kbd_key_pressed( 8, 2 );
                        break;
                    case SDL_SCANCODE_E:
                        kbd_key_pressed( 8, 1 );
                        break;
                    case SDL_SCANCODE_F:
                        kbd_key_pressed( 8, 0 );
                        break;
                    case SDL_SCANCODE_G:
                        kbd_key_pressed( 2, 4 );
                        break;
                    case SDL_SCANCODE_H:
                        kbd_key_pressed( 7, 4 );
                        break;
                    case SDL_SCANCODE_I:
                        kbd_key_pressed( 7, 3 );
                        break;
                    case SDL_SCANCODE_J:
                        kbd_key_pressed( 7, 2 );
                        break;
                    case SDL_SCANCODE_K:
                        kbd_key_pressed( 7, 1 );
                        break;
                    case SDL_SCANCODE_L:
                        kbd_key_pressed( 7, 0 );
                        break;
                    case SDL_SCANCODE_M:
                        kbd_key_pressed( 0, 4 );
                        break;
                    case SDL_SCANCODE_N:
                        kbd_key_pressed( 6, 4 );
                        break;
                    case SDL_SCANCODE_O:
                        kbd_key_pressed( 6, 3 );
                        break;
                    case SDL_SCANCODE_P:
                        kbd_key_pressed( 6, 2 );
                        break;
                    case SDL_SCANCODE_Q:
                        kbd_key_pressed( 6, 1 );
                        break;
                    case SDL_SCANCODE_R:
                        kbd_key_pressed( 6, 0 );
                        break;
                    case SDL_SCANCODE_S:
                        kbd_key_pressed( 3, 4 );
                        break;
                    case SDL_SCANCODE_T:
                        kbd_key_pressed( 5, 4 );
                        break;
                    case SDL_SCANCODE_U:
                        kbd_key_pressed( 5, 3 );
                        break;
                    case SDL_SCANCODE_V:
                        kbd_key_pressed( 5, 2 );
                        break;
                    case SDL_SCANCODE_W:
                        kbd_key_pressed( 5, 1 );
                        break;
                    case SDL_SCANCODE_X:
                        kbd_key_pressed( 5, 0 );
                        break;
                    case SDL_SCANCODE_Y:
                        kbd_key_pressed( 4, 3 );
                        break;
                    case SDL_SCANCODE_Z:
                        kbd_key_pressed( 4, 2 );
                        break;
                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                pcalc_kb_up( event.key.keysym.scancode );

                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_ESCAPE:
                        kbd_on_released();
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                        kbd_key_released( 4, 4 );
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        kbd_key_released( 4, 0 );
                        break;
                    case SDL_SCANCODE_LEFT:
                        kbd_key_released( 6, 2 );
                        break;
                    case SDL_SCANCODE_RIGHT:
                        kbd_key_released( 6, 0 );
                        break;
                    case SDL_SCANCODE_UP:
                        kbd_key_released( 7, 1 );
                        break;
                    case SDL_SCANCODE_DOWN:
                        kbd_key_released( 6, 1 );
                        break;
                    case SDL_SCANCODE_KP_PLUS:
                        kbd_key_released( 0, 0 );
                        break;
                    case SDL_SCANCODE_KP_MINUS:
                        kbd_key_released( 1, 0 );
                        break;
                    case SDL_SCANCODE_KP_MULTIPLY:
                        kbd_key_released( 2, 0 );
                        break;
                    case SDL_SCANCODE_KP_DIVIDE:
                        kbd_key_released( 3, 0 );
                        break;
                    case SDL_SCANCODE_A:
                        kbd_key_released( 1, 4 );
                        break;
                    case SDL_SCANCODE_B:
                        kbd_key_released( 8, 4 );
                        break;
                    case SDL_SCANCODE_C:
                        kbd_key_released( 8, 3 );
                        break;
                    case SDL_SCANCODE_D:
                        kbd_key_released( 8, 2 );
                        break;
                    case SDL_SCANCODE_E:
                        kbd_key_released( 8, 1 );
                        break;
                    case SDL_SCANCODE_F:
                        kbd_key_released( 8, 0 );
                        break;
                    case SDL_SCANCODE_G:
                        kbd_key_released( 2, 4 );
                        break;
                    case SDL_SCANCODE_H:
                        kbd_key_released( 7, 4 );
                        break;
                    case SDL_SCANCODE_I:
                        kbd_key_released( 7, 3 );
                        break;
                    case SDL_SCANCODE_J:
                        kbd_key_released( 7, 2 );
                        break;
                    case SDL_SCANCODE_K:
                        kbd_key_released( 7, 1 );
                        break;
                    case SDL_SCANCODE_L:
                        kbd_key_released( 7, 0 );
                        break;
                    case SDL_SCANCODE_M:
                        kbd_key_released( 0, 4 );
                        break;
                    case SDL_SCANCODE_N:
                        kbd_key_released( 6, 4 );
                        break;
                    case SDL_SCANCODE_O:
                        kbd_key_released( 6, 3 );
                        break;
                    case SDL_SCANCODE_P:
                        kbd_key_released( 6, 2 );
                        break;
                    case SDL_SCANCODE_Q:
                        kbd_key_released( 6, 1 );
                        break;
                    case SDL_SCANCODE_R:
                        kbd_key_released( 6, 0 );
                        break;
                    case SDL_SCANCODE_S:
                        kbd_key_released( 3, 4 );
                        break;
                    case SDL_SCANCODE_T:
                        kbd_key_released( 5, 4 );
                        break;
                    case SDL_SCANCODE_U:
                        kbd_key_released( 5, 3 );
                        break;
                    case SDL_SCANCODE_V:
                        kbd_key_released( 5, 2 );
                        break;
                    case SDL_SCANCODE_W:
                        kbd_key_released( 5, 1 );
                        break;
                    case SDL_SCANCODE_X:
                        kbd_key_released( 5, 0 );
                        break;
                    case SDL_SCANCODE_Y:
                        kbd_key_released( 4, 3 );
                        break;
                    case SDL_SCANCODE_Z:
                        kbd_key_released( 4, 2 );
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

static inline void mainloop()
{
    if ( please_exit || !SDL_ready )
        return;

    currentTime = SDL_GetTicks();

    emulator_run();

    // display_update
    if ( currentTime > lastTime_timer1 + delay_timer1 ) {
        lastTime_timer1 = currentTime;
        display_update();
    }

    // display show
    if ( currentTime > lastTime_timer5 + delay_timer5 ) {
        lastTime_timer5 = currentTime;
        display_show();
    }

    if ( !refreshSDL() )
        return;
}

int main( int argc, char* argv[] )
{
    parse_args( argc, argv );

    program_init();
    emulator_init();

    while ( please_exit == false )
        mainloop();

    emulator_exit();
    program_exit();

    return 0;
}
