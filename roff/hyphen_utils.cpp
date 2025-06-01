#include "cxx17_scaffold.hpp"
#include "hyphen_utils.hpp"

#include <cctype>
#include <ranges>
#include <algorithm> // Required for std::find

// Simple helpers used by the old hyphenation code.
namespace otroff::roff_legacy::util { // Changed namespace

/*
 * Determine if character ``c`` is punctuation.
 */
// Return true when ``c`` represents a punctuation character.
[[nodiscard]] constexpr bool punct(int c) noexcept {
    if (c == 0)
        return false; // Zero terminator is never punctuation
    return !std::isalpha(static_cast<unsigned char>(c));
}

// Return true when ``c`` is contained within ``vowel_table``.
[[nodiscard]] constexpr bool vowel(int c) noexcept {
    if (c == 0)
        return false; // Zero terminator is not a vowel
    const char lower =
        static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return std::find(vowel_table.begin(), vowel_table.end(), lower) != vowel_table.end();
}

} // namespace otroff::roff_legacy::util
