#include "cxx17_scaffold.hpp"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h> /* defines mode_t */
#include <sys/stat.h>
#include <cstring>
#include "runtime.hpp" // This might need namespacing if it declares things used here without qualification
#include "roff.hpp"    // For access to other roff_legacy items if needed, and for consistency

namespace otroff {
namespace roff_legacy {

// Using directive for convenience within this file
using namespace otroff::roff_legacy;

/*
 * Replacement implementations for small helper routines found in the
 * original PDP-11 assembly sources.  These do not attempt to reproduce
 * every side effect, only enough behaviour for demonstration purposes.
 */

/*
 * Toggle write permission on the controlling terminal.
 * Mirrors the behaviour of the historic mesg(1) command by
 * adjusting group/world access bits.
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
 * Calculate the distance to the next 8-column tab stop from ``column``.
 * The original PDP-11 implementation stored this value in ``ocol``; here
 * it is supplied directly as an argument.
 */
[[nodiscard]] int dsp(int column) noexcept {
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
 * Write the buffer contents to stdout and reset ``p`` so that new data
 * overwrites the previous output.
 */
void flush_output(char *buf, size_t *p) noexcept {
    if (*p) {
        write(STDOUT_FILENO, buf, *p);
        *p = 0;
    }
}

} // namespace roff_legacy
} // namespace otroff
