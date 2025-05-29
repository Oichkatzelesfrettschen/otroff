#include "time_utils.h"
#include <time.h>

/*
 * Return the current wall-clock time in seconds since the Epoch.
 */
time_t current_time(void) {
    return time(NULL);
}
