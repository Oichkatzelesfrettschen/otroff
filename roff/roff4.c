#include <stdio.h>

/* Placeholder translations of roff4.s which dealt with low level text
 * processing.  Only minimal routines are provided. */

void text_line(const char *s)
{
    /* In a full implementation this would perform filling and
       justification.  Here we simply echo the line. */
    fputs(s, stdout);
}

void nline(void)
{
    putchar('\n');
}
