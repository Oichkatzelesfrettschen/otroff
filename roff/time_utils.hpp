/// \file time_utils.hpp
/// \brief Utility functions for working with system time.

#pragma once

#include "cxx17_scaffold.hpp"

#include <chrono>

namespace otroff::roff_legacy::utils { // Changed namespace

/// Alias for system clock points with second precision.
using sys_seconds =
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

/// Return the current time as a ``sys_seconds`` value.
[[nodiscard]] sys_seconds current_time() noexcept;

} // namespace otroff::roff_legacy::utils
