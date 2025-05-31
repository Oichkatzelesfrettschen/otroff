/**
 * @file tabvt220.cpp
 * @brief DEC VT220 Terminal Driver - Pure C++17 Implementation
 * @author Modern C++17 Engineering Team
 * @version 3.0
 * @date 2024
 *
 * @details Pure C++17 implementation of DEC VT220 terminal configuration.
 *          Completely redesigned with modern C++17 principles:
 *          - Full class encapsulation and type safety
 *          - ANSI X3.64 escape sequence support
 *          - 7-bit and 8-bit character set handling
 *          - DEC special character extensions
 *          - Exception-safe design patterns
 *          - Template-based driver architecture
 *          - Compile-time validation and optimization
 */

#include "cxx23_scaffold.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace std::string_view_literals;

namespace roff::terminal::vt220 {

// Type-safe constants using constexpr
constexpr std::uint16_t INCH_RESOLUTION = 240;

/**
 * @brief VT220 Character Set Types
 */
enum class CharacterSet : std::uint8_t {
    ASCII = 0, ///< Standard ASCII (7-bit)
    DEC_Special = 1, ///< DEC special character set
    DEC_Technical = 2, ///< DEC technical character set
    ISO_Latin1 = 3 ///< ISO Latin-1 (8-bit)
};

/**
 * @brief VT220 Terminal Modes
 */
enum class TerminalMode : std::uint16_t {
    None = 0x0000,
    CursorKeys = 0x0001, ///< Cursor key mode
    ANSI = 0x0002, ///< ANSI/VT100 mode
    Column132 = 0x0004, ///< 132 column mode
    SmoothScroll = 0x0008, ///< Smooth scroll
    ReverseVideo = 0x0010, ///< Reverse video
    OriginMode = 0x0020, ///< Origin mode
    AutoWrap = 0x0040, ///< Auto wrap mode
    AutoRepeat = 0x0080, ///< Auto repeat mode
    FormFeed = 0x0100, ///< Form feed mode
    ShowCursor = 0x0200, ///< Show cursor
    ApplicationCursor = 0x0400, ///< Application cursor keys
    ApplicationKeypad = 0x0800 ///< Application keypad mode
};

/**
 * @brief Pure C++17 VT220 Terminal Configuration Class
 * 
 * This class encapsulates all VT220 terminal configuration parameters
 * with full type safety, validation, and modern C++17 design patterns.
 */
class VT220Configuration {
  public:
    // Type aliases for better code clarity
    using ResolutionType = std::uint16_t;
    using ModeFlags = std::uint16_t;
    using CharacterTable = std::array<std::string_view, 256 - 32>;

    /**
     * @brief Configuration validation results
     */
    enum class ValidationResult : std::uint8_t {
        Valid = 0,
        InvalidResolution = 1,
        InvalidModes = 2,
        InvalidSequences = 3,
        InvalidCharacterTable = 4
    };

    /**
     * @brief Terminal mode configuration
     */
    class ModeConfiguration {
      private:
        ModeFlags set_modes_{0};
        ModeFlags reset_modes_{0};

      public:
        constexpr ModeConfiguration() = default;

        constexpr ModeConfiguration(ModeFlags set, ModeFlags reset)
            : set_modes_(set), reset_modes_(reset) {}

        [[nodiscard]] constexpr ModeFlags set_modes() const noexcept { return set_modes_; }
        [[nodiscard]] constexpr ModeFlags reset_modes() const noexcept { return reset_modes_; }

        constexpr void enable_mode(TerminalMode mode) noexcept {
            set_modes_ |= static_cast<ModeFlags>(mode);
        }

        constexpr void disable_mode(TerminalMode mode) noexcept {
            reset_modes_ |= static_cast<ModeFlags>(mode);
        }

        [[nodiscard]] constexpr bool is_mode_enabled(TerminalMode mode) const noexcept {
            return (set_modes_ & static_cast<ModeFlags>(mode)) != 0;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return true; // Basic validation - could be enhanced
        }
    };

    /**
     * @brief VT220 resolution and spacing configuration
     */
    class ResolutionConfiguration {
      private:
        ResolutionType horizontal_{INCH_RESOLUTION / 10}; // 10 chars per inch
        ResolutionType vertical_{INCH_RESOLUTION / 6}; // 6 lines per inch
        ResolutionType newline_{INCH_RESOLUTION / 6}; // Same as vertical
        ResolutionType character_{INCH_RESOLUTION / 10}; // Character width
        ResolutionType em_{INCH_RESOLUTION / 10}; // Em width
        ResolutionType halfline_{INCH_RESOLUTION / 12}; // Halfline spacing
        ResolutionType adjustment_{INCH_RESOLUTION / 10}; // Adjustment spacing

