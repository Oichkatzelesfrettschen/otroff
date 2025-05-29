#include <stdio.h>
#include <ctype.h>

/* Additional formatter logic originally in roff3.s.  This translation
 * implements a couple of helpers used by the request stubs. */

/* Return non-zero if c is an alphabetic character. */
int alph(int c)
{
    return isalpha(c);
}

/* Convert a character to lower case. */
int maplow(int c)
{
    return tolower((unsigned char)c);
}

/* Placeholder for line break logic. */
void rbreak(void)
{
    printf("[stub] rbreak called\n");
}
