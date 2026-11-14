/*
 * save and restore routines
 *
 * @(#)save.c	3.9 (Berkeley) 6/16/81
 */

#include <curses.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "rogue.h"

typedef struct stat STAT;

extern char version[], *strerror();
extern const char encstr[];
#include <errno.h>

extern char __executable_start[], _end[];
extern char rg_bss[], *const rg_edata, *const rg_ebss;
extern int playit(), rg_restored();

long save_stamp[2];			/* tell one build from another */
char save_win[3][MAXLINES][MAXCOLS];	/* stdscr, mw and cw */
int save_lines, save_cols;		/* and how big they were */

#ifdef __SANITIZE_ADDRESS__
#define NOCHECK	__attribute__((no_sanitize_address))
#define read	rg_read
#else
#define NOCHECK
#endif

STAT sbuf;

/*
 * save_ok:
 *	A saved game is the game's variables from version to rg_edata
 *	and from rg_bss to rg_ebss.  See that they are where that takes
 *	them to be, by one variable of each kind, and that save_win[]
 *	has room for the screen.
 */
save_ok()
{
    return ((char *) &level >= version && (char *) &level < rg_edata
	&& (char *) &save_lines >= rg_bss && (char *) &save_lines < rg_ebss
	&& LINES <= MAXLINES && COLS <= MAXCOLS);
}

/*
 * save_wins:
 *	Copy what is in the windows that hold the level to save_win[],
 *	or back from it into the windows
 */
save_wins(back)
bool back;
{
    register int i, y, x;
    register WINDOW *win;

    for (i = 0; i < 3; i++)
    {
	win = (i == 0 ? stdscr : i == 1 ? mw : cw);
	for (y = 0; y < LINES; y++)
	    for (x = 0; x < COLS; x++)
		if (back)
		    mvwaddch(win, y, x, (unsigned char) save_win[i][y][x]);
		else
		    save_win[i][y][x] = mvwinch(win, y, x) & A_CHARTEXT;
    }
}

save_game()
{
    register FILE *savef;
    register int c;
    char buf[MAXPATH];

    if (!save_ok())
    {
	msg("This rogue cannot save a game");
	return FALSE;
    }
    /*
     * get file name
     */
    mpos = 0;
    if (file_name[0] != '\0')
    {
	msg("Save file (%s)? ", file_name);
	do
	{
	    c = readchar();
	} while (c != 'n' && c != 'N' && c != 'y' && c != 'Y');
	mpos = 0;
	if (c == 'y' || c == 'Y')
	{
	    msg("File name: %s", file_name);
	    goto gotfile;
	}
    }

    do
    {
	msg("File name: ");
	mpos = 0;
	buf[0] = '\0';
	if (get_str(buf, cw) == QUIT)
	{
	    msg("");
	    return FALSE;
	}
	strcpy(file_name, buf);
gotfile:
	if (stat(file_name, &sbuf) >= 0 && (sbuf.st_mode & S_IFMT) == S_IFREG)
	    unlink(file_name);
	c = open(file_name, O_WRONLY|O_CREAT|O_EXCL, 0400);
	if ((savef = c < 0 ? NULL : fdopen(c, "w")) == NULL)
	    msg(strerror(errno));	/* fake perror() */
    } while (savef == NULL);

    /*
     * write out encrpyted file (after a stat)
     * The fwrite is to force allocation of the buffer before the write
     */
    save_file(savef);
    return TRUE;
}

/*
 * automatically save a file.  This is used if a HUP signal is
 * recieved
 */
auto_save()
{
    register FILE *savef;
    register int i;

    for (i = 0; i < NSIG; i++)
	signal(i, SIG_IGN);
    if (!save_ok())
	exit(1);
    if (stat(file_name, &sbuf) >= 0 && (sbuf.st_mode & S_IFMT) == S_IFREG)
	unlink(file_name);
    if (file_name[0] != '\0'
	&& (i = open(file_name, O_WRONLY|O_CREAT|O_EXCL, 0400)) >= 0
	&& (savef = fdopen(i, "w")) != NULL)
	save_file(savef);
    exit(1);
}

/*
 * write the saved game on the file
 */
save_file(savef)
register FILE *savef;
{
    wmove(cw, LINES-1, 0);
    draw(cw);
    fstat(fileno(savef), &sbuf);
    fwrite("junk", 1, 5, savef);
    fseek(savef, 0L, 0);
    save_lines = LINES;
    save_cols = COLS;
    save_wins(FALSE);
    save_stamp[0] = (long) playit - (long) version;
    save_stamp[1] = (long) rg_restored - (long) __executable_start;
    encwrite(version, rg_edata - version, savef);
    encwrite(rg_bss, rg_ebss - rg_bss, savef);
    fclose(savef);
}

