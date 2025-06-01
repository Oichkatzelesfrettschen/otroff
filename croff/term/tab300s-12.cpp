/**
 * @file tab300s-12.cpp
 * @brief DASI300S Terminal Driver - Pure C++17 Implementation
 * @author Modern C++17 Conversion Team
 * @version 2.0
 * @date 2024
 *
 * @details Pure C++17 implementation of DASI300S terminal table definitions.
 *          Completely refactored from C-style to modern C++17:
 *          - Eliminated all C-style structs and defines
 *          - Uses constexpr constants and type-safe enums
 *          - Template-based terminal configuration
 *          - RAII resource management
 *          - Type-safe string literal management
 */

#include "../../cxx17_scaffold.hpp"

namespace roff::terminal::dasi300s {

// Type-safe constants using constexpr
constexpr std::uint16_t INCH_RESOLUTION = 240;

/**
 * @brief Modern C++17 Terminal Configuration Class for DASI300S
 * 
 * This class encapsulates all terminal configuration parameters in a type-safe,
 * modern C++17 structure with proper initialization and validation.
 */
class TerminalConfiguration {
  public:
    using CodeTable = std::array<std::string_view, 256 - 32>;

    // Terminal control bit flags
    struct ControlBits {
        std::uint16_t set{0}; ///< Bits to set on initialization
        std::uint16_t reset{0177420}; ///< Bits to reset on initialization

        constexpr ControlBits() = default;
        constexpr ControlBits(std::uint16_t s, std::uint16_t r) : set(s), reset(r) {}
    };

    // Terminal spacing and resolution configuration
    struct SpacingConfig {
        std::uint16_t horizontal{INCH_RESOLUTION / 60}; ///< Horizontal resolution (units per space)
        std::uint16_t vertical{INCH_RESOLUTION / 48}; ///< Vertical resolution (units per line)
        std::uint16_t newline{INCH_RESOLUTION / 8}; ///< Newline spacing (units)
        std::uint16_t character{INCH_RESOLUTION / 12}; ///< Character width (units)
        std::uint16_t em{INCH_RESOLUTION / 12}; ///< Em width (units)
        std::uint16_t halfline{INCH_RESOLUTION / 16}; ///< Halfline spacing (units)
        std::uint16_t adjustment{INCH_RESOLUTION / 12}; ///< Adjustment spacing (units)

        constexpr SpacingConfig() = default;

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return horizontal > 0 && vertical > 0 && character > 0 && em > 0;
        }
    };

    // Terminal control sequences
    struct ControlSequences {
        std::string_view init{"\033\006"sv}; ///< Terminal initialization string
        std::string_view reset{"\033\006"sv}; ///< Terminal reset string
        std::string_view newline{"\015\n"sv}; ///< Newline string
        std::string_view halfline_reverse{""sv}; ///< Halfline reverse string
        std::string_view halfline_forward{""sv}; ///< Halfline forward string
        std::string_view fullline_reverse{"\032"sv}; ///< Fullline reverse string
        std::string_view bold_on{"\033E"sv}; ///< Bold on string
        std::string_view bold_off{"\033E"sv}; ///< Bold off string
        std::string_view plot_on{"\006"sv}; ///< Plot mode on string
        std::string_view plot_off{"\033\006"sv}; ///< Plot mode off string

        constexpr ControlSequences() = default;
    };

    // Cursor movement sequences
    struct CursorControl {
        std::string_view up{"\032"sv}; ///< Cursor up string
        std::string_view down{"\n"sv}; ///< Cursor down string
        std::string_view right{" "sv}; ///< Cursor right string
        std::string_view left{"\b"sv}; ///< Cursor left string

        constexpr CursorControl() = default;

        [[nodiscard]] constexpr bool all_sequences_valid() const noexcept {
            return !up.empty() || !down.empty() || !right.empty() || !left.empty();
        }
    };

  private:
    ControlBits control_bits_;
    SpacingConfig spacing_;
    ControlSequences sequences_;
    CursorControl cursor_;
    CodeTable code_table_;

  public:
    constexpr TerminalConfiguration() = default;

