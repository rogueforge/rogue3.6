#
# Makefile for rogue
# %W% (Berkeley) %G%
#
HDRS=	rogue.h mach_dep.h
DOBJS=	vers.o armor.o chase.o command.o daemon.o daemons.o \
	fight.o init.o io.o list.o main.o misc.o \
	monsters.o move.o new_level.o options.o pack.o passages.o potions.o \
	rings.o rip.o rooms.o save.o scrolls.o sticks.o things.o \
	weapons.o wizard.o
OBJS=	$(DOBJS)
POBJS=	vers.po armor.po chase.po command.po daemon.po daemons.po \
	fight.po init.po io.po list.po main.po misc.po \
	monsters.po move.po new_level.po options.po pack.po passages.po potions.po \
	rings.po rip.po rooms.po save.po scrolls.po sticks.po things.po \
	weapons.po wizard.po
CFILES=	vers.c armor.c chase.c command.c daemon.c daemons.c \
	fight.c init.c io.c list.c main.c misc.c \
	monsters.c move.c new_level.c options.c pack.c passages.c potions.c \
	rings.c rip.c rooms.c save.c scrolls.c sticks.c things.c \
	weapons.c wizard.c
MISC=	Makefile

#DEFS=		-DMASTER -DDUMP -DALLSCORES
CFLAGS=		-O $(DEFS)
PROFLAGS=	-p -O
#LDFLAGS=-i	# For PDP-11's
LDFLAGS=	# For VAXen
CRLIB=		-lcurses
BINARY=		distr.out

LD=	ld
VGRIND=	/usr/ucb/vgrind
# for sites without sccs front end, GET= get
GET=	sccs get
#GET-	get
CC=	cc

.SUFFIXES: .po

.DEFAULT:
	$(GET) $@

.c.po:
	@echo $(CC) -c $(PROFLAGS) $?
	@rm -f x.c
	@ln $? x.c
	@$(CC) -c $(PROFLAGS) x.c
	@mv x.o $*.po

main.o rip.o mach_dep.o: mach_dep.h

a.out: $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(CRLIB) -ltermlib
	size a.out

rogue: newvers a.out
	cp a.out rogue
	strip rogue


install: rogue
	cp rogue         $(DESTDIR)/usr/games/rogue
	chown games      $(DESTDIR)/usr/games/rogue
	chgrp bin        $(DESTDIR)/usr/games/rogue
	chmod 4755       $(DESTDIR)/usr/games/rogue
	cat /dev/null >> $(DESTDIR)/usr/games/lib/rogue_roll
	chown games      $(DESTDIR)/usr/games/lib/rogue_roll
	chgrp bin        $(DESTDIR)/usr/games/lib/rogue_roll
	chmod 644        $(DESTDIR)/usr/games/lib/rogue_roll


newvers:
	$(GET) -e vers.c
	sccs delta -y vers.c

p.out:	$(HDRS) $(POBJS)
	@rm -f x.c
	$(CC) $(PROFLAGS) $(LDFLAGS) -o p.out $(POBJS) $(PCRLIB) -ltermlib 
	size p.out

tags: $(HDRS) $(CFILES)
	ctags -u $?
	ed - tags < :ctfix
	sort tags -o tags

lint:
	lint -hxbc $(DEFS) $(CFILES) -lcurses > linterrs

clean:
	rm -f $(POBJS) $(OBJS) core a.out p.out $(BINARY) rogue strings rogue.tar vgrind.* x.c x.o xs.c xs.o linterrs distmod.o main.obj xs.po

xtar: $(HDRS) $(CFILES) $(MISC)
	rm -f rogue.tar
	tar cf rogue.tar $? :ctfix
	touch xtar

vgrind:
	@csh $(VGRIND) -t -h "Rogue Version 3.6" $(HDRS) *.c > vgrind.out
	@ctags -v $(HDRS) *.c > index
	@csh $(VGRIND) -t -x index > vgrind.out.tbl

wc:
	@echo "  bytes  words  lines  pages file"
	@wc -cwlp $(HDRS) $(CFILES)

cfiles: $(CFILES)
