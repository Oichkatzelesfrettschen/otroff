#include "cxx23_scaffold.hpp"
#include <cctype> // for std::isalpha and std::tolower
#include "hyphen_utils.h"

/* Simple helpers used by the old hyphenation code. */

/*
 * Determine if character ``c`` is punctuation.
 */
[[nodiscard]] constexpr int punct(int c) noexcept {
    if (c == 0)
        return 0;
    return !std::isalpha(static_cast<unsigned char>(c));
}

[[nodiscard]] constexpr int vowel(int c) noexcept {
    c = std::tolower(static_cast<unsigned char>(c));
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}