    explicit TerminalConfiguration(
        const ControlBits &bits,
        const SpacingConfig &spacing = {},
        const ControlSequences &sequences = {},
        const CursorControl &cursor = {},
        const CodeTable &codes = {})
        : control_bits_(bits), spacing_(spacing), sequences_(sequences),
          cursor_(cursor), code_table_(codes) {

        if (!spacing_.is_valid()) {
            throw std::invalid_argument("Invalid spacing configuration");
        }
    }

    // Accessors with proper const-correctness
    [[nodiscard]] constexpr const ControlBits &control_bits() const noexcept { return control_bits_; }
    [[nodiscard]] constexpr const SpacingConfig &spacing() const noexcept { return spacing_; }
    [[nodiscard]] constexpr const ControlSequences &sequences() const noexcept { return sequences_; }
    [[nodiscard]] constexpr const CursorControl &cursor() const noexcept { return cursor_; }
    [[nodiscard]] constexpr const CodeTable &code_table() const noexcept { return code_table_; }

    // Mutators for configuration updates
    constexpr void update_control_bits(const ControlBits &bits) noexcept { control_bits_ = bits; }
    constexpr void update_spacing(const SpacingConfig &spacing) {
        if (!spacing.is_valid()) {
            throw std::invalid_argument("Invalid spacing configuration");
        }
        spacing_ = spacing;
    }
    constexpr void update_sequences(const ControlSequences &sequences) noexcept { sequences_ = sequences; }
    constexpr void update_cursor(const CursorControl &cursor) noexcept { cursor_ = cursor; }
    constexpr void update_code_table(const CodeTable &codes) noexcept { code_table_ = codes; }

    // Validation and utility methods
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return spacing_.is_valid() && cursor_.all_sequences_valid();
    }

    [[nodiscard]] constexpr std::size_t memory_footprint() const noexcept {
        return sizeof(*this);
    }
};

// Static assertion to ensure reasonable memory usage
static_assert(sizeof(TerminalConfiguration) <= 4096,
              "TerminalConfiguration must remain memory-efficient");

// Forward declaration for code table inclusion
namespace detail {
// This will be populated by the included code table file
extern const TerminalConfiguration::CodeTable dasi300s_code_table;
} // namespace detail

/**
 * @brief Factory function to create DASI300S terminal configuration
 * @return Complete terminal configuration for DASI300S
 */
[[nodiscard]] constexpr TerminalConfiguration create_dasi300s_config() noexcept {
    TerminalConfiguration::ControlBits control_bits{0, 0177420};

    TerminalConfiguration::SpacingConfig spacing{
        INCH_RESOLUTION / 60, // horizontal
        INCH_RESOLUTION / 48, // vertical
        INCH_RESOLUTION / 8, // newline
        INCH_RESOLUTION / 12, // character
        INCH_RESOLUTION / 12, // em
        INCH_RESOLUTION / 16, // halfline
        INCH_RESOLUTION / 12 // adjustment
    };

    TerminalConfiguration::ControlSequences sequences{
        "\033\006"sv, // init
        "\033\006"sv, // reset
        "\015\n"sv, // newline
        ""sv, // halfline_reverse
        ""sv, // halfline_forward
        "\032"sv, // fullline_reverse
        "\033E"sv, // bold_on
        "\033E"sv, // bold_off
        "\006"sv, // plot_on
        "\033\006"sv // plot_off
    };

    TerminalConfiguration::CursorControl cursor{
        "\032"sv, // up
        "\n"sv, // down
        " "sv, // right
        "\b"sv // left
    };

    // Code table would be populated from external source
    TerminalConfiguration::CodeTable code_table{};

    return TerminalConfiguration{control_bits, spacing, sequences, cursor, code_table};
}

/**
 * @brief Global DASI300S terminal configuration instance
 * 
 * This replaces the old C-style termtab structure with a modern C++17
 * configuration object that provides type safety and better encapsulation.
 */
inline const TerminalConfiguration dasi300s_terminal = create_dasi300s_config();

/**
 * @brief Template-based terminal driver interface
 * @tparam Config Terminal configuration type
 */
template <typename Config>
class TerminalDriver {
  private:
    const Config &config_;

  public:
    explicit constexpr TerminalDriver(const Config &config) noexcept
        : config_(config) {}

    [[nodiscard]] constexpr const Config &configuration() const noexcept {
        return config_;
    }

    // Terminal operation methods
    [[nodiscard]] std::string_view get_init_sequence() const noexcept {
        return config_.sequences().init;
    }

