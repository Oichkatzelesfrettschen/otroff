#include "cxx23_scaffold.hpp"
#include "time_utils.hpp"

#include <chrono>

// Implementation of modern time helper routines.
namespace roff::utils {

[[nodiscard]] sys_seconds current_time() noexcept {
    return std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now());
}

} // namespace roff::utils
