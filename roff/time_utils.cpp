#include "cxx23_scaffold.hpp"
#include "time_utils.h"
#include <chrono> // C++23 time utilities

/*
 * Return the current wall-clock time in seconds since the Epoch.
 */
[[nodiscard]] time_t current_time(void) {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
