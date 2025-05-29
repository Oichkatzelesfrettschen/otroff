#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Very small subset of the original hyphenation driver found in
 * roff5.s.  This uses the helper functions from hyphen_utils.c along
 * with the digram tables in roff7.c. */

int punct(int c);  /* from hyphen_utils.c */
int vowel(int c);

/* Simplified hyphenation check: return 1 if the word contains a vowel
 * that isn't the first or last character.  */
int hyphenate(const char *w)
{
    size_t len = strlen(w);
    size_t i;

    if (len <= 2)
        return 0;

    for (i = 1; i + 1 < len; ++i)
        if (vowel(w[i]))
            return 1;
    return 0;
}

void do_hyphen(const char *w)
{
    if (hyphenate(w))
        printf("%s-\n", w);
    else
        puts(w);
}
