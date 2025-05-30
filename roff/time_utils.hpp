#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include "cxx23_scaffold.hpp"

#include <chrono>

// Modern time alias for convenience
namespace roff::utils {
using sys_seconds =
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

// Return the current time as a std::chrono::sys_seconds value.
[[nodiscard]] sys_seconds current_time() noexcept;
} // namespace roff::utils

#endif // TIME_UTILS_H
