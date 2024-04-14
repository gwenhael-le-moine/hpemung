#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "keyboard.h"
#include "files.h"
#include "gui.h"
#include "emulator.h"
#include "display.h"

#define PANEL_FLAG_VISIBLE 0x01

extern SDL_Renderer* renderer;

TTF_Font* ttffont = NULL;
TTF_Font* ttffont2 = NULL;

bool SDL_ready = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texTarget = NULL;
SDL_Texture* tex2Target = NULL;

SDL_Surface* surfaces_labels[ 49 ];
SDL_Texture* textures_labels[ 49 ];

SDL_Surface* surfaces_labels_Lshift[ 49 ];
SDL_Texture* textures_labels_Lshift[ 49 ];

SDL_Surface* surfaces_labels_Rshift[ 49 ];
SDL_Texture* textures_labels_Rshift[ 49 ];

SDL_Surface* surfaces_labels_below[ 49 ];
SDL_Texture* textures_labels_below[ 49 ];

SDL_Surface* surfaces_labels_letter[ 49 ];
SDL_Texture* textures_labels_letter[ 49 ];

const int std_flags = BUTTON_B1RELEASE | BUTTON_B2TOGGLE;

Button gui_buttons[] = {
    {0,  X_COL( 0 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "",          "",         "",         "A", "",       press_A,        release_A       },
    {1,  X_COL( 1 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "",          "",         "",         "B", "",       press_B,        release_B       },
    {2,  X_COL( 2 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "",          "",         "",         "C", "",       press_C,        release_C       },
    {3,  X_COL( 3 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "",          "",         "",         "D", "",       press_D,        release_D       },
    {4,  X_COL( 4 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "",          "",         "",         "E", "",       press_E,        release_E       },
    {5,  X_COL( 5 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "",          "",         "",         "F", "",       press_F,        release_F       },

    {6,  X_COL( 0 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "MTH",       "RAD",      "POLAR",    "G", "",       press_MTH,      release_MTH     },
    {7,  X_COL( 1 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "PRG",       "",         "CHARS",    "H", "",       press_PRG,      release_PRG     },
    {8,  X_COL( 2 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "CST",       "",         "MODES",    "I", "",       press_CST,      release_CST     },
    {9,  X_COL( 3 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "VAR",       "",         "MEMORY",   "J", "",       press_VAR,      release_VAR     },
    {10, X_COL( 4 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "▲",       "",         "STACK",    "K", "",       press_UP,       release_UP      },
    {11, X_COL( 5 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "NXT",       "PREV",     "MENU",     "L", "",       press_NXT,      release_NXT     },

    {12, X_COL( 0 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "'",         "UP",       "HOME",     "M", "",       press_QUOTE,    release_QUOTE   },
    {13, X_COL( 1 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "STO",       "REF",      "RCL",      "N", "",       press_STO,      release_STO     },
    {14, X_COL( 2 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "EVAL",      "→NUM",   "UNDO",     "O", "",       press_EVAL,     release_EVAL    },
    {15, X_COL( 3 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "◀",       "",         "PICTURE",  "P", "",       press_LEFT,     release_LEFT    },
    {16, X_COL( 4 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "▼",       "",         "VIEW",     "Q", "",       press_DOWN,     release_DOWN    },
    {17, X_COL( 5 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "▶",       "",         "SWAP",     "R", "",       press_RIGHT,    release_RIGHT   },

    {18, X_COL( 0 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "SIN",       "ASIN",     "𝛛",     "S", "",       press_SIN,      release_SIN     },
    {19, X_COL( 1 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "COS",       "ACOS",     "∫",      "T", "",       press_COS,      release_COS     },
    {20, X_COL( 2 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "TAN",       "ATAN",     "𝚺",     "U", "",       press_TAN,      release_TAN     },
    {21, X_COL( 3 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "√x",      "x²",      "x√y",    "V", "",       press_SQRT,     release_SQRT    },
    {22, X_COL( 4 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "y^x",       "⏨^x",    "LOG",      "W", "",       press_POW,      release_POW     },
    {23, X_COL( 5 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "1/x",       "e^x",      "LN",       "X", "",       press_INV,      release_INV     },

    {24, X_COL( 0 ),      Y_LINE( 4 ), UI_K_WIDTH_enter, UI_K_HEIGHT_2, std_flags, "ENTER",     "EQUATION", "MATRIX",   "",  "",       press_ENTER,
     release_ENTER                                                                                                                                                     },
    {25, X_COL( 2 ),      Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "±",        "EDIT",     "CMD",      "Y", "",       press_NEG,      release_NEG     },
    {26, X_COL( 3 ),      Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "EEX",       "PURG",     "ARG",      "Z", "",       press_EEX,      release_EEX     },
    {27, X_COL( 4 ),      Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "DEL",       "",         "CLEAR",    "",  "",       press_DEL,      release_DEL     },
    {28, X_COL( 5 ),      Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "←",       "",         "DROP",     "",  "",       press_BKSP,     release_BKSP    },

    {29, X_COL( 0 ),      Y_LINE( 5 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "α",        "USER",     "ENTRY",    "",  "",       press_ALPHA,    release_ALPHA   },
    {30, X2_COL( 1 ),     Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "7",         "",         "SOLVE",    "",  "",       press_7,        release_7       },
    {31, X2_COL( 2 ),     Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "8",         "",         "PLOT",     "",  "",       press_8,        release_8       },
    {32, X2_COL( 3 ),     Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "9",         "",         "SYMBOLIC", "",  "",       press_9,        release_9       },
    {33, X2_COL( 4 ) + 2, Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "÷",        "( )",      "#",        "",  "",       press_DIV,      release_DIV     },

    {34, X_COL( 0 ),      Y_LINE( 6 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "⮢",       "",         "",         "",  "",       press_LSHIFT,   release_LSHIFT  },
    {35, X2_COL( 1 ),     Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "4",         "",         "TIME",     "",  "",       press_4,        release_4       },
    {36, X2_COL( 2 ),     Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "5",         "",         "STAT",     "",  "",       press_5,        release_5       },
    {37, X2_COL( 3 ),     Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "6",         "",         "UNITS",    "",  "",       press_6,        release_6       },
    {38, X2_COL( 4 ) + 2, Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "×",        "[ ]",     "_",        "",  "",       press_MULT,     release_MULT    },

    {39, X_COL( 0 ),      Y_LINE( 7 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "⮣",       "",         "",         "",  "",       press_RSHIFT,   release_RSHIFT  },
    {40, X2_COL( 1 ),     Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "1",         "",         "I/O",      "",  "",       press_1,        release_1       },
    {41, X2_COL( 2 ),     Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "2",         "",         "LIBRARY",  "",  "",       press_2,        release_2       },
    {42, X2_COL( 3 ),     Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "3",         "",         "EQ LIB",   "",  "",       press_3,        release_3       },
    {43, X2_COL( 4 ) + 2, Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "-",         "« »",    "\" \"",    "",  "",       press_MINUS,    release_MINUS   },

    {44, X_COL( 0 ),      Y_LINE( 8 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "ON",        "CONT",     "OFF",      "",  "CANCEL", press_ON,       release_ON      },
    {45, X2_COL( 1 ),     Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "0",         "=",        "→",      "",  "",       press_0,        release_0       },
    {46, X2_COL( 2 ),     Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, ".",         ",",        "⮐",      "",  "",       press_PERIOD,   release_PERIOD  },
    {47, X2_COL( 3 ),     Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "SPC",       "𝛑",     "⦨",      "",  "",       press_SPC,      release_SPC     },
    {48, X2_COL( 4 ) + 2, Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "+",         "{ }",      ": :",      "",  "",       press_PLUS,     release_PLUS    },

    {49, X_COL( 0 ),      Y_LINE( 9 ), 40,               UI_K_HEIGHT_2, std_flags, "load file", "",         "",         "",  "",       press_LoadFile, release_LoadFile},
};

colors_t colors = {
    .faceplate = {.r = 48,  .g = 68,  .b = 90,  .a = 255},

    .lcd_pixoff = {.r = 119, .g = 153, .b = 136, .a = 255},
    .lcd_pixgray1 = {.r = 71,  .g = 134, .b = 145, .a = 255},
    .lcd_pixgray2 = {.r = 13,  .g = 108, .b = 111, .a = 255},
    .lcd_pixon = {.r = 37,  .g = 61,  .b = 84,  .a = 255},

    .button_bg = {.r = 16,  .g = 26,  .b = 39,  .a = 33 },
    .button_active = {255,      255,      39,       33      },
    .button_inactive = {0,        0,        0,        255     },
    .label = {255,      255,      255,      255     },
    .Lshift = {191,      192,      236,      255     },
    .Rshift = {125,      215,      235,      255     },
    .letter = {255,      255,      255,      255     },
    .below = {128,      108,      29,       255     },
};

static inline void _init_keyboard_textures( Button* calcbuttons )
{
    printf( "init texts\n" );
    if ( ttffont == NULL ) {
        printf( "init texts error Font NULL\n" );
        return;
    }
    SDL_Surface* s = NULL;
    SDL_Texture* t = NULL;

    int i = 0;
    Button* buttons = calcbuttons;
    while ( buttons->label ) {
        s = NULL;
        t = NULL;
        if ( buttons->label && strcmp( buttons->label, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont, buttons->label, colors.label );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }

        surfaces_labels[ i ] = s;
        textures_labels[ i ] = t;

        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->label_Lshift ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_Lshift && strcmp( buttons->label_Lshift, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_Lshift, colors.Lshift );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfaces_labels_Lshift[ i ] = s;
        textures_labels_Lshift[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->label_Rshift ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_Rshift && strcmp( buttons->label_Rshift, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_Rshift, colors.Rshift );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfaces_labels_Rshift[ i ] = s;
        textures_labels_Rshift[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->label_below ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_below && strcmp( buttons->label_below, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_below, colors.letter );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfaces_labels_below[ i ] = s;
        textures_labels_below[ i ] = t;
        i++;
        buttons++;
    }

    i = 0;
    buttons = calcbuttons;
    while ( buttons->label_letter ) {
        s = NULL;
        t = NULL;
        if ( buttons->label_letter && strcmp( buttons->label_letter, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, buttons->label_letter, colors.below );
            if ( s ) {
                t = SDL_CreateTextureFromSurface( renderer, s );
            }
        }
        surfaces_labels_letter[ i ] = s;
        textures_labels_letter[ i ] = t;
        i++;
        buttons++;
    }
}

static inline void _draw_button_labels( int index, int x, int y, int btn_w, int btn_h )
{
    int texW;
    int texH;
    int h_padding = 3;

    SDL_Surface* surface_label = surfaces_labels[ index ];
    SDL_Texture* texture_label = textures_labels[ index ];
    if ( surface_label != NULL && texture_label != NULL ) {
        texW = surface_label->w;
        texH = surface_label->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y + ( btn_h / 3 ), texW, texH };
        SDL_RenderCopy( renderer, texture_label, NULL, &destRect );
    }

    SDL_Surface* surface_label_Lshift = surfaces_labels_Lshift[ index ];
    SDL_Texture* texture_label_Lshift = textures_labels_Lshift[ index ];
    if ( surface_label_Lshift != NULL && texture_label_Lshift != NULL ) {
        texW = surface_label_Lshift->w;
        texH = surface_label_Lshift->h;
        SDL_Rect destRect = { x + h_padding, y, texW, texH };
        SDL_RenderCopy( renderer, texture_label_Lshift, NULL, &destRect );
    }

    SDL_Surface* surface_label_Rshift = surfaces_labels_Rshift[ index ];
    SDL_Texture* texture_label_Rshift = textures_labels_Rshift[ index ];
    if ( surface_label_Rshift != NULL && texture_label_Rshift != NULL ) {
        texW = surface_label_Rshift->w;
        texH = surface_label_Rshift->h;
        SDL_Rect destRect = { ( x + btn_w ) - ( texW + h_padding ), y, texW, texH };
        if ( surface_label_Lshift == NULL )
            destRect.x = x + ( btn_w - texW ) / 2;
        SDL_RenderCopy( renderer, texture_label_Rshift, NULL, &destRect );
    }

    SDL_Surface* surface_label_letter = surfaces_labels_letter[ index ];
    SDL_Texture* texture_label_letter = textures_labels_letter[ index ];
    if ( surface_label_letter != NULL && texture_label_letter != NULL ) {
        texW = surface_label_letter->w;
        texH = surface_label_letter->h;
        SDL_Rect destRect = { ( x + btn_w ) - ( texW / 2 ), y + ( btn_h - ( 5 * UI_SCALE ) ), texW, texH };
        SDL_RenderCopy( renderer, texture_label_letter, NULL, &destRect );
    }

    SDL_Surface* surface_label_below = surfaces_labels_below[ index ];
    SDL_Texture* texture_label_below = textures_labels_below[ index ];
    if ( surface_label_below != NULL && texture_label_below != NULL ) {
        texW = surface_label_below->w;
        texH = surface_label_below->h;
        SDL_Rect destRect = { x + ( btn_w - texW ) / 2, y + ( btn_h - ( 3 * UI_SCALE ) ), texW, texH };
        SDL_RenderCopy( renderer, texture_label_below, NULL, &destRect );
    }
}

static inline void _button_draw( Button* b )
{
    SDL_Rect rectToDraw = { ( b->x + ( UI_KEY_PADDING / 2 ) ) * UI_SCALE, ( b->y + ( UI_KEY_PADDING * 1.25 ) ) * UI_SCALE,
                            ( b->w - UI_KEY_PADDING ) * UI_SCALE, ( b->h - ( UI_KEY_PADDING * 2 ) ) * UI_SCALE };

    if ( b->index < 6 )
        SDL_SetRenderDrawColor( renderer, colors.label.r, colors.label.g, colors.label.g, colors.label.a );
    else if ( b->index == 34 )
        SDL_SetRenderDrawColor( renderer, colors.Lshift.r, colors.Lshift.g, colors.Lshift.g, colors.Lshift.a );
    else if ( b->index == 39 )
        SDL_SetRenderDrawColor( renderer, colors.Rshift.r, colors.Rshift.g, colors.Rshift.g, colors.Rshift.a );
    else
        SDL_SetRenderDrawColor( renderer, colors.button_bg.r, colors.button_bg.g, colors.button_bg.g, colors.button_bg.a );
    SDL_RenderFillRect( renderer, &rectToDraw );

    if ( b->flags & BUTTON_PUSHED )
        SDL_SetRenderDrawColor( renderer, colors.button_active.r, colors.button_active.g, colors.button_active.b, colors.button_active.a );
    else
        SDL_SetRenderDrawColor( renderer, colors.button_inactive.r, colors.button_inactive.g, colors.button_inactive.b,
                                colors.button_inactive.a );

    SDL_RenderDrawRect( renderer, &rectToDraw );

    _draw_button_labels( b->index, b->x * UI_SCALE, b->y * UI_SCALE, b->w * UI_SCALE, b->h * UI_SCALE );
}

static inline Button* _find_button( Button* b, int x, int y )
{
    while ( b->label ) {
        if ( x >= b->x * UI_SCALE && x < b->x * UI_SCALE + b->w * UI_SCALE && y >= b->y * UI_SCALE &&
             y < b->y * UI_SCALE + b->h * UI_SCALE )
            return b;

        b++;
    }

    return NULL;
}

static inline int _button_mouse_down( Button* buttons, int mx, int my, int mb )
{
    Button* b = _find_button( buttons, mx, my );
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

static inline int _button_mouse_up( Button* buttons, int mx, int my, int mb )
{
    Button* b = _find_button( buttons, mx, my );
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

static inline void button_draw_all( Button* buttons )
{
    while ( buttons->label ) {
        _button_draw( buttons );
        buttons++;
    }
}

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

    button_draw_all( gui_buttons );

    SDL_RenderPresent( renderer );
}

bool gui_events()
{
    SDL_Event event;

    while ( SDL_PollEvent( &event ) ) {
        switch ( event.type ) {
            case SDL_MOUSEBUTTONUP:
                _button_mouse_up( gui_buttons, event.button.x, event.button.y, 1 );
                break;

            case SDL_MOUSEBUTTONDOWN:
                _button_mouse_down( gui_buttons, event.button.x, event.button.y, 1 );
                break;

            case SDL_KEYDOWN:
                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_KP_0:
                    case SDL_SCANCODE_0:
                        press_0();
                        break;
                    case SDL_SCANCODE_KP_1:
                    case SDL_SCANCODE_1:
                        press_1();
                        break;
                    case SDL_SCANCODE_KP_2:
                    case SDL_SCANCODE_2:
                        press_2();
                        break;
                    case SDL_SCANCODE_KP_3:
                    case SDL_SCANCODE_3:
                        press_3();
                        break;
                    case SDL_SCANCODE_KP_4:
                    case SDL_SCANCODE_4:
                        press_4();
                        break;
                    case SDL_SCANCODE_KP_5:
                    case SDL_SCANCODE_5:
                        press_5();
                        break;
                    case SDL_SCANCODE_KP_6:
                    case SDL_SCANCODE_6:
                        press_6();
                        break;
                    case SDL_SCANCODE_KP_7:
                    case SDL_SCANCODE_7:
                        press_7();
                        break;
                    case SDL_SCANCODE_KP_8:
                    case SDL_SCANCODE_8:
                        press_8();
                        break;
                    case SDL_SCANCODE_KP_9:
                    case SDL_SCANCODE_9:
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
                    case SDL_SCANCODE_UP:
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
                    case SDL_SCANCODE_LEFT:
                        press_LEFT();
                        break;
                    case SDL_SCANCODE_Q:
                    case SDL_SCANCODE_DOWN:
                        press_DOWN();
                        break;
                    case SDL_SCANCODE_R:
                    case SDL_SCANCODE_RIGHT:
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
                    case SDL_SCANCODE_0:
                        release_0();
                        break;
                    case SDL_SCANCODE_KP_1:
                    case SDL_SCANCODE_1:
                        release_1();
                        break;
                    case SDL_SCANCODE_KP_2:
                    case SDL_SCANCODE_2:
                        release_2();
                        break;
                    case SDL_SCANCODE_KP_3:
                    case SDL_SCANCODE_3:
                        release_3();
                        break;
                    case SDL_SCANCODE_KP_4:
                    case SDL_SCANCODE_4:
                        release_4();
                        break;
                    case SDL_SCANCODE_KP_5:
                    case SDL_SCANCODE_5:
                        release_5();
                        break;
                    case SDL_SCANCODE_KP_6:
                    case SDL_SCANCODE_6:
                        release_6();
                        break;
                    case SDL_SCANCODE_KP_7:
                    case SDL_SCANCODE_7:
                        release_7();
                        break;
                    case SDL_SCANCODE_KP_8:
                    case SDL_SCANCODE_8:
                        release_8();
                        break;
                    case SDL_SCANCODE_KP_9:
                    case SDL_SCANCODE_9:
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
                    case SDL_SCANCODE_UP:
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
                    case SDL_SCANCODE_LEFT:
                        release_LEFT();
                        break;
                    case SDL_SCANCODE_Q:
                    case SDL_SCANCODE_DOWN:
                        release_DOWN();
                        break;
                    case SDL_SCANCODE_R:
                    case SDL_SCANCODE_RIGHT:
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

    ttffont = TTF_OpenFont( config.ui_font, 7 * UI_SCALE );
    ttffont2 = TTF_OpenFont( config.ui_font, 5 * UI_SCALE );

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

    _init_keyboard_textures( gui_buttons );

    printf( "init done\n" );

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
