/*
 * Special wizard commands (some of which are also non-wizard commands
 * under strange circumstances)
 *
 * @(#)wizard.c	3.8 (Berkeley) 6/3/81
 */

#include <curses.h>
#include <ctype.h>
#include "rogue.h"

/*
 * whatis:
 *	What a certin object is
 */
whatis()
{
    register struct object *obj;
    register struct linked_list *item;

    if ((item = get_item("identify", 0)) == NULL)
	return;
    obj = (struct object *) ldata(item);
    switch (obj->o_type)
    {
	when SCROLL:
	    s_know[obj->o_which] = TRUE;
	    if (s_guess[obj->o_which])
	    {
		free(s_guess[obj->o_which]);
		s_guess[obj->o_which] = NULL;
	    }
	when POTION:
	    p_know[obj->o_which] = TRUE;
	    if (p_guess[obj->o_which])
	    {
		free(p_guess[obj->o_which]);
		p_guess[obj->o_which] = NULL;
	    }
	when STICK:
	    ws_know[obj->o_which] = TRUE;
	    obj->o_flags |= ISKNOW;
	    if (ws_guess[obj->o_which])
	    {
		free(ws_guess[obj->o_which]);
		ws_guess[obj->o_which] = NULL;
	    }
	when WEAPON:
	case ARMOR:
	    obj->o_flags |= ISKNOW;
	when RING:
	    r_know[obj->o_which] = TRUE;
	    obj->o_flags |= ISKNOW;
	    if (r_guess[obj->o_which])
	    {
		free(r_guess[obj->o_which]);
		r_guess[obj->o_which] = NULL;
	    }
    }
    msg(inv_name(obj, FALSE));
}

/*
 * create_obj:
 *	Wizard command for getting anything he wants
 */
create_obj()
{
    register struct linked_list *item;
    register struct object *obj;
    register char ch, bless;
    register int had;

    item = new_item(sizeof *obj);
    obj = (struct object *) ldata(item);
    obj->o_damage = obj->o_hurldmg = "0d0";
    msg("Type of item: ");
    obj->o_type = readchar();
    mpos = 0;
    msg("Which %c do you want? (0-f)", obj->o_type);
    obj->o_which = (isdigit((unsigned char) (ch = readchar())) ? ch - '0' : ch - 'a' + 10);
    obj->o_group = 0;
    obj->o_count = 1;
    mpos = 0;
    switch (obj->o_type)
    {
	when POTION: bless = MAXPOTIONS;
	when SCROLL: bless = MAXSCROLLS;
	when RING: bless = MAXRINGS;
	when STICK: bless = MAXSTICKS;
	when WEAPON: bless = MAXWEAPONS;
	when ARMOR: bless = MAXARMORS;
	when FOOD: case AMULET: bless = 16;
	otherwise: bless = 0;
    }
    if (obj->o_which < 0 || obj->o_which >= bless)
    {
	msg("There is no such thing");
	discard(item);
	return;
    }
    if (obj->o_type == WEAPON || obj->o_type == ARMOR)
    {
	msg("Blessing? (+,-,n)");
	bless = readchar();
	mpos = 0;
	if (bless == '-')
	    obj->o_flags |= ISCURSED;
	if (obj->o_type == WEAPON)
	{
	    init_weapon(obj, obj->o_which);
	    if (bless == '-')
		obj->o_hplus -= rnd(3)+1;
	    if (bless == '+')
		obj->o_hplus += rnd(3)+1;
	}
	else
	{
	    obj->o_ac = a_class[obj->o_which];
	    if (bless == '-')
		obj->o_ac += rnd(3)+1;
	    if (bless == '+')
		obj->o_ac -= rnd(3)+1;
	}
    }
    else if (obj->o_type == RING)
	switch (obj->o_which)
	{
	    case R_PROTECT:
	    case R_ADDSTR:
	    case R_ADDHIT:
	    case R_ADDDAM:
		msg("Blessing? (+,-,n)");
		bless = readchar();
		mpos = 0;
		if (bless == '-')
		    obj->o_flags |= ISCURSED;
		obj->o_ac = (bless == '-' ? -1 : rnd(2) + 1);
	}
    else if (obj->o_type == STICK)
	fix_stick(obj);
    had = total;
    add_pack(item, FALSE);
    if (total == had && pack_char(obj) == 'z')
	discard(item);
}

/*
 * telport:
 *	Bamf the hero someplace else
 */
teleport()
{
    register int rm;
    coord c;

    c = hero;
    mvwaddch(cw, hero.y, hero.x, mvwinch(stdscr, hero.y, hero.x));
    do
    {
	rm = rnd_room();
	rnd_pos(&rooms[rm], &hero);
    } until(winat(hero.y, hero.x) == FLOOR);
    light(&c);
    light(&hero);
    mvwaddch(cw, hero.y, hero.x, PLAYER);
    /*
     * turn off ISHELD in case teleportation was done while fighting
     * a Fungi
     */
    if (on(player, ISHELD)) {
	player.t_flags &= ~ISHELD;
	fung_hit = 0;
	strcpy(monsters['F'-'A'].m_stats.s_dmg, "000d0");
    }
    count = 0;
    running = FALSE;
    flushinp();		/* flush typeahead */
    return rm;
}

/*
 * passwd:
 *	see if user knows password
 */
passwd()
{
    register char *sp, c;
    char buf[MAXSTR], *crypt();

    msg("Wizard's Password:");
    mpos = 0;
    sp = buf;
    while ((c = readchar()) != '\n' && c != '\r' && c != '\033')
	if (c == killchar())
	    sp = buf;
	else if (c == erasechar() && sp > buf)
	    sp--;
	else if (sp < &buf[MAXSTR - 1])
	    *sp++ = c;
    if (sp == buf)
	return FALSE;
    *sp = '\0';
    return ((sp = crypt(buf, "mT")) != NULL && strcmp(PASSWD, sp) == 0);
}
