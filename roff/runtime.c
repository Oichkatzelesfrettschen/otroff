#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* defines mode_t */
#include <sys/stat.h>
#include <string.h>

/*
 * Replacement implementations for small helper routines found in the
 * original PDP-11 assembly sources.  These do not attempt to reproduce
 * every side effect, only enough behaviour for demonstration purposes.
 */

/*
 * Disable or restore write permission on the controlling terminal.
 *
 * The historical implementation toggled permissions via assembly code.
 * This function performs the same job by inspecting the current terminal
 * device and adjusting its mode bits.  If the terminal cannot be
 * determined the call simply returns.
 */
void mesg(int enable) {
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
 * Compute spacing to the next tab stop.
 *
 * The PDP-11 assembler used a global ``ocol`` variable to track output
 * columns.  Here the current column is passed as an argument and the
 * distance to the next 8-character boundary is returned.
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
 * Write buffered output and reset the write pointer.
 *
 * ``buf`` contains pending output and ``p`` tracks the current length.
 * When any data is buffered, it is written to STDOUT and ``p`` is reset
 * to zero so that callers can reuse the buffer.
 */
void flush_output(char *buf, size_t *p) {
    if (*p) {
        write(STDOUT_FILENO, buf, *p);
        *p = 0;
    }
}
