#define _POSIX_C_SOURCE 200809L /* expose mkstemp */

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Helpers translated from runtime.c */
void mesg(int enable);
int dsp(int column);
void flush_output(char *buf, size_t *p);

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
static int slow = 1; /* ``slow`` */
static int pfrom; /* ``pfrom`` */
static int pto; /* ``pto`` */

/* temporary buffer from ``makebf`` */
static char tmp_name[] = "roffbufXXXXXX";
static int tmp_fd = -1;

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
    int i; /* loop counter for table initialisation */
    char buf[256]; /* temporary line buffer */
    size_t pos = 0; /* current index into ``buf`` */
    int c; /* character being processed */

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

    /* trivial processing loop */
    while ((c = getchar()) != EOF) {
        buf[pos++] = (char)c;
        if (c == '\n' || pos >= sizeof(buf))
            flush_output(buf, &pos);
    }
    flush_output(buf, &pos);

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

/*
 * control -- stub matching the historic label `control` in roff1.s.
 * The original parsed a request name and dispatched to a handler.
 */
void control(void) { puts("[stub] control"); }
