#include <stdlib.h>

#include "display.h"
#include "keyboard.h"
#include "gui.h"
#include "gui_buttons.h"

const int std_flags = BUTTON_B1RELEASE | BUTTON_B2TOGGLE;

Button calc_buttons[] = {
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
