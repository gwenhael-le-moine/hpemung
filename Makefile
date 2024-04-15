PREFIX = /usr
DOCDIR = $(PREFIX)/doc/hpemu

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
	src/files.o \
	src/ports.o \
	src/rpl.o \
	src/config.o \
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

# Installing
get-roms:
	make -C dist/ROMs get-roms

install: all get-roms
	install -m 755 -d -- $(DESTDIR)$(PREFIX)/bin
	install -c -m 755 dist/hpemu $(DESTDIR)$(PREFIX)/bin/hpemu

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/hpemu
	install -c -m 644 dist/hplogo.png $(DESTDIR)$(PREFIX)/share/hpemu/hplogo.png
	cp -R dist/ROMs/ $(DESTDIR)$(PREFIX)/share/hpemu/
	sed "s|@PREFIX@|$(PREFIX)|g" dist/setup-hpemu-home.sh > $(DESTDIR)$(PREFIX)/share/hpemu/setup-hpemu-home.sh
	chmod 755 $(DESTDIR)$(PREFIX)/share/hpemu/setup-hpemu-home.sh

	install -m 755 -d -- $(DESTDIR)$(DOCDIR)
	cp -R ./*.txt $(DESTDIR)$(DOCDIR)

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/applications
	sed "s|@PREFIX@|$(PREFIX)|g" dist/hpemu.desktop > $(DESTDIR)$(PREFIX)/share/applications/hpemu.desktop