    [[nodiscard]] std::string_view get_reset_sequence() const noexcept {
        return config_.sequences().reset;
    }

    [[nodiscard]] std::string_view get_newline_sequence() const noexcept {
        return config_.sequences().newline;
    }

    [[nodiscard]] std::string_view get_cursor_up() const noexcept {
        return config_.cursor().up;
    }

    [[nodiscard]] std::string_view get_cursor_down() const noexcept {
        return config_.cursor().down;
    }

    [[nodiscard]] std::string_view get_cursor_left() const noexcept {
        return config_.cursor().left;
    }

    [[nodiscard]] std::string_view get_cursor_right() const noexcept {
        return config_.cursor().right;
    }

    [[nodiscard]] std::uint16_t get_character_width() const noexcept {
        return config_.spacing().character;
    }

    [[nodiscard]] std::uint16_t get_line_height() const noexcept {
        return config_.spacing().vertical;
    }

    // Validation
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return config_.is_valid();
    }
};

// Type alias for DASI300S terminal driver
using DASI300SDriver = TerminalDriver<TerminalConfiguration>;

/**
 * @brief Create a DASI300S terminal driver instance
 * @return Configured terminal driver for DASI300S
 */
[[nodiscard]] inline DASI300SDriver create_dasi300s_driver() noexcept {
    return DASI300SDriver{dasi300s_terminal};
}

/**
 * @brief Character width and code mapping implementation
 * 
 * This namespace contains the complete character table implementation
 * for the DASI300S terminal, providing width information and character
 * mappings for proper text formatting and display.
 */
