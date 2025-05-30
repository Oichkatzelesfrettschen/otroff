#include "cxx23_scaffold.hpp"
#include <ctype.h>
#include "hyphen_utils.h"

/* Simple helpers used by the old hyphenation code. */

/*
 * Determine if character ``c`` is punctuation.
 */
int punct(int c) {
    if (c == 0)
        return 0;
    return !isalpha((unsigned char)c);
}

int vowel(int c) {
    c = tolower((unsigned char)c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}
