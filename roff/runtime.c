#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

/*
 * Replacement implementations for small helper routines found in the
 * original PDP-11 assembly sources.  These do not attempt to reproduce
 * every side effect, only enough behaviour for demonstration purposes.
 */

/*
 * Disable or restore write permission on the controlling terminal in
 * a manner similar to the original `mesg` routine.
 */
void mesg(int enable)
{
    char *tty = ttyname(STDOUT_FILENO);
    if (!tty)
        return;

    struct stat st;
    if (stat(tty, &st) == -1)
        return;

    mode_t mode = st.st_mode;
    if (!enable)
        mode &= ~(S_IWGRP | S_IWOTH);
    /* else leave mode unchanged */
    chmod(tty, mode);
}

/*
 * Compute spacing to the next tab stop.  The PDP-11 version stored the
 * current column in the global variable ``ocol``; here it is passed in
 * explicitly and the distance to the next multiple of eight is returned.
 */
int dsp(int column)
{
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
 * Write the contents of ``buf`` to stdout and reset the pointer ``p``.
 */
void flush_output(char *buf, size_t *p)
{
    if (*p) {
        write(STDOUT_FILENO, buf, *p);
        *p = 0;
    }
}
