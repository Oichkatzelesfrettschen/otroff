/**
 * @file tab37.cpp
 * @brief TTY M37 nroff driving table definitions (pure C++17).
 *
 * Defines the TermTab class encapsulating terminal metrics and control sequences
 * for the TTY M37. Provides safe accessors and a codetab mapping character codes
 * 32..255 to string representations. Uses C++17 constexpr initialization and RAII.
 */

#include <cstdint>
#include <array>
#include <string_view>
#include <stdexcept>

using namespace std::string_view_literals;

namespace croff {
namespace term {

/**
 * @class TermTab
 * @brief TTY M37 terminal capability definitions.
 *
 * Provides horizontal/vertical metrics and control sequences,
 * along with a codetab mapping character codes (32..255) to strings.
 */
class TermTab {
  public:
    /// Number of terminal codetab entries (256 - 32).
    static constexpr std::size_t CODETAB_SIZE = 224;

    /// Base unit for measurements (240 units per inch).
    static constexpr std::uint16_t INCH = 240;

    /**
     * @brief Constructs a TermTab with TTY M37 specific values.
     *
     * Initializes all metrics (bset, breset, Hor, Vert, Newline, Char, Em, Halfline, Adj)
     * and control sequences (twinit, twrest, twnl, hlr, hlf, flr, bdon, bdoff,
     * ploton, plotoff, up, down, right, left). Also initializes codetab_ from INIT_CODETAB.
     */
    constexpr TermTab()
        : bset_(0),
          breset_(0),
          Hor_(INCH / 10),
          Vert_(INCH / 12),
          Newline_(INCH / 6),
          Char_(INCH / 10),
          Em_(INCH / 10),
          Halfline_(INCH / 12),
          Adj_(INCH / 10),
          twinit_(""sv),
          twrest_(""sv),
          twnl_("\n"sv),
          hlr_("\x1B"
               "8"sv),
          hlf_("\x1B"
               "9"sv),
          flr_("\x1B"
               "7"sv),
          bdon_(""sv),
          bdoff_(""sv),
          ploton_(""sv),
          plotoff_(""sv),
          up_(""sv),
          down_(""sv),
          right_(""sv),
          left_(""sv),
          codetab_(INIT_CODETAB) {
        // No further runtime validation needed; all fields are constexpr initialized.
    }

    /// @name Accessors for terminal metrics
    /// @{
    /**
     * @brief Retrieves the bit‐set mask for bold enabling.
     * @return The bset_ value.
     */
    constexpr std::uint16_t bset() const noexcept { return bset_; }

    /**
     * @brief Retrieves the bit‐reset mask for disabling attributes.
     * @return The breset_ value.
     */
    constexpr std::uint16_t breset() const noexcept { return breset_; }

    /**
     * @brief Horizontal resolution (units per movement).
     * @return The Hor_ value.
     */
    constexpr std::uint16_t Hor() const noexcept { return Hor_; }

    /**
     * @brief Vertical resolution (units per movement).
     * @return The Vert_ value.
     */
    constexpr std::uint16_t Vert() const noexcept { return Vert_; }

    /**
     * @brief Vertical spacing for newline.
     * @return The Newline_ value.
     */
    constexpr std::uint16_t Newline() const noexcept { return Newline_; }

    /**
     * @brief Width of a single character cell.
     * @return The Char_ value.
     */
    constexpr std::uint16_t Char() const noexcept { return Char_; }

    /**
     * @brief Width of an 'em' space.
     * @return The Em_ value.
     */
    constexpr std::uint16_t Em() const noexcept { return Em_; }

    /**
     * @brief Height of a half‐line movement.
     * @return The Halfline_ value.
     */
    constexpr std::uint16_t Halfline() const noexcept { return Halfline_; }

    /**
     * @brief Adjustment space.
     * @return The Adj_ value.
     */
    constexpr std::uint16_t Adj() const noexcept { return Adj_; }
    /// @}

    /// @name Accessors for control sequences
    /// @{
    /**
     * @brief Initialization sequence for terminal.
     * @return The twinit_ string_view.
     */
    constexpr std::string_view twinit() const noexcept { return twinit_; }

    /**
     * @brief Restart sequence for terminal.
     * @return The twrest_ string_view.
     */
    constexpr std::string_view twrest() const noexcept { return twrest_; }

    /**
     * @brief Sequence for newline on terminal.
     * @return The twnl_ string_view.
     */
    constexpr std::string_view twnl() const noexcept { return twnl_; }

    /**
     * @brief Half‐line reverse movement sequence.
     * @return The hlr_ string_view.
     */
    constexpr std::string_view hlr() const noexcept { return hlr_; }

    /**
     * @brief Half‐line forward (down) movement sequence.
     * @return The hlf_ string_view.
     */
    constexpr std::string_view hlf() const noexcept { return hlf_; }

