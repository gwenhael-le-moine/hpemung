# Makefile to build x48ng without autotools
#
# The cc-option function and the C{,PP}FLAGS logic were copied from the
# fsverity-utils project.
# https://git.kernel.org/pub/scm/fs/fsverity/fsverity-utils.git/
# The governing license can be found in the LICENSE file or at
# https://opensource.org/license/MIT.

TARGET = dist/hpemung

VERSION_MAJOR = 0
VERSION_MINOR = 10
PATCHLEVEL = 0

PREFIX ?= /usr
DOCDIR ?= $(PREFIX)/doc/hpemung

PKG_CONFIG ?= pkg-config

OPTIM ?= 2
FULL_WARNINGS ?= no
WITH_GTK ?= yes
WITH_SDL ?= yes
WITH_SDL2 = no

makeflags +=-j$(NUM_CORES) -l$(NUM_CORES)

cc-option = $(shell if $(CC) $(1) -c -x c /dev/null -o /dev/null > /dev/null 2>&1; \
		  then echo $(1); fi)

### Text UI
NCURSES_CFLAGS = $(shell "$(PKG_CONFIG)" --cflags ncursesw) -DNCURSES_WIDECHAR=1 -DHAS_NCURSES=1
NCURSES_LIBS = $(shell "$(PKG_CONFIG)" --libs ncursesw)

### SDL UI
ifeq ($(WITH_SDL), yes)
	SDL_CFLAGS = $(shell "$(PKG_CONFIG)" --cflags sdl3) -DHAS_SDL=1
	SDL_LIBS = $(shell "$(PKG_CONFIG)" --libs sdl3)
	SDL_SRC = src/ui4x/sdl.c
	SDL_HEADERS = src/ui4x/sdl.h
endif

ifeq ($(WITH_SDL2), yes)
	SDL_CFLAGS = $(shell "$(PKG_CONFIG)" --cflags sdl2) -DHAS_SDL=1 -DHAS_SDL2=1
	SDL_LIBS = $(shell "$(PKG_CONFIG)" --libs sdl2)
	SDL_SRC = src/ui4x/sdl.c
	SDL_HEADERS = src/ui4x/sdl.h
endif

ifeq ($(WITH_GTK), yes)
	GTK_CFLAGS = -DHAS_GTK=1 $(shell "$(PKG_CONFIG)" --cflags gtk4)
	GTK_LIBS = $(shell "$(PKG_CONFIG)" --libs gtk4)
	GTK_SRC = src/ui4x/gtk.c
	GTK_HEADERS = src/ui4x/gtk.h
endif

LIBS = $(GTK_LIBS) $(SDL_LIBS) $(NCURSES_LIBS)

#CFLAGS = -Wall -Werror -O3 -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=missing-braces -Wno-error=incompatible-pointer-types

ifeq ($(FULL_WARNINGS), no)
EXTRA_WARNING_CFLAGS := -Wno-unused-function \
	-Wno-redundant-decls \
	$(call cc-option,-Wno-maybe-uninitialized) \
	$(call cc-option,-Wno-discarded-qualifiers) \
	$(call cc-option,-Wno-uninitialized) \
	$(call cc-option,-Wno-ignored-qualifiers)
else
EXTRA_WARNING_CFLAGS := -Wunused-function \
	-Wredundant-decls \
	-fsanitize=thread \
	$(call cc-option,-Wunused-variable)
endif

override CFLAGS := -std=c11 \
	-Wall -Wextra -Wpedantic \
	-Wformat=2 -Wshadow \
	-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
	-Wnested-externs -Wmissing-include-dirs \
	-Wdouble-promotion \
	-Wno-sign-conversion \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-conversion \
	-Wno-format-nonliteral \
	$(call cc-option,-Wjump-misses-init) \
	$(call cc-option,-Wlogical-op) \
	$(call cc-option,-Wno-unknown-warning-option) \
	$(EXTRA_WARNING_CFLAGS) \
	$(SDL_CFLAGS) \
	$(GTK_CFLAGS) \
	-O$(OPTIM) \
	-D_GNU_SOURCE=1 \
	-DVERSION_MAJOR=$(VERSION_MAJOR) \
	-DVERSION_MINOR=$(VERSION_MINOR) \
	-DPATCHLEVEL=$(PATCHLEVEL) \
	-I./src/ \
	$(CFLAGS)

HEADERS = src/options.h \
	src/emulator_api.h \
	src/core/bus.h \
	src/core/cpu.h \
	src/core/display.h \
	src/core/emulator.h \
	src/core/hdw.h \
	src/core/keyboard.h \
	src/core/opcodes.h \
	src/core/persistence.h \
	src/core/ports.h \
	src/core/rpl.h \
	src/core/timers.h \
	src/core/types.h \
	src/ui4x/api.h \
	src/ui4x/bitmaps_misc.h \
	src/ui4x/inner.h \
	src/ui4x/ncurses.h \
	$(SDL_HEADERS) \
	$(GTK_HEADERS)

SRC = src/main.c \
	src/options.c \
	src/emulator_api.c \
	src/core/bus.c \
	src/core/cpu.c \
	src/core/display.c \
	src/core/emulator.c \
	src/core/hdw.c \
	src/core/keyboard.c \
	src/core/opcodes.c \
	src/core/persistence.c \
	src/core/ports.c \
	src/core/rpl.c \
	src/core/timers.c \
	src/core/types.c \
	src/ui4x/48gx.c \
	src/ui4x/48sx.c \
	src/ui4x/49g.c \
	src/ui4x/50g.c \
	src/ui4x/api.c \
	src/ui4x/fonts.c \
	src/ui4x/bitmaps_misc.c \
	src/ui4x/ncurses.c \
	$(SDL_SRC) \
	$(GTK_SRC)

OBJS = $(SRC:.c=.o)

.PHONY: all clean clean-all pretty-code install mrproper get-roms

all: $(TARGET)

$(TARGET): $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $(OBJS)

# Cleaning
clean:
	-rm -f $(OBJS)

mrproper: clean
	-rm -f $(TARGET)

clean-all: mrproper

# for clangd
compile_commands.json: mrproper
	bear -- make $(TARGET)

# Formatting
pretty-code:
	clang-format -i src/*.c src/*.h src/ui4x/*.c src/ui4x/*.h

# Installing
get-roms:
	make -C dist/ROMs get-roms

install: all get-roms
	install -m 755 -d -- $(DESTDIR)$(PREFIX)/bin
	install -c -m 755 dist/hpemung $(DESTDIR)$(PREFIX)/bin/hpemung
	install -c -m 755 dist/hpemung-launcher $(DESTDIR)$(PREFIX)/bin/hpemung-launcher

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/hpemung
	install -c -m 644 dist/hplogo.png $(DESTDIR)$(PREFIX)/share/hpemung/hplogo.png
	cp -R dist/ROMs/ $(DESTDIR)$(PREFIX)/share/hpemung/

	install -m 755 -d -- $(DESTDIR)$(DOCDIR)
	cp -R ./*.txt $(DESTDIR)$(DOCDIR)

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/applications
	sed "s|@PREFIX@|$(PREFIX)|g" dist/hpemung.desktop > $(DESTDIR)$(PREFIX)/share/applications/hpemung.desktop
