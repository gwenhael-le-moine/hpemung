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
#include "gui_buttons.h"
#include "pfiles.h"

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

    gui_initKeyboard( calc_buttons );

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
                        kbd_on_pressed();
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
                        program_exit();
                        exit( 0 );
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
                        kbd_on_released();
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

    while ( !please_exit )
        mainloop();

    emulator_exit();
    program_exit();

    return 0;
}
