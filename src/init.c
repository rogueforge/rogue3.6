/*
 * global variable initializaton
 *
 * @(#)init.c	3.33 (Berkeley) 6/15/81
 */

#include <curses.h>
#include <ctype.h>
#include "rogue.h"

bool amulet = FALSE, askme = FALSE, door_stop = FALSE, fight_flush = FALSE;
bool firstmove = FALSE, in_shell = FALSE, jump = FALSE, notify = TRUE;
bool passgo = FALSE, playing = TRUE, running = FALSE, slow_invent = FALSE;
bool terse = FALSE, wizard = FALSE;

char *a_names[MAXARMORS] = {
    "leather armor",
    "ring mail",
    "studded leather armor",
    "scale mail",
    "chain mail",
    "splint mail",
    "banded mail",
    "plate mail",
};

int level = 1, purse = 0, mpos = 0, no_move = 0, no_command = 0, inpack = 0;
int total = 0, lastscore = -1, no_food = 0, count = 0, fung_hit = 0, quiet = 0;
int food_left = HUNGERTIME, group = 1, hungry_state = 0;

int a_chances[MAXARMORS] = {
    20,
    35,
    50,
    63,
    75,
    85,
    95,
    100
};

int a_class[MAXARMORS] = {
    8,
    7,
    7,
    6,
    5,
    4,
    4,
    3,
};
struct linked_list *lvl_obj = NULL, *mlist = NULL;

#define ___ 1
#define _x {1,1}
struct monster monsters[26] = {
	/* Name		 CARRY	FLAG    str, exp, lvl, amr, hpt, dmg */
	{ "giant ant",	 0,	ISMEAN,	{ _x, 10,   2,   3, ___, "1d6" } },
	{ "bat",	 0,	0,	{ _x,  1,   1,   3, ___, "1d2" } },
	{ "centaur",	 15,	0,	{ _x, 15,   4,   4, ___, "1d6/1d6" } },
	{ "dragon",	 100,	ISGREED,{ _x,9000, 10,  -1, ___, "1d8/1d8/3d10" } },
	{ "floating eye",0,	0,	{ _x,  5,   1,   9, ___, "0d0" } },
	{ "violet fungi",0,	ISMEAN,	{ _x, 85,   8,   3, ___, "000d0" } },
	{ "gnome",	 10,	0,	{ _x,  8,   1,   5, ___, "1d6" } },
	{ "hobgoblin",	 0,	ISMEAN,	{ _x,  3,   1,   5, ___, "1d8" } },
	{ "invisible stalker",0,ISINVIS,{ _x,120,   8,   3, ___, "4d4" } },
	{ "jackal",	 0,	ISMEAN,	{ _x,  2,   1,   7, ___, "1d2" } },
	{ "kobold",	 0,	ISMEAN,	{ _x,  1,   1,   7, ___, "1d4" } },
	{ "leprechaun",	 0,	0,	{ _x, 10,   3,   8, ___, "1d1" } },
	{ "mimic",	 30,	0,	{ _x,140,   7,   7, ___, "3d4" } },
	{ "nymph",	 100,	0,	{ _x, 40,   3,   9, ___, "0d0" } },
	{ "orc",	 15,	ISBLOCK,{ _x,  5,   1,   6, ___, "1d8" } },
	{ "purple worm", 70,	0,	{ _x,7000, 15,   6, ___, "2d12/2d4" } },
	{ "quasit",	 30,	ISMEAN,	{ _x, 35,   3,   2, ___, "1d2/1d2/1d4" } },
	{ "rust monster",0,	ISMEAN,	{ _x, 25,   5,   2, ___, "0d0/0d0" } },
	{ "snake",	 0,	ISMEAN,	{ _x,  3,   1,   5, ___, "1d3" } },
	{ "troll",	 50,	ISREGEN|ISMEAN,{ _x, 55,   6,   4, ___, "1d8/1d8/2d6" } },
	{ "umber hulk",	 40,	ISMEAN,	{ _x,130,   8,   2, ___, "3d4/3d4/2d5" } },
	{ "vampire",	 20,	ISREGEN|ISMEAN,{ _x,380,   8,   1, ___, "1d10" } },
	{ "wraith",	 0,	0,	{ _x, 55,   5,   4, ___, "1d6" } },
	{ "xorn",	 0,	ISMEAN,	{ _x,120,   7,  -2, ___, "1d3/1d3/1d3/4d6" } },
	{ "yeti",	 30,	0,	{ _x, 50,   4,   6, ___, "1d6/1d6" } },
	{ "zombie",	 0,	ISMEAN,	{ _x,  7,   2,   8, ___, "1d8" } }
};

