#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include "cxx23_scaffold.hpp"
#include <chrono>

#include <time.h>

/* Return the current time as a time_t value. */
[[nodiscard]] time_t current_time(void); // current wall-clock time

#endif /* TIME_UTILS_H */
