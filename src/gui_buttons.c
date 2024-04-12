#include <stdlib.h>

#include "color.h"
#include "display.h"
#include "keyboard.h"
#include "gui.h"
#include "gui_buttons.h"
#include "pfiles.h"

const int std_flags = BUTTON_B1RELEASE | BUTTON_B2TOGGLE;

Button calc_buttons[] = {
    {0,  X_COL( 0 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "█",       "",         "",         "A",      press_A,        release_A       },
    {1,  X_COL( 1 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "█",       "",         "",         "B",      press_B,        release_B       },
    {2,  X_COL( 2 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "█",       "",         "",         "C",      press_C,        release_C       },
    {3,  X_COL( 3 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "█",       "",         "",         "D",      press_D,        release_D       },
    {4,  X_COL( 4 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "█",       "",         "",         "E",      press_E,        release_E       },
    {5,  X_COL( 5 ),      Y_LINE( 0 ), UI_K_WIDTH_1,     UI_K_HEIGHT_1, std_flags, "█",       "",         "",         "F",      press_F,        release_F       },

    {6,  X_COL( 0 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "MTH",       "RAD",      "POLAR",    "G",      press_MTH,      release_MTH     },
    {7,  X_COL( 1 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "PRG",       "",         "CHARS",    "H",      press_PRG,      release_PRG     },
    {8,  X_COL( 2 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "CST",       "",         "MODES",    "I",      press_CST,      release_CST     },
    {9,  X_COL( 3 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "VAR",       "",         "MEMORY",   "J",      press_VAR,      release_VAR     },
    {10, X_COL( 4 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "▲",       "",         "STACK",    "K",      press_UP,       release_UP      },
    {11, X_COL( 5 ),      Y_LINE( 1 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "NXT",       "PREV",     "MENU",     "L",      press_NXT,      release_NXT     },

    {12, X_COL( 0 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "'",         "UP",       "HOME",     "M",      press_QUOTE,    release_QUOTE   },
    {13, X_COL( 1 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "STO",       "REF",      "RCL",      "N",      press_STO,      release_STO     },
    {14, X_COL( 2 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "EVAL",      "->NUM",    "UNDO",     "O",      press_EVAL,     release_EVAL    },
    {15, X_COL( 3 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "◀",       "PICTURE",  "",         "P",      press_LEFT,     release_LEFT    },
    {16, X_COL( 4 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "▼",       "VIEW",     "",         "Q",      press_DOWN,     release_DOWN    },
    {17, X_COL( 5 ),      Y_LINE( 2 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "▶",       "SWAP",     "",         "R",      press_RIGHT,    release_RIGHT   },

    {18, X_COL( 0 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "SIN",       "ASIN",     "tet",      "S",      press_SIN,      release_SIN     },
    {19, X_COL( 1 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "COS",       "ACOS",     "",         "T",      press_COS,      release_COS     },
    {20, X_COL( 2 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "TAN",       "ATAN",     "Sig",      "U",      press_TAN,      release_TAN     },
    {21, X_COL( 3 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "√x",      "xx",       "x√y",    "V",      press_SQRT,     release_SQRT    },
    {22, X_COL( 4 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "y^x",       "⏨x",     "LOG",      "W",      press_POW,      release_POW     },
    {23, X_COL( 5 ),      Y_LINE( 3 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "1/x",       "ex",       "LN",       "X",      press_INV,      release_INV     },

    {24, X_COL( 0 ),      Y_LINE( 4 ), UI_K_WIDTH_enter, UI_K_HEIGHT_2, std_flags, "ENTER",     "EQUATION", "MATRIX",   "",       press_ENTER,
     release_ENTER                                                                                                                                                },
    {25, XE_COL( 1 ),     Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "±",        "EDIT",     "CMD",      "Y",      press_NEG,      release_NEG     },
    {26, XE_COL( 2 ),     Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "EEX",       "PURG",     "ARG",      "Z",      press_EEX,      release_EEX     },
    {27, XE_COL( 3 ),     Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "DEL",       "CLEAR",    "",         "",       press_DEL,      release_DEL     },
    {28, XE_COL( 4 ),     Y_LINE( 4 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "←",       "DROP",     "",         "",       press_BKSP,     release_BKSP    },

    {29, X_COL( 0 ),      Y_LINE( 5 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "α",        "USER",     "ENTRY",    "",       press_ALPHA,    release_ALPHA   },
    {30, X2_COL( 1 ),     Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "7",         "",         "SOLVE",    "",       press_7,        release_7       },
    {31, X2_COL( 2 ),     Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "8",         "",         "PLOT",     "",       press_8,        release_8       },
    {32, X2_COL( 3 ),     Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "9",         "",         "SYMBOLIC", "",       press_9,        release_9       },
    {33, X2_COL( 4 ) + 2, Y_LINE( 5 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "÷",        "( )",      "#",        "",       press_DIV,      release_DIV     },

    {34, X_COL( 0 ),      Y_LINE( 6 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "⮢",       "red",      "",         "",       press_LSHIFT,   release_LSHIFT  },
    {35, X2_COL( 1 ),     Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "4",         "",         "TIME",     "",       press_4,        release_4       },
    {36, X2_COL( 2 ),     Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "5",         "",         "STAT",     "",       press_5,        release_5       },
    {37, X2_COL( 3 ),     Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "6",         "",         "UNITS",    "",       press_6,        release_6       },
    {38, X2_COL( 4 ) + 2, Y_LINE( 6 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "×",        "[ ]",     "_",        "",       press_MULT,     release_MULT    },

    {39, X_COL( 0 ),      Y_LINE( 7 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "⮣",       "",         "green",    "",       press_RSHIFT,   release_RSHIFT  },
    {40, X2_COL( 1 ),     Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "1",         "",         "I/O",      "",       press_1,        release_1       },
    {41, X2_COL( 2 ),     Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "2",         "",         "LIBRARY",  "",       press_2,        release_2       },
    {42, X2_COL( 3 ),     Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "3",         "",         "EQ LIB",   "",       press_3,        release_3       },
    {43, X2_COL( 4 ) + 2, Y_LINE( 7 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "-",         "« »",    "\" \"",    "",       press_MINUS,    release_MINUS   },

    {44, X_COL( 0 ),      Y_LINE( 8 ), UI_K_WIDTH_1,     UI_K_HEIGHT_2, std_flags, "ON",        "CONT",     "OFF",      "CANCEL", press_ON,       release_ON      },
    {45, X2_COL( 1 ),     Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "0",         "=",        "->",       "",       press_0,        release_0       },
    {46, X2_COL( 2 ),     Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, ".",         ",",        "back",     "",       press_PERIOD,   release_PERIOD  },
    {47, X2_COL( 3 ),     Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "SPC",       "pi",       "rad",      "",       press_SPC,      release_SPC     },
    {48, X2_COL( 4 ) + 2, Y_LINE( 8 ), UI_K_WIDTH_2,     UI_K_HEIGHT_2, std_flags, "+",         "{}",       ": :",      "",       press_PLUS,     release_PLUS    },

    {49, X_COL( 0 ),      Y_LINE( 9 ), 130,              UI_K_HEIGHT_2, std_flags, "load file", "",         "",         "",       press_LoadFile, release_LoadFile},
};

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

void press_LoadFile( void ) {}
void release_LoadFile( void ) { load_file( "zeldahp.dir" ); }