struct magic_item things[NUMTHINGS] = {
    { 0,			27 },	/* potion */
    { 0,			27 },	/* scroll */
    { 0,			18 },	/* food */
    { 0,			 9 },	/* weapon */
    { 0,			 9 },	/* armor */
    { 0,			 5 },	/* ring */
    { 0,			 5 },	/* stick */
};

struct magic_item s_magic[MAXSCROLLS] = {
    { "monster confusion",	 8, 170 },
    { "magic mapping",		 5, 180 },
    { "light",			10, 100 },
    { "hold monster",		 2, 200 },
    { "sleep",			 5,  50 },
    { "enchant armor",		 8, 130 },
    { "identify",		21, 100 },
    { "scare monster",		 4, 180 },
    { "gold detection",		 4, 110 },
    { "teleportation",		 7, 175 },
    { "enchant weapon",		10, 150 },
    { "create monster",		 5,  75 },
    { "remove curse",		 8, 105 },
    { "aggravate monsters",	 1,  60 },
    { "blank paper",		 1,  50 },
    { "genocide",		 1, 200 },
};

struct magic_item p_magic[MAXPOTIONS] = {
    { "confusion",		 8,  50 },
    { "paralysis",		10,  50 },
    { "poison",			 8,  50 },
    { "gain strength",		15, 150 },
    { "see invisible",		 2, 170 },
    { "healing",		15, 130 },
    { "monster detection",	 6, 120 },
    { "magic detection",	 6, 105 },
    { "raise level",		 2, 220 },
    { "extra healing",		 5, 180 },
    { "haste self",		 4, 200 },
    { "restore strength",	14, 120 },
    { "blindness",		 4,  50 },
    { "thirst quenching",	 1,  50 },
};

struct magic_item r_magic[MAXRINGS] = {
    { "protection",		 9, 200 },
    { "add strength",		 9, 200 },
    { "sustain strength",	 5, 180 },
    { "searching",		10, 200 },
    { "see invisible",		10, 175 },
    { "adornment",		 1, 100 },
    { "aggravate monster",	11, 100 },
    { "dexterity",		 8, 220 },
    { "increase damage",	 8, 220 },
    { "regeneration",		 4, 260 },
    { "slow digestion",		 9, 240 },
    { "telportation",		 9, 100 },
    { "stealth",		 7, 100 },
};

struct magic_item ws_magic[MAXSTICKS] = {
    { "light",			12, 120 },
    { "striking",		 9, 115 },
    { "lightning",		 3, 200 },
    { "fire",			 3, 200 },
    { "cold",			 3, 200 },
    { "polymorph",		15, 210 },
    { "magic missile",		10, 170 },
    { "haste monster",		 9,  50 },
    { "slow monster",		11, 220 },
    { "drain life",		 9, 210 },
    { "nothing",		 1,  70 },
    { "teleport away",		 5, 140 },
    { "teleport to",		 5,  60 },
    { "cancellation",		 5, 130 },
};
#undef ___

