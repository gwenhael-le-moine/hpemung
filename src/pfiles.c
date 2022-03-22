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
#include <string.h>
#include "color.h"
#include "gui.h"
#include "rpl.h"
#include "pfiles.h"

static void load_up(boolean action);

static Button files_buttons[] = {
    { 0,    0,	79, 20,	BUTTON_B1RELEASE,   "Load", NULL,   load_up },
    { 0,    0,	0,  0,	0,		    NULL,   NULL,   NULL }
};

static BITMAP *files_bmp;

static void load_file(char *name)
{
    PACKFILE *f;
    byte *buf;
    byte *obj;
    int i, j;
    int fsize;
    address size;

    fsize = file_size(name);
    if (fsize < 11)	// "PHPH48-X" + prologue (8 + 2.5)
	return;

    buf = malloc(fsize);
    if (!buf)
	return;

    f = pack_fopen(name, "r");
    if (!f) {
	free(buf);
	return;
    }

    if (pack_fread(buf, fsize, f) != fsize) {
	free(buf);
	pack_fclose(f);
	return;
    }
    pack_fclose(f);

    if (memcmp(buf, "HPHP48-", 7)) {
	free(buf);
	return;
    }

    obj = malloc((fsize - 8) * 2);
    if (!obj) {
	free(buf);
	return;
    }

    for (i = 8, j = 0; i < fsize; i++) {
	obj[j++] = buf[i] & 0x0F;
	obj[j++] = (buf[i] >> 4) & 0x0F;
    }
    free(buf);

    size = rpl_object_size(obj);
    if (size > (fsize - 8) * 2) {
	free(obj);
	return;
    }
    rpl_push_object(obj, size);
    free(obj);
}

#define PATH_SIZE   1024

static void load_up(boolean action)
{
    static char path[PATH_SIZE] = "";

    if (action) {
	if (file_select_ex("Load Object", path, NULL, PATH_SIZE, 0, 0)) {
	    load_file(path);
	}
    }
}

void pfiles_show(BITMAP *bmp)
{
    files_bmp = bmp;

    clear_to_color(files_bmp, color[C_PANEL_BACK]);
    button_draw_all(files_bmp, files_buttons);
}

void pfiles_hide(void)
{
    files_bmp = NULL;
}

void pfiles_down(int mx, int my, int mb)
{
    button_mouse_down(files_bmp, files_buttons, mx, my, mb);
}

void pfiles_up(int mx, int my, int mb)
{
    button_mouse_up(files_bmp, files_buttons, mx, my, mb);
}
