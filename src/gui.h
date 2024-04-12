#ifndef __GUI_H
#define __GUI_H

#define FONT_FILENAME "/usr/share/fonts/TTF/unifont.ttf"

#include "types.h"

#define UI_PADDING 16
#define ANNUNC_X UI_PADDING
#define ANNUNC_Y UI_PADDING
#define ANNUNC_HEIGHT 32

#define LCD_X UI_PADDING
#define LCD_Y UI_PADDING + ANNUNC_HEIGHT
#define LCD_WIDTH 131
#define LCD_HEIGHT 64
#define LCD_SCALE 4

#define KB_UI_SCALE 2

#define UI_K_WIDTH_1 ( ( LCD_WIDTH * KB_UI_SCALE ) / 8 )
#define UI_K_HEIGHT_1 18

#define UI_K_WIDTH_2 ( ( ( LCD_WIDTH * KB_UI_SCALE ) - UI_KB_GAP_X ) / 6 )
#define UI_K_HEIGHT_2 22

#define UI_KB_OFFSET_X 10
#define UI_KB_GAP_X ( UI_K_WIDTH_1 / 3 )
#define UI_KB_GAP_X2 ( UI_K_WIDTH_2 / 4 )

#define UI_KB_OFFSET_Y ( LCD_Y + ( LCD_HEIGHT * KB_UI_SCALE ) )
#define UI_KB_GAP_Y 14

#define UI_K_WIDTH_enter ( ( UI_K_WIDTH_1 * 2 ) + UI_KB_GAP_X )

#define Y_LINE( i ) ( UI_KB_OFFSET_Y + ( i * ( UI_KB_GAP_Y + 10 ) ) )
#define X_COL( i ) ( UI_KB_OFFSET_X + ( UI_K_WIDTH_1 * i ) + ( UI_KB_GAP_X * i ) )
#define XE_COL( i ) ( UI_K_WIDTH_enter - UI_K_WIDTH_1 + X_COL( i ) )
#define X2_COL( i ) ( UI_KB_OFFSET_X + ( UI_K_WIDTH_2 * i ) + ( UI_KB_GAP_X2 * ( i - 1 ) ) )

#define UI_KB_HEIGHT ( KB_UI_SCALE * ( Y_LINE( 9 ) + UI_K_HEIGHT_2 + UI_KB_GAP_Y ) )
#define UI_KB_WIDTH ( KB_UI_SCALE * ( ( X_COL( 5 ) + UI_K_WIDTH_1 + UI_KB_OFFSET_X ) ) )

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
