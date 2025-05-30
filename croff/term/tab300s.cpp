/*
 * termtab structure and DASI300S terminal table definition
 *
 * This file defines the `termtab` structure and initializes an instance `t`
 * for the DASI300S terminal, used by nroff for driving terminal output.
 *
 * Definitions:
 * - INCH: Base unit for measurements (240 units per inch).
 * - struct termtab: Contains terminal control parameters, including:
 *   - bset, breset: Bit settings for terminal initialization/reset.
 *   - Hor, Vert: Horizontal and vertical resolution (units per movement).
 *   - Newline, Char, Em, Halfline, Adj: Spacing and adjustment parameters.
 *   - twinit, twrest, twnl: Initialization, reset, and newline sequences.
 *   - hlr, hlf, flr: Half-line up/down and full-line reverse sequences.
 *   - bdon, bdoff: Boldface on/off sequences.
 *   - ploton, plotoff: Plot mode on/off sequences.
 *   - up, down, right, left: Cursor movement sequences.
 *   - codetab: Character code translation table (populated from "code.300").
 *   - zzz: Reserved/unused.
 *
 * The `t` instance is initialized with DASI300S-specific values and control
 * sequences, enabling nroff to format output appropriately for this terminal.
 */
#define INCH 240
/*
DASI300S
nroff driving tables
width and code tables
*/
#include <cstddef>
#include <cstdint>
#include <string_view>
using namespace std::string_view_literals;

struct termtab {
    std::uint16_t bset;
    std::uint16_t breset;
    std::uint16_t Hor;
    std::uint16_t Vert;
    std::uint16_t Newline;
    std::uint16_t Char;
    std::uint16_t Em;
    std::uint16_t Halfline;
    std::uint16_t Adj;
    std::string_view twinit;
    std::string_view twrest;
    std::string_view twnl;
    std::string_view hlr;
    std::string_view hlf;
    std::string_view flr;
    std::string_view bdon;
    std::string_view bdoff;
    std::string_view ploton;
    std::string_view plotoff;
    std::string_view up;
    std::string_view down;
    std::string_view right;
    std::string_view left;
    const char *codetab[256 - 32];
    [[maybe_unused]] std::uint16_t zzz{};
};
static_assert(sizeof(decltype(*((termtab*)nullptr))) <= 4096, "terminal table must remain small");

inline const termtab t{
    /*bset*/ 0,
    /*breset*/ 0177420,
    /*Hor*/ INCH / 60,
    /*Vert*/ INCH / 48,
    /*Newline*/ INCH / 6,
    /*Char*/ INCH / 10,
    /*Em*/ INCH / 10,
    /*Halfline*/ INCH / 12,
    /*Adj*/ INCH / 10,
    /*twinit*/ "\033\006",
    /*twrest*/ "\033\006",
    /*twnl*/ "\015\n",
    /*hlr*/ "\033H",
    /*hlf*/ "\033h",
    /*flr*/ "\032",
    /*bdon*/ "\033E",
    /*bdoff*/ "\033E",
    /*ploton*/ "\006",
    /*plotoff*/ "\033\006",
    /*up*/ "\032",
    /*down*/ "\n",
    /*right*/ " ",
    /*left*/ "\b",
/*codetab*/
#include "code.300"
};
