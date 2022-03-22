#     /
#    /__  ___  ___  ____
#   /  / /  / /__/ / / / /  /
#  /  / /__/ /__  /   / /__/
#      /
#     /    version 0.9.0
#
# Copyright 2002 Daniel Nilsson
#
# This file is part of hpemu.
#
# Hpemu is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Hpemu is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with hpemu; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

HPEMU = bin/hpemu.exe
OBJS = obj/bus.o obj/color.o obj/cpu.o obj/disasm.o obj/display.o obj/emulator.o obj/gui.o obj/hdw.o obj/keyboard.o obj/main.o obj/opcodes.o obj/pabout.o obj/pcalc.o obj/pdebug.o obj/pfiles.o obj/pmenu.o obj/ports.o obj/ram.o obj/rom.o obj/rpl.o obj/timers.o

CC = gcc
CFLAGS = -Wall -Werror -O3

all: hpemu

hpemu: $(HPEMU)

$(HPEMU): $(OBJS)
	$(CC) -mwindows -o $@ $+ -lalleg

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

obj/bus.o: src/types.h src/rom.h src/ram.h src/ports.h src/hdw.h src/bus.h
obj/color.o: src/color.h
obj/cpu.o: src/types.h src/emulator.h src/bus.h src/opcodes.h src/cpu.h
obj/cpu.o: src/types.h src/opcodes.h src/disasm.h
obj/display.o: src/types.h src/bus.h
obj/emulator.o: src/types.h src/cpu.h src/bus.h src/timers.h src/display.h src/gui.h src/pdebug.h src/emulator.h
obj/gui.o: src/color.h src/pmenu.h src/pcalc.h src/pdebug.h src/pfiles.h src/pabout.h src/gui.h
obj/hdw.o: src/types.h src/bus.h src/ports.h src/timers.h src/display.h src/hdw.h
obj/keyboard.o: src/types.h src/cpu.h src/keyboard.h
obj/main.o: src/types.h src/emulator.h src/gui.h src/color.h
obj/opcodes.o: src/types.h src/cpu.h src/bus.h src/keyboard.h src/opcodes.h src/opinline.h
obj/pabout.o: src/color.h src/gui.h src/pabout.h
obj/pcalc.o: src/color.h src/display.h src/keyboard.h src/gui.h src/pcalc.h
obj/pdebug.o: src/types.h src/emulator.h src/cpu.h src/bus.h src/disasm.h src/color.h src/gui.h src/pdebug.h
obj/pfiles.o: src/color.h src/gui.h src/rpl.h src/pfiles.h
obj/pmenu.o: src/emulator.h src/color.h src/gui.h src/pmenu.h
obj/ports.o: src/types.h src/bus.h src/ports.h
obj/ram.o: src/types.h src/bus.h src/ram.h
obj/rom.o: src/types.h src/bus.h src/rom.h
obj/rpl.o: src/types.h src/bus.h src/opinline.h src/rpl.h
obj/timers.o: src/types.h src/cpu.h src/timers.h