      public:
        constexpr ResolutionConfiguration() = default;

        constexpr ResolutionConfiguration(
            ResolutionType h, ResolutionType v, ResolutionType nl,
            ResolutionType ch, ResolutionType em, ResolutionType hl, ResolutionType adj)
            : horizontal_(h), vertical_(v), newline_(nl), character_(ch),
              em_(em), halfline_(hl), adjustment_(adj) {}

        // Accessors
        [[nodiscard]] constexpr ResolutionType horizontal() const noexcept { return horizontal_; }
        [[nodiscard]] constexpr ResolutionType vertical() const noexcept { return vertical_; }
        [[nodiscard]] constexpr ResolutionType newline() const noexcept { return newline_; }
        [[nodiscard]] constexpr ResolutionType character() const noexcept { return character_; }
        [[nodiscard]] constexpr ResolutionType em() const noexcept { return em_; }
        [[nodiscard]] constexpr ResolutionType halfline() const noexcept { return halfline_; }
        [[nodiscard]] constexpr ResolutionType adjustment() const noexcept { return adjustment_; }

        // Mutators with validation
        constexpr void set_horizontal(ResolutionType value) {
            if (value == 0)
                throw std::invalid_argument("Horizontal resolution must be positive");
            horizontal_ = value;
        }

        constexpr void set_vertical(ResolutionType value) {
            if (value == 0)
                throw std::invalid_argument("Vertical resolution must be positive");
            vertical_ = value;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return horizontal_ > 0 && vertical_ > 0 && character_ > 0 && em_ > 0;
        }
    };

    /**
     * @brief VT220 ANSI control sequence configuration
     */
    class ANSISequenceConfiguration {
      private:
        // VT220 initialization: enable auto wrap, cursor keys, etc.
        std::string_view init_{"\033[?7h\033[?1h\033[?3l\033[?4l\033[?5l\033[?6l\033[?8h\033[?25h"sv};
        std::string_view reset_{"\033c\033[!p"sv}; // Full reset
        std::string_view newline_{"\r\n"sv}; // Carriage return + line feed
        std::string_view halfline_reverse_{"\033M"sv}; // Reverse index
        std::string_view halfline_forward_{"\033D"sv}; // Index (forward)
        std::string_view fullline_reverse_{"\033M"sv}; // Reverse index
        std::string_view bold_on_{"\033[1m"sv}; // Bold on
        std::string_view bold_off_{"\033[0m"sv}; // All attributes off
        std::string_view plot_on_{""sv}; // Not supported
        std::string_view plot_off_{""sv}; // Not supported

      public:
        constexpr ANSISequenceConfiguration() = default;

        // Accessors
        [[nodiscard]] constexpr std::string_view init() const noexcept { return init_; }
        [[nodiscard]] constexpr std::string_view reset() const noexcept { return reset_; }
        [[nodiscard]] constexpr std::string_view newline() const noexcept { return newline_; }
        [[nodiscard]] constexpr std::string_view halfline_reverse() const noexcept { return halfline_reverse_; }
        [[nodiscard]] constexpr std::string_view halfline_forward() const noexcept { return halfline_forward_; }
        [[nodiscard]] constexpr std::string_view fullline_reverse() const noexcept { return fullline_reverse_; }
        [[nodiscard]] constexpr std::string_view bold_on() const noexcept { return bold_on_; }
        [[nodiscard]] constexpr std::string_view bold_off() const noexcept { return bold_off_; }
        [[nodiscard]] constexpr std::string_view plot_on() const noexcept { return plot_on_; }
        [[nodiscard]] constexpr std::string_view plot_off() const noexcept { return plot_off_; }

        // Feature support queries
        [[nodiscard]] constexpr bool supports_bold() const noexcept {
            return !bold_on_.empty() && !bold_off_.empty();
        }

        [[nodiscard]] constexpr bool supports_plot_mode() const noexcept {
            return !plot_on_.empty() && !plot_off_.empty();
        }

        [[nodiscard]] constexpr bool supports_halfline_movement() const noexcept {
            return !halfline_forward_.empty() || !halfline_reverse_.empty();
        }
    };

