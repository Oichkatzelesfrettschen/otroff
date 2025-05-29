#include <ctype.h>

/* Simple helpers used by the old hyphenation code. */

int punct(int c) {
    if (c == 0)
        return 0;
    return !isalpha(c);
}

int vowel(int c) {
    c = tolower(c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}
