/*
 * tab450-12.c - DASI450 nroff driving tables (C90 portable)
 *
 * This file defines the terminal table for the DASI450 terminal (12 chars/inch, 6 lines/inch),
 * including width and code tables. Refactored for C90 portability, clarity, and maintainability.
 *
 * Usage:
 *   - The struct termtab describes the terminal's capabilities and control sequences.
 *   - The table instance 't' is initialized with DASI450-specific values.
 *   - The character code table (codetab) is included from code.300.
 *   - This file is intended for inclusion in nroff/roff terminal drivers.
 */

#define INCH 240

#include <stdint.h>

/*
 * struct termtab - Terminal table structure for DASI450
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
 *   left      - Cursor left string
 *   codetab   - Character code table (see code.300)
 *   zzz       - Reserved/unused
 */
struct TermTab {
    std::uint32_t  bset{};
    std::uint32_t  breset{};
    int            Hor{};
struct TermTab {
    uint32_t  bset;
    uint32_t  breset;
    int       Hor;
    int       Vert;
    int       Newline;
    int       Char;
    int       Em;
    int       Halfline;
    int       Adj;
    char     *twinit;
    char     *twrest;
    char     *twnl;
    char     *hlr;
    char     *hlf;
    char     *flr;
    char     *bdon;
    char     *bdoff;
    char     *ploton;
    char     *plotoff;
    char     *up;
    char     *down;
    char     *right;
    char     *left;
    char     *codetab[256 - 32];
    int       zzz;
};
 * The included file (code.300) must provide the codetab array and the zzz value,
 * and must NOT include its own closing brace or semicolon.
 */
inline constexpr TermTab t = {
    0, /* bset */
    0177420, /* breset */
    INCH / 60, /* Hor */
struct TermTab t = {
    INCH / 6, /* Newline */
    INCH / 12, /* Char */
    INCH / 12, /* Em */
    INCH / 12, /* Halfline */
    INCH / 12, /* Adj */
    "\0334\033\037\013", /* twinit */
    "\0334\033\037\015", /* twrest */
    "\015\n", /* twnl */
    "\033D", /* hlr */
    "\033U", /* hlf */
    "\033\n", /* flr */
    "", /* bdon */
    "", /* bdoff */
    "\0333", /* ploton */
    "\0334", /* plotoff */
    "\033\n", /* up */
    "\n", /* down */
    " ", /* right */
    "\b", /* left */
/* codetab and zzz are provided by the included file below: */
#include "code.300"
};
