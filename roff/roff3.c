#include <ctype.h>
#include <stdio.h>

/* Additional formatter logic originally in roff3.s.  This translation
 * implements a couple of helpers used by the request stubs. */

/*
 * alph -- helper matching the `alph2:` check in roff3.s.
 *
 * Return non-zero if c is an alphabetic character.
 */
/*
 * Check if the provided character is alphabetic.
 */
int alph(int c) { return isalpha((unsigned char)c); }

/*
 * maplow -- corresponds to the `maplow:` routine in the original code.
 * Convert a character to lower case.
 */
int maplow(int c) { return tolower((unsigned char)c); }

/*
 * ch -- global holding a character pushed back by skipcont or getchar.
 * In the original sources this variable was used by a number of helper
 * routines.  We retain the name for clarity.
 */
int ch = 0;

/*
 * getchar -- simplified translation of the `getchar:` routine from
 * roff1.s.  Only the pushback via `ch` is modeled here.
 */
#undef getchar
int getchar(void) {
    int c = ch;
    if (c != 0) {
        ch = 0;
        return c;
    }
    return fgetc(stdin);
}

/*
 * rbreak -- break the current output line.
 *
 * This corresponds to the `rbreak:` routine in roff3.s.  The original
 * assembly performed a large amount of bookkeeping around page
 * boundaries and output buffering.  Here we simply flush the line using
 * the high level helper implemented in roff4.c.
 */
extern void nline(void); /* label `newline` in roff4.s */

void rbreak(void) {
    nline();
}

/*
 * skipcont -- consume continuation characters following a request name.
 *
 * This mirrors the behaviour of the `skipcont:` routine in roff3.s.  The
 * original skipped over additional alphabetic characters (allowing long
 * request names) and any subsequent spaces.  The first non-space
 * character encountered is stored in `ch` for the caller.
 */
void skipcont(void) {
    int c;

    /* Skip over alphabetic continuation characters. */
    while ((c = getchar()) != EOF && isalpha((unsigned char)c))
        ;

    /* Consume trailing spaces. */
    while (c == ' ')
        c = getchar();

    ch = c;
}

/* ---------------------------------------------------------------------- */
/* number -- placeholder mirroring label ``number`` in roff3.s.  The
 * original parsed a signed number from the input stream.  The stub
 * simply returns zero.
 */
int number(void) { return 0; }

/*
 * donum -- simplified stand-in for the `donum` routine in roff3.s.  The
 * original output line numbers.  This version only notes invocation.
 */
void donum(void) { puts("[stub] donum"); }
