/*
 * mkstr - create a string error message file by massaging C source
 *
 * (stock: Bill Joy UCB August 1977; @(#)mkstr.c 4.1 (Berkeley) 10/1/80)
 *
 * Rogue variant, reconstructed from the overlay rogue binary and the
 * frozen /usr/games/lib/roguestrings (Dec 12 1982).  Differences from
 * the stock tool:
 *
 *   - trigger is `msg(` instead of `error(`; because the match is a
 *     character-sequence scan, addmsg( and chmsg( are caught by suffix
 *     with the same single pattern
 *   - a string is also processed when it directly follows an
 *     assignment or ternary token  =  ?  :  (but not ==, <=, >=, !=)
 *     -- this reaches fight.c's hit()/miss() verb assignments and the
 *     ternary arms; empty strings are honored only at msg( heads
 *     (msg("") binds the file's ""@220; `n1 < 0 ? "" : "+"` does not)
 *   - LOOKUP-ONLY: the message file is frozen (mode 0400).  A literal
 *     already in the file is rewritten to its decimal byte offset
 *     (io.c's string() fetches it at run time); anything else is left
 *     alone and falls to xstr's in-core pool.
 *   - works as a filter on RAW source, before cpp (which is why the
 *     ifdef'd-out CHECKTIME reprieve message is a file entry the
 *     binary never references):  mkstr mesgfile < file.c > x.c
 *
 * Message file format is stock mkstr's: text NUL newline per entry,
 * so entry stride is strlen+2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	NENT	512
#define	SLEN	512

char	*etext[NENT];		/* decoded entry text */
int	elen[NENT];
int	eoff[NENT];		/* byte offset of entry head */
int	nent;

char	usagestr[] = "usage: %s mesgfile < file.c > x.c\n";

void
loadmesg(char *file)
{
	FILE *f;
	static char buf[8192];
	char *s;
	long n;
	int i, j, pt, len;

	f = fopen(file, "r");
	if (f == NULL) {
		perror(file);
		exit(1);
	}
	n = fread(buf, 1, sizeof buf, f);
	fclose(f);
	pt = 0;
	i = 0;
	while (i < n) {
		for (j = i; j < n && buf[j] != '\0'; j++)
			continue;
		if (j >= n)
			break;
		s = &buf[i];
		len = j - i;
		if (len > 0 && s[0] == '\n')
			s++, len--;
		if (nent < NENT) {
			etext[nent] = s;
			elen[nent] = len;
			eoff[nent] = pt;
			nent++;
		}
		pt += len + 2;
		i = (j + 1 < n && buf[j + 1] == '\n') ? j + 2 : j + 1;
	}
}

int
lookup(char *s, int len)
{
	int i;

	for (i = 0; i < nent; i++)
		if (elen[i] == len && memcmp(etext[i], s, len) == 0)
			return (eoff[i]);
	return (-1);
}

/*
 * Read a string literal (opening quote already consumed).  The raw
 * text (escapes intact) goes to raw[]; the decoded bytes to val[].
 * Returns decoded length, or -1 on EOF.
 */
int
copystr(char *raw, int *rawlen, char *val)
{
	int c, v, k;
	char *rp = raw, *vp = val;

	for (;;) {
		c = getchar();
		if (c == EOF)
			return (-1);
		if (c == '"')
			break;
		*rp++ = c;
		if (c == '\\') {
			c = getchar();
			if (c == EOF)
				return (-1);
			*rp++ = c;
			switch (c) {
			case 'b': *vp++ = '\b'; continue;
			case 't': *vp++ = '\t'; continue;
			case 'r': *vp++ = '\r'; continue;
			case 'n': *vp++ = '\n'; continue;
			case 'f': *vp++ = '\f'; continue;
			case '\\': *vp++ = '\\'; continue;
			case '"': *vp++ = '"'; continue;
			case '\'': *vp++ = '\''; continue;
			default:
				if (c >= '0' && c <= '7') {
					v = c - '0';
					for (k = 1; k < 3; k++) {
						c = getchar();
						if (c < '0' || c > '7') {
							ungetc(c, stdin);
							break;
						}
						*rp++ = c;
						v = (v << 3) | (c - '0');
					}
					*vp++ = v;
					continue;
				}
				*vp++ = c;
				continue;
			}
		}
		*vp++ = c;
	}
	*rawlen = rp - raw;
	return (vp - val);
}

int
main(int argc, char **argv)
{
	int c, off, len, rawlen;
	int lastsig, last2sig;	/* last two non-whitespace chars */
	int parenmsg;		/* the last '(' followed a *msg name */
	int head, tok;
	char id[16];		/* trailing identifier characters */
	int idlen, idbreak;
	char raw[SLEN], val[SLEN];

	if (argc != 2) {
		fprintf(stderr, usagestr, argv[0]);
		exit(1);
	}
	loadmesg(argv[1]);

	lastsig = last2sig = -1;
	parenmsg = 0;
	idlen = 0;
	idbreak = 0;
	while ((c = getchar()) != EOF) {
		if (c == '"') {
			len = copystr(raw, &rawlen, val);
			if (len < 0)
				break;
			head = (lastsig == '(' && parenmsg);
			tok = (lastsig == '=' || lastsig == '?' ||
			    lastsig == ':') &&
			    !(lastsig == '=' && (last2sig == '=' ||
			    last2sig == '<' || last2sig == '>' ||
			    last2sig == '!'));
			off = -1;
			if (head || (tok && len > 0))
				off = lookup(val, len);
			if (off >= 0)
				printf("%d", off);
			else
				printf("\"%.*s\"", rawlen, raw);
			last2sig = lastsig;
			lastsig = '"';
			idlen = 0;
			continue;
		}
		putchar(c);
		if (c == ' ' || c == '\t' || c == '\n') {
			if (idlen)
				idbreak = 1;
			continue;
		}
		last2sig = lastsig;
		lastsig = c;
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
		    (c >= '0' && c <= '9') || c == '_') {
			if (idbreak)
				idlen = 0;
			idbreak = 0;
			if (idlen < (int)sizeof id - 1)
				id[idlen++] = c;
			continue;
		}
		if (c == '(')
			parenmsg = (idlen >= 3 &&
			    strncmp(&id[idlen - 3], "msg", 3) == 0);
		idlen = 0;
		idbreak = 0;
	}
	return (0);
}
