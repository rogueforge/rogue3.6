# Rogue 3.6

Reconstructed source for Rogue 3.6 (Toy, Arnold, Wichman; Berkeley 1981),
as built for the 2.9BSD PDP-11 binary `/usr/70/rogue`, linked 27 March
1983. Compiled with a modified 2.9BSD toolchain, it reproduces that binary
byte for byte.

This is the non-overlay build, in separate instruction and data spaces.
It drops the `roguestrings` message file used by the overlaid build, so
all of its strings are pooled with xstr(1).

Where the surviving trees disagree, identifier names, comments, whitespace,
details reflect a reconciliation of the available sources rather than
any single authoritative tree. Makefile follows a pattern used throughout
the reconstruction process modified to match code era.
