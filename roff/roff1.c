#define _POSIX_C_SOURCE 200809L /* expose mkstemp */

#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "runtime.h"

int mkstemp(char *);
void control(void);

/* request handler prototypes from roff2.c */
void casead(void);
void casebp(void);
void casebr(void);
void casecc(void);
void casece(void);
void caseds(void);
void casefi(void);
void casein(void);
void caseix(void);
void caseli(void);
void casell(void);
void casels(void);
void casena(void);
void casene(void);
void casenf(void);
void casepa(void);
void casebl(void);
void casepl(void);
void casesk(void);
void casesp(void);
void casess(void);
void caseta(void);
void caseti(void);
void casetr(void);
void caseul(void);
void caseun(void);
void casehe(void);
void casehx(void);
void casefo(void);
void caseeh(void);
void caseoh(void);
void caseef(void);
void caseof(void);
void casem1(void);
void casem2(void);
void casem3(void);
void casem4(void);
void casehc(void);
void casehy(void);
void casen1(void);
void casen2(void);
void casenn(void);
void caseni(void);
void casejo(void);
void casear(void);
void casero(void);
void casenx(void);
void casepo(void);
void casede(void);
void caseig(void);
void casetc(void);
void casemk(void);

/* low level helpers from other translation units */
void rbreak(void);
static void istop(void);
void text_line(const char *s);

/*
 * roff1.c - Partial translation of the PDP-11 module "roff1.s".
 *
 * The original assembly began at label ``ibuf`` and executed a series of
 * start-up routines.  This file implements those routines in C, mapping
 * each function to its historic label for reference.
 */

/* suffix table loaded from "suffil" during start-up */
static unsigned short suftab[26];

/* translation table built after initialisation (label ``trtab``) */
static unsigned char trtab[128];

/* flags roughly matching the assembly globals */
static int stop_flag; /* ``stop`` */
extern int slow;      /* output pacing flag from roff8.c */
extern int pfrom;     /* page range start from croff */
extern int pto;       /* page range end from roff8.c */
extern char cc;       /* command character */
extern char obuf[];   /* output buffer */
extern int pn;        /* current page number */
extern int ch;        /* pushback character from roff3.c */

/* working state used by the character level routines */
static int nlflg;  /* newline flag */
static int column; /* current input column */
static int nspace; /* pending spaces from input */
static int nsp;    /* spaces waiting for output */
static int ocol;   /* current output column */

/* temporary buffer from ``makebf`` */
static char tmp_name[] = "roffbufXXXXXX";
static int tmp_fd = -1;

/* local output buffer tracking */
static size_t obuf_len;

static void flush(void) { flush_output(obuf, &obuf_len); }

/* output a single character using the translation table and spacing rules */
static void roff_putchar(int c) {
    if (pn < pfrom)
        return; /* outside page range */
    if (pn >= pfrom)
        pfrom = 0; /* page range satisfied */

    c &= 0177;
    if (c == 0)
        return;

    c = trtab[c];

    if (c == ' ') {
        nsp++;
        return;
    }

    while (nsp > 0) {
        obuf[obuf_len++] = ' ';
        ocol++;
        if (obuf_len >= 128)
            flush();
        nsp--;
    }

    obuf[obuf_len++] = (char)c;
    if (c == '\n') {
        flush();
        ocol = 0;
    } else {
        if (obuf_len >= 128)
            flush();
        ocol++;
    }
}

/* check for user stop request */
static void istop(void) {
    if (stop_flag && pn >= pfrom)
        flush();
}

/* discard characters up to and including newline */
static void flushi(void) {
    int c;
    ch = 0;
    if (nlflg)
        return;
    while ((c = getchar()) != EOF && c != '\n')
        ;
    nlflg = 1;
}

/* simplified gettchar -- underline logic omitted */
static int gettchar(void) { return getchar(); }

/* cleanup -- corresponds to label ``place`` */
static void cleanup(int sig) {
    (void)sig;
    mesg(1);
    if (tmp_fd != -1) {
        close(tmp_fd);
        unlink(tmp_name);
    }
    exit(0);
}

/* makebf -- create temporary buffer file */
static void makebf(void) {
    tmp_fd = mkstemp(tmp_name);
    if (tmp_fd == -1) {
        perror("mkstemp");
        exit(1);
    }
}