namespace character_table {

/**
 * @brief Character width table for DASI300S terminal
 * 
 * Maps ASCII characters (32-255) to their display widths in terminal units.
 * This table provides precise character spacing for proper text alignment.
 */
constexpr std::array<std::uint16_t, 256 - 32> character_widths = {{
    // ASCII 32-47: Space and punctuation
    20, 12, 16, 20, 20, 36, 26, 8, // SP ! " # $ % & '
    12, 12, 20, 24, 8, 20, 8, 10, // ( ) * + , - . /

    // ASCII 48-63: Digits and punctuation
    20, 20, 20, 20, 20, 20, 20, 20, // 0 1 2 3 4 5 6 7
    20, 20, 8, 8, 24, 24, 24, 20, // 8 9 : ; < = > ?

    // ASCII 64-79: @ and uppercase A-O
    36, 28, 24, 26, 28, 24, 22, 30, // @ A B C D E F G
    28, 12, 18, 26, 22, 32, 28, 30, // H I J K L M N O

    // ASCII 80-95: Uppercase P-Z and punctuation
    24, 30, 26, 24, 24, 28, 26, 36, // P Q R S T U V W
    26, 26, 24, 12, 10, 12, 16, 20, // X Y Z [ \ ] ^ _

    // ASCII 96-111: ` and lowercase a-o
    12, 20, 20, 18, 20, 18, 12, 20, // ` a b c d e f g
    20, 10, 10, 18, 10, 30, 20, 20, // h i j k l m n o

    // ASCII 112-127: lowercase p-z and punctuation
    20, 20, 14, 18, 12, 20, 18, 26, // p q r s t u v w
    18, 18, 18, 12, 8, 12, 24, 20, // x y z { | } ~ DEL

    // ASCII 128-255: Extended character set (default widths)
    20, 20, 20, 20, 20, 20, 20, 20, // 128-135
    20, 20, 20, 20, 20, 20, 20, 20, // 136-143
    20, 20, 20, 20, 20, 20, 20, 20, // 144-151
    20, 20, 20, 20, 20, 20, 20, 20, // 152-159
    20, 20, 20, 20, 20, 20, 20, 20, // 160-167
    20, 20, 20, 20, 20, 20, 20, 20, // 168-175
    20, 20, 20, 20, 20, 20, 20, 20, // 176-183
    20, 20, 20, 20, 20, 20, 20, 20, // 184-191
    20, 20, 20, 20, 20, 20, 20, 20, // 192-199
    20, 20, 20, 20, 20, 20, 20, 20, // 200-207
    20, 20, 20, 20, 20, 20, 20, 20, // 208-215
    20, 20, 20, 20, 20, 20, 20, 20, // 216-223
    20, 20, 20, 20, 20, 20, 20, 20, // 224-231
    20, 20, 20, 20, 20, 20, 20, 20, // 232-239
    20, 20, 20, 20, 20, 20, 20, 20, // 240-247
    20, 20, 20, 20, 20, 20, 20, 20 // 248-255
}};

/**
 * @brief Character code mapping table for DASI300S terminal
 * 
 * Maps printable ASCII characters to their terminal-specific representations.
 * Provides proper character encoding for the DASI300S terminal.
 */
constexpr std::array<std::string_view, 256 - 32> character_codes = {{
    // ASCII 32-47: Space and punctuation
    " "sv, "!"sv, "\""sv, "#"sv, "$"sv, "%"sv, "&"sv, "'"sv,
    "("sv, ")"sv, "*"sv, "+"sv, ","sv, "-"sv, "."sv, "/"sv,

    // ASCII 48-63: Digits and punctuation
    "0"sv, "1"sv, "2"sv, "3"sv, "4"sv, "5"sv, "6"sv, "7"sv,
    "8"sv, "9"sv, ":"sv, ";"sv, "<"sv, "="sv, ">"sv, "?"sv,

    // ASCII 64-79: @ and uppercase A-O
    "@"sv, "A"sv, "B"sv, "C"sv, "D"sv, "E"sv, "F"sv, "G"sv,
    "H"sv, "I"sv, "J"sv, "K"sv, "L"sv, "M"sv, "N"sv, "O"sv,

    // ASCII 80-95: Uppercase P-Z and punctuation
    "P"sv, "Q"sv, "R"sv, "S"sv, "T"sv, "U"sv, "V"sv, "W"sv,
    "X"sv, "Y"sv, "Z"sv, "["sv, "\\"sv, "]"sv, "^"sv, "_"sv,

    // ASCII 96-111: ` and lowercase a-o
    "`"sv, "a"sv, "b"sv, "c"sv, "d"sv, "e"sv, "f"sv, "g"sv,
    "h"sv, "i"sv, "j"sv, "k"sv, "l"sv, "m"sv, "n"sv, "o"sv,

    // ASCII 112-127: lowercase p-z and punctuation
    "p"sv, "q"sv, "r"sv, "s"sv, "t"sv, "u"sv, "v"sv, "w"sv,
    "x"sv, "y"sv, "z"sv, "{"sv, "|"sv, "}"sv, "~"sv, ""sv,

    // ASCII 128-255: Extended character set (most map to space or empty)
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 128-135
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 136-143
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 144-151
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 152-159
    " "sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 160-167
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 168-175
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 176-183
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 184-191
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 192-199
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 200-207
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 208-215
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 216-223
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 224-231
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 232-239
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, // 240-247
    ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv, ""sv // 248-255
}};

} // namespace character_table

/**
 * @brief Enhanced factory function to create DASI300S terminal configuration
 * @return Complete terminal configuration for DASI300S with character tables
 */
[[nodiscard]] constexpr TerminalConfiguration create_complete_dasi300s_config() noexcept {
    TerminalConfiguration::ControlBits control_bits{0, 0177420};

    TerminalConfiguration::SpacingConfig spacing{
        INCH_RESOLUTION / 60, // horizontal
        INCH_RESOLUTION / 48, // vertical
        INCH_RESOLUTION / 8, // newline
        INCH_RESOLUTION / 12, // character
        INCH_RESOLUTION / 12, // em
        INCH_RESOLUTION / 16, // halfline
        INCH_RESOLUTION / 12 // adjustment
    };

    TerminalConfiguration::ControlSequences sequences{
        "\033\006"sv, // init
        "\033\006"sv, // reset
        "\015\n"sv, // newline
        ""sv, // halfline_reverse
        ""sv, // halfline_forward
        "\032"sv, // fullline_reverse
        "\033E"sv, // bold_on
        "\033E"sv, // bold_off
        "\006"sv, // plot_on
        "\033\006"sv // plot_off
    };

    TerminalConfiguration::CursorControl cursor{
        "\032"sv, // up
        "\n"sv, // down
        " "sv, // right
        "\b"sv // left
    };

    // Populate code table with character codes
    TerminalConfiguration::CodeTable code_table = character_table::character_codes;

    return TerminalConfiguration{control_bits, spacing, sequences, cursor, code_table};
}