    /**
     * @brief Full‐line reverse (up) sequence.
     * @return The flr_ string_view.
     */
    constexpr std::string_view flr() const noexcept { return flr_; }

    /**
     * @brief Boldface on sequence.
     * @return The bdon_ string_view.
     */
    constexpr std::string_view bdon() const noexcept { return bdon_; }

    /**
     * @brief Boldface off sequence.
     * @return The bdoff_ string_view.
     */
    constexpr std::string_view bdoff() const noexcept { return bdoff_; }

    /**
     * @brief Plot mode on sequence.
     * @return The ploton_ string_view.
     */
    constexpr std::string_view ploton() const noexcept { return ploton_; }

    /**
     * @brief Plot mode off sequence.
     * @return The plotoff_ string_view.
     */
    constexpr std::string_view plotoff() const noexcept { return plotoff_; }

    /**
     * @brief Cursor up movement sequence.
     * @return The up_ string_view.
     */
    constexpr std::string_view up() const noexcept { return up_; }

    /**
     * @brief Cursor down movement sequence.
     * @return The down_ string_view.
     */
    constexpr std::string_view down() const noexcept { return down_; }

    /**
     * @brief Cursor right movement sequence.
     * @return The right_ string_view.
     */
    constexpr std::string_view right() const noexcept { return right_; }

    /**
     * @brief Cursor left movement sequence.
     * @return The left_ string_view.
     */
    constexpr std::string_view left() const noexcept { return left_; }
    /// @}

