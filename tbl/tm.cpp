#include "../cxx17_scaffold.hpp"
/* tm.c: split numerical fields */
#include "tbl.hpp"

namespace tbl {
/* Split a numeric string into two fields. */
char *maknew(char *str) {
    /* make two numerical fields */
    char *dpoint, c; // Changed dpoint to char*
    char *p, *q, *ba;
    p = str;
    for (ba = nullptr; c = *str; str++) // Initialize ba to nullptr
        if (c == '\\' && *(str + 1) == '&')
            ba = str;
    str = p;
    if (ba == nullptr) { // Compare ba with nullptr
        for (dpoint = nullptr; *str; str++) // Initialize dpoint to nullptr
            if (*str == '.' &&
                (str > p && digit(*(str - 1)) ||
                 digit(*(str + 1))))
                dpoint = str;
        if (!dpoint && *(str - 1) == '$')
            return (nullptr); // Return nullptr
        if (dpoint == nullptr) // Compare dpoint with nullptr
            for (; str > p; str--) {
                if (digit(*(str - 1)))
                    break;
            }
        if (!dpoint && p == str) /* not numerical, don't split */
            return (nullptr); // Return nullptr
        if (dpoint)
            str = dpoint;
    } else
        str = ba;
    p = str;
    if (exstore == nullptr || exstore > exlim) { // Compare exstore with nullptr
        exstore = chspace();
        exlim = exstore + MAXCHS;
    }
    q = exstore;
    while (*exstore++ = *str++)
        ;
    *p = 0;
    return (q);
}
} // namespace tbl
