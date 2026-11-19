# Rogue 3.6

Reconstructed source for Rogue 3.6 (Toy, Arnold, Wichman; Berkeley 1981),
as built for the overlaid 2.9BSD PDP-11 binary, `/usr/games/rogue`, dated
7 February 1983. Compiled with a modified 2.9BSD toolchain. Its code and
data match the original binary; 49 entries in its symbol table still
differ.

This build keeps its messages in a separate file, `roguestrings`, read
from disk at run time: mkstr(1) replaces the message literals with offsets
into that file, and the remaining strings are pooled with xstr(1). The
Makefile builds the sources in a plain separate I&D layout, without the
overlays.

Where the surviving trees disagree, identifier names, comments, whitespace,
details reflect a reconciliation of the available sources rather than
any single authoritative tree. Makefile follows a pattern used throughout
the reconstruction process modified to match code era.
