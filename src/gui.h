#ifndef __GUI_H
#define __GUI_H

#include "types.h"

#define FONT_FILENAME "/usr/share/fonts/TTF/unifont.ttf"

#define UI_SCALE 4

#define UI_PADDING 4

#define ANNUNC_X UI_PADDING
#define ANNUNC_Y UI_PADDING
#define ANNUNC_HEIGHT 8

#define LCD_X UI_PADDING
#define LCD_Y ( UI_PADDING + ANNUNC_HEIGHT )
#define LCD_WIDTH 131
#define LCD_HEIGHT 64

#define UI_K_WIDTH_1 19
#define UI_K_HEIGHT_1 10
#define UI_K_WIDTH_2 23
#define UI_K_HEIGHT_2 12
#define UI_KB_GAP_X 3

#define UI_KB_OFFSET_X 1
#define UI_KB_OFFSET_Y ( UI_PADDING + ANNUNC_HEIGHT + LCD_HEIGHT + UI_PADDING + UI_K_HEIGHT_2 )
#define UI_KB_GAP_Y 10

#define UI_K_WIDTH_enter ( ( UI_K_WIDTH_1 * 2 ) + UI_KB_GAP_X )

#define Y_LINE( i ) ( UI_KB_OFFSET_Y + ( i * ( UI_KB_GAP_Y + 10 ) ) )
#define X_COL( i ) ( UI_PADDING + UI_KB_OFFSET_X + ( ( UI_K_WIDTH_1 + UI_KB_GAP_X ) * i ) )
#define X2_COL( i ) ( UI_PADDING + UI_KB_OFFSET_X + ( ( UI_K_WIDTH_2 + UI_KB_GAP_X ) * i ) )

#define UI_KB_HEIGHT ( UI_SCALE * ( Y_LINE( 9 ) + UI_K_HEIGHT_2 + UI_KB_GAP_Y ) )

typedef struct {
    int index;
    int x, y;
    int w, h;
    int flags;
    char* label;
    char* label_Lshift;
    char* label_Rshift;
    char* label_below;
    void ( *down )( void );
    void ( *up )( void );
} Button;

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

extern void gui_initKeyboard( Button* calcbuttons );

extern void button_draw_all( /*BITMAP *bmp,*/ Button* buttons );
extern int button_mouse_down( /*BITMAP *bmp,*/ Button* butons, int mx, int my, int mb );
extern int button_mouse_up( /*BITMAP *bmp,*/ Button* buttons, int mx, int my, int mb );

#endif
