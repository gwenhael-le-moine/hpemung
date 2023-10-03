#ifndef __GUI_H
#define __GUI_H

#define FONT_FILENAME "/usr/share/fonts/TTF/unifont.ttf"

#include "types.h"

typedef struct {
    int index;
    int x, y;
    int w, h;
    int flags;
    char* text;
    char* textB;
    char* textC;
    char* textD;
    void ( *down )( void );
    void ( *up )( void );
} Button;

/* Button flags:
 * Use BUTTON_B1RELEASE for normal buttons.
 * Use BUTTON_B1RELEASE | BUTTON_B2TOGGLE for calculator buttons.
 * Use BUTTON_B1TOGGLE for toggle buttons
 */
#define BUTTON_PUSHED 0x01    // Set if button is pushed
#define BUTTON_DISABLED 0x02  // If set the button will be grayed out
#define BUTTON_B1TOGGLE 0x04  // Mouse button 1 toggles this button
#define BUTTON_B2TOGGLE 0x08  // Mouse button 2 toggles this button
#define BUTTON_B1RELEASE 0x10 // Releaseing mouse button 1 anywhere unpushes the button

extern void gui_initKeyboard( Button* calcbuttons );

extern void button_draw_all( /*BITMAP *bmp,*/ Button* buttons );
extern int button_mouse_down( /*BITMAP *bmp,*/ Button* butons, int mx, int my, int mb );
extern int button_mouse_up( /*BITMAP *bmp,*/ Button* buttons, int mx, int my, int mb );

#endif