    /**
     * @brief VT220 cursor movement configuration
     */
    class CursorConfiguration {
      private:
        std::string_view up_{"\033[A"sv}; // ANSI cursor up
        std::string_view down_{"\033[B"sv}; // ANSI cursor down
        std::string_view right_{"\033[C"sv}; // ANSI cursor right
        std::string_view left_{"\033[D"sv}; // ANSI cursor left

      public:
        constexpr CursorConfiguration() = default;

        // Accessors
        [[nodiscard]] constexpr std::string_view up() const noexcept { return up_; }
        [[nodiscard]] constexpr std::string_view down() const noexcept { return down_; }
        [[nodiscard]] constexpr std::string_view right() const noexcept { return right_; }
        [[nodiscard]] constexpr std::string_view left() const noexcept { return left_; }

        [[nodiscard]] constexpr bool supports_movement() const noexcept {
            return !up_.empty() && !down_.empty() && !right_.empty() && !left_.empty();
        }
    };

  private:
    // Private data members - full encapsulation
    ModeConfiguration modes_;
    ResolutionConfiguration resolution_;
    ANSISequenceConfiguration sequences_;
    CursorConfiguration cursor_;
    CharacterTable character_codes_{};
    CharacterSet character_set_{CharacterSet::ASCII};
    bool validated_{false};

  public:
    // Constructors
    constexpr VT220Configuration() = default;

    explicit VT220Configuration(
        const ModeConfiguration &modes,
        const ResolutionConfiguration &resolution = {},
        const ANSISequenceConfiguration &sequences = {},
        const CursorConfiguration &cursor = {},
        const CharacterTable &character_codes = {},
        CharacterSet char_set = CharacterSet::ASCII)
        : modes_(modes), resolution_(resolution), sequences_(sequences),
          cursor_(cursor), character_codes_(character_codes), character_set_(char_set) {
        validate_configuration();
    }

    // Copy constructor with validation
    VT220Configuration(const VT220Configuration &other)
        : modes_(other.modes_), resolution_(other.resolution_),
          sequences_(other.sequences_), cursor_(other.cursor_),
          character_codes_(other.character_codes_), character_set_(other.character_set_) {
        validate_configuration();
    }

    // Move constructor
    VT220Configuration(VT220Configuration &&other) noexcept
        : modes_(std::move(other.modes_)), resolution_(std::move(other.resolution_)),
          sequences_(std::move(other.sequences_)), cursor_(std::move(other.cursor_)),
          character_codes_(std::move(other.character_codes_)),
          character_set_(other.character_set_), validated_(other.validated_) {}

    // Assignment operators
    VT220Configuration &operator=(const VT220Configuration &other) {
        if (this != &other) {
            modes_ = other.modes_;
            resolution_ = other.resolution_;
            sequences_ = other.sequences_;
            cursor_ = other.cursor_;
            character_codes_ = other.character_codes_;
            character_set_ = other.character_set_;
            validate_configuration();
        }
        return *this;
    }

    VT220Configuration &operator=(VT220Configuration &&other) noexcept {
        if (this != &other) {
            modes_ = std::move(other.modes_);
            resolution_ = std::move(other.resolution_);
            sequences_ = std::move(other.sequences_);
            cursor_ = std::move(other.cursor_);
            character_codes_ = std::move(other.character_codes_);
            character_set_ = other.character_set_;
            validated_ = other.validated_;
        }
        return *this;
    }

    // Accessors - const-correct and type-safe
    [[nodiscard]] constexpr const ModeConfiguration &modes() const noexcept { return modes_; }
    [[nodiscard]] constexpr const ResolutionConfiguration &resolution() const noexcept { return resolution_; }
    [[nodiscard]] constexpr const ANSISequenceConfiguration &sequences() const noexcept { return sequences_; }
    [[nodiscard]] constexpr const CursorConfiguration &cursor() const noexcept { return cursor_; }
    [[nodiscard]] constexpr const CharacterTable &character_codes() const noexcept { return character_codes_; }
    [[nodiscard]] constexpr CharacterSet character_set() const noexcept { return character_set_; }

    // Mutators with validation
    void update_modes(const ModeConfiguration &config) {
        modes_ = config;
        validate_configuration();
    }

    void update_resolution(const ResolutionConfiguration &config) {
        resolution_ = config;
        validate_configuration();
    }

