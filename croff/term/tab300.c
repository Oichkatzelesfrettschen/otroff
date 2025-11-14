/* C17 - no scaffold */
/*
 * tab300.c - DASI 300 nroff driving tables (C90 portable)
 *
 * This file defines the terminal table for the DASI 300 terminal, including width and code tables.
 * Refactored for C90 portability, clarity, and maintainability.
 *
 * Usage:
 *   - The struct termtab300 describes the terminal's capabilities and control sequences.
 *   - The table instance 't' is initialized with DASI 300-specific values.
 *   - The character code table (codetab) is included from code.300.
 *   - This file is intended for inclusion in nroff/roff terminal drivers.
 */

// Define the number of units per inch for the terminal's resolution.
// The value 240 was chosen because it represents the resolution of the DASI 300 terminal,
// which operates at 240 units per inch, providing sufficient precision for text and graphics rendering.
#define INCH 240

// DASI300: This comment refers to the DASI 300 terminal, which is the target device for these driving tables.
// Replace <stddef.h> with C++ headers and namespace usage
#include <stddef.h>
#include <stdint.h>
#include <string_view>

// Define named constants for repeated strings
#define BELL_CHARACTER "\007"
#define BELL "\007"

/*
 * struct termtab300 - Terminal table structure for DASI 300
 *
 * Members:
 *   bset      - Bits to set on initialization
 *   breset    - Bits to reset on initialization
 *   Hor       - Horizontal resolution (units per space)
 *   Vert      - Vertical resolution (units per line)
 *   Newline   - Newline spacing (units)
 *   Char      - Character width (units)
 *   Em        - Em width (units)
 *   Halfline  - Halfline spacing (units)
 *   Adj       - Adjustment spacing (units)
 *   twinit    - Terminal initialization string
 *   twrest    - Terminal reset string
 *   twnl      - Newline string
 *   hlr       - Halfline reverse string
 *   hlf       - Halfline forward string
 *   flr       - Fullline reverse string
 *   bdon      - Bold on string
 *   bdoff     - Bold off string
 *   ploton    - Plot mode on string
 *   plotoff   - Plot mode off string
 *   up        - Cursor up string
 *   down      - Cursor down string
 *   right     - Cursor right string
 *   codetab   - Character code table (see code.300)
 *   reserved  - Reserved/unused
 */
struct termtab300 {
    uint16_t bset;
    uint16_t breset;
    uint16_t Hor;
    uint16_t Vert;
    uint16_t Newline;
    uint16_t Char;
    uint16_t Em;
    uint16_t Halfline;
    uint16_t Adj;
    string_view twinit;
    string_view twrest; // Both fields will share the same pointer to reduce memory usage
    string_view twnl;
    string_view hlr;
    string_view hlf;
    string_view flr;
    string_view bdon;
    string_view bdoff;
    string_view ploton;
    string_view plotoff;
    string_view up;
    string_view down;
    string_view right;
    const char *codetab[256 - 32];
     uint16_t reserved{};
};
static_assert(sizeof(termtab300) <= 4096, "terminal table must remain small");

/*
 * Table instance for DASI 300 terminal.
 * All fields are initialized in C90-compliant order.
 * The included file (code.300) must provide the codetab array and must NOT include its own closing brace or semicolon.
 * The reserved field is set to 0.
 */
/**
 * @brief Configuration table for Termtab 300 terminal device.
 * 
 * This structure defines the terminal characteristics and control sequences
 * for a Termtab 300 terminal, including resolution settings, character
 * dimensions, and various control strings for terminal operations.
 * 
 * The configuration includes:
 * - Terminal initialization and reset bit patterns
 * - Resolution specifications for horizontal/vertical positioning
 * - Character and spacing dimensions in units per inch
 * - Control sequences for cursor movement, plotting, and formatting
 * - Terminal-specific escape sequences and command strings
 * 
 * @note Some features like halfline adjustment and bold formatting are
 *       not supported on this terminal type and are set to empty values.
 * @note The codetab character encoding table is included from an external
 *       file "code.300" which contains terminal-specific character mappings.
 */
inline const termtab300 t{
    0, // bset: Bit settings for terminal initialization.
    0177420, // breset: Octal value representing terminal reset settings; used to configure the terminal's control bits during initialization.
    INCH / 60, // Hor: Horizontal resolution in units per inch
    INCH / 48, // Vert: Vertical resolution in units per inch
    INCH / 6, // Newline: Newline spacing in units per inch
    INCH / 10, // Char: Character width in units per inch
    INCH / 10, // Em: Em width in units per inch
    0, // Halfline: Not used, set to 0
    0, // Adj: Not used, set to 0
    "\007"sv, // twinit: Bell character
    "\007"sv, // twrest: Bell character
    "\007"sv, // twnl: Bell character
    "\007"sv, // hlr: Bell character
    string_view{}, // hlf: Halfline forward string (not used)
    "\013"sv, // flr: Full-line reverse string
    ""sv, // bdon: Bold on string (not used)
    ""sv, // bdoff: Bold off string (not used)
    "\006"sv, // ploton: Plot mode on string
    "\033\006"sv, // plotoff: Plot mode off string
    "\013"sv, // up: Cursor up string
    "\n"sv, // down: Cursor down string
    " "sv, // right: Cursor right string
// codetab is included from external file
#include "code.300"
    , 0 // reserved: Reserved field
};
