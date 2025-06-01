#include "../cxx17_scaffold.hpp"
/* te.c: error message control, input line count */
#include "tbl.hpp"
#include <cstdlib> /* exit */
#include <cstdio> /* fprintf */

namespace tbl {
/* Report an error and exit. */
void error(char *s) {
    fprintf(stderr, "\n%s: line %d: %s\n", ifile, iline, s);
#ifdef unix
    fprintf(stderr, "tbl quits\n");
    exit(1);
#endif
#ifdef gcos
    fprintf(stderr, "run terminated due to error condition detected by tbl preprocessor\n");
    exit(0);
#endif
}
/* Get a line from input with continuation handling. */
char *gets1(char *s) {
    char *p;
    iline++;
    p = fgets(s, 512, tabin);
    if (p == nullptr) {
        if (swapin() == 0 || (p = fgets(s, 512, tabin)) == nullptr)
            return (nullptr); // Return nullptr for char*
    }
    while (*s)
        s++;
    s--;
    if (*s == '\n')
        *s-- = 0;
    if (*s == '\\')
        gets1(s);
    return (p);
}
#define BACKMAX 500
char backup[BACKMAX];
char *backp = backup; // Corrected syntax
/* Push a character back to the input stream. */
void un1getc(int c) {
    if (c == '\n')
        iline--;
    *backp++ = c;
    if (backp >= backup + BACKMAX)
        error("too much backup");
}
/* Retrieve a single character from input. */
int get1char(void) {
    int c;
    if (backp > backup)
        c = *--backp;
    else
        c = getc(tabin);
    if (c == EOF) /* EOF */
    {
        if (swapin() == 0)
            error("unexpected EOF", 0);
        c = getc(tabin);
    }
    if (c == '\n')
        iline++;
    return (c);
}
} // namespace tbl
