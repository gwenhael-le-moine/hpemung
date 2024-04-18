#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"      /* config.ui_* */
#include "keyboard.h"    /* press_key(); release_key() */
#include "emulator.h"    /* for please_exit */
#include "display.h"     /* LCD_HEIGHT; LCD_WIDTH; shouldRender; lcdScreenGS[] */
#include "persistence.h" /* load_file_on_stack(); */

/***********/
/* DEFINES */
/***********/
#define PANEL_FLAG_VISIBLE 0x01

#define UI_PADDING 4
#define UI_KEY_PADDING 4

#define ANNUNC_X UI_PADDING
#define ANNUNC_Y UI_PADDING
#define ANNUNC_HEIGHT 8

#define LCD_X UI_PADDING
#define LCD_Y ( UI_PADDING + ANNUNC_HEIGHT + UI_PADDING )

#define UI_K_WIDTH_1 22
#define UI_K_HEIGHT_1 ( UI_K_WIDTH_1 * 0.65 )
#define UI_K_WIDTH_2 26
#define UI_K_HEIGHT_2 ( UI_K_WIDTH_2 * 0.65 )

#define UI_KB_OFFSET_Y ( UI_PADDING + ANNUNC_HEIGHT + LCD_HEIGHT + UI_PADDING )

#define UI_K_WIDTH_enter ( UI_K_WIDTH_1 * 2 )

#define Y_LINE( i ) ( UI_KB_OFFSET_Y + ( i * UI_K_HEIGHT_2 ) )
#define X_COL( i ) ( UI_PADDING + ( UI_K_WIDTH_1 * i ) )
#define X2_COL( i ) ( UI_PADDING + ( UI_K_WIDTH_2 * i ) )

#define UI_KB_HEIGHT ( config.ui_scale * Y_LINE( 9 ) )

/* Button flags:
 * Use BUTTON_B1RELEASE for normal buttons.
 * Use BUTTON_B1RELEASE | BUTTON_B2TOGGLE for calculator buttons.
 * Use BUTTON_B1TOGGLE for toggle buttons
 */
// Set if button is pushed
#define BUTTON_PUSHED 0x01
// If set the button will be grayed out
#define BUTTON_DISABLED 0x02
// Mouse button 1 toggles this button
#define BUTTON_B1TOGGLE 0x04
// Mouse button 2 toggles this button
#define BUTTON_B2TOGGLE 0x08
// Releaseing mouse button 1 anywhere unpushes the button
#define BUTTON_B1RELEASE 0x10

/***********/
/* TYPEDEF */
/***********/
typedef struct {
    int index;
    int x, y;
    int w, h;
    int flags;
    char* label;
    char* label_Lshift;
    char* label_Rshift;
    char* label_letter;
    char* label_below;
    int hpkey;
} Button;

typedef struct {
    SDL_Color faceplate;
    SDL_Color button_bg;
    SDL_Color button_active;
    SDL_Color button_inactive;
    SDL_Color label;
    SDL_Color Lshift;
    SDL_Color Rshift;
    SDL_Color letter;
    SDL_Color below;
} colors_t;

/*************/
/* VARIABLES */
/*************/
static TTF_Font* ttffont = NULL;
static TTF_Font* ttffont2 = NULL;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* window_texture = NULL;

SDL_Surface* surfaces_labels[ NB_KEYS ];
SDL_Texture* textures_labels[ NB_KEYS ];

SDL_Surface* surfaces_labels_Lshift[ NB_KEYS ];
SDL_Texture* textures_labels_Lshift[ NB_KEYS ];

SDL_Surface* surfaces_labels_Rshift[ NB_KEYS ];
SDL_Texture* textures_labels_Rshift[ NB_KEYS ];

SDL_Surface* surfaces_labels_below[ NB_KEYS ];
SDL_Texture* textures_labels_below[ NB_KEYS ];

SDL_Surface* surfaces_labels_letter[ NB_KEYS ];
SDL_Texture* textures_labels_letter[ NB_KEYS ];

static const int std_flags = BUTTON_B1RELEASE | BUTTON_B2TOGGLE;

