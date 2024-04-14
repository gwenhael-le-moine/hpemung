CC = gcc
LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)
CFLAGS = -Wall -Werror -O3 -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=missing-braces -Wno-error=incompatible-pointer-types

.PHONY: all clean clean-all pretty-code install mrproper get-roms

all: dist/hpemu

dist/hpemu: src/bus.o \
	src/cpu.o \
	src/display.o \
	src/emulator.o \
	src/gui.o \
	src/hdw.o \
	src/keyboard.o \
	src/main.o \
	src/opcodes.o \
	src/gui_buttons.o \
	src/gui_display.o \
	src/files.o \
	src/ports.o \
	src/rpl.o \
	src/config.o \
	src/timers.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $+

# Installing

get-roms:
	make -C ROMs get-roms

# Cleaning
clean:
	-rm src/*.o

mrproper: clean
	-rm dist/hpemu

clean-all: mrproper

# Formatting
pretty-code:
	clang-format -i src/*.c src/*.h
