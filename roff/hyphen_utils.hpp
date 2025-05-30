#ifndef HYPHEN_UTILS_H
#define HYPHEN_UTILS_H
#include "cxx23_scaffold.hpp"

[[nodiscard]] constexpr int punct(int c) noexcept; // punctuation check
[[nodiscard]] constexpr int vowel(int c) noexcept; // vowel check

#endif /* HYPHEN_UTILS_H */