struct h_list helpstr[] = {
    '?',	"	prints help",
    '/',	"	identify object",
    'h',	"	left",
    'j',	"	down",
    'k',	"	up",
    'l',	"	right",
    'y',	"	up & left",
    'u',	"	up & right",
    'b',	"	down & left",
    'n',	"	down & right",
    'H',	"	run left",
    'J',	"	run down",
    'K',	"	run up",
    'L',	"	run right",
    'Y',	"	run up & left",
    'U',	"	run up & right",
    'B',	"	run down & left",
    'N',	"	run down & right",
    't',	"<dir>	throw something",
    'f',	"<dir>	forward until find something",
    'p',	"<dir>	zap a wand in a direction",
    'z',	"	zap a wand or staff",
    '>',	"	go down a staircase",
    's',	"	search for trap/secret door",
    ' ',	"	(space) rest for a while",
    'i',	"	inventory",
    'I',	"	inventory single item",
    'q',	"	quaff potion",
    'r',	"	read paper",
    'e',	"	eat food",
    'w',	"	wield a weapon",
    'W',	"	wear armor",
    'T',	"	take armor off",
    'P',	"	put on ring",
    'R',	"	remove ring",
    'd',	"	drop object",
    'c',	"	call object",
    'o',	"	examine/set options",
    '^',	"<dir>	identify trap type",
    CTRL(L),	"	redraw screen",
    CTRL(R),	"	repeat last message",
    ESCAPE,	"	cancel command",
    'v',	"	print program version number",
    '!',	"	shell escape",
    'S',	"	save game",
    'Q',	"	quit",
    0, 0
};

long e_levels[] = {
    10L,20L,40L,80L,160L,320L,640L,1280L,2560L,5120L,10240L,20480L,
    40920L, 81920L, 163840L, 327680L, 655360L, 1310720L, 2621440L, 0L };

/*
 * hit:
 *	Print a message to indicate a succesful hit
 */

hit(er, ee)
register char *er, *ee;
{
    register char *s;

    addmsg(prname(er, TRUE));
    if (terse)
	s = " hit.";
    else
	switch (rnd(4))
	{
	    when 0: s = " scored an excellent hit on ";
	    when 1: s = " hit ";
	    when 2: s = (er == 0 ? " have injured " : " has injured ");
	    when 3: s = (er == 0 ? " swing and hit " : " swings and hits ");
	}
    addmsg(s);
    if (!terse)
	addmsg(prname(ee, FALSE));
    endmsg();
}

/*
 * miss:
 *	Print a message to indicate a poor swing
 */

miss(er, ee)
register char *er, *ee;
{
    register char *s;

    addmsg(prname(er, TRUE));
    switch (terse ? 0 : rnd(4))
    {
	when 0: s = (er == 0 ? " miss" : " misses");
	when 1: s = (er == 0 ? " swing and miss" : " swings and misses");
	when 2: s = (er == 0 ? " barely miss" : " barely misses");
	when 3: s = (er == 0 ? " don't hit" : " doesn't hit");
    }
    addmsg(s);
    if (!terse)
	addmsg(" %s", prname(ee, FALSE));
    endmsg();
}

/*
 * save_throw:
 *	See if a creature save against something
 */
save_throw(which, tp)
int which;
struct thing *tp;
{
    register int need;

    need = 14 + which - tp->t_stats.s_lvl / 2;
    return (roll(1, 20) >= need);
}
/*
 * save:
 *	See if he saves against various nasty things
 */

save(which)
int which;
{
    return save_throw(which, &player);
}

/*
 * str_plus:
 *	compute bonus/penalties for strength on the "to hit" roll
 */

str_plus(str)
register str_t *str;
{
    if (str->st_str == 18)
    {
	if (str->st_add == 100)
	    return 3;
	if (str->st_add > 50)
	    return 2;
    }
    if (str->st_str >= 17)
	return 1;
    if (str->st_str > 6)
	return 0;
    return str->st_str - 7;
}

/*
 * add_dam:
 *	compute additional damage done for exceptionally high or low strength
 */

 add_dam(str)
 register str_t *str;
 {
    if (str->st_str == 18)
    {
	if (str->st_add == 100)
	    return 6;
	if (str->st_add > 90)
	    return 5;
	if (str->st_add > 75)
	    return 4;
	if (str->st_add != 0)
	    return 3;
	return 2;
    }
    if (str->st_str > 15)
	return 1;
    if (str->st_str > 6)
	return 0;
    return str->st_str - 7;
}

