#include "../../cxx17_scaffold.hpp"
/*
 * tabvt100.cpp - VT100 terminal driver tables (C++23)
 *
 * Modern C++23 implementation of VT100 terminal capabilities and control sequences.
 * Provides comprehensive terminal abstraction with type safety and performance.
 *
 * Features:
 *   - Complete VT100/ANSI escape sequence support
 *   - Type-safe terminal capability queries
 *   - Efficient character mapping and rendering
 *   - Extended Unicode and special character support
 *   - Comprehensive error handling and validation
 */

#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <optional>
#include <span>
#include <ranges>
#include <format>
#include <concepts>
#include <type_traits>
#include <cassert>

namespace croff::terminal::vt100 {

// Terminal resolution constants
inline constexpr std::int32_t INCH = 240;
inline constexpr std::int32_t CHARS_PER_INCH = 10;
inline constexpr std::int32_t LINES_PER_INCH = 6;
inline constexpr std::int32_t HALFLINES_PER_INCH = 12;

// Character code table size (printable ASCII + extended)
inline constexpr std::size_t CODETAB_SIZE = 256 - 32;
inline constexpr std::size_t ASCII_PRINTABLE_START = 32;

// Terminal capability flags
enum class TerminalFlags : std::uint32_t {
    None = 0,
    BoldSupport = 1 << 0,
    UnderlineSupport = 1 << 1,
    ReverseVideoSupport = 1 << 2,
    CursorPositioning = 1 << 3,
    ScrollRegion = 1 << 4,
    InsertDeleteLine = 1 << 5,
    InsertDeleteChar = 1 << 6,
    ColorSupport = 1 << 7
};

constexpr TerminalFlags operator|(TerminalFlags a, TerminalFlags b) noexcept {
    return static_cast<TerminalFlags>(
        static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

constexpr TerminalFlags operator&(TerminalFlags a, TerminalFlags b) noexcept {
    return static_cast<TerminalFlags>(
        static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
}

constexpr bool has_flag(TerminalFlags flags, TerminalFlags flag) noexcept {
    return (flags & flag) != TerminalFlags::None;
}

// Character encoding information
struct CharacterInfo {
    std::string_view sequence;
    std::uint8_t width;
    bool is_combining{false};
    bool is_control{false};

    constexpr CharacterInfo(std::string_view seq, std::uint8_t w) noexcept
        : sequence(seq), width(w) {}

    constexpr CharacterInfo(std::string_view seq, std::uint8_t w, bool combining, bool control = false) noexcept
        : sequence(seq), width(w), is_combining(combining), is_control(control) {}
};

// Terminal capabilities and control sequences
class TerminalTable {
  public:
    // Terminal configuration
    TerminalFlags capabilities;
    std::int32_t horizontal_resolution; // Units per character width
    std::int32_t vertical_resolution; // Units per line height
    std::int32_t newline_spacing; // Units per newline
    std::int32_t character_width; // Units per character
    std::int32_t em_width; // Units per em
    std::int32_t halfline_spacing; // Units per halfline
    std::int32_t adjustment_spacing; // Units for fine adjustment

    // Control sequences
    std::string_view terminal_init; // Terminal initialization
    std::string_view terminal_reset; // Terminal reset
    std::string_view newline; // Newline sequence
    std::string_view halfline_reverse; // Halfline up
    std::string_view halfline_forward; // Halfline down
    std::string_view fullline_reverse; // Full line up
    std::string_view bold_on; // Enable bold
    std::string_view bold_off; // Disable bold
    std::string_view underline_on; // Enable underline
    std::string_view underline_off; // Disable underline
    std::string_view reverse_on; // Enable reverse video
    std::string_view reverse_off; // Disable reverse video
    std::string_view plot_on; // Enter plot mode
    std::string_view plot_off; // Exit plot mode
    std::string_view cursor_up; // Move cursor up
    std::string_view cursor_down; // Move cursor down
    std::string_view cursor_right; // Move cursor right
    std::string_view cursor_left; // Move cursor left
    std::string_view cursor_home; // Move cursor to home
    std::string_view clear_screen; // Clear entire screen
    std::string_view clear_line; // Clear current line

    // Character encoding table
    std::array<CharacterInfo, CODETAB_SIZE> character_table;

    constexpr TerminalTable() noexcept = default;

    // Query capabilities
    constexpr bool supports(TerminalFlags flag) const noexcept {
        return has_flag(capabilities, flag);
    }

    // Get character information
    constexpr std::optional<CharacterInfo> get_character_info(unsigned char c) const noexcept {
        if (c < ASCII_PRINTABLE_START || c >= 256) {
            return std::nullopt;
        }
        return character_table[c - ASCII_PRINTABLE_START];
    }

    // Calculate string width in terminal units
    constexpr std::int32_t calculate_width(std::string_view text) const noexcept {
        std::int32_t total_width = 0;
        for (char c : text) {
            if (auto info = get_character_info(static_cast<unsigned char>(c))) {
                if (!info->is_combining) {
                    total_width += info->width * character_width;
                }
            }
        }
        return total_width;
    }

    // Format cursor positioning sequence
    std::string format_cursor_position(std::int32_t row, std::int32_t col) const {
        return std::format("\033[{};{}H", row + 1, col + 1);
    }

    // Format color sequence (if supported)
    std::string format_color(std::uint8_t fg, std::uint8_t bg = 0) const {
        if (!supports(TerminalFlags::ColorSupport)) {
            return "";
        }
        if (bg == 0) {
            return std::format("\033[{}m", 30 + fg);
        }
        return std::format("\033[{};{}m", 30 + fg, 40 + bg);
    }
};

// VT100 terminal table instance
inline constexpr TerminalTable vt100_table = {
    .capabilities = TerminalFlags::BoldSupport | TerminalFlags::CursorPositioning |
                    TerminalFlags::ScrollRegion | TerminalFlags::InsertDeleteLine,
    .horizontal_resolution = INCH / CHARS_PER_INCH,
    .vertical_resolution = INCH / LINES_PER_INCH,
    .newline_spacing = INCH / LINES_PER_INCH,
    .character_width = INCH / CHARS_PER_INCH,
    .em_width = INCH / CHARS_PER_INCH,
    .halfline_spacing = INCH / HALFLINES_PER_INCH,
    .adjustment_spacing = INCH / CHARS_PER_INCH,

    // VT100 control sequences
    .terminal_init = "\033[?3l\033[?4l\033[?5l\033[?7h\033[?8h\033[2J\033[H",
    .terminal_reset = "\033c",
    .newline = "\r\n",
    .halfline_reverse = "\033M",
    .halfline_forward = "\033D",
    .fullline_reverse = "\033M",
    .bold_on = "\033[1m",
    .bold_off = "\033[0m",
    .underline_on = "\033[4m",
    .underline_off = "\033[24m",
    .reverse_on = "\033[7m",
    .reverse_off = "\033[27m",
    .plot_on = "",
    .plot_off = "",
    .cursor_up = "\033[A",
    .cursor_down = "\033[B",
    .cursor_right = "\033[C",
    .cursor_left = "\033[D",
    .cursor_home = "\033[H",
    .clear_screen = "\033[2J",
    .clear_line = "\033[K",

    // Comprehensive character table
    .character_table = {{// Basic ASCII printable characters (32-126)
                         CharacterInfo{" ", 1}, // space
                         CharacterInfo{"!", 1}, // !
                         CharacterInfo{"\"", 1}, // "
                         CharacterInfo{"#", 1}, // #
                         CharacterInfo{"$", 1}, // $
                         CharacterInfo{"%", 1}, // %
                         CharacterInfo{"&", 1}, // &
                         CharacterInfo{"'", 1}, // '
                         CharacterInfo{"(", 1}, // (
                         CharacterInfo{")", 1}, // )
                         CharacterInfo{"*", 1}, // *
                         CharacterInfo{"+", 1}, // +
                         CharacterInfo{",", 1}, // ,
                         CharacterInfo{"-", 1}, // -
                         CharacterInfo{".", 1}, // .
                         CharacterInfo{"/", 1}, // /
                         CharacterInfo{"0", 1}, // 0
                         CharacterInfo{"1", 1}, // 1
                         CharacterInfo{"2", 1}, // 2
                         CharacterInfo{"3", 1}, // 3
                         CharacterInfo{"4", 1}, // 4
                         CharacterInfo{"5", 1}, // 5
                         CharacterInfo{"6", 1}, // 6
                         CharacterInfo{"7", 1}, // 7
                         CharacterInfo{"8", 1}, // 8
                         CharacterInfo{"9", 1}, // 9
                         CharacterInfo{":", 1}, // :
                         CharacterInfo{";", 1}, // ;
                         CharacterInfo{"<", 1}, // <
                         CharacterInfo{"=", 1}, // =
                         CharacterInfo{">", 1}, // >
                         CharacterInfo{"?", 1}, // ?
                         CharacterInfo{"@", 1}, // @
                         CharacterInfo{"A", 1}, // A
                         CharacterInfo{"B", 1}, // B
                         CharacterInfo{"C", 1}, // C
                         CharacterInfo{"D", 1}, // D
                         CharacterInfo{"E", 1}, // E
                         CharacterInfo{"F", 1}, // F
                         CharacterInfo{"G", 1}, // G
                         CharacterInfo{"H", 1}, // H
                         CharacterInfo{"I", 1}, // I
                         CharacterInfo{"J", 1}, // J
                         CharacterInfo{"K", 1}, // K
                         CharacterInfo{"L", 1}, // L
                         CharacterInfo{"M", 1}, // M
                         CharacterInfo{"N", 1}, // N
                         CharacterInfo{"O", 1}, // O
                         CharacterInfo{"P", 1}, // P
                         CharacterInfo{"Q", 1}, // Q
                         CharacterInfo{"R", 1}, // R
                         CharacterInfo{"S", 1}, // S
                         CharacterInfo{"T", 1}, // T
                         CharacterInfo{"U", 1}, // U
                         CharacterInfo{"V", 1}, // V
                         CharacterInfo{"W", 1}, // W
                         CharacterInfo{"X", 1}, // X
                         CharacterInfo{"Y", 1}, // Y
                         CharacterInfo{"Z", 1}, // Z
                         CharacterInfo{"[", 1}, // [
                         CharacterInfo{"\\", 1}, // backslash
                         CharacterInfo{"]", 1}, // ]
                         CharacterInfo{"^", 1}, // ^
                         CharacterInfo{"_", 1}, // _
                         CharacterInfo{"`", 1}, // `
                         CharacterInfo{"a", 1}, // a
                         CharacterInfo{"b", 1}, // b
                         CharacterInfo{"c", 1}, // c
                         CharacterInfo{"d", 1}, // d
                         CharacterInfo{"e", 1}, // e
                         CharacterInfo{"f", 1}, // f
                         CharacterInfo{"g", 1}, // g
                         CharacterInfo{"h", 1}, // h
                         CharacterInfo{"i", 1}, // i
                         CharacterInfo{"j", 1}, // j
                         CharacterInfo{"k", 1}, // k
                         CharacterInfo{"l", 1}, // l
                         CharacterInfo{"m", 1}, // m
                         CharacterInfo{"n", 1}, // n
                         CharacterInfo{"o", 1}, // o
                         CharacterInfo{"p", 1}, // p
                         CharacterInfo{"q", 1}, // q
                         CharacterInfo{"r", 1}, // r
                         CharacterInfo{"s", 1}, // s
                         CharacterInfo{"t", 1}, // t
                         CharacterInfo{"u", 1}, // u
                         CharacterInfo{"v", 1}, // v
                         CharacterInfo{"w", 1}, // w
                         CharacterInfo{"x", 1}, // x
                         CharacterInfo{"y", 1}, // y
                         CharacterInfo{"z", 1}, // z
                         CharacterInfo{"{", 1}, // {
                         CharacterInfo{"|", 1}, // |
                         CharacterInfo{"}", 1}, // }
                         CharacterInfo{"~", 1}, // ~

                         // Extended and special characters (127+)
                         CharacterInfo{"", 0}, // narrow space
                         CharacterInfo{"-", 1}, // hyphen
                         CharacterInfo{"•", 1}, // bullet
                         CharacterInfo{"□", 1}, // square
                         CharacterInfo{"—", 1}, // 3/4 em dash
                         CharacterInfo{"_", 1}, // rule
                         CharacterInfo{"¼", 1}, // 1/4
                         CharacterInfo{"½", 1}, // 1/2
                         CharacterInfo{"¾", 1}, // 3/4
                         CharacterInfo{"−", 1}, // minus
                         CharacterInfo{"ﬁ", 2}, // fi ligature
                         CharacterInfo{"ﬂ", 2}, // fl ligature
                         CharacterInfo{"ﬀ", 2}, // ff ligature
                         CharacterInfo{"ﬃ", 3}, // ffi ligature
                         CharacterInfo{"ﬄ", 3}, // ffl ligature
                         CharacterInfo{"°", 1}, // degree
                         CharacterInfo{"†", 1}, // dagger
                         CharacterInfo{"§", 1}, // section
                         CharacterInfo{"′", 1}, // foot mark
                         CharacterInfo{"´", 1}, // acute accent
                         CharacterInfo{"`", 1}, // grave accent
                         CharacterInfo{"_", 1}, // underrule
                         CharacterInfo{"⁄", 1}, // fraction slash
                         CharacterInfo{"", 0}, // half narrow space
                         CharacterInfo{" ", 1}, // unpaddable space

                         // Greek letters (lowercase)
                         CharacterInfo{"α", 1}, // alpha
                         CharacterInfo{"β", 1}, // beta
                         CharacterInfo{"γ", 1}, // gamma
                         CharacterInfo{"δ", 1}, // delta
                         CharacterInfo{"ε", 1}, // epsilon
                         CharacterInfo{"ζ", 1}, // zeta
                         CharacterInfo{"η", 1}, // eta
                         CharacterInfo{"θ", 1}, // theta
                         CharacterInfo{"ι", 1}, // iota
                         CharacterInfo{"κ", 1}, // kappa
                         CharacterInfo{"λ", 1}, // lambda
                         CharacterInfo{"μ", 1}, // mu
                         CharacterInfo{"ν", 1}, // nu
                         CharacterInfo{"ξ", 1}, // xi
                         CharacterInfo{"ο", 1}, // omicron
                         CharacterInfo{"π", 1}, // pi
                         CharacterInfo{"ρ", 1}, // rho
                         CharacterInfo{"σ", 1}, // sigma
                         CharacterInfo{"τ", 1}, // tau
                         CharacterInfo{"υ", 1}, // upsilon
                         CharacterInfo{"φ", 1}, // phi
                         CharacterInfo{"χ", 1}, // chi
                         CharacterInfo{"ψ", 1}, // psi
                         CharacterInfo{"ω", 1}, // omega

                         // Greek letters (uppercase)
                         CharacterInfo{"Γ", 1}, // Gamma
                         CharacterInfo{"Δ", 1}, // Delta
                         CharacterInfo{"Θ", 1}, // Theta
                         CharacterInfo{"Λ", 1}, // Lambda
                         CharacterInfo{"Ξ", 1}, // Xi
                         CharacterInfo{"Π", 1}, // Pi
                         CharacterInfo{"Σ", 1}, // Sigma
                         CharacterInfo{"", 0}, // unused
                         CharacterInfo{"Υ", 1}, // Upsilon
                         CharacterInfo{"Φ", 1}, // Phi
                         CharacterInfo{"Ψ", 1}, // Psi
                         CharacterInfo{"Ω", 1}, // Omega

                         // Mathematical symbols
                         CharacterInfo{"√", 1}, // square root
                         CharacterInfo{"ς", 1}, // terminal sigma
                         CharacterInfo{"∛", 1}, // cube root
                         CharacterInfo{"≥", 2}, // greater than or equal
                         CharacterInfo{"≤", 2}, // less than or equal
                         CharacterInfo{"≡", 2}, // identically equal
                         CharacterInfo{"−", 1}, // equation minus
                         CharacterInfo{"≈", 2}, // approximately equal
                         CharacterInfo{"∼", 1}, // similar to
                         CharacterInfo{"≠", 2}, // not equal
                         CharacterInfo{"→", 2}, // right arrow
                         CharacterInfo{"←", 2}, // left arrow
                         CharacterInfo{"↑", 1}, // up arrow
                         CharacterInfo{"↓", 1}, // down arrow
                         CharacterInfo{"=", 1}, // equation equal
                         CharacterInfo{"×", 1}, // multiply
                         CharacterInfo{"÷", 1}, // divide
                         CharacterInfo{"±", 1}, // plus-minus
                         CharacterInfo{"∪", 1}, // union
                         CharacterInfo{"∩", 1}, // intersection
                         CharacterInfo{"⊂", 1}, // subset
                         CharacterInfo{"⊃", 1}, // superset
                         CharacterInfo{"⊆", 1}, // subset or equal
                         CharacterInfo{"⊇", 1}, // superset or equal
                         CharacterInfo{"∞", 2}, // infinity
                         CharacterInfo{"∂", 1}, // partial derivative
                         CharacterInfo{"∇", 1}, // gradient
                         CharacterInfo{"¬", 1}, // not
                         CharacterInfo{"∫", 1}, // integral
                         CharacterInfo{"∝", 1}, // proportional to
                         CharacterInfo{"∅", 1}, // empty set
                         CharacterInfo{"∈", 1}, // element of
                         CharacterInfo{"+", 1}, // equation plus

                         // Misc symbols
                         CharacterInfo{"®", 1}, // registered
                         CharacterInfo{"©", 1}, // copyright
                         CharacterInfo{"│", 1}, // box rule
                         CharacterInfo{"¢", 1}, // cent
                         CharacterInfo{"‡", 1}, // double dagger
                         CharacterInfo{"☞", 1}, // right hand
                         CharacterInfo{"☜", 1}, // left hand
                         CharacterInfo{"∗", 1}, // math star
                         CharacterInfo{"🔔", 1}, // bell
                         CharacterInfo{"∨", 1}, // logical or
                         CharacterInfo{"○", 1}, // circle
                         CharacterInfo{"┌", 1}, // box drawing
                         CharacterInfo{"└", 1}, // box drawing
                         CharacterInfo{"┐", 1}, // box drawing
                         CharacterInfo{"┘", 1}, // box drawing
                         CharacterInfo{"├", 1}, // box drawing
                         CharacterInfo{"┤", 1}, // box
