#include <stdlib.h>

#include "color.h"
#include "display.h"
#include "keyboard.h"
#include "gui.h"
#include "pcalc.h"
#include "pfiles.h"

const int pox = 2;
const int poy = 55;
const int pow1 = 40;
const int poh1 = 18;
const int poh2 = 22;
const int pow2 = 48;
const int ystart = 80;
const int yspacing = 14;
const int xstart = 5;
const int xspacing = ( pow1 + 2 );
const int xspacing2 = ( pow2 + 2 );
const int enter_w = pow1 * 2 + 2;

void press_PLUS( void ) { kbd_key_pressed( 0, 0 ); }
void release_PLUS( void ) { kbd_key_released( 0, 0 ); }
void press_SPC( void ) { kbd_key_pressed( 0, 1 ); }
void release_SPC( void ) { kbd_key_released( 0, 1 ); }
void press_PERIOD( void ) { kbd_key_pressed( 0, 2 ); }
void release_PERIOD( void ) { kbd_key_released( 0, 2 ); }
void press_0( void ) { kbd_key_pressed( 0, 3 ); }
void release_0( void ) { kbd_key_released( 0, 3 ); }
void press_QUOTE( void ) { kbd_key_pressed( 0, 4 ); }
void release_QUOTE( void ) { kbd_key_released( 0, 4 ); }
void press_MINUS( void ) { kbd_key_pressed( 1, 0 ); }
void release_MINUS( void ) { kbd_key_released( 1, 0 ); }
void press_3( void ) { kbd_key_pressed( 1, 1 ); }
void release_3( void ) { kbd_key_released( 1, 1 ); }
void press_2( void ) { kbd_key_pressed( 1, 2 ); }
void release_2( void ) { kbd_key_released( 1, 2 ); }
void press_1( void ) { kbd_key_pressed( 1, 3 ); }
void release_1( void ) { kbd_key_released( 1, 3 ); }
void press_A( void ) { kbd_key_pressed( 1, 4 ); }
void release_A( void ) { kbd_key_released( 1, 4 ); }
void press_RSHIFT( void ) { kbd_key_pressed( 1, 5 ); }
void release_RSHIFT( void ) { kbd_key_released( 1, 5 ); }
void press_MULT( void ) { kbd_key_pressed( 2, 0 ); }
void release_MULT( void ) { kbd_key_released( 2, 0 ); }
void press_6( void ) { kbd_key_pressed( 2, 1 ); }
void release_6( void ) { kbd_key_released( 2, 1 ); }
void press_5( void ) { kbd_key_pressed( 2, 2 ); }
void release_5( void ) { kbd_key_released( 2, 2 ); }
void press_4( void ) { kbd_key_pressed( 2, 3 ); }
void release_4( void ) { kbd_key_released( 2, 3 ); }
void press_MTH( void ) { kbd_key_pressed( 2, 4 ); }
void release_MTH( void ) { kbd_key_released( 2, 4 ); }
void press_LSHIFT( void ) { kbd_key_pressed( 2, 5 ); }
void release_LSHIFT( void ) { kbd_key_released( 2, 5 ); }
void press_DIV( void ) { kbd_key_pressed( 3, 0 ); }
void release_DIV( void ) { kbd_key_released( 3, 0 ); }
void press_9( void ) { kbd_key_pressed( 3, 1 ); }
void release_9( void ) { kbd_key_released( 3, 1 ); }
void press_8( void ) { kbd_key_pressed( 3, 2 ); }
void release_8( void ) { kbd_key_released( 3, 2 ); }
void press_7( void ) { kbd_key_pressed( 3, 3 ); }
void release_7( void ) { kbd_key_released( 3, 3 ); }
void press_SIN( void ) { kbd_key_pressed( 3, 4 ); }
void release_SIN( void ) { kbd_key_released( 3, 4 ); }
void press_ALPHA( void ) { kbd_key_pressed( 3, 5 ); }
void release_ALPHA( void ) { kbd_key_released( 3, 5 ); }
void press_BKSP( void ) { kbd_key_pressed( 4, 0 ); }
void release_BKSP( void ) { kbd_key_released( 4, 0 ); }
void press_DEL( void ) { kbd_key_pressed( 4, 1 ); }
void release_DEL( void ) { kbd_key_released( 4, 1 ); }
void press_EEX( void ) { kbd_key_pressed( 4, 2 ); }
void release_EEX( void ) { kbd_key_released( 4, 2 ); }
void press_NEG( void ) { kbd_key_pressed( 4, 3 ); }
void release_NEG( void ) { kbd_key_released( 4, 3 ); }
void press_ENTER( void ) { kbd_key_pressed( 4, 4 ); }
void release_ENTER( void ) { kbd_key_released( 4, 4 ); }
void press_INV( void ) { kbd_key_pressed( 5, 0 ); }
void release_INV( void ) { kbd_key_released( 5, 0 ); }
void press_POW( void ) { kbd_key_pressed( 5, 1 ); }
void release_POW( void ) { kbd_key_released( 5, 1 ); }
void press_SQRT( void ) { kbd_key_pressed( 5, 2 ); }
void release_SQRT( void ) { kbd_key_released( 5, 2 ); }
void press_TAN( void ) { kbd_key_pressed( 5, 3 ); }
void release_TAN( void ) { kbd_key_released( 5, 3 ); }
void press_COS( void ) { kbd_key_pressed( 5, 4 ); }
void release_COS( void ) { kbd_key_released( 5, 4 ); }
void press_RIGHT( void ) { kbd_key_pressed( 6, 0 ); }
void release_RIGHT( void ) { kbd_key_released( 6, 0 ); }
void press_DOWN( void ) { kbd_key_pressed( 6, 1 ); }
void release_DOWN( void ) { kbd_key_released( 6, 1 ); }
void press_LEFT( void ) { kbd_key_pressed( 6, 2 ); }
void release_LEFT( void ) { kbd_key_released( 6, 2 ); }
void press_EVAL( void ) { kbd_key_pressed( 6, 3 ); }
void release_EVAL( void ) { kbd_key_released( 6, 3 ); }
void press_STO( void ) { kbd_key_pressed( 6, 4 ); }
void release_STO( void ) { kbd_key_released( 6, 4 ); }
void press_NXT( void ) { kbd_key_pressed( 7, 0 ); }
void release_NXT( void ) { kbd_key_released( 7, 0 ); }
void press_UP( void ) { kbd_key_pressed( 7, 1 ); }
void release_UP( void ) { kbd_key_released( 7, 1 ); }
void press_VAR( void ) { kbd_key_pressed( 7, 2 ); }
void release_VAR( void ) { kbd_key_released( 7, 2 ); }
void press_CST( void ) { kbd_key_pressed( 7, 3 ); }
void release_CST( void ) { kbd_key_released( 7, 3 ); }
void press_PRG( void ) { kbd_key_pressed( 7, 4 ); }
void release_PRG( void ) { kbd_key_released( 7, 4 ); }
void press_F( void ) { kbd_key_pressed( 8, 0 ); }
void release_F( void ) { kbd_key_released( 8, 0 ); }
void press_E( void ) { kbd_key_pressed( 8, 1 ); }
void release_E( void ) { kbd_key_released( 8, 1 ); }
void press_D( void ) { kbd_key_pressed( 8, 2 ); }
void release_D( void ) { kbd_key_released( 8, 2 ); }
void press_C( void ) { kbd_key_pressed( 8, 3 ); }
void release_C( void ) { kbd_key_released( 8, 3 ); }
void press_B( void ) { kbd_key_pressed( 8, 4 ); }
void release_B( void ) { kbd_key_released( 8, 4 ); }
void press_ON( void ) { kbd_on_pressed(); }
void release_ON( void ) { kbd_on_released(); }

