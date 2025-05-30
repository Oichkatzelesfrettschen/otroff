#include "cxx23_scaffold.hpp" // Modern C++23 enforcement
#include <array>
#include <string_view>
#include <span>
#include <optional>
#include <concepts>
#include <sstream>
#include <iomanip>

namespace otroff::terminal {

/**
 * @brief VT220 Terminal Driver for nroff/troff text formatting
 *
 *Provides comprehensive VT220 terminal support with ANSI X3.64 escape sequences,
 * DEC special characters, and proper C++23 safety guarantees.
 */
class VT220Terminal {
  public:
    // Type-safe constants
    static constexpr int INCH = 240;
    static constexpr int CHARS_PER_INCH = 10;
    static constexpr int LINES_PER_INCH = 6;
    static constexpr int HALFLINES_PER_INCH = 12;
    static constexpr size_t ASCII_PRINTABLE_START = 32;
    static constexpr size_t ASCII_EXTENDED_END = 256;
    static constexpr size_t CODETAB_SIZE = ASCII_EXTENDED_END - ASCII_PRINTABLE_START;

    // Terminal capabilities structure with strong typing
    struct Capabilities {
        int bset{0};
        int breset{0};
        int horizontal_resolution{INCH / CHARS_PER_INCH};
        int vertical_resolution{INCH / LINES_PER_INCH};
        int newline_spacing{INCH / LINES_PER_INCH};
        int char_width{INCH / CHARS_PER_INCH};
        int em_width{INCH / CHARS_PER_INCH};
        int halfline_spacing{INCH / HALFLINES_PER_INCH};
        int adjustment_spacing{INCH / CHARS_PER_INCH};

        constexpr bool operator==(const Capabilities &) const noexcept = default;
    };

    // Control sequences with semantic names
    struct ControlSequences {
        std::string_view init{"\033[?7h\033[?1h\033[?3l\033[?4l\033[?5l\033[?6l\033[?8h\033[?25h"};
        std::string_view reset{"\033c\033[!p"};
        std::string_view newline{"\r\n"};
        std::string_view halfline_reverse{"\033M"};
        std::string_view halfline_forward{"\033D"};
        std::string_view fullline_reverse{"\033M"};
        std::string_view bold_on{"\033[1m"};
        std::string_view bold_off{"\033[0m"};
        std::string_view plot_on{""};
        std::string_view plot_off{""};
        std::string_view cursor_up{"\033[A"};
        std::string_view cursor_down{"\033[B"};
        std::string_view cursor_right{"\033[C"};
        std::string_view cursor_left{"\033[D"};

        constexpr bool operator==(const ControlSequences &) const noexcept = default;
    };

    // Character encoding entry with width and representation
    struct CharacterEntry {
        uint8_t width;
        std::string_view representation;

        constexpr CharacterEntry() noexcept : width(0), representation("") {}
        constexpr CharacterEntry(uint8_t w, std::string_view repr) noexcept
            : width(w), representation(repr) {}

        constexpr bool operator==(const CharacterEntry &) const noexcept = default;
    };

    using CodeTable = std::array<CharacterEntry, CODETAB_SIZE>;

  public:
    constexpr VT220Terminal() noexcept = default;

    // Accessors with compile-time safety
    [[nodiscard]] constexpr const Capabilities &capabilities() const noexcept {
        return capabilities_;
    }

    [[nodiscard]] constexpr const ControlSequences &control_sequences() const noexcept {
        return sequences_;
    }

    [[nodiscard]] constexpr std::span<const CharacterEntry> code_table() const noexcept {
        return code_table_;
    }

    // Character lookup with bounds checking
    [[nodiscard]] std::optional<CharacterEntry> get_character(unsigned char ch) const noexcept {
        if (ch < ASCII_PRINTABLE_START) {
            return std::nullopt;
        }

        const size_t index = ch - ASCII_PRINTABLE_START;
        if (index >= CODETAB_SIZE) {
            return std::nullopt;
        }

        return code_table_[index];
    }

    // Format character with safety
    [[nodiscard]] std::string format_character(unsigned char ch) const {
        if (const auto entry = get_character(ch)) {
            return std::string{entry->representation};
            std::ostringstream oss;
            oss << "\\" << std::oct << std::setfill('0') << std::setw(3) << static_cast<int>(ch);
            return oss.str(); // Octal escape for invalid chars
            return std::format("\\{:03o}", ch); // Octal escape for invalid chars
        }

        // Terminal initialization and control
        [[nodiscard]] std::string initialize() const {
            return std::string{sequences_.init};
        }

        [[nodiscard]] std::string reset() const {
            return std::string{sequences_.reset};
        }

        if (dy > 0) {
            result += "\033[" + std::to_string(dy) + "B";
        } else if (dy < 0) {
            result += "\033[" + std::to_string(-dy) + "A";
        }

        if (dx > 0) {
            result += "\033[" + std::to_string(dx) + "C";
        } else if (dx < 0) {
            result += "\033[" + std::to_string(-dx) + "D";
        }
    }
    else if (dx < 0) {
        result += std::format("\033[{}D", -dx);
    }

