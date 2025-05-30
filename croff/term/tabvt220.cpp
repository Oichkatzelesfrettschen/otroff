#include "cxx23_scaffold.hpp"
/*
 * tabvt220.c - VT220 nroff driving tables (C90 portable)
 *
 * This file defines the terminal table for the DEC VT220 terminal, including width and code tables.
 * Refactored for C90 portability, clarity, and maintainability.
 *
 * The VT220 is a Digital Equipment Corporation terminal that supports ANSI X3.64 escape sequences,
 * 7-bit and 8-bit character sets, and various display enhancements.
 *
 * Usage:
 *   - The struct termtab describes the terminal's capabilities and control sequences.
 *   - The table instance 't' is initialized with VT220-specific values.
 *   - The character code table (codetab) uses standard ASCII with DEC special characters.
 *   - This file is intended for inclusion in nroff/roff terminal drivers.
 */

#define INCH 240

#include <stddef.h> /* standard definitions */
#include <stdint.h>
#include <stdbool.h>

/*
 * struct termtab - Terminal table structure for VT220
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
 *   codetab   - Character code table (ASCII + DEC special)
 *   zzz       - Reserved/unused
 */
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
    const char *twinit;
    const char *twrest;
    const char *twnl;
    const char *hlr;
    const char *hlf;
    const char *flr;
    const char *bdon;
    const char *bdoff;
    const char *ploton;
    const char *plotoff;
    const char *up;
    const char *down;
    const char *right;
    const char *left;
    const char *codetab[256 - 32];
    int zzz;
};

/*
 * Table instance for VT220 terminal.
 * All fields are initialized in C90-compliant order.
 * The VT220 supports ANSI escape sequences and DEC private modes.
 */
