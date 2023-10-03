CC = gcc
LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)
CFLAGS = -Wall -Werror -O3 -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=missing-braces -Wno-error=incompatible-pointer-types
CFLAGS += -DSDL_TTF=1 #-DFONT_FILENAME="/usr/share/fonts/TTF/unifont.ttf"

.PHONY: all clean clean-all pretty-code install mrproper

all: dist/hpemu

dist/hpemu: src/bus.o \
	src/color.o \
	src/cpu.o \
	src/disasm.o \
	src/display.o \
	src/emulator.o \
	src/gui.o \
	src/hdw.o \
	src/keyboard.o \
	src/main.o \
	src/opcodes.o \
	src/pabout.o \
	src/pcalc.o \
	src/pdebug.o \
	src/pfiles.o \
	src/pmenu.o \
	src/ports.o \
	src/ram.o \
	src/rom.o \
	src/rpl.o \
	src/timers.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $+

# Cleaning
clean:
	-rm src/*.o

mrproper: clean
	-rm dist/hpemu

clean-all: mrproper

# Formatting
pretty-code:
	clang-format -i src/*.c src/*.h
