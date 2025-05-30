#include "os/os_abstraction.h"
#include <stdio.h>

/* Stub implementations generated for missing PDP-11 assembly functions */

/* Placeholder get character; returns EOF until implemented. */
int getchar_roff(void) { return EOF; }

/* Placeholder put character; currently discards output. */
void putchar_roff(int c) { (void)c; }

/* Flush input stream; no-op stub. */
void flushi(void) {}

/* Write top and bottom margins; not implemented. */
void topbot(void) {}

/* Process header at input; stub ignoring pointer. */
void headin(char **p) { (void)p; }

/* Output header; stub ignoring pointer. */
void headout(char **p) { (void)p; }

/* Set the number of lines and spacing; stub. */
void nlines(int count, int spacing) {
    (void)count;
    (void)spacing;
}

/* Switch to next input file; stub returns 0 to signal end. */
int nextfile(void) { return 0; }

/* Read character with roff semantics; stub returns EOF. */
int gettchar(void) { return EOF; }

/* Flush output; stub performs no work. */
void flush(void) {}

/* Determine if character is alphabetic. */
int alph(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

/* Secondary alphabetic check alias. */
int alph2(int c) { return alph(c); }
