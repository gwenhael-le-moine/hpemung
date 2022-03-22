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
#include "rom.h"

void rom_init(void)
{
    int size;
    char *name = "hpemu.rom";
    byte *buf, *ptr1, *ptr2;
    PACKFILE *f;

    size = file_size(name);
    if (!size) {
	exit(0x10);
    }
    if (size != 256*1024 && size != 512*1024 && size != 1024*1024) {
	exit (0x11);
    }
    buf = malloc(size);
    if (!buf) {
	exit(0x12);
    }
    f = pack_fopen(name, "r");
    if (!f) {
	exit(0x13);
    }
    if (pack_fread(buf, size, f) != size) {
	exit(0x14);
    }
    pack_fclose(f);
    if (buf[0] & 0xF0 || buf[1] & 0xF0) {
	if (size == 1024*1024) {
	    exit(0x15);
	}
	buf = realloc(buf, size*2);
	if (!buf) {
	    exit(0x16);
	}
	ptr1 = buf+size-1;
	size *= 2;
	ptr2 = buf+size-1;
	do {
	    *(ptr2--) = (*ptr1 >> 4) & 0x0F;
	    *(ptr2--) = *(ptr1--) & 0x0F;
	} while (ptr1 != ptr2);
    }
    bus_info.rom_data = buf;
    bus_info.rom_mask = size - 1;
}

void rom_exit(void)
{
    free(bus_info.rom_data);
    bus_info.rom_data = NULL;
    bus_info.rom_mask = 0x00000;
}
