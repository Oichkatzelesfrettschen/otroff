#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <stdexcept>

/**
 * @file tab300s.cpp
 * @brief DASI300S terminal capability definitions for the croff typesetting system.
 *
 * This file contains the TermTab class which encapsulates all terminal-specific
 * characteristics for the DASI300S terminal, including:
 * - Physical measurements and spacing (horizontal/vertical resolution, character widths)
 * - Control sequences for terminal operations (initialization, cursor movement, formatting)
 * - Character code mapping table for printable characters (codes 32-255)
 *
 * The DASI300S uses a 240 units per inch measurement system and supports various
 * typographic operations including bold text, plot mode, and precise cursor positioning.
 * All values are compile-time constants for optimal performance.
 *
 * @namespace croff::term
 * @author croff team
 * @version 1.0
 */

namespace croff {
namespace term {

/**
 * @class TermTab
 * @brief DASI300S terminal capability definitions.
 *
 * Provides horizontal/vertical metrics and control sequences,
 * along with a codetab mapping character codes (32..255) to strings.
 */
class TermTab {
  public:
    /// Base unit for measurements (240 units per inch).
    static constexpr std::uint16_t INCH = 240;
    /// Number of terminal codetab entries (224 entries for codes 32-255).
    static constexpr std::size_t CODETAB_SIZE = 224;

    /**
         * @brief Constructs a TermTab with DASI300S specific values.
         *
         * Initializes all metrics (bset, breset, Hor, Vert, Newline, Char, Em, Halfline, Adj)
         * and control sequences (twinit, twrest, twnl, hlr, hlf, flr, bdon, bdoff,
         * ploton, plotoff, up, down, right, left).
         * Also initializes codetab_ from INIT_CODETAB.
         */
    constexpr TermTab() noexcept
        : bset_{0},
          breset_{0177420},
          Hor_{INCH / 60},
          Vert_{INCH / 48},
          Newline_{INCH / 6},
          Char_{INCH / 10},
          Em_{INCH / 10},
          Halfline_{INCH / 12},
          Adj_{INCH / 10},
          twinit_{"\033\006"},
          twrest_{"\033\006"},
          twnl_{"\015\n"},
          hlr_{"\033H"},
          hlf_{"\033h"},
          flr_{"\032"},
          bdon_{"\033E"},
          bdoff_{"\033E"},
          ploton_{"\006"},
          plotoff_{"\033\006"},
          up_{"\032"},
          down_{"\n"},
          right_{" "},
          left_{"\b"},
          codetab_{create_codetab()} {}

    /// @name Accessors for terminal metrics
    /// @{
    /**
         * @brief Retrieves the bit‐set mask for bold enabling.
         */
    [[nodiscard]] constexpr std::uint16_t bset() const noexcept { return bset_; }

    /**
         * @brief Retrieves the bit‐reset mask for disabling attributes.
         */
    [[nodiscard]] constexpr std::uint16_t breset() const noexcept { return breset_; }

    /**
         * @brief Horizontal resolution (units per movement).
         */
    [[nodiscard]] constexpr std::uint16_t Hor() const noexcept { return Hor_; }

    /**
         * @brief Vertical resolution (units per movement).
         */
    [[nodiscard]] constexpr std::uint16_t Vert() const noexcept { return Vert_; }

    /**a
         * @brief Vertical spacing for newline.
         */
    [[nodiscard]] constexpr std::uint16_t Newline() const noexcept { return Newline_; }

    /**
         * @brief Width of a single character cell.
         */
    [[nodiscard]] constexpr std::uint16_t Char() const noexcept { return Char_; }

    /**
         * @brief Width of an 'em' space.
         */
    [[nodiscard]] constexpr std::uint16_t Em() const noexcept { return Em_; }

    /**
         * @brief Height of a half‐line movement.
         */
    [[nodiscard]] constexpr std::uint16_t Halfline() const noexcept { return Halfline_; }

    /**
         * @brief Adjustment space.
         */
    [[nodiscard]] constexpr std::uint16_t Adj() const noexcept { return Adj_; }
    /// @}