static const struct termtab t = {
    0, /* bset */
    0, /* breset */
    INCH / 10, /* Hor - 10 chars per inch */
    INCH / 6, /* Vert - 6 lines per inch */
    INCH / 6, /* Newline */
    INCH / 10, /* Char */
    INCH / 10, /* Em */
    INCH / 12, /* Halfline */
    INCH / 10, /* Adj */
    "\033[?7h\033[?1h\033[?3l\033[?4l\033[?5l\033[?6l\033[?8h\033[?25h", /* twinit */
    "\033c\033[!p", /* twrest - full reset */
    "\r\n", /* twnl */
    "\033M", /* hlr - reverse index */
    "\033D", /* hlf - index */
    "\033M", /* flr - reverse index */
    "\033[1m", /* bdon - bold on */
    "\033[0m", /* bdoff - all attributes off */
    "", /* ploton - not supported */
    "", /* plotoff - not supported */
    "\033[A", /* up - cursor up */
    "\033[B", /* down - cursor down */
    "\033[C", /* right - cursor right */
    "\033[D", /* left - cursor left */
    /* VT220 character code table with DEC special characters */
    {
        "\001 ", /*space*/
        "\001!", /*!*/
        "\001\"", /*"*/
        "\001#", /*#*/
        "\001$", /*$*/
        "\001%", /*%*/
        "\001&", /*&*/
        "\001'", /*'*/
        "\001(", /*(*/
        "\001)", /*)*/
        "\001*", /***/
        "\001+", /*+*/
        "\001,", /*,*/
        "\001-", /*-*/
        "\001.", /*.*/
        "\001/", /*/*/
        "\0010", /*0*/
        "\0011", /*1*/
        "\0012", /*2*/
        "\0013", /*3*/
        "\0014", /*4*/
        "\0015", /*5*/
        "\0016", /*6*/
        "\0017", /*7*/
        "\0018", /*8*/
        "\0019", /*9*/
        "\001:", /*:*/
        "\001;", /*;*/
        "\001<", /*<*/
        "\001=", /*=*/
        "\001>", /*>*/
        "\001?", /*?*/
        "\001@", /*@*/
        "\001A", /*A*/
        "\001B", /*B*/
        "\001C", /*C*/
        "\001D", /*D*/
        "\001E", /*E*/
        "\001F", /*F*/
        "\001G", /*G*/
        "\001H", /*H*/
        "\001I", /*I*/
        "\001J", /*J*/
        "\001K", /*K*/
        "\001L", /*L*/
        "\001M", /*M*/
        "\001N", /*N*/
        "\001O", /*O*/
        "\001P", /*P*/
        "\001Q", /*Q*/
        "\001R", /*R*/
        "\001S", /*S*/
        "\001T", /*T*/
        "\001U", /*U*/
        "\001V", /*V*/
        "\001W", /*W*/
        "\001X", /*X*/
        "\001Y", /*Y*/
        "\001Z", /*Z*/
        "\001[", /*[*/
        "\001\\", /*\*/
        "\001]", /*]*/
        "\001^", /*^*/
        "\001_", /*_*/
        "\001`", /*`*/
        "\001a", /*a*/
        "\001b", /*b*/
        "\001c", /*c*/
        "\001d", /*d*/
        "\001e", /*e*/
        "\001f", /*f*/
        "\001g", /*g*/
        "\001h", /*h*/
        "\001i", /*i*/
        "\001j", /*j*/
        "\001k", /*k*/
        "\001l", /*l*/
        "\001m", /*m*/
        "\001n", /*n*/
        "\001o", /*o*/
        "\001p", /*p*/
        "\001q", /*q*/
        "\001r", /*r*/
        "\001s", /*s*/
        "\001t", /*t*/
        "\001u", /*u*/
        "\001v", /*v*/
        "\001w", /*w*/
        "\001x", /*x*/
        "\001y", /*y*/
        "\001z", /*z*/
        "\001{", /*{*/
        "\001|", /*|*/
        "\001}", /*}*/
        "\001~", /*~*/
        "\000\0", /*narrow sp*/
        "\001-", /*hyphen*/
        "\001\260", /*bullet - degree symbol*/
        "\001\261", /*square - plus-minus*/
        "\001-", /*3/4 em*/
        "\001_", /*rule*/
        "\0031/4", /*1/4*/
        "\0031/2", /*1/2*/
        "\0033/4", /*3/4*/
        "\001-", /*minus*/
        "\002fi", /*fi*/
        "\002fl", /*fl*/
        "\002ff", /*ff*/
        "\003ffi", /*ffi*/
        "\003ffl", /*ffl*/
        "\001\260", /*degree*/
        "\001\262", /*dagger*/
        "\001\247", /*section*/
        "\001'", /*foot mark*/
        "\001\264", /*acute accent*/
        "\001\140", /*grave accent*/
        "\001_", /*underrule*/
        "\001/", /*slash*/
        "\000\0", /*half narrow space*/
        "\001 ", /*unpaddable space*/
        "\001\341", /*alpha*/
        "\001\342", /*beta*/
        "\001\343", /*gamma*/
        "\001\344", /*delta*/
        "\001\345", /*epsilon*/
        "\001\346", /*zeta*/
        "\001\347", /*eta*/
        "\001\350", /*theta*/
        "\001\351", /*iota*/
        "\001\352", /*kappa*/
        "\001\353", /*lambda*/
        "\001\354", /*mu*/
        "\001\355", /*nu*/
        "\001\356", /*xi*/
        "\001\357", /*omicron*/
        "\001\360", /*pi*/
        "\001\361", /*rho*/
        "\001\362", /*sigma*/
        "\001\363", /*tau*/
        "\001\364", /*upsilon*/
        "\001\365", /*phi*/
        "\001\366", /*chi*/
        "\001\367", /*psi*/
        "\001\370", /*omega*/
        "\001\301", /*Gamma*/
        "\001\304", /*Delta*/
        "\001\310", /*Theta*/
        "\001\313", /*Lambda*/
        "\001\316", /*Xi*/
        "\001\320", /*Pi*/
        "\001\323", /*Sigma*/
        "\000\0", /**/
        "\001\325", /*Upsilon*/
        "\001\326", /*Phi*/
        "\001\327", /*Psi*/
        "\001\330", /*Omega*/
        "\001\221", /*square root*/
        "\001\362", /*terminal sigma*/
        "\000\0", /*root en*/
        "\002>=", /*>=*/
        "\002<=", /*<=*/
        "\002==", /*identically equal*/
        "\001-", /*equation minus*/
        "\002~=", /*approx =*/
        "\002~", /*approximates*/
        "\002!=", /*not equal*/
        "\002->", /*right arrow*/
        "\002<-", /*left arrow*/
        "\001^", /*up arrow*/
        "\001v", /*down arrow*/
        "\001=", /*equation equal*/
        "\001x", /*multiply*/
        "\001/", /*divide*/
        "\002+-", /*plus-minus*/
        "\001U", /*cup (union)*/
        "\000\0", /*cap (intersection)*/
        "\000\0", /*subset of*/
        "\000\0", /*superset of*/
        "\000\0", /*improper subset*/
        "\000\0", /*improper superset*/
        "\002oo", /*infinity*/
        "\001d", /*partial derivative*/
        "\001\\/", /*gradient*/
        "\001~", /*not*/
        "\001S", /*integral sign*/
        "\000\0", /*proportional to*/
        "\000\0", /*empty set*/
        "\000\0", /*member of*/
        "\001+", /*equation plus*/
        "\002(R)", /*registered*/
        "\002(C)", /*copyright*/
        "\001|", /*box rule*/
        "\002ct", /*cent sign*/
        "\000\0", /*dbl dagger*/
        "\000\0", /*right hand*/
        "\000\0", /*left hand*/
        "\001*", /*math **/
        "\000\0", /*bell system sign*/
        "\001|", /*or*/
        "\001O", /*circle*/
        "\001\154", /*left top (DEC line drawing)*/
        "\001\155", /*left bottom*/
        "\001\153", /*right top*/
        "\001\152", /*right bot*/
        "\001\156", /*left center*/
        "\001\165", /*right center*/
        "\001\170", /*bold vertical*/
        "\001\155", /*left floor*/
        "\001\152", /*right floor*/
        "\001\154", /*left ceiling*/
        "\001\153" /*right ceiling*/
    },
    0 /* zzz */
};