    /**
     * @brief Access the codetab mapping for a given index [0..CODETAB_SIZE).
     *
     * @param idx  Zero-based index corresponding to character code (32 + idx).
     * @return The string_view from codetab_ at position idx.
     * @throws std::out_of_range if idx ≥ CODETAB_SIZE.
     */
    std::string_view code_at(std::size_t idx) const {
        if (idx >= CODETAB_SIZE)
            throw std::out_of_range("TermTab: code index out of range");
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

    /// Static initializer for codetab_ (must be defined out-of-line).
    static const std::array<std::string_view, CODETAB_SIZE> INIT_CODETAB;
};

} // namespace term
} // namespace croff

/// Global instance of TermTab for TTY M37
const croff::term::TermTab t;
// Definition of const static member INIT_CODETAB
const std::array<std::string_view, croff::term::TermTab::CODETAB_SIZE> croff::term::TermTab::INIT_CODETAB{{
    "\001 ", /* space */
    "\001!", /*!*/
    "\001\"", /* " */
    "\001#", /* # */
    "\001$", /* $ */
    "\001%", /* % */
    "\001&", /* & */
    "\001'", /* ' close */
    "\001(", /* ( */
    "\001)", /* ) */
    "\001*", /* * */
    "\001+", /* + */
    "\001,", /* , */
    "\001-", /* - hyphen */
    "\001.", /* . */
    "\001/", /* / */
    "\0010", /* 0 */
    "\0011", /* 1 */
    "\0012", /* 2 */
    "\0013", /* 3 */
    "\0014", /* 4 */
    "\0015", /* 5 */
    "\0016", /* 6 */
    "\0017", /* 7 */
    "\0018", /* 8 */
    "\0019", /* 9 */
    "\001:", /* : */
    "\001;", /* ; */
    "\001<", /* < */
    "\001=", /* = */
    "\001>", /* > */
    "\001?", /* ? */
    "\001@", /* @ */
    "\001A", /* A */
    "\001B", /* B */
    "\001C", /* C */
    "\001D", /* D */
    "\001E", /* E */
    "\001F", /* F */
    "\001G", /* G */
    "\001H", /* H */
    "\001I", /* I */
    "\001J", /* J */
    "\001K", /* K */
    "\001L", /* L */
    "\001M", /* M */
    "\001N", /* N */
    "\001O", /* O */
    "\001P", /* P */
    "\001Q", /* Q */
    "\001R", /* R */
    "\001S", /* S */
    "\001T", /* T */
    "\001U", /* U */
    "\001V", /* V */
    "\001W", /* W */
    "\001X", /* X */
    "\001Y", /* Y */
    "\001Z", /* Z */
    "\001[", /* [ */
    "\001\\", /* \\ */
    "\001]", /* ] */
    "\001^", /* ^ */
    "\001_", /* _ dash */
    "\001`", /* ` open */
    "\001a", /* a */
    "\001b", /* b */
    "\001c", /* c */
    "\001d", /* d */
    "\001e", /* e */
    "\001f", /* f */
    "\001g", /* g */
    "\001h", /* h */
    "\001i", /* i */
    "\001j", /* j */
    "\001k", /* k */
    "\001l", /* l */
    "\001m", /* m */
    "\001n", /* n */
    "\001o", /* o */
    "\001p", /* p */
    "\001q", /* q */
    "\001r", /* r */
    "\001s", /* s */
    "\001t", /* t */
    "\001u", /* u */
    "\001v", /* v */
    "\001w", /* w */
    "\001x", /* x */
    "\001y", /* y */
    "\001z", /* z */
    "\001{", /* { */
    "\001|", /* | */
    "\001}", /* } */
    "\001~", /* ~ */
    "\000", /* narrow sp */
    "\001-", /* hyphen */
    "\001o\b+", /* bullet */
    "\002[]", /* square */
    "\001-", /* 3/4 em */
    "\001_", /* rule */
    "\0031/4", /* 1/4 */
    "\0031/2", /* 1/2 */
    "\0033/4", /* 3/4 */
    "\001-", /* minus */
    "\002fi", /* fi */
    "\002fl", /* fl */
    "\002ff", /* ff */
    "\003ffi", /* ffi */
    "\003ffl", /* ffl */
    "\000", /* degree */
    "\000", /* dagger */
    "\000", /* section */
    "\001'", /* foot mark */
    "\001'", /* acute accent */
    "\001`", /* grave accent */
    "\001_", /* underrule */
    "\001/", /* slash (longer) */
    "\000", /* half narrow space */
    "\001 ", /* non-paddable space */
    "\001\016A\017", /* alpha */
    "\001\016B\017", /* beta */
    "\001\016\\\017", /* gamma */
    "\001\016D\017", /* delta */
    "\001\016S\017", /* epsilon */
    "\001\016Q\017", /* zeta */
    "\001\016N\017", /* eta */
    "\001\016O\017", /* theta */
    "\001i", /* iota */
    "\001k", /* kappa */
    "\001\016L\017", /* lambda */
    "\001\016M\017", /* mu */
    "\001\016@\017", /* nu */
    "\001\016X\017", /* xi */
    "\001o", /* omicron */
    "\001\016J\017", /* pi */
    "\001\016K\017", /* rho */
    "\001\016Y\017", /* sigma */
    "\001\016I\017", /* tau */
    "\001v", /* upsilon */
    "\001\016U\017", /* phi */
    "\001x", /* chi */
    "\001\016V\017", /* psi */
    "\001\016C\017", /* omega */
    "\001\016G\017", /* Gamma */
    "\001\016W\017", /* Delta */
    "\001\016T\017", /* Theta */
    "\001\016E\017", /* Lambda */
    "\000", /* Xi */
    "\001\016P\017", /* Pi */
    "\001\016R\017", /* Sigma */
    "\000", /*   */
    "\001Y", /* Upsilon */
    "\001\016F\017", /* Phi */
    "\001\016H\017", /* Psi */
    "\001\016Z\017", /* Omega */
    "\000", /* square root */
    "\000", /* terminal sigma */
    "\000", /* root en */
    "\001>\b_", /* >= */
    "\001<\b_", /* <= */
    "\001=\b_", /* identically equal */
    "\001-", /* equation minus */
    "\001=\b~", /* approx = */
    "\001\0339~\0338", /* approximates */
    "\001=\b/", /* not equal */
    "\002->", /* right arrow */
    "\002<-", /* left arrow */
    "\001|\b^", /* up arrow */
    "\000", /* down arrow */
    "\001=", /* equation equal */
    "\001x", /* multiply */
    "\001/", /* divide */
    "\001+\b_", /* plus-minus */
    "\001U", /* cup (union) */
    "\000", /* cap (intersection) */
    "\000", /* subset of */
    "\000", /* superset of */
    "\000", /* improper subset */
    "\000", /* improper superset */
    "\002oo", /* infinity */
    "\001\016]\017", /* partial derivative */
    "\001\016[\017", /* gradient */
    "\001\016_\017", /* not */
    "\001\016^\017", /* integral sign */
    "\000", /* proportional to */
    "\000", /* empty set */
    "\000", /* member of */
    "\001+", /* equation plus */
    "\001\0338r\0339", /* registered */
    "\001\0338c\0339", /* copyright */
    "\001|", /* box rule */
    "\001c\b/", /* cent sign */
    "\000", /* dbl dagger */
    "\000", /* right hand */
    "\001*", /* left hand */
    "\001*", /* math * */
    "\000", /* bell system sign */
    "\001|", /* or (was star) */
    "\001O", /* circle */
    "\001|", /* left top (of big curly) */
    "\001|", /* left bottom */
    "\001|", /* right top */
    "\001|", /* right bot */
    "\001|", /* left center of big curly bracket */
    "\001|", /* right center of big curly bracket */
    "\001|", /* bold vertical */
    "\001|", /* left floor (left bot of big sq bract) */
    "\001|", /* right floor (rb of ") */
    "\001|", /* left ceiling (lt of ") */
    "\001|" /* right ceiling (rt of ") */
}};
