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

/*
 * Uninitialized globals -- declared extern in rogue.h, defined here
 * (the reason this globals file exists).
 */
bool after;
char runch;
struct thing player;
WINDOW *mw;
WINDOW *cw;
struct object *cur_weapon;
char take;
struct object *cur_ring[2];
int ntraps;
char prbuf[80];
struct room *oldrp;
coord oldpos;
struct room rooms[MAXROOMS];
char fruit[80];
struct stats max_stats;
struct object *cur_armor;
coord delta;
char file_name[80];
int errno;
char whoami[80];
int seed;
bool waswizard;
int max_level;
bool s_know[MAXSCROLLS];
bool p_know[MAXPOTIONS];
bool r_know[MAXRINGS];
bool ws_know[MAXSTICKS];
char huh[80];
int max_hp;
WINDOW *hw;
char *p_guess[MAXPOTIONS];
char *s_guess[MAXSCROLLS];
char *s_names[MAXSCROLLS];
char *p_colors[MAXPOTIONS];
char *ws_type[MAXSTICKS];
char *ws_made[MAXSTICKS];
char *ws_guess[MAXSTICKS];
char *r_stones[MAXRINGS];
char *r_guess[MAXRINGS];
int dnum;
char home[80];
