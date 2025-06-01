#include "cxx17_scaffold.hpp"
#include "time_utils.hpp"

#include <chrono>

// Implementation of modern time helper routines.
namespace otroff::roff_legacy::utils { // Changed namespace

// Using the outer namespace for consistency, though not strictly needed here
using namespace otroff::roff_legacy;

/// Obtain the current system time with second precision.
[[nodiscard]] sys_seconds current_time() noexcept {
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now());
}

} // namespace otroff::roff_legacy::utils