restore(file, envp)
register char *file;
char **envp;
{
    register int inf;
    extern char **environ;
    char buf[80], name[MAXPATH];
    STAT sbuf2;
    int y, x;

    if (strcmp(file, "-r") == 0)
	file = strcpy(name, file_name);
    if ((inf = open(file, 0)) < 0
	|| fstat(inf, &sbuf2) < 0)
    {
	perror(file);
	return FALSE;
    }
    if (sbuf2.st_uid != getuid())
    {
	printf("Sorry, saved game is not yours.\n");
	return FALSE;
    }
    if ((sbuf2.st_mode & 022) != 0)
    {
	printf("Sorry, saved game can be written by others.\n");
	return FALSE;
    }

    fflush(stdout);
    encread(buf, strlen(version) + 1, inf);
    if (strcmp(buf, version) != 0)
    {
	printf("Sorry, saved game is out of date.\n");
	return FALSE;
    }

    if (sbuf2.st_size != (rg_edata - version) + (rg_ebss - rg_bss))
    {
	printf("Sorry, saved game is out of date.\n");
	return FALSE;
    }
    fflush(stdout);
    lseek(inf, 0L, 0);
    if (encread(version, (unsigned int) (rg_edata - version), inf)
	    != rg_edata - version
	|| encread(rg_bss, (unsigned int) (rg_ebss - rg_bss), inf)
	    != rg_ebss - rg_bss)
    {
	printf("Sorry, saved game is damaged.\n");
	return FALSE;
    }
    if (save_stamp[0] != (long) playit - (long) version
	|| save_stamp[1] != (long) rg_restored - (long) __executable_start)
    {
	printf("Sorry, saved game is out of date.\n");
	return FALSE;
    }
    rg_rebase(__executable_start, _end, version, rg_edata, rg_bss, rg_ebss);
    if (!rg_restored())
    {
	printf("Sorry, saved game is damaged.\n");
	return FALSE;
    }
    /*
     * we do not close the file so that we will have a hold of the
     * inode for as long as possible
     */

    if (!wizard)
	if (sbuf2.st_ino != sbuf.st_ino)
	{
	    printf("Sorry, saved game is not in the same file.\n");
	    return FALSE;
	}
	else if (sbuf2.st_ctime - sbuf.st_ctime > 15)
	{
	    printf("Sorry, file has been touched.\n");
	    return FALSE;
	}
    initscr();
    getmaxyx(stdscr, y, x);
    if (y < save_lines || x < save_cols)
    {
	endwin();
	printf("Sorry, this saved game needs a terminal with %d lines and %d columns.\n", save_lines, save_cols);
	return FALSE;
    }
    LINES = save_lines;
    COLS = save_cols;
    cw = newwin(LINES, COLS, 0, 0);
    mw = newwin(LINES, COLS, 0, 0);
    hw = newwin(LINES, COLS, 0, 0);
    scrollok(cw, FALSE);
    scrollok(mw, FALSE);
    scrollok(stdscr, FALSE);
    scrollok(hw, FALSE);
    kw = NULL;
    save_wins(TRUE);
    mpos = 0;
    for (x = 0; x < COLS - 28 && x < sizeof buf - 1 && file[x] != '\0'; x++)
	buf[x] = file[x] < ' ' || file[x] > '~' ? '?' : file[x];
    buf[x] = '\0';
    mvwprintw(cw, 0, 0, "%s: %s", buf, ctime(&sbuf2.st_mtime));

    /*
     * defeat multiple restarting from the same place
     */
    if (!wizard)
	if (sbuf2.st_nlink != 1)
	{
	    endwin();
	    printf("Cannot restore from a linked file\n");
	    return FALSE;
	}
	else if (unlink(file) < 0)
	{
	    endwin();
	    printf("Cannot unlink file\n");
	    return FALSE;
	}

    environ = envp;
    strncpy(file_name, file, MAXPATH - 1);
    file_name[MAXPATH - 1] = '\0';
    setup();
    clearok(curscr, TRUE);
    touchwin(cw);
    srand(getpid());
    playit();
    /*NOTREACHED*/
}

/*
 * perform an encrypted write
 */
NOCHECK
encwrite(start, size, outf)
register char *start;
unsigned int size;
register FILE *outf;
{
    register const char *ep;

    ep = encstr;

    while (size--)
    {
	putc(*start++ ^ *ep++, outf);
	if (*ep == '\0')
	    ep = encstr;
    }
}

/*
 * perform an encrypted read
 */
NOCHECK
encread(start, size, inf)
register char *start;
unsigned int size;
register int inf;
{
    register const char *ep;
    register int read_size;

    if ((read_size = read(inf, start, size)) == -1 || read_size == 0)
	return read_size;

    ep = encstr;

    while (size--)
    {
	*start++ ^= *ep++;
	if (*ep == '\0')
	    ep = encstr;
    }
    return read_size;
}