    void update_character_set(CharacterSet char_set) noexcept {
        character_set_ = char_set;
    }

    // Validation and utility methods
    [[nodiscard]] ValidationResult validate() const noexcept {
        if (!modes_.is_valid())
            return ValidationResult::InvalidModes;
        if (!resolution_.is_valid())
            return ValidationResult::InvalidResolution;
        return ValidationResult::Valid;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return validated_ && modes_.is_valid() && resolution_.is_valid();
    }

    [[nodiscard]] constexpr std::size_t memory_footprint() const noexcept {
        return sizeof(*this);
    }

    // VT220-specific capability queries
    [[nodiscard]] constexpr bool supports_132_columns() const noexcept {
        return modes_.is_mode_enabled(TerminalMode::Column132);
    }

    [[nodiscard]] constexpr bool supports_ansi_mode() const noexcept {
        return modes_.is_mode_enabled(TerminalMode::ANSI);
    }

    [[nodiscard]] constexpr bool supports_smooth_scroll() const noexcept {
        return modes_.is_mode_enabled(TerminalMode::SmoothScroll);
    }

  private:
    void validate_configuration() {
        const auto result = validate();
        if (result != ValidationResult::Valid) {
            validated_ = false;
            throw std::invalid_argument("Invalid VT220 configuration");
        }
        validated_ = true;
    }
};

/**
 * @brief VT220 DEC Special Character Set Definitions
 */
namespace character_sets {

/**
 * @brief VT220 character width table for optimal spacing
 * 
 * Provides character widths in terminal resolution units for proper
 * text formatting and alignment on VT220 terminals.
 */
constexpr std::array<std::uint16_t, 256 - 32> character_widths = {{
    // ASCII 32-47: Space and punctuation
    24, 12, 16, 24, 24, 36, 26, 8, // SP ! " # $ % & '
    12, 12, 24, 24, 8, 24, 8, 12, // ( ) * + , - . /

    // ASCII 48-63: Digits and punctuation
    24, 24, 24, 24, 24, 24, 24, 24, // 0 1 2 3 4 5 6 7
    24, 24, 8, 8, 24, 24, 24, 24, // 8 9 : ; < = > ?

    // ASCII 64-79: @ and uppercase A-O
    36, 28, 24, 26, 28, 24, 22, 30, // @ A B C D E F G
    28, 12, 18, 26, 22, 32, 28, 30, // H I J K L M N O

    // ASCII 80-95: Uppercase P-Z and punctuation
    24, 30, 26, 24, 24, 28, 26, 36, // P Q R S T U V W
    26, 26, 24, 12, 12, 12, 16, 24, // X Y Z [ \ ] ^ _

    // ASCII 96-111: ` and lowercase a-o
    12, 24, 24, 20, 24, 20, 12, 24, // ` a b c d e f g
    24, 12, 12, 20, 12, 36, 24, 24, // h i j k l m n o

    // ASCII 112-127: lowercase p-z and punctuation
    24, 24, 16, 20, 12, 24, 20, 30, // p q r s t u v w
    20, 20, 20, 12, 8, 12, 24, 24, // x y z { | } ~ DEL

    // ASCII 128-255: Extended character set (uniform width for VT220)
    24, 24, 24, 24, 24, 24, 24, 24, // 128-135
    24, 24, 24, 24, 24, 24, 24, 24, // 136-143
    24, 24, 24, 24, 24, 24, 24, 24, // 144-151
    24, 24, 24, 24, 24, 24, 24, 24, // 152-159
    24, 24, 24, 24, 24, 24, 24, 24, // 160-167
    24, 24, 24, 24, 24, 24, 24, 24, // 168-175
    24, 24, 24, 24, 24, 24, 24, 24, // 176-183
    24, 24, 24, 24, 24, 24, 24, 24, // 184-191
    24, 24, 24, 24, 24, 24, 24, 24, // 192-199
    24, 24, 24, 24, 24, 24, 24, 24, // 200-207
    24, 24, 24, 24, 24, 24, 24, 24, // 208-215
    24, 24, 24, 24, 24, 24, 24, 24, // 216-223
    24, 24, 24, 24, 24, 24, 24, 24, // 224-231
    24, 24, 24, 24, 24, 24, 24, 24, // 232-239
    24, 24, 24, 24, 24, 24, 24, 24, // 240-247
    24, 24, 24, 24, 24, 24, 24, 24 // 248-255
}};

/**
 * @brief Complete VT220 character code table with DEC special characters
 * 
 * This table includes standard ASCII characters plus DEC special character
 * extensions for mathematical symbols, line drawing, and typographic elements.
 */
constexpr std::array<std::string_view, 256 - 32> vt220_character_codes = {{
    // ASCII 32-47: Space and punctuation
    "\001 "sv, "\001!"sv, "\001\""sv, "\001#"sv, "\001$"sv, "\001%"sv, "\001&"sv, "\001'"sv,
    "\001("sv, "\001)"sv, "\001*"sv, "\001+"sv, "\001,"sv, "\001-"sv, "\001."sv, "\001/"sv,

    // ASCII 48-63: Digits and punctuation
    "\0010"sv, "\0011"sv, "\0012"sv, "\0013"sv, "\0014"sv, "\0015"sv, "\0016"sv, "\0017"sv,
    "\0018"sv, "\0019"sv, "\001:"sv, "\001;"sv, "\001<"sv, "\001="sv, "\001>"sv, "\001?"sv,

    // ASCII 64-79: @ and uppercase A-O
    "\001@"sv, "\001A"sv, "\001B"sv, "\001C"sv, "\001D"sv, "\001E"sv, "\001F"sv, "\001G"sv,
    "\001H"sv, "\001I"sv, "\001J"sv, "\001K"sv, "\001L"sv, "\001M"sv, "\001N"sv, "\001O"sv,

    // ASCII 80-95: Uppercase P-Z and punctuation
    "\001P"sv, "\001Q"sv, "\001R"sv, "\001S"sv, "\001T"sv, "\001U"sv, "\001V"sv, "\001W"sv,
    "\001X"sv, "\001Y"sv, "\001Z"sv, "\001["sv, "\001\\"sv, "\001]"sv, "\001^"sv, "\001_"sv,

    // ASCII 96-111: ` and lowercase a-o
    "\001`"sv, "\001a"sv, "\001b"sv, "\001c"sv, "\001d"sv, "\001e"sv, "\001f"sv, "\001g"sv,
    "\001h"sv, "\001i"sv, "\001j"sv, "\001k"sv, "\001l"sv, "\001m"sv, "\001n"sv, "\001o"sv,

    // ASCII 112-127: lowercase p-z and punctuation
    "\001p"sv, "\001q"sv, "\001r"sv, "\001s"sv, "\001t"sv, "\001u"sv, "\001v"sv, "\001w"sv,
    "\001x"sv, "\001y"sv, "\001z"sv, "\001{"sv, "\001|"sv, "\001}"sv, "\001~"sv, "\000\0"sv,

    // Extended character set - narrow space, typography, symbols
    "\000\0"sv, // narrow space
    "\001-"sv, // hyphen
    "\001\260"sv, // bullet - degree symbol
    "\001\261"sv, // square - plus-minus
    "\001-"sv, // 3/4 em dash
    "\001_"sv, // rule
    "\0031/4"sv, // 1/4 fraction
    "\0031/2"sv, // 1/2 fraction
    "\0033/4"sv, // 3/4 fraction
    "\001-"sv, // minus sign
    "\002fi"sv, // fi ligature
    "\002fl"sv, // fl ligature
    "\002ff"sv, // ff ligature
    "\003ffi"sv, // ffi ligature
    "\003ffl"sv, // ffl ligature
    "\001\260"sv, // degree symbol
    "\001\262"sv, // dagger
    "\001\247"sv, // section symbol
    "\001'"sv, // foot mark
    "\001\264"sv, // acute accent
    "\001\140"sv, // grave accent
    "\001_"sv, // underrule
    "\001/"sv, // slash
    "\000\0"sv, // half narrow space
    "\001 "sv, // unpaddable space

    // Greek lowercase letters
    "\001\341"sv, // alpha
    "\001\342"sv, // beta
    "\001\343"sv, // gamma
    "\001\344"sv, // delta
    "\001\345"sv, // epsilon
    "\001\346"sv, // zeta
    "\001\347"sv, // eta
    "\001\350"sv, // theta
    "\001\351"sv, // iota
    "\001\352"sv, // kappa
    "\001\353"sv, // lambda
    "\001\354"sv, // mu
    "\001\355"sv, // nu
    "\001\356"sv, // xi
    "\001\357"sv, // omicron
    "\001\360"sv, // pi
    "\001\361"sv, // rho
    "\001\362"sv, // sigma
    "\001\363"sv, // tau
    "\001\364"sv, // upsilon
    "\001\365"sv, // phi
    "\001\366"sv, // chi
    "\001\367"sv, // psi
    "\001\370"sv, // omega

    // Greek uppercase letters
    "\001\301"sv, // Gamma
    "\001\304"sv, // Delta
    "\001\310"sv, // Theta
    "\001\313"sv, // Lambda
    "\001\316"sv, // Xi
    "\001\320"sv, // Pi
    "\001\323"sv, // Sigma
    "\000\0"sv, // (unused)
    "\001\325"sv, // Upsilon
    "\001\326"sv, // Phi
    "\001\327"sv, // Psi
    "\001\330"sv, // Omega

    // Mathematical symbols
    "\001\221"sv, // square root
    "\001\362"sv, // terminal sigma
    "\000\0"sv, // root en
    "\002>="sv, // greater than or equal
    "\002<="sv, // less than or equal
    "\002=="sv, // identically equal
    "\001-"sv, // equation minus
    "\002~="sv, // approximately equal
    "\002~"sv, // approximates
    "\002!="sv, // not equal
    "\002->"sv, // right arrow
    "\002<-"sv, // left arrow
    "\001^"sv, // up arrow
    "\001v"sv, // down arrow
    "\001="sv, // equation equal
    "\001x"sv, // multiply
    "\001/"sv, // divide
    "\002+-"sv, // plus-minus
    "\001U"sv, // cup (union)
    "\000\0"sv, // cap (intersection)
    "\000\0"sv, // subset of
    "\000\0"sv, // superset of
    "\000\0"sv, // improper subset
    "\000\0"sv, // improper superset
    "\002oo"sv, // infinity
    "\001d"sv, // partial derivative
    "\001\\/"sv, // gradient
    "\001~"sv, // not
    "\001S"sv, // integral sign
    "\000\0"sv, // proportional to
    "\000\0"sv, // empty set
    "\000\0"sv, // member of
    "\001+"sv, // equation plus

    // Special symbols and line drawing
    "\002(R)"sv, // registered trademark
    "\002(C)"sv, // copyright
    "\001|"sv, // box rule
    "\002ct"sv, // cent sign
    "\000\0"sv, // double dagger
    "\000\0"sv, // right hand
    "\000\0"sv, // left hand
    "\001*"sv, // math asterisk
    "\000\0"sv, // bell system sign
    "\001|"sv, // or
    "\001O"sv, // circle

    // DEC line drawing characters (VT220 special)
    "\001\154"sv, // left top corner
    "\001\155"sv, // left bottom corner
    "\001\153"sv, // right top corner
    "\001\152"sv, // right bottom corner
    "\001\156"sv, // left tee
    "\001\165"sv, // right tee
    "\001\170"sv, // bold vertical line
    "\001\155"sv, // left floor
    "\001\152"sv, // right floor
    "\001\154"sv, // left ceiling
    "\001\153"sv // right ceiling
}};

} // namespace character_sets

/**
 * @brief Factory function to create default VT220 terminal configuration
 */
[[nodiscard]] constexpr VT220Configuration create_default_vt220_config() noexcept {
    VT220Configuration::ModeConfiguration modes{0, 0}; // Default mode configuration

    VT220Configuration::ResolutionConfiguration resolution{
        INCH_RESOLUTION / 10, // horizontal (10 chars per inch)
        INCH_RESOLUTION / 6, // vertical (6 lines per inch)
        INCH_RESOLUTION / 6, // newline
        INCH_RESOLUTION / 10, // character
        INCH_RESOLUTION / 10, // em
        INCH_RESOLUTION / 12, // halfline
        INCH_RESOLUTION / 10 // adjustment
    };

    VT220Configuration::ANSISequenceConfiguration sequences{};
    VT220Configuration::CursorConfiguration cursor{};
    VT220Configuration::CharacterTable character_codes = character_sets::vt220_character_codes;

    return VT220Configuration{modes, resolution, sequences, cursor, character_codes, CharacterSet::ASCII};
}

/**
 * @brief Template-based VT220 terminal driver
 */
template <typename Config>
class VT220Driver {
  private:
    const Config &config_;

