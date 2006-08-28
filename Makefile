#
#  Makefile for rogue
# %W% (Berkeley) %G%
#
HDRS=	rogue.h mach_dep.h
OBJS=	vers.o armor.o chase.o command.o daemon.o daemons.o fight.o \
	init.o io.o list.o main.o mdport.o misc.o monsters.o move.o new_level.o \
	options.o pack.o passages.o potions.o rings.o rip.o rooms.o \
	save.o scrolls.o state.o sticks.o things.o weapons.o wizard.o xcrypt.o
POBJS=	vers.po armor.po chase.po command.po daemon.po daemons.po fight.po \
	init.po io.po list.po main.po mdport.po misc.po monsters.po move.po new_level.po \
	options.po pack.po passages.po potions.po rings.po rip.po rooms.po \
	save.po scrolls.po state.po sticks.po things.po weapons.po wizard.po xcrypt.po
CFILES=	vers.c armor.c chase.c command.c daemon.c daemons.c fight.c \
	init.c io.c list.c main.c mdport.c misc.c monsters.c move.c new_level.c \
	options.c pack.c passages.c potions.c rings.c rip.c rooms.c \
	save.c scrolls.c state.c sticks.c things.c weapons.c wizard.c xcrypt.c
CFLAGS= -g
PROFLAGS= -p -O
#LDFLAGS=-i	# For PDP-11's
LDFLAGS=	# For VAXes
VGRIND=/usr/ucb/vgrind
CRLIB=	-lcurses
#CRLIB=	/ra/csr/arnold/=lib/=curses/crlib
#CRLIB=	/ra/csr/toy/_nc/crlib
PCRLIB= -lcurses
MISC=	Makefile TODO
# for sites without sccs front end, GET= get
#GET=	sccs get
GET=	get

.SUFFIXES: .po

.c.po:
	@echo $(CC) -c $(PROFLAGS) $?
	@rm -f x.c
	@ln $? x.c
	@$(CC) -c $(PROFLAGS) x.c
	@mv x.o $*.po

.DEFAULT:
	$(GET) $@

a.out: $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(CRLIB)
	size a.out

k.out: $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(CRLIB) -o k.out

strip: rogue
	strip rogue

install: rogue
	cp rogue $(DESTDIR)/usr/games/rogue

p.out:	$(HDRS) $(POBJS)
	@rm -f x.c
	$(CC) $(PROFLAGS) $(LDFLAGS) $(POBJS) $(PCRLIB) -o p.out
	size p.out

newvers:
	$(GET) -e vers.c
	sccs delta -y vers.c

main.o rip.o: mach_dep.h

tags: $(HDRS) $(CFILES)
	ctags -u $?
	ed - tags < :ctfix
	sort tags -o tags

lint:
	lint -hxbc $(CFILES) -lcurses > linterrs

clean:
	rm -f $(POBJS) $(OBJS) core rogue make.out ? rogue.tar vgrind.*

count:
	wc -l $(HDRS) $(CFILES)

realcount:
	cc -E $(CFILES) | ssp - | wc -l

update:
	ar uv .SAVE $(CFILES) $(HDRS) Makefile

dist:
	@mkdir dist
	cp $(CFILES) $(HDRS) Makefile dist

xtar: $(CFILES) $(HDRS) $(MISC)
	rm -f rogue.tar
	tar cf rogue.tar $? :ctfix
	touch xtar

vgrind:
	@csh $(VGRIND) -t -h "Rogue Version 3.6" $(HDRS) *.c > vgrind.out
	@ctags -v $(HDRS) *.c > index
	@csh $(VGRIND) -t -x index > vgrind.out.tbl

cfiles: $(CFILES)

rogue rogue.exe: $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(CRLIB) -o $@

unixdist:
	make clean
	make rogue
	tar cf rogue36.tar rogue rogue.6 rogue.r
	gzip rogue36.tar
	mv rogue36.tar.gz rogue36-`uname`.tar.gz

dist.irix:
	make clean
	make CFLAGS="-woff 1116 -mips3 -g -O2 -DDUMP" rogue
	tar cf rogue36.tar rogue rogue.6 rogue.r
	gzip rogue36.tar
	mv rogue36.tar.gz rogue36-irix65.tar.gz

dist.aix:
	make clean
	make CFLAGS="-qmaxmem=16768 -g -O2 -DDUMP" rogue
	tar cf rogue36.tar rogue rogue.6 rogue.r
	gzip rogue36.tar
	mv rogue36.tar.gz rogue36-aix43.tar.gz

dist.linux:
	make clean
	make CFLAGS="-g -O2 -DDUMP" rogue
	tar cf rogue36.tar rogue rogue.6 rogue.r
	gzip rogue36.tar
	mv rogue36.tar.gz rogue36-linux.tar.gz

dist.interix: 
	make clean
	make rogue
	tar cf rogue36.tar rogue rogue.6 rogue.r
	gzip rogue36.tar
	mv rogue36.tar.gz rogue36-interix.tar.gz

dist.cygwin:
	make clean
	make rogue
	tar cf rogue36.tar rogue rogue.6 rogue.r
	gzip rogue36.tar
	mv rogue36.tar.gz rogue36-cygwin.tar.gz
	
dist.djgpp:
	del *.o 
	del rogue.exe
	make CRLIB=-lpdcurses LDFLAGS=-L$(DJDIR)/LIB CFLAGS="-g -O2 -DDUMP" rogue.exe
	zip rogue36.zip rogue.exe rogue.6 rogue.r

