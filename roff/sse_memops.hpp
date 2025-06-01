/* Header guard to prevent multiple inclusions. */
#pragma once
#include "cxx17_scaffold.hpp" // utilities

#include <cstddef>

#include <span>

namespace roff::util {

// Portable memory operations using modern C++ spans.
[[nodiscard]] void *fast_memcpy(void *dst, const void *src, std::size_t n);

[[nodiscard]] int fast_memcmp(const void *s1, const void *s2, std::size_t n);

} // namespace roff::util

