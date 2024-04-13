#ifndef __COLOR_H
#define __COLOR_H

enum Colors {
    C_BACKGROUND,
    C_PANEL_BACK,
    C_PANEL_BORDER,
    C_PANEL_TEXT,
    C_PANEL_DISABLED,
    C_BUTTON_BACK,
    C_BUTTON_BORDER,
    C_BUTTON_PUSHED,
    C_BUTTON_TEXT,
    C_BUTTON_DISABLED,
    C_COUNT
};

extern int color[ C_COUNT ];

extern void color_init( void );

#endif
