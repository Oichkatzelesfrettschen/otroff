/*
 * tabvt100.c - VT100 nroff driving tables (C90 portable)
 *
 * This file defines the terminal table for the VT100 terminal, including width and code tables.
 * Refactored for C90 portability, clarity, and maintainability.
 *
 * Usage:
 *   - The struct termtab describes the terminal's capabilities and control sequences.
 *   - The table instance 't' is initialized with VT100-specific values.
 *   - The character code table (codetab) uses standard ASCII mappings.
 *   - This file is intended for inclusion in nroff/roff terminal drivers.
 */

#define INCH 240

#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <stdint.h>

/*
 * struct termtab - Terminal table structure for VT100
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
 *   codetab   - Character code table (ASCII standard)
 *   zzz       - Reserved/unused
 */
struct TermTab {
    std::uint32_t  bset{};
    std::uint32_t  breset{};
    int            Hor{};
    int            Vert{};
    int            Newline{};
    int            Char{};
    int            Em{};
    int            Halfline{};
    int            Adj{};
    std::string_view twinit{};
    std::string_view twrest{};
    std::string_view twnl{};
    std::string_view hlr{};
    std::string_view hlf{};
    std::string_view flr{};
    std::string_view bdon{};
    std::string_view bdoff{};
    std::string_view ploton{};
    std::string_view plotoff{};
    std::string_view up{};
    std::string_view down{};
    std::string_view right{};
    std::string_view left{};
    std::array<std::string_view, 256 - 32> codetab{};
    int zzz{};
};

/*
 * Table instance for VT100 terminal.
 * All fields are initialized in C90-compliant order.
 * VT100 uses standard ANSI escape sequences for cursor control and formatting.
 */
inline constexpr TermTab t = {
    0, /* bset */
    0, /* breset */
    INCH / 10, /* Hor - 10 chars per inch */
    INCH / 6, /* Vert - 6 lines per inch */
    INCH / 6, /* Newline */
    INCH / 10, /* Char */
    INCH / 10, /* Em */
    INCH / 12, /* Halfline */
    INCH / 10, /* Adj */
    "\033[?3l\033[?4l\033[?5l\033[?7h\033[?8h", /* twinit - VT100 init */
    "\033c", /* twrest - reset terminal */
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
    /* codetab - Standard ASCII character table */
    {
        "\001 ", /*space*/
        "\001!", /*!*/
        "\001\"", /*"*/
        "\001#", /*#*/
        "\001$", /*$*/
        "\001%", /*%*/
        "\001&", /*&*/
        "\001'", /*' close*/
        "\001(", /*(*/
        "\001)", /*)*/
        "\001*", /***/
        "\001+", /*+*/
        "\001,", /*,*/
        "\001-", /*- hyphen*/
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
        "\001_", /*_ dash*/
        "\001`", /*` open*/
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
        "\001*", /*bullet*/
        "\001+", /*square*/
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
        "\000\0", /*degree*/
        "\000\0", /*dagger*/
        "\000\0", /*section*/
        "\001'", /*foot mark*/
        "\001'", /*acute accent*/
        "\001`", /*grave accent*/
        "\001_", /*underrule*/
        "\001/", /*slash (longer)*/
        "\000\0", /*half narrow space*/
        "\001 ", /*unpaddable space*/
        "\001a", /*alpha*/
        "\001b", /*beta*/
        "\001g", /*gamma*/
        "\001d", /*delta*/
        "\001e", /*epsilon*/
        "\001z", /*zeta*/
        "\001h", /*eta*/
        "\001t", /*theta*/
        "\001i", /*iota*/
        "\001k", /*kappa*/
        "\001l", /*lambda*/
        "\001m", /*mu*/
        "\001n", /*nu*/
        "\001x", /*xi*/
        "\001o", /*omicron*/
        "\001p", /*pi*/
        "\001r", /*rho*/
        "\001s", /*sigma*/
        "\001t", /*tau*/
        "\001u", /*upsilon*/
        "\001f", /*phi*/
        "\001c", /*chi*/
        "\001p", /*psi*/
        "\001w", /*omega*/
        "\001G", /*Gamma*/
        "\001D", /*Delta*/
        "\001T", /*Theta*/
        "\001L", /*Lambda*/
        "\001X", /*Xi*/
        "\001P", /*Pi*/
        "\001S", /*Sigma*/
        "\000\0", /**/
        "\001U", /*Upsilon*/
        "\001F", /*Phi*/
        "\001P", /*Psi*/
        "\001W", /*Omega*/
        "\000\0", /*square root*/
        "\000\0", /*terminal sigma*/
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
        "\001|", /*left top*/
        "\001|", /*left bottom*/
        "\001|", /*right top*/
        "\001|", /*right bot*/
        "\001|", /*left center*/
        "\001|", /*right center*/
        "\001|", /*bold vertical*/
        "\001|", /*left floor*/
        "\001|", /*right floor*/
        "\001|", /*left ceiling*/
        "\001|" /*right ceiling*/
    },
    0 /* zzz */
};
