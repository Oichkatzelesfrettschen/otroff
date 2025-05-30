#ifndef HYPHEN_UTILS_H
#define HYPHEN_UTILS_H

#include "cxx23_scaffold.hpp"

namespace roff::util {

[[nodiscard]] constexpr bool punct(int c) noexcept; // punctuation check
[[nodiscard]] constexpr bool vowel(int c) noexcept; // vowel check

} // namespace roff::util

#endif // HYPHEN_UTILS_H
