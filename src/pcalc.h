#ifndef __PCALC_H
#define __PCALC_H

#include <SDL2/SDL.h>

#include "types.h"

typedef struct {
    SDL_Keycode SDL_event_id;
    void ( *down )( void );
    void ( *up )( void );
} KBMapping;

extern void pcalc_init();
extern void pcalc_show();
extern void pcalc_down( int mx, int my, int mb );
extern void pcalc_up( int mx, int my, int mb );
extern void pcalc_kb_down( SDL_Keycode sdl_event );
extern void pcalc_kb_up( SDL_Keycode sdl_event );

#endif
