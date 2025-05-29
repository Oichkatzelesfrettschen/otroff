/*
 * tab300s-12.c - DASI300S nroff driving tables (C90 portable)
 *
 * This file defines the terminal table for the DASI300S terminal, including width and code tables.
 * Refactored for C90 portability, clarity, and maintainability.
 */

#define INCH 240

/*
 * Terminal table structure for DASI300S.
 *
 * Members:
 *   bset     - Bits to set on initialization
 *   breset   - Bits to reset on initialization
 *   Hor      - Horizontal resolution (units per space)
 *   Vert     - Vertical resolution (units per line)
 *   Newline  - Newline spacing (units)
 *   Char     - Character width (units)
 *   Em       - Em width (units)
 *   Halfline - Halfline spacing (units)
 *   Adj      - Adjustment spacing (units)
 *   twinit   - Terminal initialization string
 *   twrest   - Terminal reset string
 *   twnl     - Newline string
 *   hlr      - Halfline reverse string
 *   hlf      - Halfline forward string
 *   flr      - Fullline reverse string
 *   bdon     - Bold on string
 *   bdoff    - Bold off string
 *   ploton   - Plot mode on string
 *   plotoff  - Plot mode off string
 *   up       - Cursor up string
 *   down     - Cursor down string
 *   right    - Cursor right string
 *   left     - Cursor left string
 *   codetab  - Character code table (see code.300)
 *   zzz      - Reserved/unused
 */
#include <stddef.h> /* standard definitions */

struct termtab {
    int bset;
    int breset;
    int Hor;
    int Vert;
    int Newline;
    int Char;
    int Em;
    int Halfline;
    int Adj;
    char *twinit;
    char *twrest;
    char *twnl;
    char *hlr;
    char *hlf;
    char *flr;
    char *bdon;
    char *bdoff;
    char *ploton;
    char *plotoff;
    char *up;
    char *down;
    char *right;
    char *left;
    char *codetab[256 - 32];
    int zzz;
};

/*
 * Table instance for DASI300S terminal.
 * All fields are initialized in C90-compliant order.
 * The included file (code.300) must provide the codetab array and the zzz value,
 * and must NOT include its own closing brace or semicolon.
 * This file must end with a single closing brace and semicolon after the include.
 */
struct termtab t = {
    0,          /* bset */
    0177420,    /* breset */
    INCH / 60,  /* Hor */
    INCH / 48,  /* Vert */
    INCH / 8,   /* Newline */
    INCH / 12,  /* Char */
    INCH / 12,  /* Em */
    INCH / 16,  /* Halfline */
    INCH / 12,  /* Adj */
    "\033\006", /* twinit */
    "\033\006", /* twrest */
    "\015\n",    /* twnl */
    "",         /* hlr */
    "",         /* hlf */
    "\032",     /* flr */
    "\033E",    /* bdon */
    "\033E",    /* bdoff */
    "\006",     /* ploton */
    "\033\006", /* plotoff */
    "\032",     /* up */
    "\n",       /* down */
    " ",        /* right */
    "\b",       /* left */
    /* codetab and zzz are provided by the included file below. */
    /* IMPORTANT: code.300 must NOT contain a closing brace or semicolon! */
#include "code.300"
};
