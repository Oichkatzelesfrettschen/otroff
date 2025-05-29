#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* Declarations for helper routines provided elsewhere in the
 * translated code base. */
void rbreak(void);

/* Global state variables approximating the original assembly data. */
extern int ad, fi, ce, in, un, ll, ls, ls1, pn, pl, skip, ul, hx, hyf;
extern int ohc, tabc, cc;

/*
 * Simplified translations of the request handlers from the original
 * roff2.s module.  Only a subset of behaviour is reproduced here so
 * the program can be built and experimented with.  Each function is
 * named after the request it services.
 */

/* Utility used for functionality that has yet to be implemented. */
static void stub(const char *name) { printf("[stub] %s\n", name); }

/* .ad - enable text adjustment */
void casead(void) {
  rbreak();
  ad++;
}

/* .br - break the current line */
void casebr(void) { rbreak(); }

/* .cc - change control character */
void casecc(void) {
  int c = getchar();
  if (c != '\n' && c != EOF)
    cc = c;
}

/* .ce - center next lines; simplified */
void casece(void) {
  rbreak();
  ce = 0; /* argument parsing not implemented */
}

/* .ds - double spacing */
void caseds(void) {
  rbreak();
  ls = 2;
}

/* .fi - fill output lines */
void casefi(void) {
  rbreak();
  fi++;
}

/* .in - set indent */
void casein(void) {
  rbreak();
  in = 0; /* argument parsing omitted */
  un = in;
}

/* .li - literal input (not implemented) */
void caseli(void) { stub("li"); }

/* .ll - set line length */
void casell(void) { ll = 65; /* argument parsing omitted */ }

/* .tr - set character translation (not implemented) */
void casetr(void) { stub("tr"); }

/* .na - disable adjustment */
void casena(void) {
  rbreak();
  ad = 0;
}

/* .nf - no-fill mode */
void casenf(void) {
  rbreak();
  fi = 0;
}

/* .pl - page length */
void casepl(void) { pl = 66; }

/* .sk - vertical skip */
void casesk(void) { skip = 0; }

/* .ss - single spacing */
void casess(void) {
  rbreak();
  ls = 1;
}

/* .ti - temporary indent */
void caseti(void) {
  rbreak();
  un = in; /* simplified */
}

/* .ul - underline next N lines */
void caseul(void) { ul = 1; }

/* .un - undo underline */
void caseun(void) { ul = 0; }

/* .hx - toggle headers */
void casehx(void) { hx = !hx; }

/* .hy - set hyphenation flag */
void casehy(void) { hyf = 0; }

/* placeholders for other requests */
void casecc_remaining(void) { stub("other requests"); }