/*
 * raise_level:
 *	The guy just magically went up a level.
 */

raise_level()
{
    pstats.s_exp = e_levels[pstats.s_lvl-1] + 1L;
    check_level();
}

/*
 * thunk:
 *	A missile hits a monster
 */

thunk(weap, mname)
register struct object *weap;
register char *mname;
{
    if (weap->o_type == WEAPON)
	msg("The %s hits the %s", w_names[weap->o_which], mname);
    else
	msg("You hit the %s.", mname);
}

/*
 * bounce:
 *	A missile misses a monster
 */

bounce(weap, mname)
register struct object *weap;
register char *mname;
{
    if (weap->o_type == WEAPON)
	msg("The %s misses the %s", w_names[weap->o_which], mname);
    else
	msg("You missed the %s.", mname);
}

/*
 * remove a monster from the screen
 */
remove(mp, item)
register coord *mp;
register struct linked_list *item;
{
    mvwaddch(mw, mp->y, mp->x, ' ');
    mvwaddch(cw, mp->y, mp->x, ((struct thing *) ldata(item))->t_oldch);
    detach(mlist, item);
    discard(item);
}

/*
 * is_magic:
 *	Returns true if an object radiates magic
 */

is_magic(obj)
register struct object *obj;
{
    switch (obj->o_type)
    {
	case ARMOR:
	    return obj->o_ac != a_class[obj->o_which];
	when WEAPON:
	    return obj->o_hplus != 0 || obj->o_dplus != 0;
	when POTION:
	case SCROLL:
	case STICK:
	case RING:
	case AMULET:
	    return TRUE;
    }
    return FALSE;
}

/*
 * killed:
 *	Called to put a monster to death
 */

killed(item, pr)
register struct linked_list *item;
bool pr;
{
    register struct thing *tp;
    register struct linked_list *pitem, *nexti;

    tp = (struct thing *) ldata(item);
    if (pr)
    {
	addmsg(terse ? "Defeated " : "You have defeated ");
	if (on(player, ISBLIND))
	    msg("it.");
	else
	{
	    if (!terse)
		addmsg("the ");
	    msg("%s.", monsters[tp->t_type-'A'].m_name);
	}
    }
    pstats.s_exp += tp->t_stats.s_exp;
    /*
     * Do adjustments if he went up a level
     */
    check_level();
    /*
     * If the monster was a violet fungi, un-hold him
     */
    switch (tp->t_type)
    {
	when 'F':
	    player.t_flags &= ~ISHELD;
	    fung_hit = 0;
	    strcpy(monsters['F'-'A'].m_stats.s_dmg, "000d0");
	when 'L':
	{
	    register struct room *rp;

	    if ((rp = roomin(&tp->t_pos)) == NULL)
		break;
	    if (rp->r_goldval != 0 || fallpos(&tp->t_pos,&rp->r_gold,FALSE))
	    {
		rp->r_goldval += GOLDCALC;
		if (save(VS_MAGIC))
		    rp->r_goldval += GOLDCALC + GOLDCALC
				   + GOLDCALC + GOLDCALC;
		mvwaddch(stdscr, rp->r_gold.y, rp->r_gold.x, GOLD);
		if (!(rp->r_flags & ISDARK))
		{
		    light(&hero);
		    mvwaddch(cw, hero.y, hero.x, PLAYER);
		}
	    }
	}
    }
    /*
     * Empty the monsters pack
     */
    pitem = tp->t_pack;
    /*
     * Get rid of the monster.
     */
    remove(&tp->t_pos, item);
    while (pitem != NULL)
    {
	register struct object *obj;

	nexti = next(tp->t_pack);
	obj = (struct object *) ldata(pitem);
	obj->o_pos = tp->t_pos;
	detach(tp->t_pack, pitem);
	fall(pitem, FALSE);
	pitem = nexti;
    }
}

/*
 * init_player:
 *	roll up the rogue
 */

