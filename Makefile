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

BIN = hpemu
OBJS = bus.o color.o cpu.o disasm.o display.o emulator.o gui.o hdw.o keyboard.o main.o opcodes.o pabout.o pcalc.o pdebug.o pfiles.o pmenu.o ports.o ram.o rom.o rpl.o timers.o

CC = gcc
CFLAGS = -Wall -Werror -O3 -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=missing-braces -Wno-error=incompatible-pointer-types

all: hpemu

clean:
	-rm $(BIN)
	-rm $(OBJS)

$(BIN): $(OBJS)
	$(CC) -o $@ $+ -lSDL2 -lSDL2_ttf

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

bus.o: src/types.h src/rom.h src/ram.h src/ports.h src/hdw.h src/bus.h
color.o: src/color.h
cpu.o: src/types.h src/emulator.h src/bus.h src/opcodes.h src/cpu.h
cpu.o: src/types.h src/opcodes.h src/disasm.h
display.o: src/types.h src/bus.h
emulator.o: src/types.h src/cpu.h src/bus.h src/timers.h src/display.h src/gui.h src/pdebug.h src/emulator.h
gui.o: src/color.h src/pmenu.h src/pcalc.h src/pdebug.h src/pfiles.h src/pabout.h src/gui.h
hdw.o: src/types.h src/bus.h src/ports.h src/timers.h src/display.h src/hdw.h
keyboard.o: src/types.h src/cpu.h src/keyboard.h
main.o: src/types.h src/emulator.h src/gui.h src/color.h
opcodes.o: src/types.h src/cpu.h src/bus.h src/keyboard.h src/opcodes.h src/opinline.h
pabout.o: src/color.h src/gui.h src/pabout.h
pcalc.o: src/color.h src/display.h src/keyboard.h src/gui.h src/pcalc.h
pdebug.o: src/types.h src/emulator.h src/cpu.h src/bus.h src/disasm.h src/color.h src/gui.h src/pdebug.h
pfiles.o: src/color.h src/gui.h src/rpl.h src/pfiles.h
pmenu.o: src/emulator.h src/color.h src/gui.h src/pmenu.h
ports.o: src/types.h src/bus.h src/ports.h
ram.o: src/types.h src/bus.h src/ram.h
rom.o: src/types.h src/bus.h src/rom.h
rpl.o: src/types.h src/bus.h src/opinline.h src/rpl.h
timers.o: src/types.h src/cpu.h src/timers.h
