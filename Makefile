PREFIX = /usr
DOCDIR = $(PREFIX)/doc/hpemung

CC = gcc
LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)
CFLAGS = -Wall -Werror -O3 -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=missing-braces -Wno-error=incompatible-pointer-types

.PHONY: all clean clean-all pretty-code install mrproper get-roms

all: dist/hpemung

dist/hpemung: src/bus.o \
	src/cpu.o \
	src/display.o \
	src/emulator.o \
	src/gui.o \
	src/hdw.o \
	src/keyboard.o \
	src/main.o \
	src/opcodes.o \
	src/persistence.o \
	src/ports.o \
	src/rpl.o \
	src/config.o \
	src/timers.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $+

# Cleaning
clean:
	-rm src/*.o

mrproper: clean
	-rm dist/hpemung

clean-all: mrproper

# Formatting
pretty-code:
	clang-format -i src/*.c src/*.h

# Installing
get-roms:
	make -C dist/ROMs get-roms

install: all get-roms
	install -m 755 -d -- $(DESTDIR)$(PREFIX)/bin
	install -c -m 755 dist/hpemung $(DESTDIR)$(PREFIX)/bin/hpemung

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/hpemung
	install -c -m 644 dist/hplogo.png $(DESTDIR)$(PREFIX)/share/hpemung/hplogo.png
	cp -R dist/ROMs/ $(DESTDIR)$(PREFIX)/share/hpemung/
	sed "s|@PREFIX@|$(PREFIX)|g" dist/setup-hpemung-home.sh > $(DESTDIR)$(PREFIX)/share/hpemung/setup-hpemung-home.sh
	chmod 755 $(DESTDIR)$(PREFIX)/share/hpemung/setup-hpemung-home.sh

	install -m 755 -d -- $(DESTDIR)$(DOCDIR)
	cp -R ./*.txt $(DESTDIR)$(DOCDIR)

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/applications
	sed "s|@PREFIX@|$(PREFIX)|g" dist/hpemung.desktop > $(DESTDIR)$(PREFIX)/share/applications/hpemung.desktop