  public:
    explicit constexpr VT220Driver(const Config &config) noexcept
        : config_(config) {}

    [[nodiscard]] constexpr const Config &configuration() const noexcept {
        return config_;
    }

    // Terminal initialization and control
    [[nodiscard]] std::string_view get_init_sequence() const noexcept {
        return config_.sequences().init();
    }

    [[nodiscard]] std::string_view get_reset_sequence() const noexcept {
        return config_.sequences().reset();
    }

    [[nodiscard]] std::string_view get_newline_sequence() const noexcept {
        return config_.sequences().newline();
    }

    // Cursor movement
    [[nodiscard]] std::string_view get_cursor_up() const noexcept {
        return config_.cursor().up();
    }

    [[nodiscard]] std::string_view get_cursor_down() const noexcept {
        return config_.cursor().down();
    }

    [[nodiscard]] std::string_view get_cursor_left() const noexcept {
        return config_.cursor().left();
    }

    [[nodiscard]] std::string_view get_cursor_right() const noexcept {
        return config_.cursor().right();
    }

    // Text formatting
    [[nodiscard]] std::string_view get_bold_on() const noexcept {
        return config_.sequences().bold_on();
    }

    [[nodiscard]] std::string_view get_bold_off() const noexcept {
        return config_.sequences().bold_off();
    }

