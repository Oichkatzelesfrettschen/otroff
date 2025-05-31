/**
 * @file tab450-12-8.cpp
 * @brief DASI450 Terminal Driver - Pure C++17 Implementation (12 chars/inch, 8 lines/inch)
 * @author Modern C++17 Engineering Team
 * @version 3.0
 * @date 2024
 *
 * @details Pure C++17 implementation of DASI450 terminal configuration.
 *          Completely redesigned with modern C++17 principles:
 *          - Full class encapsulation and type safety
 *          - High-resolution character spacing (12 chars/inch, 8 lines/inch)
 *          - DASI-specific control sequences
 *          - Exception-safe design patterns
 *          - Template-based driver architecture
 *          - Compile-time validation and optimization
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>

using namespace std::string_view_literals;

namespace roff {
namespace terminal {
namespace dasi450 {

// Type-safe constants using constexpr
constexpr std::uint16_t INCH_RESOLUTION = 240;

/**
 * @brief Pure C++17 DASI450 Terminal Configuration Class
 * 
 * High-resolution DASI450 terminal configuration with 12 characters per inch
 * and 8 lines per inch spacing for precise typographic output.
 */
class DASI450Configuration {
  public:
    // Type aliases for better code clarity
    using ResolutionType = std::uint16_t;
    using ControlBitsType = std::uint16_t;
    using CharacterTable = std::array<std::string_view, 256 - 32>;

    /**
     * @brief Configuration validation results
     */
    enum class ValidationResult : std::uint8_t {
        Valid = 0,
        InvalidResolution = 1,
        InvalidControlBits = 2,
        InvalidSequences = 3,
        InvalidCharacterTable = 4
    };

    /**
     * @brief DASI450 control bit configuration
     */
    class ControlBitConfiguration {
      private:
        ControlBitsType set_bits_{0};
        ControlBitsType reset_bits_{0177420};

      public:
        constexpr ControlBitConfiguration() = default;

        constexpr ControlBitConfiguration(ControlBitsType set, ControlBitsType reset)
            : set_bits_(set), reset_bits_(reset) {}

        [[nodiscard]] constexpr ControlBitsType set_bits() const noexcept { return set_bits_; }
        [[nodiscard]] constexpr ControlBitsType reset_bits() const noexcept { return reset_bits_; }