init_player()
{
    pstats.s_lvl = 1;
    pstats.s_exp = 0L;
    max_hp = pstats.s_hpt = 12;
    if (rnd(100) == 7)
    {
	pstats.s_str.st_str = 18;
	pstats.s_str.st_add = rnd(100) + 1;
    }
    else
    {
	pstats.s_str.st_str = 16;
	pstats.s_str.st_add = 0;
    }
    pstats.s_dmg = "1d4";
    pstats.s_arm = 10;
    max_stats = pstats;
    pack = NULL;
}

/*
 * Contains defintions and functions for dealing with things like
 * potions and scrolls
 */

static char *rainbow[] = {
    "Red",
    "Blue",
    "Green",
    "Yellow",
    "Black",
    "Brown",
    "Orange",
    "Pink",
    "Purple",
    "Grey",
    "White",
    "Silver",
    "Gold",
    "Violet",
    "Clear",
    "Vermilion",
    "Ecru",
    "Turquoise",
    "Magenta",
    "Amber",
    "Topaz",
    "Plaid",
    "Tan",
    "Tangerine"
};

#define NCOLORS (sizeof rainbow / sizeof (char *))

static char *sylls[] = {
    "a", "ab", "ag", "aks", "ala", "an", "ankh", "app", "arg", "arze",
    "ash", "ban", "bar", "bat", "bek", "bie", "bin", "bit", "bjor",
    "blu", "bot", "bu", "byt", "comp", "con", "cos", "cre", "dalf",
    "dan", "den", "do", "e", "eep", "el", "eng", "er", "ere", "erk",
    "esh", "evs", "fa", "fid", "for", "fri", "fu", "gan", "gar",
    "glen", "gop", "gre", "ha", "he", "hyd", "i", "ing", "ion", "ip",
    "ish", "it", "ite", "iv", "jo", "kho", "kli", "klis", "la", "lech",
    "man", "mar", "me", "mi", "mic", "mik", "mon", "mung", "mur",
    "nej", "nelg", "nep", "ner", "nes", "nes", "nih", "nin", "o", "od",
    "ood", "org", "orn", "ox", "oxy", "pay", "pet", "ple", "plu", "po",
    "pot", "prok", "re", "rea", "rhov", "ri", "ro", "rog", "rok", "rol",
    "sa", "san", "sat", "see", "sef", "seh", "shu", "ski", "sna",
    "sne", "snik", "sno", "so", "sol", "sri", "sta", "sun", "ta",
    "tab", "tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
    "ulk", "um", "un", "uni", "ur", "val", "viv", "vly", "vom", "wah",
    "wed", "werg", "wex", "whon", "wun", "xo", "y", "yot", "yu",
    "zant", "zap", "zeb", "zim", "zok", "zon", "zum",
};

static char *stones[] = {
    "Agate",
    "Alexandrite",
    "Amethyst",
    "Carnelian",
    "Diamond",
    "Emerald",
    "Granite",
    "Jade",
    "Kryptonite",
    "Lapus lazuli",
    "Moonstone",
    "Obsidian",
    "Onyx",
    "Opal",
    "Pearl",
    "Ruby",
    "Saphire",
    "Tiger eye",
    "Topaz",
    "Turquoise",
};

#define NSTONES (sizeof stones / sizeof (char *))

static char *wood[] = {
    "Avocado wood",
    "Balsa",
    "Banyan",
    "Birch",
    "Cedar",
    "Cherry",
    "Cinnibar",
    "Driftwood",
    "Ebony",
    "Eucalyptus",
    "Hemlock",
    "Ironwood",
    "Mahogany",
    "Manzanita",
    "Maple",
    "Oak",
    "Persimmon wood",
    "Redwood",
    "Rosewood",
    "Teak",
    "Walnut",
    "Zebra wood",
};

#define NWOOD (sizeof wood / sizeof (char *))

static char *metal[] = {
    "Aluminium",
    "Bone",
    "Brass",
    "Bronze",
    "Copper",
    "Iron",
    "Lead",
    "Pewter",
    "Steel",
    "Tin",
    "Zinc",
};