/* load_suffixes -- part of the ``ibuf`` start-up */
static void load_suffixes(void) {
    int fd = open("suffil", O_RDONLY);
    if (fd == -1)
        return;
    lseek(fd, 020, SEEK_SET); /* seek 20 bytes like the original */
    (void)read(fd, suftab, sizeof(suftab));
    close(fd);
}

/* parse_args -- translate argument handling from ``ibuf`` */
static void parse_args(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; ++i) {
        char *a = argv[i];
        if (a[0] == '+') {
            pfrom = atoi(a + 1);
            continue;
        }
        if (a[0] == '-') {
            if (a[1] == 's') {
                stop_flag = 1;
                continue;
            }
            if (a[1] == 'h') {
                slow = 0;
                continue;
            }
            pto = atoi(a + 1);
            continue;
        }
    }
}

/* main -- entry translated from label ``ibuf`` */
int main(int argc, char **argv) {
    int i;  /* loop counter for table initialisation */
    int c;  /* character being processed */

    mesg(0); /* disable messages to tty */
    signal(SIGINT, cleanup); /* jump to ``place`` on signals */
    signal(SIGQUIT, cleanup);

    makebf();
    load_suffixes();

    if (argc <= 1)
        cleanup(0);
    parse_args(argc, argv);

    /* build identity translation table */
    for (i = 0; i < 128; ++i)
        trtab[i] = (unsigned char)i;

    rbreak();
    istop();

    while ((c = gettchar()) != EOF) {
        nlflg = 0;
        if (c == cc) {
            control();
            flushi();
            continue;
        }
        roff_putchar(c);
    }

    flush();

    cleanup(0);
    return 0;
}

/* ---------------------------------------------------------------------- */
/* Escape lookup tables lifted from ``roff1.s`` (labels ``esctab`` and
 * ``pfxtab``).  These values map escape sequences to internal codes.
 */
static const unsigned char esctab[] = {
    'd', 032, 'u', 035, 'r', 036, 'x', 016, 'y', 017, 'l', 0177,
    't', 011, 'a', 0100, 'n', 043, '\\', 134, 0, 0};

static const unsigned char pfxtab[] = {'7', 036, '8', 035, '9', 032,
                                       '4', 030, '3', 031, '1', 026,
                                       '2', 027, 0, 0};

/* switch -- approximate translation of label ``switch:``.  Given a
 * character and a two byte table, return the mapped value or 037 if the
 * character is not present in ``pfxtab``.  ``esctab`` returns zero on
 * failure in the original code, which we mimic here.
 */
static int switch_code(int c, const unsigned char *tab) {
    const unsigned char *p = tab;
    while (p[0] && p[0] != (unsigned char)c)
        p += 2;
    if (!p[0])
        return tab == pfxtab ? 037 : 0;
    return p[1];
}

/* mapping between request names and handler functions */
struct request {
    const char name[3];
    void (*func)(void);
};

static const struct request contab[] = {
    {"ad", casead}, {"bp", casebp}, {"br", casebr}, {"cc", casecc},
    {"ce", casece}, {"ds", caseds}, {"fi", casefi}, {"in", casein},
    {"ix", caseix}, {"li", caseli}, {"ll", casell}, {"ls", casels},
    {"na", casena}, {"ne", casene}, {"nf", casenf}, {"pa", casepa},
    {"bl", casebl}, {"pl", casepl}, {"sk", casesk}, {"sp", casesp},
    {"ss", casess}, {"ta", caseta}, {"ti", caseti}, {"tr", casetr},
    {"ul", caseul}, {"un", caseun}, {"he", casehe}, {"hx", casehx},
    {"fo", casefo}, {"eh", caseeh}, {"oh", caseoh}, {"ef", caseef},
    {"of", caseof}, {"m1", casem1}, {"m2", casem2}, {"m3", casem3},
    {"m4", casem4}, {"hc", casehc}, {"hy", casehy}, {"n1", casen1},
    {"n2", casen2}, {"nn", casenn}, {"ni", caseni}, {"jo", casejo},
    {"ar", casear}, {"ro", casero}, {"nx", casenx}, {"po", casepo},
    {"de", casede}, {"ig", caseig}, {"tc", casetc}, {"mk", casemk},
    {"", NULL}}
;

/* parse two-character request and dispatch to handler */
void control(void) {
    int c1 = getchar();
    int c2 = getchar();
    const struct request *r;

    if (c1 == EOF || c2 == EOF)
        return;

    for (r = contab; r->func; ++r)
        if (r->name[0] == c1 && r->name[1] == c2) {
            r->func();
            return;
        }
}
