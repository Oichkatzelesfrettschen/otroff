#include "time_utils.h"
#include <time.h>

/*
 * Return the current time as a time_t value.
 */
time_t current_time(void) {
    return time(NULL);
}
