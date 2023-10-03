#ifndef __PCALC_H
#define __PCALC_H

#include <SDL2/SDL.h>
#include "types.h"

typedef struct {
    SDL_Keycode SDL_event_id;
    void ( *down )( void );
    void ( *up )( boolean action );
} KBMapping;

void pcalc_init();
void pcalc_show();
void pcalc_hide( void );
void pcalc_down( int mx, int my, int mb );
void pcalc_up( int mx, int my, int mb );
void pcalc_kb_down( SDL_Keycode sdl_event );
void pcalc_kb_up( SDL_Keycode sdl_event );

#endif