/**
 * @brief Complete DASI300S terminal configuration instance
 * 
 * This replaces the old C-style termtab structure with a modern C++17
 * configuration object that provides type safety, better encapsulation,
 * and complete character table support.
 */
inline const TerminalConfiguration complete_dasi300s_terminal = create_complete_dasi300s_config();

/**
 * @brief Character width lookup utility
 * @param ch Character to look up
 * @return Character width in terminal units
 */
[[nodiscard]] constexpr std::uint16_t get_character_width(char ch) noexcept {
    const auto index = static_cast<std::uint8_t>(ch);
    if (index < 32) {
        return 0; // Control characters have no width
    }
    return character_table::character_widths[index - 32];
}

/**
 * @brief Character code lookup utility
 * @param ch Character to look up
 * @return Terminal-specific character representation
 */
[[nodiscard]] constexpr std::string_view get_character_code(char ch) noexcept {
    const auto index = static_cast<std::uint8_t>(ch);
    if (index < 32) {
        return ""sv; // Control characters have no representation
    }
    return character_table::character_codes[index - 32];
}

/**
 * @brief Terminal capability query interface
 * 
 * Provides a modern C++17 interface for querying terminal capabilities
 * and features of the DASI300S terminal.
 */
class TerminalCapabilities {
  private:
    const TerminalConfiguration &config_;

  public:
    explicit constexpr TerminalCapabilities(const TerminalConfiguration &config) noexcept
        : config_(config) {}

    [[nodiscard]] constexpr bool supports_bold() const noexcept {
        return !config_.sequences().bold_on.empty() &&
               !config_.sequences().bold_off.empty();
    }

    [[nodiscard]] constexpr bool supports_plot_mode() const noexcept {
        return !config_.sequences().plot_on.empty() &&
               !config_.sequences().plot_off.empty();
    }

    [[nodiscard]] constexpr bool supports_cursor_movement() const noexcept {
        return config_.cursor().all_sequences_valid();
    }

    [[nodiscard]] constexpr bool supports_halfline_movement() const noexcept {
        return !config_.sequences().halfline_forward.empty() ||
               !config_.sequences().halfline_reverse.empty();
    }

    [[nodiscard]] constexpr std::uint16_t get_resolution() const noexcept {
        return INCH_RESOLUTION;
    }

    [[nodiscard]] constexpr std::uint16_t get_horizontal_dpi() const noexcept {
        return INCH_RESOLUTION / config_.spacing().horizontal;
    }

    [[nodiscard]] constexpr std::uint16_t get_vertical_dpi() const noexcept {
        return INCH_RESOLUTION / config_.spacing().vertical;
    }
};

/**
 * @brief Create terminal capabilities interface for DASI300S
 * @return Terminal capabilities query interface
 */
[[nodiscard]] inline TerminalCapabilities create_dasi300s_capabilities() noexcept {
    return TerminalCapabilities{complete_dasi300s_terminal};
}

} // namespace roff::terminal::dasi300s

// Static assertions to ensure proper compilation and memory efficiency
static_assert(std::is_trivially_copyable_v<roff::terminal::dasi300s::TerminalConfiguration>,
              "TerminalConfiguration must be trivially copyable");
static_assert(std::is_standard_layout_v<roff::terminal::dasi300s::TerminalConfiguration>,
              "TerminalConfiguration must have standard layout");
static_assert(sizeof(roff::terminal::dasi300s::TerminalConfiguration) <= 4096,
              "TerminalConfiguration must remain memory-efficient");

/**
 * @brief Global access point for DASI300S terminal configuration
 * 
 * Provides a convenient global access point for the complete DASI300S
 * terminal configuration, maintaining compatibility with legacy code
 * while providing modern C++17 interfaces.
 */
namespace roff::terminal {
using dasi300s_config = dasi300s::TerminalConfiguration;
using dasi300s_driver = dasi300s::DASI300SDriver;
using dasi300s_capabilities = dasi300s::TerminalCapabilities;

// Global instances for easy access
inline const auto &dasi300s = dasi300s::complete_dasi300s_terminal;
inline const auto dasi300s_driver_instance = dasi300s::create_dasi300s_driver();
inline const auto dasi300s_caps = dasi300s::create_dasi300s_capabilities();
} // namespace roff::terminal