    return result;
}

// Validation
[[nodiscard]] constexpr bool
is_valid() const noexcept {
    return capabilities_.horizontal_resolution > 0 &&
           capabilities_.vertical_resolution > 0 &&
           capabilities_.char_width > 0;
}

private:
static constexpr Capabilities capabilities_{};
static constexpr ControlSequences sequences_{};
static constexpr CodeTable code_table_ = create_code_table();

// Constexpr code table creation
static constexpr CodeTable create_code_table() noexcept {
    CodeTable table{};

    // ASCII printable characters (32-126)
    constexpr const char *ascii_chars =
        " !\"#$%&'()*+,-./0123456789:;<=>?@"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~";

    size_t i = 0;

    // Standard ASCII characters
    for (char ch : ascii_chars) {
        table[i++] = CharacterEntry{1, std::string_view(&ch, 1)};
    }

    // Extended characters and special symbols
    constexpr std::array special_chars{
        CharacterEntry{0, "\0"}, // narrow space
        CharacterEntry{1, "-"}, // hyphen
        CharacterEntry{1, "\u00B0"}, // bullet/degree
        CharacterEntry{1, "\u00B1"}, // square/plus-minus
        CharacterEntry{1, "-"}, // 3/4 em
        CharacterEntry{1, "_"}, // rule
        CharacterEntry{3, "1/4"}, // 1/4 fraction
        CharacterEntry{3, "1/2"}, // 1/2 fraction
        CharacterEntry{3, "3/4"}, // 3/4 fraction
        CharacterEntry{1, "-"}, // minus
        CharacterEntry{2, "fi"}, // fi ligature
        CharacterEntry{2, "fl"}, // fl ligature
        CharacterEntry{2, "ff"}, // ff ligature
        CharacterEntry{3, "ffi"}, // ffi ligature
        CharacterEntry{3, "ffl"}, // ffl ligature
        CharacterEntry{1, "\u00B0"}, // degree
        CharacterEntry{1, "\u2020"}, // dagger
        CharacterEntry{1, "\u00A7"}, // section
        CharacterEntry{1, "'"}, // foot mark
        CharacterEntry{1, "\u00B4"}, // acute accent
        CharacterEntry{1, "`"}, // grave accent
        CharacterEntry{1, "_"}, // underrule
        CharacterEntry{1, "/"}, // slash
        CharacterEntry{0, ""}, // half narrow space
        CharacterEntry{1, " "}, // unpaddable space
    };

    // Copy special characters
    for (const auto &entry : special_chars) {
        if (i < CODETAB_SIZE) {
            table[i++] = entry;
        }
    }

    // Greek letters and mathematical symbols
    constexpr std::array math_chars{
        CharacterEntry{1, "\u03B1"}, // alpha
        CharacterEntry{1, "\u03B2"}, // beta
        CharacterEntry{1, "\u03B3"}, // gamma
        CharacterEntry{1, "\u03B4"}, // delta
        CharacterEntry{1, "\u03B5"}, // epsilon
        CharacterEntry{1, "\u03B6"}, // zeta
        CharacterEntry{1, "\u03B7"}, // eta
        CharacterEntry{1, "\u03B8"}, // theta
        CharacterEntry{1, "\u03B9"}, // iota
        CharacterEntry{1, "\u03BA"}, // kappa
        CharacterEntry{1, "\u03BB"}, // lambda
        CharacterEntry{1, "\u03BC"}, // mu
        CharacterEntry{1, "\u03BD"}, // nu
        CharacterEntry{1, "\u03BE"}, // xi
        CharacterEntry{1, "\u03BF"}, // omicron
        CharacterEntry{1, "\u03C0"}, // pi
        CharacterEntry{1, "\u03C1"}, // rho
        CharacterEntry{1, "\u03C3"}, // sigma
        CharacterEntry{1, "\u03C4"}, // tau
        CharacterEntry{1, "\u03C5"}, // upsilon
        CharacterEntry{1, "\u03C6"}, // phi
        CharacterEntry{1, "\u03C7"}, // chi
        CharacterEntry{1, "\u03C8"}, // psi
        CharacterEntry{1, "\u03C9"}, // omega
    };

    // Copy mathematical characters
    for (const auto &entry : math_chars) {
        if (i < CODETAB_SIZE) {
            table[i++] = entry;
        }
    }

    // Fill remaining entries with safe defaults
    while (i < CODETAB_SIZE) {
        table[i++] = CharacterEntry{1, "?"};
    }

    return table;
}
};

// Factory function for easy instantiation
[[nodiscard]] constexpr VT220Terminal create_vt220_terminal() noexcept {
    return VT220Terminal{};
}

// Concepts for terminal compatibility
template <typename T>
concept TerminalDriver = requires(T t) {
    { t.capabilities() } -> std::same_as<const typename T::Capabilities &>;
    { t.control_sequences() } -> std::same_as<const typename T::ControlSequences &>;
    { t.initialize() } -> std::convertible_to<std::string>;
    { t.reset() } -> std::convertible_to<std::string>;
    { t.is_valid() } -> std::same_as<bool>;
};

static_assert(TerminalDriver<VT220Terminal>);

} // namespace otroff::terminal