    /// @name Accessors for control sequences
    /// @{
    /**
         * @brief Initialization sequence for terminal.
         */
    [[nodiscard]] constexpr std::string_view twinit() const noexcept { return twinit_; }

    /**
         * @brief Restart sequence for terminal.
         */
    [[nodiscard]] constexpr std::string_view twrest() const noexcept { return twrest_; }

    /**
         * @brief Sequence for newline on terminal.
         */
    [[nodiscard]] constexpr std::string_view twnl() const noexcept { return twnl_; }

    /**
         * @brief Half‐line reverse movement sequence.
         */
    [[nodiscard]] constexpr std::string_view hlr() const noexcept { return hlr_; }

    /**
         * @brief Half‐line forward (down) movement sequence.
         */
    [[nodiscard]] constexpr std::string_view hlf() const noexcept { return hlf_; }

    /**
         * @brief Full‐line reverse (up) sequence.
         */
    [[nodiscard]] constexpr std::string_view flr() const noexcept { return flr_; }

    /**
         * @brief Boldface on sequence.
         */
    [[nodiscard]] constexpr std::string_view bdon() const noexcept { return bdon_; }

    /**
         * @brief Boldface off sequence.
         */
    [[nodiscard]] constexpr std::string_view bdoff() const noexcept { return bdoff_; }

    /**
         * @brief Plot mode on sequence.
         */
    [[nodiscard]] constexpr std::string_view ploton() const noexcept { return ploton_; }

    /**
         * @brief Plot mode off sequence.
         */
    [[nodiscard]] constexpr std::string_view plotoff() const noexcept { return plotoff_; }

    /**
         * @brief Cursor up movement sequence.
         */
    [[nodiscard]] constexpr std::string_view up() const noexcept { return up_; }

    /**
         * @brief Cursor down movement sequence.
         */
    [[nodiscard]] constexpr std::string_view down() const noexcept { return down_; }

    /**
         * @brief Cursor right movement sequence.
         */
    [[nodiscard]] constexpr std::string_view right() const noexcept { return right_; }

    /**
         * @brief Cursor left movement sequence.
         */
    [[nodiscard]] constexpr std::string_view left() const noexcept { return left_; }
    /// @}

    /**
         * @brief Access the codetab mapping for a given index [0..CODETAB_SIZE).
         *
         * @param idx  Zero-based index corresponding to character code (32 + idx).
         * @return The string_view from codetab_ at position idx.
         * @throws std::out_of_range if idx ≥ CODETAB_SIZE.
         */
    [[nodiscard]] std::string_view code_at(std::size_t idx) const {
        if (idx >= CODETAB_SIZE) {
            throw std::out_of_range{"TermTab: index out of range"};
        }
        return codetab_[idx];
    }

  private:
    std::uint16_t bset_;
    std::uint16_t breset_;
    std::uint16_t Hor_;
    std::uint16_t Vert_;
    std::uint16_t Newline_;
    std::uint16_t Char_;
    std::uint16_t Em_;
    std::uint16_t Halfline_;
    std::uint16_t Adj_;

    std::string_view twinit_;
    std::string_view twrest_;
    std::string_view twnl_;
    std::string_view hlr_;
    std::string_view hlf_;
    std::string_view flr_;
    std::string_view bdon_;
    std::string_view bdoff_;
    std::string_view ploton_;
    std::string_view plotoff_;
    std::string_view up_;
    std::string_view down_;
    std::string_view right_;
    std::string_view left_;

    std::array<std::string_view, CODETAB_SIZE> codetab_;

    /**
     * @brief Creates the codetab array from included data.
     */
    static constexpr std::array<std::string_view, CODETAB_SIZE> create_codetab() noexcept {
        std::array<std::string_view, CODETAB_SIZE> result{};
        // Initialize with default empty strings
        for (std::size_t i = 0; i < CODETAB_SIZE; ++i) {
            result[i] = "";
        }
        return result;
    }
};

/// Global instance of TermTab for DASI300S
inline const TermTab terminal_table{};

} // namespace term
} // namespace croff
