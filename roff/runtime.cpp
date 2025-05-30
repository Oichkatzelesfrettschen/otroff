#include "cxx23_scaffold.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* defines mode_t */
#include <sys/stat.h>
#include <string.h>
#include "runtime.h"

/*
 * Replacement implementations for small helper routines found in the
 * original PDP-11 assembly sources.  These do not attempt to reproduce
 * every side effect, only enough behaviour for demonstration purposes.
 */

/*
 * Toggle write permission on the controlling terminal.  This mirrors the
 * behaviour of the historic mesg(1) command by adjusting group/world access.
 */
void mesg(int enable) {
    char *tty = ttyname(STDOUT_FILENO);
    struct stat st;
    mode_t mode;

    if (!tty)
        return;

    if (stat(tty, &st) == -1)
        return;

    mode = st.st_mode;
    if (!enable)
        mode &= ~(S_IWGRP | S_IWOTH);
    /* else leave mode unchanged */
    chmod(tty, mode);
}

/*
 * Calculate the distance to the next 8-column tab stop from the supplied
 * column position.  The PDP-11 code kept this in ``ocol``; here it is
 * provided as an argument.
 */
int dsp(int column) {
    int r = 0;
    do {
        r += 8;
    } while (column > r);
    r -= column;
    if (r == 0)
        r = 8;
    return r;
}

/*
 * Write the buffer contents to stdout and clear the index ``p`` so that
 * new data overwrites the previous output.
 */
void flush_output(char *buf, size_t *p) {
    if (*p) {
        write(STDOUT_FILENO, buf, *p);
        *p = 0;
    }
}