static Button gui_buttons[ NB_KEYS ] = {
    /* line 0 */
    {.index = 0,
     .x = X_COL( 0 ),
     .y = Y_LINE( 0 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_1,
     .flags = std_flags,
     .label = "",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "A",
     .label_below = "",
     .hpkey = HPKEY_A     },
    {.index = 1,
     .x = X_COL( 1 ),
     .y = Y_LINE( 0 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_1,
     .flags = std_flags,
     .label = "",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "B",
     .label_below = "",
     .hpkey = HPKEY_B     },
    {.index = 2,
     .x = X_COL( 2 ),
     .y = Y_LINE( 0 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_1,
     .flags = std_flags,
     .label = "",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "C",
     .label_below = "",
     .hpkey = HPKEY_C     },
    {.index = 3,
     .x = X_COL( 3 ),
     .y = Y_LINE( 0 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_1,
     .flags = std_flags,
     .label = "",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "D",
     .label_below = "",
     .hpkey = HPKEY_D     },
    {.index = 4,
     .x = X_COL( 4 ),
     .y = Y_LINE( 0 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_1,
     .flags = std_flags,
     .label = "",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "E",
     .label_below = "",
     .hpkey = HPKEY_E     },
    {.index = 5,
     .x = X_COL( 5 ),
     .y = Y_LINE( 0 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_1,
     .flags = std_flags,
     .label = "",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "F",
     .label_below = "",
     .hpkey = HPKEY_F     },
    /* line 1 */
    {.index = 6,
     .x = X_COL( 0 ),
     .y = Y_LINE( 1 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "MTH",
     .label_Lshift = "RAD",
     .label_Rshift = "POLAR",
     .label_letter = "G",
     .label_below = "",
     .hpkey = HPKEY_MTH   },
    {.index = 7,
     .x = X_COL( 1 ),
     .y = Y_LINE( 1 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "PRG",
     .label_Lshift = "",
     .label_Rshift = "CHARS",
     .label_letter = "H",
     .label_below = "",
     .hpkey = HPKEY_PRG   },
    {.index = 8,
     .x = X_COL( 2 ),
     .y = Y_LINE( 1 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "CST",
     .label_Lshift = "",
     .label_Rshift = "MODES",
     .label_letter = "I",
     .label_below = "",
     .hpkey = HPKEY_CST   },
    {.index = 9,
     .x = X_COL( 3 ),
     .y = Y_LINE( 1 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "VAR",
     .label_Lshift = "",
     .label_Rshift = "MEMORY",
     .label_letter = "J",
     .label_below = "",
     .hpkey = HPKEY_VAR   },
    {.index = 10,
     .x = X_COL( 4 ),
     .y = Y_LINE( 1 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "▲",
     .label_Lshift = "",
     .label_Rshift = "STACK",
     .label_letter = "K",
     .label_below = "",
     .hpkey = HPKEY_UP    },
    {.index = 11,
     .x = X_COL( 5 ),
     .y = Y_LINE( 1 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "NXT",
     .label_Lshift = "PREV",
     .label_Rshift = "MENU",
     .label_letter = "L",
     .label_below = "",
     .hpkey = HPKEY_NXT   },
    /* line 2 */
    {.index = 12,
     .x = X_COL( 0 ),
     .y = Y_LINE( 2 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "'",
     .label_Lshift = "UP",
     .label_Rshift = "HOME",
     .label_letter = "M",
     .label_below = "",
     .hpkey = HPKEY_QUOTE },
    {.index = 13,
     .x = X_COL( 1 ),
     .y = Y_LINE( 2 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "STO",
     .label_Lshift = "REF",
     .label_Rshift = "RCL",
     .label_letter = "N",
     .label_below = "",
     .hpkey = HPKEY_STO   },
    {.index = 14,
     .x = X_COL( 2 ),
     .y = Y_LINE( 2 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "EVAL",
     .label_Lshift = "→NUM",
     .label_Rshift = "UNDO",
     .label_letter = "O",
     .label_below = "",
     .hpkey = HPKEY_EVAL  },
    {.index = 15,
     .x = X_COL( 3 ),
     .y = Y_LINE( 2 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "◀",
     .label_Lshift = "",
     .label_Rshift = "PICTURE",
     .label_letter = "P",
     .label_below = "",
     .hpkey = HPKEY_LEFT  },
    {.index = 16,
     .x = X_COL( 4 ),
     .y = Y_LINE( 2 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "▼",
     .label_Lshift = "",
     .label_Rshift = "VIEW",
     .label_letter = "Q",
     .label_below = "",
     .hpkey = HPKEY_DOWN  },
    {.index = 17,
     .x = X_COL( 5 ),
     .y = Y_LINE( 2 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "▶",
     .label_Lshift = "",
     .label_Rshift = "SWAP",
     .label_letter = "R",
     .label_below = "",
     .hpkey = HPKEY_RIGHT },
    /* line 3 */
    {.index = 18,
     .x = X_COL( 0 ),
     .y = Y_LINE( 3 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "SIN",
     .label_Lshift = "ASIN",
     .label_Rshift = "\u2202",
     .label_letter = "S",
     .label_below = "",
     .hpkey = HPKEY_SIN   },
    {.index = 19,
     .x = X_COL( 1 ),
     .y = Y_LINE( 3 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "COS",
     .label_Lshift = "ACOS",
     .label_Rshift = "∫",
     .label_letter = "T",
     .label_below = "",
     .hpkey = HPKEY_COS   },
    {.index = 20,
     .x = X_COL( 2 ),
     .y = Y_LINE( 3 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "TAN",
     .label_Lshift = "ATAN",
     .label_Rshift = "\u03a3",
     .label_letter = "U",
     .label_below = "",
     .hpkey = HPKEY_TAN   },
    {.index = 21,
     .x = X_COL( 3 ),
     .y = Y_LINE( 3 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "√x",
     .label_Lshift = "x²",
     .label_Rshift = "x√y",
     .label_letter = "V",
     .label_below = "",
     .hpkey = HPKEY_SQRT  },
    {.index = 22,
     .x = X_COL( 4 ),
     .y = Y_LINE( 3 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "y\u02e3",
     .label_Lshift = "\u23e8\u02e3",
     .label_Rshift = "LOG",
     .label_letter = "W",
     .label_below = "",
     .hpkey = HPKEY_POWER },
    {.index = 23,
     .x = X_COL( 5 ),
     .y = Y_LINE( 3 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "1/x",
     .label_Lshift = "e\u02e3",
     .label_Rshift = "LN",
     .label_letter = "X",
     .label_below = "",
     .hpkey = HPKEY_INV   },
    /* line 4 */
    {.index = 24,
     .x = X_COL( 0 ),
     .y = Y_LINE( 4 ),
     .w = UI_K_WIDTH_enter,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "ENTER",
     .label_Lshift = "EQUATION",
     .label_Rshift = "MATRIX",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_ENTER },
    {.index = 25,
     .x = X_COL( 2 ),
     .y = Y_LINE( 4 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "+/-",
     .label_Lshift = "EDIT",
     .label_Rshift = "CMD",
     .label_letter = "Y",
     .label_below = "",
     .hpkey = HPKEY_NEG   },
    {.index = 26,
     .x = X_COL( 3 ),
     .y = Y_LINE( 4 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "EEX",
     .label_Lshift = "PURG",
     .label_Rshift = "ARG",
     .label_letter = "Z",
     .label_below = "",
     .hpkey = HPKEY_EEX   },
    {.index = 27,
     .x = X_COL( 4 ),
     .y = Y_LINE( 4 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "DEL",
     .label_Lshift = "",
     .label_Rshift = "CLEAR",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_DEL   },
    {.index = 28,
     .x = X_COL( 5 ),
     .y = Y_LINE( 4 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "←",
     .label_Lshift = "",
     .label_Rshift = "DROP",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_BS    },
    /* line 5 */
    {.index = 29,
     .x = X_COL( 0 ),
     .y = Y_LINE( 5 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "α",
     .label_Lshift = "USER",
     .label_Rshift = "ENTRY",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_ALPHA },
    {.index = 30,
     .x = X2_COL( 1 ),
     .y = Y_LINE( 5 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "7",
     .label_Lshift = "",
     .label_Rshift = "SOLVE",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_7     },
    {.index = 31,
     .x = X2_COL( 2 ),
     .y = Y_LINE( 5 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "8",
     .label_Lshift = "",
     .label_Rshift = "PLOT",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_8     },
    {.index = 32,
     .x = X2_COL( 3 ),
     .y = Y_LINE( 5 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "9",
     .label_Lshift = "",
     .label_Rshift = "SYMBOLIC",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_9     },
    {.index = 33,
     .x = X2_COL( 4 ) + 2,
     .y = Y_LINE( 5 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "÷",
     .label_Lshift = "( )",
     .label_Rshift = "#",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_DIV   },
    /* line 6 */
    {.index = 34,
     .x = X_COL( 0 ),
     .y = Y_LINE( 6 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "\u2ba2",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_SHL   },
    {.index = 35,
     .x = X2_COL( 1 ),
     .y = Y_LINE( 6 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "4",
     .label_Lshift = "",
     .label_Rshift = "TIME",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_4     },
    {.index = 36,
     .x = X2_COL( 2 ),
     .y = Y_LINE( 6 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "5",
     .label_Lshift = "",
     .label_Rshift = "STAT",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_5     },
    {.index = 37,
     .x = X2_COL( 3 ),
     .y = Y_LINE( 6 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "6",
     .label_Lshift = "",
     .label_Rshift = "UNITS",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_6     },
    {.index = 38,
     .x = X2_COL( 4 ) + 2,
     .y = Y_LINE( 6 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "×",
     .label_Lshift = "[ ]",
     .label_Rshift = "_",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_MUL   },
    /* line 7 */
    {.index = 39,
     .x = X_COL( 0 ),
     .y = Y_LINE( 7 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "\u2ba3",
     .label_Lshift = "",
     .label_Rshift = "",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_SHR   },
    {.index = 40,
     .x = X2_COL( 1 ),
     .y = Y_LINE( 7 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "1",
     .label_Lshift = "",
     .label_Rshift = "I/O",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_1     },
    {.index = 41,
     .x = X2_COL( 2 ),
     .y = Y_LINE( 7 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "2",
     .label_Lshift = "",
     .label_Rshift = "LIBRARY",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_2     },
    {.index = 42,
     .x = X2_COL( 3 ),
     .y = Y_LINE( 7 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "3",
     .label_Lshift = "",
     .label_Rshift = "EQ LIB",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_3     },
    {.index = 43,
     .x = X2_COL( 4 ) + 2,
     .y = Y_LINE( 7 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "-",
     .label_Lshift = "« »",
     .label_Rshift = "\" \"",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_MINUS },
    /* line 8 */
    {.index = 44,
     .x = X_COL( 0 ),
     .y = Y_LINE( 8 ),
     .w = UI_K_WIDTH_1,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "ON",
     .label_Lshift = "CONT",
     .label_Rshift = "OFF",
     .label_letter = "",
     .label_below = "CANCEL",
     .hpkey = HPKEY_ON    },
    {.index = 45,
     .x = X2_COL( 1 ),
     .y = Y_LINE( 8 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "0",
     .label_Lshift = "=",
     .label_Rshift = "\u2192",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_0     },
    {.index = 46,
     .x = X2_COL( 2 ),
     .y = Y_LINE( 8 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = ".",
     .label_Lshift = ",",
     .label_Rshift = "\u2ba0",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_PERIOD},
    {.index = 47,
     .x = X2_COL( 3 ),
     .y = Y_LINE( 8 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "SPC",
     .label_Lshift = "\u03c0",
     .label_Rshift = "\u29a8",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_SPC   },
    {.index = 48,
     .x = X2_COL( 4 ) + 2,
     .y = Y_LINE( 8 ),
     .w = UI_K_WIDTH_2,
     .h = UI_K_HEIGHT_2,
     .flags = std_flags,
     .label = "+",
     .label_Lshift = "{ }",
     .label_Rshift = ": :",
     .label_letter = "",
     .label_below = "",
     .hpkey = HPKEY_PLUS  },
};

static colors_t gui_colors = {
    .faceplate = {.r = 48,  .g = 68,  .b = 90,  .a = 255},
    .button_bg = {.r = 16,  .g = 26,  .b = 39,  .a = 33 },
    .button_active = {.r = 255, .g = 255, .b = 39,  .a = 33 },
    .button_inactive = {.r = 0,   .g = 0,   .b = 0,   .a = 255},
    .label = {.r = 255, .g = 255, .b = 255, .a = 255},
    .Lshift = {.r = 191, .g = 192, .b = 236, .a = 255},
    .Rshift = {.r = 125, .g = 215, .b = 235, .a = 255},
    .letter = {.r = 255, .g = 255, .b = 255, .a = 255},
    .below = {.r = 128, .g = 108, .b = 29,  .a = 255},
};
static SDL_Color pixels_colors[] = {
    {.r = 119, .g = 153, .b = 136, .a = 255},
    {.r = 71,  .g = 134, .b = 145, .a = 255},
    {.r = 13,  .g = 108, .b = 111, .a = 255},
    {.r = 37,  .g = 61,  .b = 84,  .a = 255},
};

/*************/
/* FUNCTIONS */
/*************/
static inline bool _init_keyboard_textures()
{
    SDL_Surface* s = NULL;
    SDL_Texture* t = NULL;

    if ( ttffont == NULL ) {
        printf( "init texts error Font NULL\n" );
        return false;
    }

    for ( int i = 0; i < NB_KEYS; ++i ) {
        s = NULL;
        t = NULL;
        if ( gui_buttons[ i ].label && strcmp( gui_buttons[ i ].label, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont, gui_buttons[ i ].label, gui_colors.label );
            if ( s )
                t = SDL_CreateTextureFromSurface( renderer, s );
        }
        surfaces_labels[ i ] = s;
        textures_labels[ i ] = t;

        s = NULL;
        t = NULL;
        if ( gui_buttons[ i ].label_Lshift && strcmp( gui_buttons[ i ].label_Lshift, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, gui_buttons[ i ].label_Lshift, gui_colors.Lshift );
            if ( s )
                t = SDL_CreateTextureFromSurface( renderer, s );
        }
        surfaces_labels_Lshift[ i ] = s;
        textures_labels_Lshift[ i ] = t;

        s = NULL;
        t = NULL;
        if ( gui_buttons[ i ].label_Rshift && strcmp( gui_buttons[ i ].label_Rshift, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, gui_buttons[ i ].label_Rshift, gui_colors.Rshift );
            if ( s )
                t = SDL_CreateTextureFromSurface( renderer, s );
        }
        surfaces_labels_Rshift[ i ] = s;
        textures_labels_Rshift[ i ] = t;

        s = NULL;
        t = NULL;
        if ( gui_buttons[ i ].label_below && strcmp( gui_buttons[ i ].label_below, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, gui_buttons[ i ].label_below, gui_colors.below );
            if ( s )
                t = SDL_CreateTextureFromSurface( renderer, s );
        }
        surfaces_labels_below[ i ] = s;
        textures_labels_below[ i ] = t;

        s = NULL;
        t = NULL;
        if ( gui_buttons[ i ].label_letter && strcmp( gui_buttons[ i ].label_letter, "" ) != 0 ) {
            s = TTF_RenderUTF8_Blended( ttffont2, gui_buttons[ i ].label_letter, gui_colors.letter );
            if ( s )
                t = SDL_CreateTextureFromSurface( renderer, s );
        }
        surfaces_labels_letter[ i ] = s;
        textures_labels_letter[ i ] = t;
    }

    return true;
}

static inline void _draw_key_labels( Button b )
{
    int texW;
    int texH;
    int h_padding = 3;

    SDL_Surface* surface_label = surfaces_labels[ b.index ];
    SDL_Texture* texture_label = textures_labels[ b.index ];
    if ( surface_label != NULL && texture_label != NULL ) {
        texW = surface_label->w / config.ui_scale;
        texH = surface_label->h / config.ui_scale;
        SDL_Rect destRect = { config.ui_scale * ( b.x + ( b.w - texW ) / 2 ), config.ui_scale * ( b.y + ( b.h / 3 ) ),
                              config.ui_scale * texW, config.ui_scale * texH };
        SDL_RenderCopy( renderer, texture_label, NULL, &destRect );
    }

    SDL_Surface* surface_label_Lshift = surfaces_labels_Lshift[ b.index ];
    SDL_Texture* texture_label_Lshift = textures_labels_Lshift[ b.index ];
    if ( surface_label_Lshift != NULL && texture_label_Lshift != NULL ) {
        texW = surface_label_Lshift->w / config.ui_scale;
        texH = surface_label_Lshift->h / config.ui_scale;
        SDL_Rect destRect = { config.ui_scale * ( b.x + h_padding ), config.ui_scale * b.y, config.ui_scale * texW,
                              config.ui_scale * texH };
        SDL_RenderCopy( renderer, texture_label_Lshift, NULL, &destRect );
    }

    SDL_Surface* surface_label_Rshift = surfaces_labels_Rshift[ b.index ];
    SDL_Texture* texture_label_Rshift = textures_labels_Rshift[ b.index ];
    if ( surface_label_Rshift != NULL && texture_label_Rshift != NULL ) {
        texW = surface_label_Rshift->w / config.ui_scale;
        texH = surface_label_Rshift->h / config.ui_scale;
        SDL_Rect destRect = { config.ui_scale * ( ( b.x + b.w ) - ( texW + h_padding ) ), config.ui_scale * b.y, config.ui_scale * texW,
                              config.ui_scale * texH };
        if ( surface_label_Lshift == NULL )
            destRect.x = config.ui_scale * ( b.x + ( ( b.w / 2 ) - ( texW / 2 ) ) );
        SDL_RenderCopy( renderer, texture_label_Rshift, NULL, &destRect );
    }

    SDL_Surface* surface_label_letter = surfaces_labels_letter[ b.index ];
    SDL_Texture* texture_label_letter = textures_labels_letter[ b.index ];
    if ( surface_label_letter != NULL && texture_label_letter != NULL ) {
        texW = surface_label_letter->w / config.ui_scale;
        texH = surface_label_letter->h / config.ui_scale;
        SDL_Rect destRect = { config.ui_scale * ( ( b.x + b.w ) - ( texW / 2 ) ), config.ui_scale * ( b.y + ( b.h - 5 ) ),
                              config.ui_scale * texW, config.ui_scale * texH };
        SDL_RenderCopy( renderer, texture_label_letter, NULL, &destRect );
    }

    SDL_Surface* surface_label_below = surfaces_labels_below[ b.index ];
    SDL_Texture* texture_label_below = textures_labels_below[ b.index ];
    if ( surface_label_below != NULL && texture_label_below != NULL ) {
        texW = surface_label_below->w / config.ui_scale;
        texH = surface_label_below->h / config.ui_scale;
        SDL_Rect destRect = { config.ui_scale * ( b.x + ( b.w - texW ) / 2 ), config.ui_scale * ( b.y + ( b.h - 3 ) ),
                              config.ui_scale * texW, config.ui_scale * texH };
        SDL_RenderCopy( renderer, texture_label_below, NULL, &destRect );
    }
}

static inline void _draw_key( Button b )
{
    SDL_Rect rectToDraw = { ( b.x + ( UI_KEY_PADDING / 2 ) ) * config.ui_scale, ( b.y + ( UI_KEY_PADDING * 1.25 ) ) * config.ui_scale,
                            ( b.w - UI_KEY_PADDING ) * config.ui_scale, ( b.h - ( UI_KEY_PADDING * 2 ) ) * config.ui_scale };

    if ( b.index < 6 )
        SDL_SetRenderDrawColor( renderer, gui_colors.label.r, gui_colors.label.g, gui_colors.label.g, gui_colors.label.a );
    else if ( b.index == 34 )
        SDL_SetRenderDrawColor( renderer, gui_colors.Lshift.r, gui_colors.Lshift.g, gui_colors.Lshift.g, gui_colors.Lshift.a );
    else if ( b.index == 39 )
        SDL_SetRenderDrawColor( renderer, gui_colors.Rshift.r, gui_colors.Rshift.g, gui_colors.Rshift.g, gui_colors.Rshift.a );
    else
        SDL_SetRenderDrawColor( renderer, gui_colors.button_bg.r, gui_colors.button_bg.g, gui_colors.button_bg.g, gui_colors.button_bg.a );
    SDL_RenderFillRect( renderer, &rectToDraw );

    if ( b.flags & BUTTON_PUSHED )
        SDL_SetRenderDrawColor( renderer, gui_colors.button_active.r, gui_colors.button_active.g, gui_colors.button_active.b,
                                gui_colors.button_active.a );
    else
        SDL_SetRenderDrawColor( renderer, gui_colors.button_inactive.r, gui_colors.button_inactive.g, gui_colors.button_inactive.b,
                                gui_colors.button_inactive.a );

    SDL_RenderDrawRect( renderer, &rectToDraw );

    _draw_key_labels( b );
}

static inline void _draw_keyboard()
{
    for ( int i = 0; i < NB_KEYS; ++i )
        _draw_key( gui_buttons[ i ] );
}

static inline int _find_button( int x, int y )
{
    for ( int i = 0; i < NB_KEYS; ++i )
        if ( x >= gui_buttons[ i ].x * config.ui_scale && x < gui_buttons[ i ].x * config.ui_scale + gui_buttons[ i ].w * config.ui_scale &&
             y >= gui_buttons[ i ].y * config.ui_scale &&
             y < gui_buttons[ i ].y * config.ui_scale + gui_buttons[ i ].h * config.ui_scale ) {
            if ( gui_buttons[ i ].flags & BUTTON_DISABLED )
                return -1;
            else
                return i;
        }

    return -1;
}

static inline void _button_mouse_down( int mouse_x, int mouse_y, int mouse_button )
{
    int bindex = _find_button( mouse_x, mouse_y );
    if ( bindex == -1 )
        return;

    if ( ( mouse_button == 2 && ( gui_buttons[ bindex ].flags & BUTTON_B2TOGGLE ) ) ||
         ( mouse_button == 1 && ( gui_buttons[ bindex ].flags & BUTTON_B1TOGGLE ) ) ) {
        fprintf( stderr, "Toggle mouse_button %i\n", mouse_button );

        if ( gui_buttons[ bindex ].flags & BUTTON_PUSHED ) {
            gui_buttons[ bindex ].flags &= ~BUTTON_PUSHED;

            release_key( gui_buttons[ bindex ].hpkey );
        } else {
            gui_buttons[ bindex ].flags |= BUTTON_PUSHED;

            press_key( gui_buttons[ bindex ].hpkey );
        }
    } else if ( mouse_button == 1 && !( gui_buttons[ bindex ].flags & BUTTON_PUSHED ) ) {
        gui_buttons[ bindex ].flags |= BUTTON_PUSHED;

        press_key( gui_buttons[ bindex ].hpkey );
    }
}

static inline void _button_mouse_up( int mouse_x, int mouse_y, int mouse_button )
{
    int bindex = _find_button( mouse_x, mouse_y );
    if ( bindex == -1 )
        return;

    if ( !( gui_buttons[ bindex ].flags & BUTTON_DISABLED ) ) {
        if ( mouse_button == 1 && ( gui_buttons[ bindex ].flags & BUTTON_PUSHED ) && !( gui_buttons[ bindex ].flags & BUTTON_B1TOGGLE ) ) {
            gui_buttons[ bindex ].flags &= ~BUTTON_PUSHED;

            press_key( gui_buttons[ bindex ].hpkey );
        }
    }
    if ( mouse_button == 1 ) {
        /* for ( b = buttons; gui_buttons[ bindex ].label; b++ ) { */
        if ( ( gui_buttons[ bindex ].flags & ( BUTTON_B1RELEASE | BUTTON_PUSHED ) ) == ( BUTTON_B1RELEASE | BUTTON_PUSHED ) ) {
            gui_buttons[ bindex ].flags &= ~BUTTON_PUSHED;

            release_key( gui_buttons[ bindex ].hpkey );
        }
        /* } */
    }
}

/********************/
/* PUBLIC FUNCTIONS */
/********************/
void gui_refresh()
{
    SDL_SetRenderDrawColor( renderer, gui_colors.faceplate.r, gui_colors.faceplate.g, gui_colors.faceplate.b, gui_colors.faceplate.a );
    SDL_RenderClear( renderer );

    if ( shouldRender ) {
        shouldRender = false;

        int pitch, w, h;
        Uint32* pixels;
        int access;
        Uint32 format;
        byte pixel;
        Uint32 color;
        Uint32 pixelPosition;

        if ( SDL_QueryTexture( window_texture, &format, &access, &w, &h ) ) {
            printf( "SDL_QueryTexture: %s.\n", SDL_GetError() );
            please_exit = true;
        }

        if ( SDL_LockTexture( window_texture, NULL, ( void** )&pixels, &pitch ) ) {
            printf( "SDL_LockTexture: %s.\n", SDL_GetError() );
            please_exit = true;
        }

        SDL_PixelFormat* pixelFormat = SDL_AllocFormat( format );

        // do stuff
        for ( int y = 0; y < LCD_HEIGHT; y++ ) {
            for ( int x = 0; x < LCD_WIDTH; x++ ) {
                pixel = lcdScreenGS[ x + y * LCD_WIDTH ];

                // Now you want to format the color to a correct format that SDL
                // can use. Basically we convert our RGB color to a hex-like BGR
                // color.
                color = SDL_MapRGB( pixelFormat, pixels_colors[ pixel ].r, pixels_colors[ pixel ].g, pixels_colors[ pixel ].b );

                // Before setting the color, we need to know where we have to
                // place it.
                pixelPosition = y * ( pitch / sizeof( unsigned int ) ) + x;

                pixels[ pixelPosition ] = color;
            }
        }

        SDL_UnlockTexture( window_texture );
    }

    // Show rendered to texture
    SDL_Rect r1 = { 0, 0, LCD_WIDTH, LCD_HEIGHT };
    SDL_Rect r2 = { LCD_X * config.ui_scale, LCD_Y * config.ui_scale, LCD_WIDTH * config.ui_scale, LCD_HEIGHT * config.ui_scale };
    SDL_RenderCopyEx( renderer, window_texture, &r1, &r2, 0, NULL, SDL_FLIP_NONE );

    _draw_keyboard();

    SDL_RenderPresent( renderer );
}

bool gui_events()
{
    SDL_Event event;

    while ( SDL_PollEvent( &event ) ) {
        switch ( event.type ) {
            case SDL_MOUSEBUTTONUP:
                _button_mouse_up( event.button.x, event.button.y, 1 );
                break;

            case SDL_MOUSEBUTTONDOWN:
                _button_mouse_down( event.button.x, event.button.y, 1 );
                break;

            case SDL_KEYDOWN:
                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_KP_0:
                    case SDL_SCANCODE_0:
                        press_key( HPKEY_0 );
                        break;
                    case SDL_SCANCODE_KP_1:
                    case SDL_SCANCODE_1:
                        press_key( HPKEY_1 );
                        break;
                    case SDL_SCANCODE_KP_2:
                    case SDL_SCANCODE_2:
                        press_key( HPKEY_2 );
                        break;
                    case SDL_SCANCODE_KP_3:
                    case SDL_SCANCODE_3:
                        press_key( HPKEY_3 );
                        break;
                    case SDL_SCANCODE_KP_4:
                    case SDL_SCANCODE_4:
                        press_key( HPKEY_4 );
                        break;
                    case SDL_SCANCODE_KP_5:
                    case SDL_SCANCODE_5:
                        press_key( HPKEY_5 );
                        break;
                    case SDL_SCANCODE_KP_6:
                    case SDL_SCANCODE_6:
                        press_key( HPKEY_6 );
                        break;
                    case SDL_SCANCODE_KP_7:
                    case SDL_SCANCODE_7:
                        press_key( HPKEY_7 );
                        break;
                    case SDL_SCANCODE_KP_8:
                    case SDL_SCANCODE_8:
                        press_key( HPKEY_8 );
                        break;
                    case SDL_SCANCODE_KP_9:
                    case SDL_SCANCODE_9:
                        press_key( HPKEY_9 );
                        break;
                    case SDL_SCANCODE_KP_PERIOD:
                        press_key( HPKEY_PERIOD );
                        break;
                    case SDL_SCANCODE_SPACE:
                        press_key( HPKEY_SPC );
                        break;
                    case SDL_SCANCODE_ESCAPE:
                    case SDL_SCANCODE_F5:
                        press_key( HPKEY_ON );
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                    case SDL_SCANCODE_F1:
                        press_key( HPKEY_ENTER );
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        press_key( HPKEY_BS );
                        break;
                    case SDL_SCANCODE_KP_PLUS:
                        press_key( HPKEY_PLUS );
                        break;
                    case SDL_SCANCODE_KP_MINUS:
                        press_key( HPKEY_MINUS );
                        break;
                    case SDL_SCANCODE_KP_MULTIPLY:
                        press_key( HPKEY_MUL );
                        break;
                    case SDL_SCANCODE_KP_DIVIDE:
                        press_key( HPKEY_DIV );
                        break;
                    case SDL_SCANCODE_A:
                        press_key( HPKEY_A );
                        break;
                    case SDL_SCANCODE_B:
                        press_key( HPKEY_B );
                        break;
                    case SDL_SCANCODE_C:
                        press_key( HPKEY_C );
                        break;
                    case SDL_SCANCODE_D:
                        press_key( HPKEY_D );
                        break;
                    case SDL_SCANCODE_E:
                        press_key( HPKEY_E );
                        break;
                    case SDL_SCANCODE_F:
                        press_key( HPKEY_F );
                        break;
                    case SDL_SCANCODE_G:
                        press_key( HPKEY_MTH );
                        break;
                    case SDL_SCANCODE_H:
                        press_key( HPKEY_PRG );
                        break;
                    case SDL_SCANCODE_I:
                        press_key( HPKEY_CST );
                        break;
                    case SDL_SCANCODE_J:
                        press_key( HPKEY_VAR );
                        break;
                    case SDL_SCANCODE_K:
                    case SDL_SCANCODE_UP:
                        press_key( HPKEY_UP );
                        break;
                    case SDL_SCANCODE_L:
                        press_key( HPKEY_NXT );
                        break;
                    case SDL_SCANCODE_M:
                        press_key( HPKEY_QUOTE );
                        break;
                    case SDL_SCANCODE_N:
                        press_key( HPKEY_STO );
                        break;
                    case SDL_SCANCODE_O:
                        press_key( HPKEY_EVAL );
                        break;
                    case SDL_SCANCODE_P:
                    case SDL_SCANCODE_LEFT:
                        press_key( HPKEY_LEFT );
                        break;
                    case SDL_SCANCODE_Q:
                    case SDL_SCANCODE_DOWN:
                        press_key( HPKEY_DOWN );
                        break;
                    case SDL_SCANCODE_R:
                    case SDL_SCANCODE_RIGHT:
                        press_key( HPKEY_RIGHT );
                        break;
                    case SDL_SCANCODE_S:
                        press_key( HPKEY_SIN );
                        break;
                    case SDL_SCANCODE_T:
                        press_key( HPKEY_COS );
                        break;
                    case SDL_SCANCODE_U:
                        press_key( HPKEY_TAN );
                        break;
                    case SDL_SCANCODE_V:
                        press_key( HPKEY_SQRT );
                        break;
                    case SDL_SCANCODE_W:
                        press_key( HPKEY_POWER );
                        break;
                    case SDL_SCANCODE_X:
                        press_key( HPKEY_INV );
                        break;
                    case SDL_SCANCODE_Y:
                        press_key( HPKEY_NEG );
                        break;
                    case SDL_SCANCODE_Z:
                        press_key( HPKEY_EEX );
                        break;
                    case SDL_SCANCODE_F2:
                        press_key( HPKEY_SHL );
                        break;
                    case SDL_SCANCODE_F3:
                        press_key( HPKEY_SHR );
                        break;
                    case SDL_SCANCODE_F4:
                        press_key( HPKEY_ALPHA );
                        break;
                    case SDL_SCANCODE_F7:
                        please_exit = true;
                        break;

                    case SDL_SCANCODE_F11:
                        {
                            load_file_on_stack( "zeldahp.dir" );
                        }
                        break;

                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                switch ( event.key.keysym.scancode ) {
                    case SDL_SCANCODE_KP_0:
                    case SDL_SCANCODE_0:
                        release_key( HPKEY_0 );
                        break;
                    case SDL_SCANCODE_KP_1:
                    case SDL_SCANCODE_1:
                        release_key( HPKEY_1 );
                        break;
                    case SDL_SCANCODE_KP_2:
                    case SDL_SCANCODE_2:
                        release_key( HPKEY_2 );
                        break;
                    case SDL_SCANCODE_KP_3:
                    case SDL_SCANCODE_3:
                        release_key( HPKEY_3 );
                        break;
                    case SDL_SCANCODE_KP_4:
                    case SDL_SCANCODE_4:
                        release_key( HPKEY_4 );
                        break;
                    case SDL_SCANCODE_KP_5:
                    case SDL_SCANCODE_5:
                        release_key( HPKEY_5 );
                        break;
                    case SDL_SCANCODE_KP_6:
                    case SDL_SCANCODE_6:
                        release_key( HPKEY_6 );
                        break;
                    case SDL_SCANCODE_KP_7:
                    case SDL_SCANCODE_7:
                        release_key( HPKEY_7 );
                        break;
                    case SDL_SCANCODE_KP_8:
                    case SDL_SCANCODE_8:
                        release_key( HPKEY_8 );
                        break;
                    case SDL_SCANCODE_KP_9:
                    case SDL_SCANCODE_9:
                        release_key( HPKEY_9 );
                        break;
                    case SDL_SCANCODE_KP_PERIOD:
                        release_key( HPKEY_PERIOD );
                        break;
                    case SDL_SCANCODE_SPACE:
                        release_key( HPKEY_SPC );
                        break;
                    case SDL_SCANCODE_ESCAPE:
                    case SDL_SCANCODE_F5:
                        release_key( HPKEY_ON );
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                    case SDL_SCANCODE_F1:
                        release_key( HPKEY_ENTER );
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        release_key( HPKEY_BS );
                        break;
                    case SDL_SCANCODE_KP_PLUS:
                        release_key( HPKEY_PLUS );
                        break;
                    case SDL_SCANCODE_KP_MINUS:
                        release_key( HPKEY_MINUS );
                        break;
                    case SDL_SCANCODE_KP_MULTIPLY:
                        release_key( HPKEY_MUL );
                        break;
                    case SDL_SCANCODE_KP_DIVIDE:
                        release_key( HPKEY_DIV );
                        break;
                    case SDL_SCANCODE_A:
                        release_key( HPKEY_A );
                        break;
                    case SDL_SCANCODE_B:
                        release_key( HPKEY_B );
                        break;
                    case SDL_SCANCODE_C:
                        release_key( HPKEY_C );
                        break;
                    case SDL_SCANCODE_D:
                        release_key( HPKEY_D );
                        break;
                    case SDL_SCANCODE_E:
                        release_key( HPKEY_E );
                        break;
                    case SDL_SCANCODE_F:
                        release_key( HPKEY_F );
                        break;
                    case SDL_SCANCODE_G:
                        release_key( HPKEY_MTH );
                        break;
                    case SDL_SCANCODE_H:
                        release_key( HPKEY_PRG );
                        break;
                    case SDL_SCANCODE_I:
                        release_key( HPKEY_CST );
                        break;
                    case SDL_SCANCODE_J:
                        release_key( HPKEY_VAR );
                        break;
                    case SDL_SCANCODE_K:
                    case SDL_SCANCODE_UP:
                        release_key( HPKEY_UP );
                        break;
                    case SDL_SCANCODE_L:
                        release_key( HPKEY_NXT );
                        break;
                    case SDL_SCANCODE_M:
                        release_key( HPKEY_QUOTE );
                        break;
                    case SDL_SCANCODE_N:
                        release_key( HPKEY_STO );
                        break;
                    case SDL_SCANCODE_O:
                        release_key( HPKEY_EVAL );
                        break;
                    case SDL_SCANCODE_P:
                    case SDL_SCANCODE_LEFT:
                        release_key( HPKEY_LEFT );
                        break;
                    case SDL_SCANCODE_Q:
                    case SDL_SCANCODE_DOWN:
                        release_key( HPKEY_DOWN );
                        break;
                    case SDL_SCANCODE_R:
                    case SDL_SCANCODE_RIGHT:
                        release_key( HPKEY_RIGHT );
                        break;
                    case SDL_SCANCODE_S:
                        release_key( HPKEY_SIN );
                        break;
                    case SDL_SCANCODE_T:
                        release_key( HPKEY_COS );
                        break;
                    case SDL_SCANCODE_U:
                        release_key( HPKEY_TAN );
                        break;
                    case SDL_SCANCODE_V:
                        release_key( HPKEY_SQRT );
                        break;
                    case SDL_SCANCODE_W:
                        release_key( HPKEY_POWER );
                        break;
                    case SDL_SCANCODE_X:
                        release_key( HPKEY_INV );
                        break;
                    case SDL_SCANCODE_Y:
                        release_key( HPKEY_NEG );
                        break;
                    case SDL_SCANCODE_Z:
                        release_key( HPKEY_EEX );
                        break;
                    case SDL_SCANCODE_F2:
                        release_key( HPKEY_SHL );
                        break;
                    case SDL_SCANCODE_F3:
                        release_key( HPKEY_SHR );
                        break;
                    case SDL_SCANCODE_F4:
                        release_key( HPKEY_ALPHA );
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

bool gui_init( void )
{
    if ( SDL_Init( SDL_INIT_VIDEO | IMG_INIT_PNG | SDL_INIT_TIMER ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    if ( TTF_Init() == -1 ) {
        fprintf( stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError() );
        return false;
    }

    ttffont = TTF_OpenFont( config.ui_font1, config.ui_font_size1 * config.ui_scale );
    ttffont2 = TTF_OpenFont( config.ui_font2, config.ui_font_size2 * config.ui_scale );

    int window_width = ( LCD_WIDTH + ( 2 * UI_PADDING ) ) * config.ui_scale;
    int window_height = ( UI_KB_OFFSET_Y + UI_KB_HEIGHT ) + 2 * UI_PADDING;

    window = SDL_CreateWindow( "hpemung", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if ( renderer == NULL ) {
        printf( "Erreur lors de la creation d'un renderer : %s", SDL_GetError() );
        return false;
    }

    window_texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height );

    SDL_UpdateWindowSurface( window );

    return _init_keyboard_textures();
}

bool gui_exit( void )
{
    TTF_CloseFont( ttffont );
    TTF_CloseFont( ttffont2 );
    TTF_Quit();

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return true;
}