#define NMETAL (sizeof metal / sizeof (char *))






/*
 * init_things
 *	Initialize the probabilities for types of things
 */
init_things()
{
    register struct magic_item *mp;

    for (mp = &things[1]; mp < &things[NUMTHINGS]; mp++)
	mp->mi_prob += (mp-1)->mi_prob;
    badcheck("things", things, NUMTHINGS);
}

/*
 * init_colors:
 *	Initialize the potion color scheme for this time
 */

init_colors()
{
    register int i;
    register char *str;

    for (i = 0; i < MAXPOTIONS; i++)
    {
	do
	    str = rainbow[rnd(NCOLORS)];
	until (isupper(*str));
	*str = tolower(*str);
	p_colors[i] = str;
	p_know[i] = FALSE;
	p_guess[i] = NULL;
	if (i > 0)
		p_magic[i].mi_prob += p_magic[i-1].mi_prob;
    }
    badcheck("potions", p_magic, MAXPOTIONS);
}

/*
 * init_names:
 *	Generate the names of the various scrolls
 */

init_names()
{
    register int nsyl;
    register char *cp, *sp;
    register int i, nwords;

    for (i = 0; i < MAXSCROLLS; i++)
    {
	cp = prbuf;
	nwords = rnd(4)+2;
	while(nwords--)
	{
	    nsyl = rnd(3)+1;
	    while(nsyl--)
	    {
		sp = sylls[rnd((sizeof sylls) / (sizeof (char *)))];
		while(*sp)
		    *cp++ = *sp++;
	    }
	    *cp++ = ' ';
	}
	*--cp = '\0';
	s_names[i] = (char *) new(strlen(prbuf)+1);
	s_know[i] = FALSE;
	s_guess[i] = NULL;
	strcpy(s_names[i], prbuf);
	if (i > 0)
		s_magic[i].mi_prob += s_magic[i-1].mi_prob;
    }
    badcheck("scrolls", s_magic, MAXSCROLLS);
}

/*
 * init_stones:
 *	Initialize the ring stone setting scheme for this time
 */

init_stones()
{
    register int i;
    register char *str;

    for (i = 0; i < MAXRINGS; i++)
    {
	do
	    str = stones[rnd(NSTONES)];
	until (isupper(*str));
	*str = tolower(*str);
	r_stones[i] = str;
	r_know[i] = FALSE;
	r_guess[i] = NULL;
	if (i > 0)
		r_magic[i].mi_prob += r_magic[i-1].mi_prob;
    }
    badcheck("rings", r_magic, MAXRINGS);
}

/*
 * init_materials:
 *	Initialize the construction materials for wands and staffs
 */

init_materials()
{
    register int i;
    register char *str;

    for (i = 0; i < MAXSTICKS; i++)
    {
	do
	    if (rnd(2) == 0)
	    {
		str = metal[rnd(NMETAL)];
		if (isupper(*str))
			ws_type[i] = "wand";
	    }
	    else
	    {
		str = wood[rnd(NWOOD)];
		if (isupper(*str))
			ws_type[i] = "staff";
	    }
	until (isupper(*str));
	*str = tolower(*str);
	ws_made[i] = str;
	ws_know[i] = FALSE;
	ws_guess[i] = NULL;
	if (i > 0)
		ws_magic[i].mi_prob += ws_magic[i-1].mi_prob;
    }
    badcheck("sticks", ws_magic, MAXSTICKS);
}

badcheck(name, magic, bound)
char *name;
register struct magic_item *magic;
register int bound;
{
    register struct magic_item *end;

    if (magic[bound - 1].mi_prob == 100)
	return;
    printf("\nBad percentages for %s:\n", name);
    for (end = &magic[bound]; magic < end; magic++)
	printf("%3d%% %s\n", magic->mi_prob, magic->mi_name);
    printf("[hit RETURN to continue]");
    fflush(stdout);
    while (getchar() != '\n')
	continue;
}