    [[nodiscard]] std::string_view get_halfline_forward() const noexcept {
        return config_.sequences().halfline_forward();
    }

    [[nodiscard]] std::string_view get_halfline_reverse() const noexcept {
        return config_.sequences().halfline_reverse();
    }

    // Terminal properties
    [[nodiscard]] auto get_character_width() const noexcept {
        return config_.resolution().character();
    }

    [[nodiscard]] auto get_line_height() const noexcept {
        return config_.resolution().vertical();
    }

    [[nodiscard]] auto get_horizontal_resolution() const noexcept {
        return config_.resolution().horizontal();
    }

    [[nodiscard]] auto get_vertical_resolution() const noexcept {
        return config_.resolution().vertical();
    }

    // VT220-specific capabilities
    [[nodiscard]] constexpr bool supports_bold() const noexcept {
        return config_.sequences().supports_bold();
    }

    [[nodiscard]] constexpr bool supports_ansi_sequences() const noexcept {
        return config_.supports_ansi_mode();
    }

    [[nodiscard]] constexpr bool supports_132_columns() const noexcept {
        return config_.supports_132_columns();
    }

    [[nodiscard]] constexpr bool supports_smooth_scroll() const noexcept {
        return config_.supports_smooth_scroll();
    }

    [[nodiscard]] constexpr bool supports_cursor_movement() const noexcept {
        return config_.cursor().supports_movement();
    }

