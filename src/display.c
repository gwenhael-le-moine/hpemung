/*
 *     /
 *    /__  ___  ___  ____
 *   /  / /  / /__/ / / / /  /
 *  /  / /__/ /__  /   / /__/
 *      /
 *     /    version 0.9.0
 *
 * Copyright 2002 Daniel Nilsson
 *
 * This file is part of hpemu.
 *
 * Hpemu is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hpemu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with hpemu; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <allegro.h>
#include "types.h"
#include "bus.h"

#define LCD_X	364
#define LCD_Y	60

address menu_base;
address display_base;
address display_line_offset;
byte display_line_count;
byte display_height;
byte display_offset;
boolean display_enable;

static address cur_adr;
static boolean in_menu;
static byte off_cur_line;
static byte off_line;
static int off_cnt;
static BITMAP *lcd;

static void fade_lcd_line(int y)
{
    byte *lcd_line0 = (byte *)lcd->line[y*2];
    byte *lcd_line1 = (byte *)lcd->line[y*2+1];
    int x = 0;

    while (x < 131) {
	lcd_line0[x*2] = lcd_line0[x*2+1] = lcd_line1[x*2] = lcd_line1[x*2+1] = (lcd_line0[x*2] >> 1);
	x++;
    }
}

static address draw_lcd_line(address adr, int y)
{
    int x = 0;
    int bit = 0;
    byte data = 0;	//Initialized to remove warning
    byte *ptr;
    byte *lcd_line0 = (byte *)lcd->line[y*2];
    byte *lcd_line1 = (byte *)lcd->line[y*2+1];

    ptr = bus_fast_peek(NULL, adr, NULL);

    /* Horisontal pixel offset */
    if (!in_menu) {
	if (display_offset > 3) ptr++;
	data = *ptr++;
	data >>= display_offset & 3;
	bit = 4 - (display_offset & 3);
    }

    while (x < 131) {
	if (bit == 0) {
	    data = *ptr++;
	    bit = 4;
	}
	lcd_line0[x*2] = lcd_line0[x*2+1] = lcd_line1[x*2] = lcd_line1[x*2+1] = (lcd_line0[x*2] >> 1) | ((data & 1) << 6);
	data >>= 1;
	bit--;
	x++;
    }
    return (adr + 0x22 + (!in_menu && (display_offset&4)?2:0)) & 0xFFFFF;
}

void display_init(void)
{
    lcd = create_bitmap_ex(8, 131*2, 64*2);
    clear_to_color(lcd, 0);
}

void display_exit(void)
{
    destroy_bitmap (lcd);
    lcd = NULL;
}

void display_update(void)
{
    if (!display_enable && !off_cnt) {		/* Turn off display */
	off_cnt = 1;
	off_cur_line = off_line = display_line_count;
	display_line_count = 0;
    } else if (display_enable && off_cnt) {	/* Turn on display */
	off_cnt = 0;
	display_line_count = 0;
	in_menu = 0;
	cur_adr = display_base;
    }
    if (!off_cnt) {	/* Display is on */
	cur_adr = draw_lcd_line(cur_adr, display_line_count);

	acquire_screen();
	scare_mouse_area(LCD_X, LCD_Y+display_line_count*2, 131*2, 2);
	blit(lcd, screen, 0, display_line_count*2, LCD_X, LCD_Y+display_line_count*2, 131*2, 2);
	unscare_mouse();
	release_screen();

	if (!in_menu) {
	    cur_adr += display_line_offset;
	}
	if (display_line_count == display_height) {
	    in_menu = 1;
	    cur_adr = menu_base;
	}
	display_line_count++;
	if (display_line_count == 64) {
	    display_line_count = 0;
	    in_menu = 0;
	    cur_adr = display_base;
	}
    } else if (off_cnt <= 7) {	/* Display is off and still fading */
	fade_lcd_line(off_cur_line);

	acquire_screen();
	scare_mouse_area(LCD_X, LCD_Y+off_cur_line*2, 131*2, 2);
	blit(lcd, screen, 0, off_cur_line*2, LCD_X, LCD_Y+off_cur_line*2, 131*2, 2);
	unscare_mouse();
	release_screen();

	off_cur_line++;
	if (off_cur_line == 64) off_cur_line = 0;
	if (off_cur_line == off_line) {
	    off_cnt++;
	}
    }
}
