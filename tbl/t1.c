/* C17 - no scaffold needed */
/* t1.c: main control and input switching */
#include "tbl.h"
#include <stdio.h> /* fopen, fprintf */
#include <stdlib.h> /* exit */
#include <signal.h> /* signal */
#define BROKPIPE 13

#if unix
#define MACROS "/usr/lib/tmac.s"
#endif

#if gcos
#define MACROS "cc/troff/smac"
#endif
#ifndef MACROS
#define MACROS "/usr/lib/tmac.s"
#endif

int tbl(int argc, char **argv);
void setinp(int argc, char **argv);
int swapin(void);
int badsig(int signo);
#define ever (;;)
/* Entry point. */
int main(int argc, char *argv[]) {
#if gcos
    if (!intss())
        tabout = fopen("qq", "w"); /* default media code is type 5 */
#endif
#if unix
    signal(BROKPIPE, badsig);
#endif
    tabin = stdin;
    tabout = stdout;
    exit(tbl::tbl(argc, argv));
}

/* Process command line arguments and tables in input files. */
int tbl(int argc, char **argv) {
    char line[512];
    setinp(argc, argv);
    while (gets1(line)) {
        fprintf(tabout, "%s\n", line);
        if (prefix(".TS", line))
            tableput();
    }
    fclose(tabin);
    return (0);
}
int sargc;
char **sargv;
/* Initialize input file pointers. */
void setinp(int argc, char **argv) {
    sargc = argc;
    sargv = argv;
    sargc--;
    sargv++;
    if (sargc > 0)
        swapin();
}
/* Switch to the next input file. */
int swapin(void) {
    if (sargc <= 0)
        return (0);
    if (match("-ms", *sargv))
        *sargv = MACROS;
    if (tabin != stdin)
        fclose(tabin);
    tabin = fopen(ifile = *sargv, "r");
    iline = 1;
    fprintf(tabout, ".ds f. %s\n", ifile);
    if (tabin == NULL)
        error("Can't open file");
    sargc--;
    sargv++;
    return (1);
}
#if unix
/* Clean exit on broken pipe. */
extern "C" int badsig(int signo) {
    signal(BROKPIPE, SIG_DFL); // Corrected: should be SIG_DFL or a handler, not 1
    exit(0);
}
#endif