    [[nodiscard]] constexpr bool supports_halfline_movement() const noexcept {
        return config_.sequences().supports_halfline_movement();
    }

    // Character handling
    [[nodiscard]] constexpr std::uint16_t get_character_width(char ch) const noexcept {
        const auto index = static_cast<std::uint8_t>(ch);
        if (index < 32) {
            return 0; // Control characters have no width
        }
        return character_sets::character_widths[index - 32];
    }

    [[nodiscard]] constexpr std::string_view get_character_code(char ch) const noexcept {
        const auto index = static_cast<std::uint8_t>(ch);
        if (index < 32) {
            return ""; // Control characters have no representation
        }
        return config_.character_codes()[index - 32];
    }

    // Validation
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return config_.is_valid();
    }
};

// Type aliases
using VT220TerminalDriver = VT220Driver<VT220Configuration>;

/**
 * @brief VT220 terminal capability analysis interface
 */
class VT220Capabilities {
  private:
    const VT220Configuration &config_;

  public:
    explicit constexpr VT220Capabilities(const VT220Configuration &config) noexcept
        : config_(config) {}

    [[nodiscard]] constexpr bool has_ansi_support() const noexcept {
        return config_.supports_ansi_mode();
    }

    [[nodiscard]] constexpr bool has_bold_support() const noexcept {
        return config_.sequences().supports_bold();
    }

    [[nodiscard]] constexpr bool has_cursor_movement() const noexcept {
        return config_.cursor().supports_movement();
    }

    [[nodiscard]] constexpr bool has_132_column_mode() const noexcept {
        return config_.supports_132_columns();
    }

    [[nodiscard]] constexpr bool has_smooth_scroll() const noexcept {
        return config_.supports_smooth_scroll();
    }

    [[nodiscard]] constexpr bool has_halfline_movement() const noexcept {
        return config_.sequences().supports_halfline_movement();
    }

