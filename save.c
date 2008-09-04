/*
 * save and restore routines
 *
 * @(#)save.c	3.9 (Berkeley) 6/16/81
 */

#include "curses.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "rogue.h"

typedef struct stat STAT;

extern char version[], encstr[];

STAT sbuf;

int
save_game()
{
    register FILE *savef;
    register int c;
    char buf[80];

    /*
     * get file name
     */
    mpos = 0;
    if (file_name[0] != '\0')
    {
	msg("Save file (%s)? ", file_name);
	do
	{
	    c = readchar(cw);
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
	if ((savef = fopen(file_name, "w")) == NULL)
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
void
auto_save(signum)
int signum;
{
    register FILE *savef;
    register int i;

    for (i = 0; i < NSIG; i++)
	signal(i, SIG_IGN);
    if (file_name[0] != '\0' && (savef = fopen(file_name, "w")) != NULL)
	save_file(savef);
    endwin();
    exit(1);
}

/*
 * write the saved game on the file
 */
int
save_file(savef)
register FILE *savef;
{
    char buf[80];
    int ret;
    
    wmove(cw, LINES-1, 0);
    draw(cw);
    fseek(savef, 0L, 0);

    memset(buf,0,80);
    strcpy(buf,version);
    encwrite(buf,80,savef);
    memset(buf,0,80);
    strcpy(buf,"R36 2\n");
    encwrite(buf,80,savef);
    memset(buf,0,80);
    sprintf(buf,"%d x %d\n", LINES, COLS);
    encwrite(buf,80,savef);
    
    ret = rs_save_file(savef);

    fclose(savef);

    return(ret);
}

int
restore(file, envp)
register char *file;
char **envp;
{
    FILE *inf;
    extern char **environ;
    char buf[80];
    int slines, scols;
    int rogue_version = 0, savefile_version = 0;

    if (strcmp(file, "-r") == 0)
	file = file_name;

    if ((inf = fopen(file, "r")) == NULL)
    {
	perror(file);
	return FALSE;
    }

    fflush(stdout);
    encread(buf, 80, inf);

    if (strcmp(buf, version) != 0)
    {
	printf("Sorry, saved game is out of date.\n");
	return FALSE;
    }

    encread(buf, 80, inf);
    sscanf(buf, "R%d %d\n", &rogue_version, &savefile_version);

    if ((rogue_version != 36) && (savefile_version != 2))
    {
	printf("Sorry, saved game format is out of date.\n");
	return FALSE;
    }

    encread(buf,80,inf);
    sscanf(buf,"%d x %d\n",&slines, &scols);

    /*
     * we do not close the file so that we will have a hold of the
     * inode for as long as possible
     */

    initscr();
	
    if (slines > LINES)
    {
	endwin();
    	printf("Sorry, original game was played on a screen with %d lines.\n",slines);
		printf("Current screen only has %d lines. Unable to restore game\n",LINES);
		return(FALSE);
    }
	
    if (scols > COLS)
    {
	endwin();
    	printf("Sorry, original game was played on a screen with %d columns.\n",scols);
		printf("Current screen only has %d columns. Unable to restore game\n",COLS);
		return(FALSE);
    }
    
    cw = newwin(LINES, COLS, 0, 0);
    mw = newwin(LINES, COLS, 0, 0);
    hw = newwin(LINES, COLS, 0, 0);
    nocrmode();    
    keypad(cw,1);
    mpos = 0;
    mvwprintw(cw, 0, 0, "%s", file);

    if (rs_restore_file(inf) != 0)
    {
	endwin();
	printf("Cannot restore file\n");
    	return(FALSE);
    }
	
    if (!wizard && (md_unlink_open_file(file, inf) < 0))
    {
	endwin();
	printf("Cannot unlink file\n");
	return FALSE;
    }

    environ = envp;
    strcpy(file_name, file);
    setup();
    clearok(curscr, TRUE);
    touchwin(cw);
    srand(getpid());
    status();
    playit();
    /*NOTREACHED*/
    return(0);
}

static int encerrno = 0;

int
encerror()
{
    return encerrno;
}

void
encseterr(int err)
{
    encerrno = err;
}

int
encclearerr()
{
    int n = encerrno;

    encerrno = 0;

    return(n);
}

/*
 * perform an encrypted write
 */
int
encwrite(start, size, outf)
register void *start;
unsigned int size;
register FILE *outf;
{
    register char *ep;
    register char *cstart = start;
    unsigned int o_size = size;
    ep = encstr;

    while (size)
    {
	if (putc(*cstart++ ^ *ep++, outf) == EOF)
	    return(o_size - size);
	if (*ep == '\0')
	    ep = encstr;
	size--;
    }

    return(o_size - size);
}

/*
 * perform an encrypted read
 */
int
encread(start, size, inf)
register void *start;
unsigned int size;
register FILE *inf;
{
    register char *ep;
    register int read_size;
    register char *cstart = start;

    if ((read_size = fread(start, 1, size, inf)) == -1 || read_size == 0)
	return read_size;

    ep = encstr;

    while (size--)
    {
	*cstart++ ^= *ep++;
	if (*ep == '\0')
	    ep = encstr;
    }
    return read_size;
}
