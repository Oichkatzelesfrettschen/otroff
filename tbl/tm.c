/* C17 - no scaffold needed */
/* tm.c: split numerical fields */
#include "tbl.h"

/* Split a numeric string into two fields. */
char *maknew(char *str) {
    /* make two numerical fields */
    char *dpoint, c; // Changed dpoint to char*
    char *p, *q, *ba;
    p = str;
    for (ba = NULL; c = *str; str++) // Initialize ba to NULL
        if (c == '\\' && *(str + 1) == '&')
            ba = str;
    str = p;
    if (ba == NULL) { // Compare ba with NULL
        for (dpoint = NULL; *str; str++) // Initialize dpoint to NULL
            if (*str == '.' &&
                (str > p && digit(*(str - 1)) ||
                 digit(*(str + 1))))
                dpoint = str;
        if (!dpoint && *(str - 1) == '$')
            return (NULL); // Return NULL
        if (dpoint == NULL) // Compare dpoint with NULL
            for (; str > p; str--) {
                if (digit(*(str - 1)))
                    break;
            }
        if (!dpoint && p == str) /* not numerical, don't split */
            return (NULL); // Return NULL
        if (dpoint)
            str = dpoint;
    } else
        str = ba;
    p = str;
    if (exstore == NULL || exstore > exlim) { // Compare exstore with NULL
        exstore = chspace();
        exlim = exstore + MAXCHS;
    }
    q = exstore;
    while (*exstore++ = *str++)
        ;
    *p = 0;
    return (q);
}