    [[nodiscard]] constexpr std::uint16_t get_resolution() const noexcept {
        return INCH_RESOLUTION;
    }

    [[nodiscard]] constexpr std::uint16_t get_horizontal_dpi() const noexcept {
        return INCH_RESOLUTION / config_.resolution().horizontal();
    }

    [[nodiscard]] constexpr std::uint16_t get_vertical_dpi() const noexcept {
        return INCH_RESOLUTION / config_.resolution().vertical();
    }

    [[nodiscard]] constexpr CharacterSet get_character_set() const noexcept {
        return config_.character_set();
    }

    [[nodiscard]] constexpr std::size_t get_memory_usage() const noexcept {
        return config_.memory_footprint();
    }
};

/**
 * @brief Global VT220 terminal configuration instance
 */
inline const VT220Configuration vt220_terminal_config = create_default_vt220_config();

/**
 * @brief Global VT220 terminal driver instance
 */
inline const VT220TerminalDriver vt220_terminal_driver{vt220_terminal_config};

/**
 * @brief Global VT220 terminal capabilities instance
 */
inline const VT220Capabilities vt220_capabilities{vt220_terminal_config};

/**
 * @brief Factory functions for VT220 terminal components
 */
[[nodiscard]] inline const VT220Configuration &get_vt220_config() noexcept {
    return vt220_terminal_config;
}

[[nodiscard]] inline VT220TerminalDriver create_vt220_driver() noexcept {
    return VT220TerminalDriver{vt220_terminal_config};
}

[[nodiscard]] inline VT220Capabilities create_vt220_capabilities() noexcept {
    return VT220Capabilities{vt220_terminal_config};
}

/**
 * @brief VT220 character set switching utilities
 */
class VT220CharacterSetManager {
  private:
    VT220Configuration config_;

  public:
    explicit VT220CharacterSetManager(const VT220Configuration &config)
        : config_(config) {}

    void switch_to_ascii() noexcept {
        config_.update_character_set(CharacterSet::ASCII);
    }

    void switch_to_dec_special() noexcept {
        config_.update_character_set(CharacterSet::DEC_Special);
    }

    void switch_to_dec_technical() noexcept {
        config_.update_character_set(CharacterSet::DEC_Technical);
    }

    void switch_to_latin1() noexcept {
        config_.update_character_set(CharacterSet::ISO_Latin1);
    }

    [[nodiscard]] constexpr CharacterSet current_character_set() const noexcept {
        return config_.character_set();
    }

    [[nodiscard]] std::string_view get_character_set_select_sequence() const noexcept {
        switch (config_.character_set()) {
        case CharacterSet::ASCII:
            return "\033(B"sv;
        case CharacterSet::DEC_Special:
            return "\033(0"sv;
        case CharacterSet::DEC_Technical:
            return "\033(>"sv;
        case CharacterSet::ISO_Latin1:
            return "\033(A"sv;
        default:
            return "\033(B"sv; // Default to ASCII
        }
    }
};

} // namespace roff::terminal::vt220

// Static assertions to ensure memory efficiency and type safety
static_assert(std::is_standard_layout_v<roff::terminal::vt220::VT220Configuration>,
              "VT220Configuration must have standard layout");
static_assert(std::is_trivially_copyable_v<roff::terminal::vt220::VT220Configuration::ModeConfiguration>,
              "ModeConfiguration must be trivially copyable");
static_assert(std::is_trivially_copyable_v<roff::terminal::vt220::VT220Configuration::ResolutionConfiguration>,
              "ResolutionConfiguration must be trivially copyable");
static_assert(sizeof(roff::terminal::vt220::VT220Configuration) <= 4096,
              "VT220Configuration must remain memory-efficient");

/**
 * @brief Global namespace aliases for convenient access
 */
namespace roff::terminal {
using vt220_config = vt220::VT220Configuration;
using vt220_driver = vt220::VT220TerminalDriver;
using vt220_capabilities = vt220::VT220Capabilities;
using vt220_charset_manager = vt220::VT220CharacterSetManager;

// Legacy compatibility aliases
using termtab = vt220::VT220Configuration;

// Global instances for easy access
inline const auto &vt220 = vt220::vt220_terminal_config;
inline const auto &vt220_driver_instance = vt220::vt220_terminal_driver;
inline const auto &vt220_caps = vt220::vt220_capabilities;

// Legacy compatibility instance
inline const auto &t = vt220::vt220_terminal_config;
} // namespace roff::terminal
