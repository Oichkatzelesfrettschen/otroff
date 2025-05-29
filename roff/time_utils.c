#include "time_utils.h"
#include <time.h>

/*
 * Retrieve the current wall clock time.
 *
 * This small wrapper exists so that callers do not depend directly on
 * the C library's time function.  It also makes it trivial to substitute
 * a mocked implementation during testing if desired.
 */
time_t current_time(void) {
    return time(NULL);
}
