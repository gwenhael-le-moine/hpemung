CC = gcc
LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)
CFLAGS = -Wall -Werror -O3 -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=missing-braces -Wno-error=incompatible-pointer-types

.PHONY: all clean clean-all pretty-code install mrproper

all: dist/hpemu

dist/hpemu: src/bus.o \
	src/color.o \
	src/cpu.o \
	src/display.o \
	src/emulator.o \
	src/gui.o \
	src/hdw.o \
	src/keyboard.o \
	src/main.o \
	src/opcodes.o \
	src/pcalc.o \
	src/pfiles.o \
	src/ports.o \
	src/ram.o \
	src/rom.o \
	src/rpl.o \
	src/timers.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $+

# Installing
ROMs/sxrom-a:
	curl "https://www.hpcalc.org/hp48/pc/emulators/sxrom-a.zip" --output - | funzip > "ROMs/sxrom-a"
ROMs/sxrom-b:
	curl "https://www.hpcalc.org/hp48/pc/emulators/sxrom-b.zip" --output - | funzip > "ROMs/sxrom-b"
ROMs/sxrom-c:
	curl "https://www.hpcalc.org/hp48/pc/emulators/sxrom-c.zip" --output - | funzip > "ROMs/sxrom-c"
ROMs/sxrom-d:
	curl "https://www.hpcalc.org/hp48/pc/emulators/sxrom-d.zip" --output - | funzip > "ROMs/sxrom-d"
ROMs/sxrom-e:
	curl "https://www.hpcalc.org/hp48/pc/emulators/sxrom-e.zip" --output - | funzip > "ROMs/sxrom-e"
ROMs/sxrom-j:
	curl "https://www.hpcalc.org/hp48/pc/emulators/sxrom-j.zip" --output - | funzip > "ROMs/sxrom-j"
ROMs/gxrom-l:
	curl "https://www.hpcalc.org/hp48/pc/emulators/gxrom-l.zip" --output - | funzip > "ROMs/gxrom-l"
ROMs/gxrom-m:
	curl "https://www.hpcalc.org/hp48/pc/emulators/gxrom-m.zip" --output - | funzip > "ROMs/gxrom-m"
ROMs/gxrom-p:
	curl "https://www.hpcalc.org/hp48/pc/emulators/gxrom-p.zip" --output - | funzip > "ROMs/gxrom-p"
ROMs/gxrom-r:
	curl "https://www.hpcalc.org/hp48/pc/emulators/gxrom-r.zip" --output - | funzip > "ROMs/gxrom-r"

get-roms: ROMs/sxrom-a ROMs/sxrom-b ROMs/sxrom-c ROMs/sxrom-d ROMs/sxrom-e ROMs/sxrom-j ROMs/gxrom-l ROMs/gxrom-m ROMs/gxrom-p ROMs/gxrom-r

# Cleaning
clean:
	-rm src/*.o

mrproper: clean
	-rm dist/hpemu

clean-all: mrproper

# Formatting
pretty-code:
	clang-format -i src/*.c src/*.h
