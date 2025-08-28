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
WITH_SDL ?= yes

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

LIBS = $(SDL_LIBS) $(NCURSES_LIBS)

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
	-O$(OPTIM) \
	-D_GNU_SOURCE=1 \
	-DVERSION_MAJOR=$(VERSION_MAJOR) \
	-DVERSION_MINOR=$(VERSION_MINOR) \
	-DPATCHLEVEL=$(PATCHLEVEL) \
	-I./src/ \
	$(CFLAGS)

HEADERS = src/bus.h \
	src/options.h \
	src/cpu.h \
	src/display.h \
	src/emulator.h \
	src/hdw.h \
	src/keyboard.h \
	src/opcodes.h \
	src/opinline.h \
	src/persistence.h \
	src/ports.h \
	src/rpl.h \
	src/timers.h \
	src/types.h \
	src/emulator_ui4x_api.h \
	src/ui4x/bitmaps_misc.h \
	src/ui4x/common.h \
	src/ui4x/inner.h \
	src/ui4x/ncurses.h \
	$(SDL_HEADERS) \
	$(GTK_HEADERS)

SRC = src/bus.c \
	src/options.c \
	src/cpu.c \
	src/display.c \
	src/emulator.c \
	src/hdw.c \
	src/keyboard.c \
	src/main.c \
	src/opcodes.c \
	src/persistence.c \
	src/ports.c \
	src/rpl.c \
	src/timers.c \
	src/emulator_ui4x_api.c \
	src/ui4x/48gx.c \
	src/ui4x/48sx.c \
	src/ui4x/49g.c \
	src/ui4x/50g.c \
	src/ui4x/common.c \
	src/ui4x/fonts.c \
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
	-rm src/*.o

mrproper: clean
	-rm dist/hpemung

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

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/hpemung
	install -c -m 644 dist/hplogo.png $(DESTDIR)$(PREFIX)/share/hpemung/hplogo.png
	cp -R dist/ROMs/ $(DESTDIR)$(PREFIX)/share/hpemung/
	sed "s|@PREFIX@|$(PREFIX)|g" dist/setup-hpemung-home.sh > $(DESTDIR)$(PREFIX)/share/hpemung/setup-hpemung-home.sh
	chmod 755 $(DESTDIR)$(PREFIX)/share/hpemung/setup-hpemung-home.sh

	install -m 755 -d -- $(DESTDIR)$(DOCDIR)
	cp -R ./*.txt $(DESTDIR)$(DOCDIR)

	install -m 755 -d -- $(DESTDIR)$(PREFIX)/share/applications
	sed "s|@PREFIX@|$(PREFIX)|g" dist/hpemung.desktop > $(DESTDIR)$(PREFIX)/share/applications/hpemung.desktop
