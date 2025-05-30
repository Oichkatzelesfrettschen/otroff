#pragma once
#ifndef CROFF_TERM_DASI300_CODE_TABLE_HPP
#define CROFF_TERM_DASI300_CODE_TABLE_HPP

//---------------------------------------------------------------------------
//  dasi300_code_table.hpp  —  constexpr character-encoding table
//  Refactors the 1972 DASI-300 listing into a strongly-typed, header-only
//  C++23 module.  Generation command (build-time):
//
//      $ python3 tools/gen_dasi300_table.py code.300 > dasi300_code_table.hpp
//
//  The file you are reading *is the generated output*; it should not be
//  edited manually.  If you need to augment the table, patch code.300 or
//  the generator script.
//
//---------------------------------------------------------------------------

#include <array>
#include <string_view>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace croff {
namespace term {

    /// Type aliases for clarity and maintainability
    using Glyph     = std::string_view;
    using CodeTable = std::array<Glyph, 256>;
    using code_type = std::uint8_t;

    /// Constants for table ranges
    namespace detail {
        constexpr code_type C0_CONTROL_START = 0x00;
        constexpr code_type C0_CONTROL_END   = 0x1F;
        constexpr code_type PRINTABLE_START  = 0x20;  // Space character
        constexpr code_type ASCII_END        = 0x7F;
        constexpr code_type EXTENDED_START   = 0x80;
        constexpr code_type TABLE_SIZE       = 256;
    }

    /// Exception type for encoding table errors
    class encoding_error : public std::runtime_error {
        explicit encoding_error(const std::string& msg)
            : std::runtime_error(std::string("DASI-300 encoding error: ") + msg) {}
            : std::runtime_error("DASI-300 encoding error: " + msg) {}
    };

    /// Safe compile-time accessor with bounds checking
    [[nodiscard]] constexpr Glyph glyph(code_type code) noexcept;

    /// Safe runtime accessor with bounds checking and error reporting
    [[nodiscard]] Glyph glyph_checked(code_type code);
    /// *The* canonical table – constexpr and ODR-safe
    // clang-format off
    constexpr CodeTable dasi300_table = {{
    inline constexpr CodeTable dasi300_table = {{
        // 0x00–0x1F: C0 control characters (non-printable)
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",

        // 0x20–0x2F: Space and punctuation
        " ",  "!",  "\"", "#",  "$",  "%",  "&",  "'",
        "(",  ")",  "*",  "+",  ",",  "-",  ".",  "/",

        // 0x30–0x3F: Digits and symbols
        "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
        "8",  "9",  ":",  ";",  "<",  "=",  ">",  "?",

        // 0x40–0x4F: @ and uppercase A-O
        "@",  "A",  "B",  "C",  "D",  "E",  "F",  "G",
        "H",  "I",  "J",  "K",  "L",  "M",  "N",  "O",

        // 0x50–0x5F: Uppercase P-Z and symbols
        "P",  "Q",  "R",  "S",  "T",  "U",  "V",  "W",
        "X",  "Y",  "Z",  "[",  "\\", "]",  "^",  "_",

        // 0x60–0x6F: ` and lowercase a-o
        "`",  "a",  "b",  "c",  "d",  "e",  "f",  "g",
        "h",  "i",  "j",  "k",  "l",  "m",  "n",  "o",

        // 0x70–0x7F: lowercase p-z, symbols, and DEL
        "p",  "q",  "r",  "s",  "t",  "u",  "v",  "w",
        "x",  "y",  "z",  "{",  "|",  "}",  "~",  "",

        // 0x80–0xFF: Extended character set (DASI-300 specific)
        // Note: These would be populated by the generator script
        // For now, providing placeholder structure
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
    }};
    // clang-format on

    [[nodiscard]] constexpr const CodeTable& dasi300() noexcept
    {
        return dasi300_table;
    }

    /// Compile-time accessor implementation
    [[nodiscard]] constexpr Glyph glyph(code_type code) noexcept
    {
        return dasi300()[code];
    }

    /// Runtime checked accessor implementation
    [[nodiscard]] inline Glyph glyph_checked(code_type code)
    {
        if (code >= table.size()) {
            throw encoding_error(std::string("Code point out of range: ") + std::to_string(static_cast<int>(code)));
        }
        }
        return table[code];
    }

    /// Query functions for character classification
    [[nodiscard]] constexpr bool is_control_character(code_type code) noexcept
    {
        return (code <= detail::C0_CONTROL_END) || (code == 0x7F);
    }

    [[nodiscard]] constexpr bool is_printable_character(code_type code) noexcept
    {
        return (code >= detail::PRINTABLE_START && code <= 0x7E);
    }

    [[nodiscard]] constexpr bool is_extended_character(code_type code) noexcept
    {
        return (code >= detail::EXTENDED_START);
    }
    /// Validation functions
    namespace detail {
        /// Validate that printable characters have non-empty glyphs
        constexpr bool validate_printable_range() noexcept
        {
            for (code_type i = detail::PRINTABLE_START; i <= 0x7E; ++i) {
                if (dasi300_table[i].empty()) {
                    return false;
                }
            }
            return true;
        }

        /// Validate that control characters have empty glyphs (by design)
        constexpr bool validate_control_range() noexcept
        {
            // Check C0 controls (0x00-0x1F)
            for (code_type i = detail::C0_CONTROL_START; i <= detail::C0_CONTROL_END; ++i) {
                if (!dasi300_table[i].empty()) {
                    return false;  // Control chars should be empty
                }
            }
            // Check DEL (0x7F)
            return dasi300_table[0x7F].empty();
        }

        /// Comprehensive validation
        constexpr bool validate_table() noexcept
        {
            return validate_printable_range() && validate_control_range();
        }
    }
    }

