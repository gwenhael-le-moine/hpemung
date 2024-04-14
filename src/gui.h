#ifndef __GUI_H
#define __GUI_H

#include <SDL2/SDL.h>

#include "types.h"

#define UI_SCALE 4

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

#define UI_KB_HEIGHT ( UI_SCALE * Y_LINE( 9 ) )

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
    void ( *down )( void );
    void ( *up )( void );
} Button;

typedef struct {
    SDL_Color faceplate;

    SDL_Color lcd_pixoff;
    SDL_Color lcd_pixgray1;
    SDL_Color lcd_pixgray2;
    SDL_Color lcd_pixon;

    SDL_Color button_bg;
    SDL_Color button_active;
    SDL_Color button_inactive;
    SDL_Color label;
    SDL_Color Lshift;
    SDL_Color Rshift;
    SDL_Color letter;
    SDL_Color below;
} colors_t;
extern colors_t colors;

extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern SDL_Texture* texTarget;
extern SDL_Texture* tex2Target;

extern bool SDL_ready;

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

extern void SDL__display_show( void );

extern bool gui_events();
extern void gui_init( void );
extern void gui_exit( void );
#endif
