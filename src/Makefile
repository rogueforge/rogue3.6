#
# Makefile for rogue 3.6 -- FLAT (0411 separate I&D) build of the
# overlay-era sources
#
# These are the sources as they stood when rogue used mkstr(1) + the
# roguestrings message file (io.c's string() fetches messages from disk at
# run time) on top of xstr string pooling, with the globals collected in
# extern.c.  This Makefile builds them in a plain non-overlay layout -- the
# classic rogue 3.6 Makefile with the rogue 5.4 xstr flow, plus mkstr in
# front.  (No overlays, no overlay libraries; links the installed curses +
# termlib the same way the flat /usr/70/rogue build does.)
#
# Per-module pipeline, in the era order:
#   1. mkstr  raw .c -> $*.mk.c   message literals (msg/addmsg/= /?/: that are
#                                 entries of the frozen roguestrings file) are
#                                 rewritten to decimal file offsets.  mkstr
#                                 runs on RAW source, BEFORE cpp.
#   2. cpp    $*.mk.c -> stream   (drops #ifdef'd-out code before xstr)
#   3. xstr -c -      -> x.c      remaining literals -> shared xstr[] pool
#   4. cc  -c  x.c    -> $@
# After all modules: xstr (no args) turns the accumulated `strings` pool into
# xs.c -> xs.o.  extern.c (the globals/tables file) is compiled raw.  Then the
# flat link.  BUILD SERIALLY (no make -j): the pool accumulates in OBJS order.
#
# mkstr is the rogue variant (mkstr.c here: stock ucb/mkstr.c idiom, but the
# trigger is msg(/= /?/: instead of error(); it is built locally).  xstr is
# the installed system xstr(1).

# --- toolchain (the pdp11-bsd29 cross tools) --------------------------------
TOOLS=	$(HOME)/pdp11-bsd29-toolchain
BIN=	$(TOOLS)/usr/bin/pdp11-bsd29
CC=	$(BIN)-cc
CPP=	$(BIN)-cc -E
XSTR=	$(BIN)-xstr		# the installed system xstr(1)
STRIP=	$(BIN)-strip
SIZE=	$(BIN)-size
HOSTCC=	cc

CFLAGS=	-O -DMENLO_JCL
INCL=	-I.
LDFLAGS=	-i			# separate I&D (0411), non-overlay
CRLIB=	-lcurses
TERMLIB=	-ltermlib

# the frozen mkstr message file (read by mkstr at build time, and by the
# built rogue's string() at run time)
ROGUESTRINGS= $(HOME)/bsd/2.9/usr/games/lib/roguestrings

OBJS=	vers.o armor.o chase.o command.o daemon.o daemons.o fight.o \
	init.o io.o list.o main.o misc.o monsters.o move.o new_level.o \
	options.o pack.o passages.o potions.o rings.o rip.o rooms.o \
	save.o scrolls.o sticks.o things.o weapons.o wizard.o
HDRS=	rogue.h mach_dep.h

# ===========================================================================
rogue: mkstr poolinit $(OBJS) xs.o extern.o
	$(CC) $(LDFLAGS) $(OBJS) extern.o xs.o $(CRLIB) $(TERMLIB) -o rogue.unstripped
	cp rogue.unstripped rogue
	$(STRIP) rogue
	$(SIZE) rogue

# the rogue-variant mkstr, built locally (its msg(-trigger is rogue-specific)
mkstr: mkstr.c
	$(HOSTCC) -O -o mkstr mkstr.c

# stage the frozen message file (writable copy for mkstr) and RESET the xstr
# pool so accumulation starts empty
poolinit: mkstr
	rm -f strings x.c xs.c
	mkdir -p rdom
	cp $(ROGUESTRINGS) rdom/strings
	chmod u+w rdom/strings

# per-module rule: mkstr | cpp | xstr | cc
.c.o:
	./mkstr rdom/strings < $< > $*.mk.c
	$(CPP) $(CFLAGS) $(INCL) $*.mk.c 2>/dev/null | $(XSTR) -c -
	$(CC) $(CFLAGS) $(INCL) -c x.c
	mv x.o $@
	rm -f $*.mk.c x.c

# vers.c is special: its `char release[] = "3.6"' array form can't take xstr's
# (&xstr[N]) rewrite, so feed it through xstr only to APPEND its strings to the
# pool, then compile the RAW file (discarding the xstr'd x.c).
vers.o: vers.c
	$(CPP) $(CFLAGS) $(INCL) vers.c 2>/dev/null | $(XSTR) -c -
	$(CC) $(CFLAGS) -c vers.c
	rm -f x.c

# extern.c is the globals file: initialised tables + the message/help strings.
# It is compiled RAW (not through xstr) -- its tables reference their strings
# directly, so they stay inline in extern.o rather than joining the pool.
extern.o: extern.c $(HDRS)
	$(CC) $(CFLAGS) -c extern.c

# the shared pool: built from the FULL `strings` file, after every module
xs.o: $(OBJS)
	$(XSTR)
	$(CC) $(CFLAGS) -c xs.c
	rm -f xs.c

$(OBJS): $(HDRS)
main.o rip.o: mach_dep.h

install: rogue
	install -c rogue $(DESTDIR)/usr/games/rogue

clean:
	rm -f $(OBJS) xs.o extern.o mkstr rogue rogue.unstripped \
	      x.c xs.c *.mk.c strings rdom/strings
	rm -rf rdom
