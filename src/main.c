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
#include "emulator.h"
#include "gui.h"
#include "color.h"

static int fullscreen = FALSE;

static void parse_args(int argc, char *argv[])
{
    while (--argc) {
	argv++;
	if (argv[0][0] == '-') {
	    switch (argv[0][1]) {
	    case 'f':
		fullscreen = TRUE;
		break;
	    case 'w':
		fullscreen = FALSE;
		break;
	    }
	}
    }
}

static void close_hook(void)
{
    please_exit = TRUE;
}

static void program_init(void)
{
    int depth;

    if (install_allegro(SYSTEM_AUTODETECT, &errno, atexit) < 0) {
	allegro_message("Can't install allegro");
	exit(1);
    }
    if (install_timer() < 0) {
	allegro_message("Can't install timer driver");
	exit(1);
    }
    if (install_keyboard() < 0) {
	allegro_message("Can't install keyboard driver");
	exit(1);
    }
    if (install_mouse() < 0) {
	allegro_message("Can't install mouse driver");
	exit(1);
    }

    if (fullscreen) {
	depth = 8;
    } else {
	depth = desktop_color_depth();
	if (depth == 0) {
	    depth = 8;
	}
    }
    set_color_depth(depth);

    if (set_gfx_mode(fullscreen
		     ? GFX_AUTODETECT_FULLSCREEN
		     : GFX_AUTODETECT_WINDOWED,
		     640, 480, 0, 0) < 0) {
	allegro_message("Can't set graphics mode (%s)", allegro_error);
	exit(1);
    }

    set_window_title("hpemu 0.9.0");
    set_window_close_hook(close_hook);

    color_init();

    show_mouse(screen);
}

static void program_exit(void)
{
}

int main (int argc, char *argv[])
{
    parse_args (argc, argv);
    program_init();
    emulator_init();
    gui_init();

    emulator_run();

    gui_exit();
    emulator_exit();
    program_exit();

    return 0;
} END_OF_MAIN();

