#include "cxx23_scaffold.hpp"
#include "hyphen_utils.hpp"

#include <cctype>

// Simple helpers used by the old hyphenation code.
namespace roff::util {

/*
 * Determine if character ``c`` is punctuation.
 */
[[nodiscard]] constexpr bool punct(int c) noexcept {
    if (c == 0)
        return false;
    return !std::isalpha(static_cast<unsigned char>(c));
}

[[nodiscard]] constexpr bool vowel(int c) noexcept {
    c = std::tolower(static_cast<unsigned char>(c));
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}

} // namespace roff::util
