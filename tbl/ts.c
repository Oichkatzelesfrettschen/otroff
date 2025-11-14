/* C17 - no scaffold needed */
/* ts.c: minor string processing subroutines */
#include "tbl.h" /* shared table data and prototypes */

/* Compare two strings for equality. */
int match(char *s1, char *s2) {
    while (*s1 == *s2)
        if (*s1++ == '\0')
            return (1);
        else
            s2++;
    return (0);
}
/* Test if 'small' is a prefix of 'big'. */
int prefix(char *small, char *big) {
    int c;
    while ((c = *small++) == *big++)
        if (c == 0)
            return (1);
    return (c == 0);
}
/* Return true if character is alphabetic. */
int letter(int ch) {
    if (ch >= 'a' && ch <= 'z')
        return (1);
    if (ch >= 'A' && ch <= 'Z')
        return (1);
    return (0);
}
/* Convert string to integer. */
int numb(char *str) {
    int k;
    for (k = 0; *str >= '0' && *str <= '9'; str++)
        k = k * 10 + *str - '0';
    return (k);
}
/* True if character is a digit. */
int digit(int x) {
    return (x >= '0' && x <= '9');
}
/* Return maximum of two integers. */
int max(int a, int b) {
    return (a > b ? a : b);
}
/* Find character position in string. */
int index(char *s, int c) {
    int k;
    for (k = 0; s[k]; k++)
        if (s[k] == c)
            return (k);
    return (-1);
}
/* Copy string t to s. */
void tcopy(char *s, char *t) {
    while (*s++ = *t++)
        ;
}
