#
# Makefile for rogue 3.6 -- 2.9BSD non-overlay build
# (reproduces /usr/70/rogue, linked 1983-03-27)
#
# The classic rogue 3.6 Makefile with the rogue 5.4 per-file xstr flow.
# This is the FLAT (separate I&D, 0411) rogue: no overlays, and no mkstr --
# the roguestrings message-file mechanism was dropped in this build, so all
# strings live in the shared xstr[] pool.
#
# Per-module pipeline (the Berkeley xstr(1) idiom):
#   1. cpp   $*.c    -> stream   (drops #ifdef'd-out code before xstr sees it)
#   2. xstr -c -     -> x.c      literals replaced by (&xstr[N]); N appends to
#                                the shared `strings` pool file (dedup)
#   3. cc  -c  x.c   -> x.o -> $@
# After all modules: xstr (no args) turns the accumulated `strings` pool into
# xs.c -> xs.o, linked with the program.  BUILD SERIALLY (no make -j): the
# pool accumulates across modules in OBJS order.
#
# xstr is the installed system xstr(1) (the toolchain's pdp11-bsd29-xstr).

# --- toolchain (the pdp11-bsd29 cross tools) --------------------------------
TOOLS=	$(HOME)/pdp11-bsd29-toolchain
BIN=	$(TOOLS)/usr/bin/pdp11-bsd29
CC=	$(BIN)-cc
CPP=	$(BIN)-cc -E
XSTR=	$(BIN)-xstr		# the installed system xstr(1)
STRIP=	$(BIN)-strip
SIZE=	$(BIN)-size

CFLAGS=	-O -DMENLO_JCL
INCL=	-I.
# non-overlay separate-I&D link; curses + termlib are installed libraries
LDFLAGS=	-i
CRLIB=	-lcurses
TERMLIB=	-ltermlib

OBJS=	vers.o armor.o chase.o command.o daemon.o daemons.o fight.o \
	init.o io.o list.o main.o misc.o monsters.o move.o new_level.o \
	options.o pack.o passages.o potions.o rings.o rip.o rooms.o \
	save.o scrolls.o sticks.o things.o weapons.o wizard.o
HDRS=	rogue.h mach_dep.h

# ===========================================================================
rogue: poolinit $(OBJS) xs.o
	$(CC) $(LDFLAGS) $(OBJS) xs.o $(CRLIB) $(TERMLIB) -o rogue.unstripped
	cp rogue.unstripped rogue
	$(STRIP) rogue
	$(SIZE) rogue

# reset the xstr pool so accumulation starts empty
poolinit:
	rm -f strings x.c xs.c

# per-module rule: cpp | xstr | cc
.c.o:
	$(CPP) $(CFLAGS) $(INCL) $< 2>/dev/null | $(XSTR) -c -
	$(CC) $(CFLAGS) $(INCL) -c x.c
	mv x.o $@
	rm -f x.c

# vers.c is special: its `char release[] = "3.6"' array form can't take xstr's
# (&xstr[N]) rewrite, so feed it through xstr only to APPEND its strings to the
# pool, then compile the RAW file (discarding the xstr'd x.c).
vers.o: vers.c
	$(CPP) $(CFLAGS) $(INCL) vers.c 2>/dev/null | $(XSTR) -c -
	$(CC) $(CFLAGS) -c vers.c
	rm -f x.c

# the shared pool: built from the FULL `strings` file, after every module
xs.o: $(OBJS)
	$(XSTR)
	$(CC) $(CFLAGS) -c xs.c
	rm -f xs.c

$(OBJS): $(HDRS)
main.o rip.o: mach_dep.h

# newvers: bump vers.c's SCCS id (era target; needs an SCCS tree)
newvers:
	sccs get -e vers.c
	sccs delta -y vers.c

install: rogue
	install -c rogue $(DESTDIR)/usr/70/rogue

clean:
	rm -f $(OBJS) xs.o rogue rogue.unstripped x.c xs.c strings

# ---------------------------------------------------------------------------
# A CLEAN single-pass rebuild, the way rogue would have been built in the era.
# It produces a working non-overlay rogue with the same code (text) and BSS
# layout as the shipped /usr/70/rogue; the data segment differs only in the
# order of pooled strings (a fresh xstr pool vs the original's incremental
# accumulation history).  Byte-exact reproduction is not a goal here.
# ---------------------------------------------------------------------------