        constexpr void set_bits(ControlBitsType bits) noexcept { set_bits_ = bits; }
        constexpr void reset_bits(ControlBitsType bits) noexcept { reset_bits_ = bits; }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return true; // Basic validation - could be enhanced with specific bit patterns
        }
    };

    /**
     * @brief High-resolution spacing configuration for DASI450
     */
    class ResolutionConfiguration {
      private:
        ResolutionType horizontal_{INCH_RESOLUTION / 60}; // 60 units per inch horizontally
        ResolutionType horizontal_{INCH_RESOLUTION / 60}; // 60 units per inch horizontally
        ResolutionType vertical_{INCH_RESOLUTION / 48}; // 48 units per inch vertically
        ResolutionType newline_{INCH_RESOLUTION / 8}; // 8 lines per inch
        ResolutionType character_{INCH_RESOLUTION / 12}; // 12 characters per inch
        ResolutionType em_{INCH_RESOLUTION / 12}; // 12 em units per inch
        ResolutionType halfline_{INCH_RESOLUTION / 12}; // Halfline spacing
        ResolutionType adjustment_{INCH_RESOLUTION / 12}; // Adjustment spacing

      public:
        ResolutionConfiguration() = default;
        constexpr ResolutionConfiguration(
        ResolutionConfiguration(
            ResolutionType h, ResolutionType v, ResolutionType nl,
            ResolutionType ch, ResolutionType em, ResolutionType hl, ResolutionType adj)
            : horizontal_(h), vertical_(v), newline_(nl), character_(ch),
              em_(em), halfline_(hl), adjustment_(adj) {}
        // Accessors
        [[nodiscard]] constexpr ResolutionType horizontal() const noexcept {
            return horizontal_; }
        [[nodiscard]] constexpr ResolutionType vertical() const noexcept {
            return vertical_; }
        [[nodiscard]] constexpr ResolutionType newline() const noexcept {
            return newline_; }
        [[nodiscard]] constexpr ResolutionType character() const noexcept {
            return character_; }
        [[nodiscard]] constexpr ResolutionType em() const noexcept {
            return em_; }
        [[nodiscard]] constexpr ResolutionType halfline() const noexcept {
            return halfline_; }
        [[nodiscard]] constexpr ResolutionType adjustment() const noexcept {
            return adjustment_; }

        // Mutators with validation
        // Mutators with validation
        void set_horizontal(ResolutionType value) {
            if (value == 0)
                throw std::invalid_argument("Horizontal resolution must be positive");
            horizontal_ = value;
        }

        void set_vertical(ResolutionType value) {
            if (value == 0)
                throw std::invalid_argument("Vertical resolution must be positive");
            vertical_ = value;
        }
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return horizontal_ > 0 && vertical_ > 0 && character_ > 0 && em_ > 0;
        }
    };

    /**
     * @brief DASI450-specific control sequence configuration
     */
    class ControlSequenceConfiguration {
      private:
        // DASI450 initialization and control sequences
        std::string_view init_{"\0334\033\037\013\033\036\007"sv};
        std::string_view reset_{"\0334\033\037\015\033\036\011"sv};
        std::string_view newline_{"\015\n"sv}; // CR + LF
        std::string_view halfline_reverse_{"\033D"sv}; // DASI halfline reverse
        std::string_view halfline_forward_{"\033U"sv}; // DASI halfline forward
        std::string_view fullline_reverse_{"\033F"sv}; // DASI fullline reverse
        std::string_view bold_on_{""sv}; // No bold support
        std::string_view bold_off_{""sv}; // No bold support
        std::string_view plot_on_{"\0333"sv}; // DASI plot mode on
        std::string_view plot_off_{"\0334"sv}; // DASI plot mode off

      public:
      public:
        ControlSequenceConfiguration() = default;
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
     * @brief DASI450 cursor movement configuration
     */
    class CursorConfiguration {
      private:
        std::string_view up_{"\033\n"sv}; // DASI cursor up
        std::string_view down_{"\n"sv}; // DASI cursor down
        std::string_view right_{" "sv}; // DASI cursor right (space)
        std::string_view left_{"\b"sv}; // DASI cursor left (backspace)

      public:
      public:
        CursorConfiguration() = default;
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
    ControlBitConfiguration control_bits_;
    ResolutionConfiguration resolution_;
    ControlSequenceConfiguration sequences_;
    CursorConfiguration cursor_;
    CharacterTable character_codes_{};
    bool validated_{false};

  public:
    // Constructors
    constexpr DASI450Configuration() = default;

    explicit DASI450Configuration(
        const ControlBitConfiguration &control_bits,
        const ResolutionConfiguration &resolution = {},
        const ControlSequenceConfiguration &sequences = {},
        const CursorConfiguration &cursor = {},
        const CharacterTable &character_codes = {})
        : control_bits_(control_bits), resolution_(resolution),
          sequences_(sequences), cursor_(cursor), character_codes_(character_codes) {
        validate_configuration();
    }

    // Copy constructor with validation
    DASI450Configuration(const DASI450Configuration &other)
        : control_bits_(other.control_bits_), resolution_(other.resolution_),
          sequences_(other.sequences_), cursor_(other.cursor_),
          character_codes_(other.character_codes_) {
        validate_configuration();
    }

    // Move constructor
    DASI450Configuration(DASI450Configuration &&other) noexcept
        : control_bits_(std::move(other.control_bits_)),
          resolution_(std::move(other.resolution_)),
          sequences_(std::move(other.sequences_)),
          cursor_(std::move(other.cursor_)),
          character_codes_(std::move(other.character_codes_)),
          validated_(other.validated_) {}

    // Assignment operators
    DASI450Configuration &operator=(const DASI450Configuration &other) {
        if (this != &other) {
            control_bits_ = other.control_bits_;
            resolution_ = other.resolution_;
            sequences_ = other.sequences_;
            cursor_ = other.cursor_;
            character_codes_ = other.character_codes_;
            validate_configuration();
        }
        return *this;
    }

    DASI450Configuration &operator=(DASI450Configuration &&other) noexcept {
        if (this != &other) {
            control_bits_ = std::move(other.control_bits_);
            resolution_ = std::move(other.resolution_);
            sequences_ = std::move(other.sequences_);
            cursor_ = std::move(other.cursor_);
            character_codes_ = std::move(other.character_codes_);
            validated_ = other.validated_;
        }
        return *this;
    }

    // Accessors - const-correct and type-safe
    [[nodiscard]] constexpr const ControlBitConfiguration &control_bits() const noexcept {
        return control_bits_;
    }

    [[nodiscard]] constexpr const ResolutionConfiguration &resolution() const noexcept {
        return resolution_;
    }

    [[nodiscard]] constexpr const ControlSequenceConfiguration &sequences() const noexcept {
        return sequences_;
    }

    [[nodiscard]] constexpr const CursorConfiguration &cursor() const noexcept {
        return cursor_;
    }

    [[nodiscard]] constexpr const CharacterTable &character_codes() const noexcept {
        return character_codes_;
    }

    // Validation and utility methods
    [[nodiscard]] ValidationResult validate() const noexcept {
        if (!control_bits_.is_valid())
            return ValidationResult::InvalidControlBits;
        if (!resolution_.is_valid())
            return ValidationResult::InvalidResolution;
        return ValidationResult::Valid;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return validated_ && control_bits_.is_valid() && resolution_.is_valid();
    }

    [[nodiscard]] constexpr std::size_t memory_footprint() const noexcept {
        return sizeof(*this);
    }

    // DASI450-specific capability queries
    [[nodiscard]] constexpr bool supports_high_resolution() const noexcept {
        return resolution_.character() == INCH_RESOLUTION / 12; // 12 chars/inch
    }

    [[nodiscard]] constexpr bool supports_plot_mode() const noexcept {
        return sequences_.supports_plot_mode();
    }

    [[nodiscard]] constexpr bool supports_halfline_movement() const noexcept {
        return sequences_.supports_halfline_movement();
    }

  private:
    void validate_configuration() {
        const auto result = validate();
        if (result != ValidationResult::Valid) {
            validated_ = false;
            throw std::invalid_argument("Invalid DASI450 configuration");
        }
        validated_ = true;
    }
};

/**
 * @brief DASI450 character width and code tables
 */
namespace character_tables {

/**
 * @brief DASI450 character width table optimized for 12 chars/inch resolution
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

    // Extended character set - DASI450 specific spacing
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
 * @brief DASI450 character code table
 */
constexpr std::array<std::string_view, 256 - 32> dasi450_character_codes = {{// ASCII 32-47: Space and punctuation
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

                                                                             // Extended character set - DASI450 specific
                                                                             "\000\0"sv, // narrow space
                                                                             "\001-"sv, // hyphen
                                                                             "\001\260"sv, // bullet
                                                                             "\001\261"sv, // square
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

                                                                             // Continue with mathematical and special symbols...
                                                                             "\001\341"sv, "\001\342"sv, "\001\343"sv, "\001\344"sv, "\001\345"sv, "\001\346"sv, "\001\347"sv, "\001\350"sv,
                                                                             "\001\351"sv, "\001\352"sv, "\001\353"sv, "\001\354"sv, "\001\355"sv, "\001\356"sv, "\001\357"sv, "\001\360"sv,
                                                                             "\001\361"sv, "\001\362"sv, "\001\363"sv, "\001\364"sv, "\001\365"sv, "\001\366"sv, "\001\367"sv, "\001\370"sv,
                                                                             "\001\301"sv, "\001\304"sv, "\001\310"sv, "\001\313"sv, "\001\316"sv, "\001\320"sv, "\001\323"sv, "\000\0"sv,
                                                                             "\001\325"sv, "\001\326"sv, "\001\327"sv, "\001\330"sv, "\001\221"sv, "\001\362"sv, "\000\0"sv, "\002>="sv,
                                                                             "\002<="sv, "\002=="sv, "\001-"sv, "\002~="sv, "\002~"sv, "\002!="sv, "\002->"sv, "\002<-"sv,
                                                                             "\001^"sv, "\001v"sv, "\001="sv, "\001x"sv, "\001/"sv, "\002+-"sv, "\001U"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\002oo"sv, "\001d"sv, "\001\\/"sv, "\001~"sv,
                                                                             "\001S"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\001+"sv, "\002(R)"sv, "\002(C)"sv, "\001|"sv,
                                                                             "\002ct"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\001*"sv, "\000\0"sv, "\001|"sv, "\001O"sv,
                                                                             "\001\154"sv, "\001\155"sv, "\001\153"sv, "\001\152"sv, "\001\156"sv, "\001\165"sv, "\001\170"sv, "\001\155"sv,
                                                                             "\001\152"sv, "\001\154"sv, "\001\153"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv,
                                                                             "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv}};

} // namespace character_tables

} // namespace dasi450
} // namespace terminal
} // namespace roff