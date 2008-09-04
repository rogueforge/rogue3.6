/*
    mdport.h - Machine Dependent Code for Porting Unix/Curses games

    Copyright (C) 2008 Nicholas J. Kisseberth
    All rights reserved.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifdef HAVE_CONFIG_H
#ifdef PDCURSES
#undef HAVE_UNISTD_H
#undef HAVE_LIMITS_H
#undef HAVE_MEMORY_H
#undef HAVE_STRING_H
#endif
#include "config.h"

#elif defined(__DJGPP__)
#define HAVE_SYS_TYPES_H 1
#define HAVE_PROCESS_H 1
#define HAVE_PWD_H 1
#define HAVE_TERMIOS_H 1
#define HAVE_SETGID 1
#define HAVE_GETGID 1
#define HAVE_SETUID 1
#define HAVE_GETUID 1
#define HAVE_GETPASS 1
#define HAVE_SPAWNL 1
#define HAVE_ALARM 1
#define HAVE_ERASECHAR 1
#define HAVE_KILLCHAR 1
#define HAVE_CRYPT

#elif defined(_WIN32)
#define HAVE_CURSES_H
#define HAVE_TERM_H
#define HAVE__SPAWNL
#define HAVE_SYS_TYPES_H
#define HAVE_PROCESS_H
#define HAVE_ERASECHAR 1
#define HAVE_KILLCHAR 1
#ifndef uid_t
typedef unsigned int uid_t;
#endif
#ifndef pid_t
typedef unsigned int pid_t;
#endif

#elif defined(__CYGWIN__)
#define HAVE_SYS_TYPES_H 1
#define HAVE_PWD_H 1
#define HAVE_PWD_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_UNISTD_H 1
#define HAVE_TERMIOS_H 1
#define HAVE_NCURSES_TERM_H 1
#define HAVE_ESCDELAY
#define HAVE_SETGID 1
#define HAVE_GETGID 1
#define HAVE_SETUID 1
#define HAVE_GETUID 1
#define HAVE_GETPASS 1
#define HAVE_GETPWUID 1
#define HAVE_WORKING_FORK 1
#define HAVE_ALARM 1
#define HAVE_SPAWNL 1
#define HAVE__SPAWNL 1
#define HAVE_ERASECHAR 1
#define HAVE_KILLCHAR 1
#define HAVE_CRYPT 1

#else /* standards based unix */
#define HAVE_SYS_TYPES_H 1
#define HAVE_PWD_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_UNISTD_H 1
#define HAVE_CRYPT_H 1
#define HAVE_LIMITS_H 1
#define HAVE_TERMIOS_H 1
#define HAVE_UTMPX_H 1
#define HAVE_ERRNO_H 1
#define HAVE_TERM_H 1
#define HAVE_SETGID 1
#define HAVE_GETGID 1
#define HAVE_SETUID 1
#define HAVE_GETUID 1
#define HAVE_SETREUID 1
#define HAVE_SETREGID 1
#define HAVE_CRYPT 1
#define HAVE_GETPASS 1
#define HAVE_GETPWUID 1
#define HAVE_WORKING_FORK 1
#define HAVE_ERASECHAR 1
#define HAVE_KILLCHAR 1
#ifndef _AIX
#define HAVE_GETLOADAVG 1
#endif
#define HAVE_ALARM 1
#endif

#ifdef __DJGPP__
#undef HAVE_GETPWUID /* DJGPP's limited version doesn't even work as documented */
#endif

#define MD_STRIP_CTRL_KEYPAD 1

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#if !defined(PATH_MAX) && defined(_MAX_PATH)
#define PATH_MAX _MAX_PATH
#endif

#if !defined(PATH_MAX) && defined(_PATH_MAX)
#define PATH_MAX _PATH_MAX
#endif

#ifndef HAVE_CRYPT
char *	crypt(const char *key, const char *setting);
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif
#endif

int	md_chmod(const char *filename, int mode);
int	md_dsuspchar(void);
int	md_erasechar(void);
char *	md_gethomedir(void);
char *	md_getusername(void);
uid_t	md_getuid(void);
char *	md_getpass(char *prompt);
pid_t	md_getpid(void);
char *	md_getrealname(uid_t uid);
void	md_init(int options);
int	md_killchar(void);
void	md_normaluser(void);
void	md_raw_standout(void);
void	md_raw_standend(void);
int	md_readchar(WINDOW *win);
int	md_setdsuspchar(int c);
int	md_shellescape(void);
void	md_sleep(int s);
int	md_suspchar(void);
int	md_hasclreol(void);
int	md_unlink(char *file);
int	md_unlink_open_file(const char *file, FILE *inf);
void	md_tstpsignal(void);
void	md_tstphold(void);
void	md_tstpresume(void (*tstp)(int));
void	md_ignoreallsignals(void);
void	md_onsignal_autosave(void);
void	md_onsignal_exit(void);
void	md_onsignal_default(void);
int	md_issymlink(char *sp);
int     md_loadav(double *avg);
long    md_memused(void);
int     md_ucount(void);
int     md_unlockfile(FILE *fp);
int     md_lockfile(FILE *fp);
