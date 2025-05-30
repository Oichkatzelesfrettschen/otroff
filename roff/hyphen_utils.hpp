#ifndef HYPHEN_UTILS_H
#define HYPHEN_UTILS_H

#include "cxx23_scaffold.hpp"

#include <array>
#include <cctype>
#include <ranges>

namespace roff::util {

// Predefined table of vowel characters for quick lookup.
inline constinit const std::array<char, 6> vowel_table{
    'a', 'e', 'i', 'o', 'u', 'y'};

// Determine whether ``c`` is punctuation.
[[nodiscard]] constexpr bool punct(int c) noexcept;

// Determine whether ``c`` is a vowel using ``vowel_table``.
[[nodiscard]] constexpr bool vowel(int c) noexcept;

} // namespace roff::util

#endif // HYPHEN_UTILS_H