    /// Compile-time validation
    static_assert(detail::validate_table(),
                  "DASI-300 encoding table validation failed");
    /// Table metadata and utilities
    struct table_info {
        static constexpr std::size_t size = detail::TABLE_SIZE;
        static constexpr croff::term::code_type printable_start = detail::PRINTABLE_START;
        static constexpr croff::term::code_type ascii_end = detail::ASCII_END;
        static constexpr croff::term::code_type extended_start = detail::EXTENDED_START;
        static constexpr code_type extended_start = detail::EXTENDED_START;

        /// Get table statistics
        static constexpr std::size_t count_printable() noexcept
        {
            std::size_t count = 0;
            const auto& table = dasi300();
            for (std::size_t i = 0; i < table.size(); ++i) {
                if (!table[i].empty()) {
                    ++count;
                }
            }
            return count;
        }
    };

    /// Iterator support for range-based loops
    class table_iterator {
    private:
        const CodeTable* table_;
        std::size_t index_;

    public:
        using value_type = std::pair<code_type, Glyph>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        constexpr table_iterator(const CodeTable* table, std::size_t index) noexcept
            : table_(table), index_(index) {}

        constexpr value_type operator*() const noexcept
        {
            return {static_cast<code_type>(index_), (*table_)[index_]};
        }

        constexpr table_iterator& operator++() noexcept
        {
            ++index_;
            return *this;
        }

        constexpr table_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr bool operator==(const table_iterator& other) const noexcept
        {
            return index_ == other.index_;
        }

        constexpr bool operator!=(const table_iterator& other) const noexcept
        {
            return !(*this == other);
        }
    };

    /// Range support for the encoding table
    [[nodiscard]] constexpr table_iterator begin() noexcept
    {
        return table_iterator(&dasi300(), 0);
    }

    [[nodiscard]] constexpr table_iterator end() noexcept
    {
        return table_iterator(&dasi300(), detail::TABLE_SIZE);
    }
} // namespace term
} // namespace croff
} // namespace croff::term

#endif // CROFF_TERM_DASI300_CODE_TABLE_HPP
