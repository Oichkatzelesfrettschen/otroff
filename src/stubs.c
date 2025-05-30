#include "os/os_abstraction.h"
#include <stdio.h>

/* Stub implementations generated for missing PDP-11 assembly functions */
int getchar_roff(void) { return EOF; }
void putchar_roff(int c) { (void)c; }
void flushi(void) {}
void topbot(void) {}
void headin(char **p) { (void)p; }
void headout(char **p) { (void)p; }
void nlines(int count, int spacing) { (void)count; (void)spacing; }
int nextfile(void) { return 0; }
int gettchar(void) { return EOF; }
void flush(void) {}
int alph(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
int alph2(int c) { return alph(c); }
