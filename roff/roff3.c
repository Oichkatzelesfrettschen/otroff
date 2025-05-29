#include <ctype.h>
#include <stdio.h>

/* Additional formatter logic originally in roff3.s.  This translation
 * implements a couple of helpers used by the request stubs. */

/* Return non-zero if c is an alphabetic character. */
int alph(int c) { return isalpha(c); }

/* Convert a character to lower case. */
int maplow(int c) { return tolower((unsigned char)c); }

/* Placeholder for line break logic. */
void rbreak(void) { printf("[stub] rbreak called\n"); }

/* Last character fetched by skipcont().  This mimics the ``ch`` variable
 * from the original assembly sources.  */
int ch = 0;

/*
 * Consume continuation characters following a request name.
 *
 * The original skipcont routine skipped over any additional
 * alphabetic characters (allowing long request names) and any
 * subsequent spaces.  The first non-space character encountered is
 * stored in ``ch`` for the caller.
 */
void skipcont(void) {
  int c;

  /* Skip over alphabetic continuation characters. */
  while ((c = getchar()) != EOF && isalpha(c))
    ;

  /* Consume trailing spaces. */
  while (c == ' ')
    c = getchar();

  ch = c;
}
