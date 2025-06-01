#include "cxx17_scaffold.hpp"
#include "sse_memops.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <span>

/*
 * Portable C versions of the fast memory routines that were
 * previously implemented in assembly.  These wrappers simply
 * forward to the standard library implementations but are
 * provided as normal functions so the rest of the codebase
 * does not rely on assembly sources.
 */
namespace otroff::roff_legacy::util { // Changed namespace

// Using the outer namespace for consistency, though not strictly needed here
using namespace otroff::roff_legacy;

void *fast_memcpy(void *dst, const void *src, std::size_t n) {
    std::span<std::byte> d{static_cast<std::byte *>(dst), n};
    std::span<const std::byte> s{static_cast<const std::byte *>(src), n};
    std::ranges::copy(s, d.begin());
    return dst;
}

int fast_memcmp(const void *s1, const void *s2, std::size_t n) {
    std::span<const std::byte> a{static_cast<const std::byte *>(s1), n};
    std::span<const std::byte> b{static_cast<const std::byte *>(s2), n};
    auto mismatch = std::ranges::mismatch(a, b);
    if (mismatch.in1 == a.end())
        return 0;
    return static_cast<int>(*mismatch.in1) - static_cast<int>(*mismatch.in2);
}

} // namespace otroff::roff_legacy::util
