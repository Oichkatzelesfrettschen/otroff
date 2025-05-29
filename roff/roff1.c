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
    mesg(0); /* disable messages to tty */
    signal(SIGINT, cleanup); /* jump to ``place`` on signals */
    signal(SIGQUIT, cleanup);

    makebf();
    load_suffixes();

    if (argc <= 1)
        cleanup(0);
    parse_args(argc, argv);

    /* build identity translation table */
    int i;
    for (i = 0; i < 128; ++i)
        trtab[i] = (unsigned char)i;

    /* trivial processing loop */
    char buf[256];
    size_t pos = 0;
    int c;
    while ((c = getchar()) != EOF) {
        buf[pos++] = (char)c;
        if (c == '\n' || pos >= sizeof(buf))
            flush_output(buf, &pos);
    }
    flush_output(buf, &pos);

    cleanup(0);
    return 0;
}