void press_Zelda( void ) {}
void release_Zelda( void ) { load_file( "zeldahp.dir" ); }

static Button calc_buttons[] = {
    {0,  pox + xstart + ( xspacing * 0 ),                  ystart + ( 0 * yspacing ) + poy,      pow1,    poh1, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "█",   "",      "",       "A",
     press_A,                                                                                                                                                                                           release_A     },
    {1,  pox + xstart + ( xspacing * 1 ),                  ystart + ( 0 * yspacing ) + poy,      pow1,    poh1, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "█",   "",      "",       "B",
     press_B,                                                                                                                                                                                           release_B     },
    {2,  pox + xstart + ( xspacing * 2 ),                  ystart + ( 0 * yspacing ) + poy,      pow1,    poh1, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "█",   "",      "",       "C",
     press_C,                                                                                                                                                                                           release_C     },
    {3,  pox + xstart + ( xspacing * 3 ),                  ystart + ( 0 * yspacing ) + poy,      pow1,    poh1, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "█",   "",      "",       "D",
     press_D,                                                                                                                                                                                           release_D     },
    {4,  pox + xstart + ( xspacing * 4 ),                  ystart + ( 0 * yspacing ) + poy,      pow1,    poh1, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "█",   "",      "",       "E",
     press_E,                                                                                                                                                                                           release_E     },
    {5,  pox + xstart + ( xspacing * 5 ),                  ystart + ( 0 * yspacing ) + poy,      pow1,    poh1, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "█",   "",      "",       "F",
     press_F,                                                                                                                                                                                           release_F     },

    {6,  pox + xstart + ( xspacing * 0 ),                  ystart + ( 1 * yspacing ) + poy + 10, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "MTH",
     "RAD",                                                                                                                                                           "POLAR",  "G",      press_MTH,    release_MTH   },
    {7,  pox + xstart + ( xspacing * 1 ),                  ystart + ( 1 * yspacing ) + poy + 10, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "PRG",   "",
     "CHARS",                                                                                                                                                                   "H",      press_PRG,    release_PRG   },
    {8,  pox + xstart + ( xspacing * 2 ),                  ystart + ( 1 * yspacing ) + poy + 10, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "CST",   "",
     "MODES",                                                                                                                                                                   "I",      press_CST,    release_CST   },
    {9,  pox + xstart + ( xspacing * 3 ),                  ystart + ( 1 * yspacing ) + poy + 10, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "VAR",   "",
     "MEMORY",                                                                                                                                                                  "J",      press_VAR,    release_VAR   },
    {10, pox + xstart + ( xspacing * 4 ),                  ystart + ( 1 * yspacing ) + poy + 10, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "▲",   "",
     "STACK",                                                                                                                                                                   "K",      press_UP,     release_UP    },
    {11, pox + xstart + ( xspacing * 5 ),                  ystart + ( 1 * yspacing ) + poy + 10, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "NXT",
     "PREV",                                                                                                                                                          "MENU",   "L",      press_NXT,    release_NXT   },

    {12, pox + xstart + ( xspacing * 0 ),                  ystart + ( 2 * yspacing ) + poy + 20, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "'",     "UP",
     "HOME",                                                                                                                                                                    "M",      press_QUOTE,  release_QUOTE },
    {13, pox + xstart + ( xspacing * 1 ),                  ystart + ( 2 * yspacing ) + poy + 20, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "STO",
     "REF",                                                                                                                                                           "RCL",    "N",      press_STO,    release_STO   },
    {14, pox + xstart + ( xspacing * 2 ),                  ystart + ( 2 * yspacing ) + poy + 20, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "EVAL",
     "->NUM",                                                                                                                                                         "UNDO",   "O",      press_EVAL,   release_EVAL  },
    {15, pox + xstart + ( xspacing * 3 ),                  ystart + ( 2 * yspacing ) + poy + 20, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "◀",
     "PICTURE",                                                                                                                                                       "",       "P",      press_LEFT,   release_LEFT  },
    {16, pox + xstart + ( xspacing * 4 ),                  ystart + ( 2 * yspacing ) + poy + 20, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "▼",
     "VIEW",                                                                                                                                                          "",       "Q",      press_DOWN,   release_DOWN  },
    {17, pox + xstart + ( xspacing * 5 ),                  ystart + ( 2 * yspacing ) + poy + 20, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "▶",
     "SWAP",                                                                                                                                                          "",       "R",      press_RIGHT,  release_RIGHT },

    {18, pox + xstart + ( xspacing * 0 ),                  ystart + ( 3 * yspacing ) + poy + 30, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "SIN",
     "ASIN",                                                                                                                                                          "tet",    "S",      press_SIN,    release_SIN   },
    {19, pox + xstart + ( xspacing * 1 ),                  ystart + ( 3 * yspacing ) + poy + 30, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "COS",
     "ACOS",                                                                                                                                                          "",       "T",      press_COS,    release_COS   },
    {20, pox + xstart + ( xspacing * 2 ),                  ystart + ( 3 * yspacing ) + poy + 30, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "TAN",
     "ATAN",                                                                                                                                                          "Sig",    "U",      press_TAN,    release_TAN   },
    {21, pox + xstart + ( xspacing * 3 ),                  ystart + ( 3 * yspacing ) + poy + 30, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "√x",  "xx",
     "x√y",                                                                                                                                                                   "V",      press_SQRT,   release_SQRT  },
    {22, pox + xstart + ( xspacing * 4 ),                  ystart + ( 3 * yspacing ) + poy + 30, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "y^x",
     "⏨x",                                                                                                                                                          "LOG",    "W",      press_POW,    release_POW   },
    {23, pox + xstart + ( xspacing * 5 ),                  ystart + ( 3 * yspacing ) + poy + 30, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "1/x",
     "ex",                                                                                                                                                            "LN",     "X",      press_INV,    release_INV   },

    {24, pox + xstart + ( xspacing * 0 ),                  ystart + ( 4 * yspacing ) + poy + 40, enter_w, poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "ENTER",
     "EQUATION",                                                                                                                                                      "MATRIX", "",       press_ENTER,  release_ENTER },
    {25, enter_w - pow1 + pox + xstart + ( xspacing * 1 ), ystart + ( 4 * yspacing ) + poy + 40, pow1,    poh2,
     BUTTON_B1RELEASE | BUTTON_B2TOGGLE,                                                                                                            "±",    "EDIT",  "CMD",    "Y",      press_NEG,    release_NEG   },
    {26, enter_w - pow1 + pox + xstart + ( xspacing * 2 ), ystart + ( 4 * yspacing ) + poy + 40, pow1,    poh2,
     BUTTON_B1RELEASE | BUTTON_B2TOGGLE,                                                                                                            "EEX",   "PURG",  "ARG",    "Z",      press_EEX,    release_EEX   },
    {27, enter_w - pow1 + pox + xstart + ( xspacing * 3 ), ystart + ( 4 * yspacing ) + poy + 40, pow1,    poh2,
     BUTTON_B1RELEASE | BUTTON_B2TOGGLE,                                                                                                            "DEL",   "CLEAR", "",       "",       press_DEL,    release_DEL   },
    {28, enter_w - pow1 + pox + xstart + ( xspacing * 4 ), ystart + ( 4 * yspacing ) + poy + 40, pow1,    poh2,
     BUTTON_B1RELEASE | BUTTON_B2TOGGLE,                                                                                                            "←",   "DROP",  "",       "",       press_BKSP,   release_BKSP  },

    {29, pox + xstart + ( xspacing * 0 ),                  ystart + ( 5 * yspacing ) + poy + 50, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "α",
     "USER",                                                                                                                                                          "ENTRY",  "",       press_ALPHA,  release_ALPHA },
    {30, pox + xstart + ( xspacing2 * 1 ),                 ystart + ( 5 * yspacing ) + poy + 50, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "7",     "",
     "SOLVE",                                                                                                                                                                   "",       press_7,      release_7     },
    {31, pox + xstart + ( xspacing2 * 2 ),                 ystart + ( 5 * yspacing ) + poy + 50, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "8",     "",
     "PLOT",                                                                                                                                                                    "",       press_8,      release_8     },
    {32, pox + xstart + ( xspacing2 * 3 ),                 ystart + ( 5 * yspacing ) + poy + 50, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "9",     "",
     "SYMBOLIC",                                                                                                                                                                "",       press_9,      release_9     },
    {33, pox + xstart + ( xspacing2 * 4 ),                 ystart + ( 5 * yspacing ) + poy + 50, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "÷",
     "( )",                                                                                                                                                           "#",      "",       press_DIV,    release_DIV   },

    {34, pox + xstart + ( xspacing * 0 ),                  ystart + ( 6 * yspacing ) + poy + 60, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "⮢",   "red",
     "",                                                                                                                                                                        "",       press_LSHIFT, release_LSHIFT},
    {35, pox + xstart + ( xspacing2 * 1 ),                 ystart + ( 6 * yspacing ) + poy + 60, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "4",     "",
     "TIME",                                                                                                                                                                    "",       press_4,      release_4     },
    {36, pox + xstart + ( xspacing2 * 2 ),                 ystart + ( 6 * yspacing ) + poy + 60, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "5",     "",
     "STAT",                                                                                                                                                                    "",       press_5,      release_5     },
    {37, pox + xstart + ( xspacing2 * 3 ),                 ystart + ( 6 * yspacing ) + poy + 60, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "6",     "",
     "UNITS",                                                                                                                                                                   "",       press_6,      release_6     },
    {38, pox + xstart + ( xspacing2 * 4 ),                 ystart + ( 6 * yspacing ) + poy + 60, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "×",
     "[ ]",                                                                                                                                                          "_",      "",       press_MULT,   release_MULT  },

    {39, pox + xstart + ( xspacing * 0 ),                  ystart + ( 7 * yspacing ) + poy + 70, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "⮣",   "",
     "green",                                                                                                                                                                   "",       press_RSHIFT, release_RSHIFT},
    {40, pox + xstart + ( xspacing2 * 1 ),                 ystart + ( 7 * yspacing ) + poy + 70, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "1",     "",
     "I/O",                                                                                                                                                                     "",       press_1,      release_1     },
    {41, pox + xstart + ( xspacing2 * 2 ),                 ystart + ( 7 * yspacing ) + poy + 70, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "2",     "",
     "LIBRARY",                                                                                                                                                                 "",       press_2,      release_2     },
    {42, pox + xstart + ( xspacing2 * 3 ),                 ystart + ( 7 * yspacing ) + poy + 70, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "3",     "",
     "EQ LIB",                                                                                                                                                                  "",       press_3,      release_3     },
    {43, pox + xstart + ( xspacing2 * 4 ),                 ystart + ( 7 * yspacing ) + poy + 70, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "-",
     "<< >>",                                                                                                                                                         "\" \"",  "",       press_MINUS,  release_MINUS },

    {44, pox + xstart + ( xspacing * 0 ),                  ystart + ( 8 * yspacing ) + poy + 80, pow1,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "ON",
     "CONT",                                                                                                                                                          "OFF",    "CANCEL", press_ON,     release_ON    },
    {45, pox + xstart + ( xspacing2 * 1 ),                 ystart + ( 8 * yspacing ) + poy + 80, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "0",     "=",
     "->",                                                                                                                                                                      "",       press_0,      release_0     },
    {46, pox + xstart + ( xspacing2 * 2 ),                 ystart + ( 8 * yspacing ) + poy + 80, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, ".",     ",",
     "back",                                                                                                                                                                    "",       press_PERIOD, release_PERIOD},
    {47, pox + xstart + ( xspacing2 * 3 ),                 ystart + ( 8 * yspacing ) + poy + 80, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "SPC",
     "pi",                                                                                                                                                            "rad",    "",       press_SPC,    release_SPC   },
    {48, pox + xstart + ( xspacing2 * 4 ),                 ystart + ( 8 * yspacing ) + poy + 80, pow2,    poh2, BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "+",     "{}",
     ": :",                                                                                                                                                                     "",       press_PLUS,   release_PLUS  },

    {49, pox + xstart,                                     ystart + ( 9 * yspacing ) + poy + 90, 130,     20,   BUTTON_B1RELEASE | BUTTON_B2TOGGLE, "Game",  "",      "",       "",       press_Zelda,
     release_Zelda                                                                                                                                                                                                    },
    {50, pox + xstart,                                     poy,                                  0,       0,    0,                                  NULL,    NULL,    NULL,     NULL,     NULL,         NULL          }
};

void pcalc_init() { gui_initKeyboard( calc_buttons ); }

void pcalc_show()
{
    // clear_to_color(calc_bmp, color[C_PANEL_BACK]);
    button_draw_all( calc_buttons );
}

void pcalc_down( int mx, int my, int mb ) { button_mouse_down( calc_buttons, mx, my, mb ); }

void pcalc_up( int mx, int my, int mb ) { button_mouse_up( calc_buttons, mx, my, mb ); }
