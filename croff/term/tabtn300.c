/*
 * tabtn300.c - Terminet300 nroff driving tables (C90 portable)
 *
 * This file defines the terminal table for the Terminet300 terminal, including width and code tables.
 * Refactored for C90 portability, clarity, and maintainability.
 *
 * Usage:
 *   - The struct termtab describes the terminal's capabilities and control sequences.
 *   - The table instance 't' is initialized with Terminet300-specific values.
 *   - The character code table (codetab) is embedded directly in the structure.
 *   - This file is intended for inclusion in nroff/roff terminal drivers.
 */

#define INCH 240

#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <stdint.h>

/*
 * struct termtab - Terminal table structure for Terminet300
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
 *   codetab   - Character code table (embedded)
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
 * Table instance for Terminet300 terminal.
 * All fields are initialized in C90-compliant order.
 * The Terminet300 is a simple terminal with minimal control sequences.
 */
inline constexpr TermTab t = {
    0, /* bset */
    0, /* breset */
    INCH / 10, /* Hor */
    INCH / 6, /* Vert */
    INCH / 6, /* Newline */
    INCH / 10, /* Char */
    INCH / 10, /* Em */
    INCH / 12, /* Halfline */
    INCH / 10, /* Adj */
    "", /* twinit */
    "", /* twrest */
    "\n", /* twnl */
    "", /* hlr */
    "", /* hlf */
    "", /* flr */
    "", /* bdon */
    "", /* bdoff */
    "", /* ploton */
    "", /* plotoff */
    "", /* up */
    "", /* down */
    "", /* right */
    "", /* left */
    /* codetab - Character code table for Terminet300 */
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
        "\2010", /*0*/
        "\2011", /*1*/
        "\2012", /*2*/
        "\2013", /*3*/
        "\2014", /*4*/
        "\2015", /*5*/
        "\2016", /*6*/
        "\2017", /*7*/
        "\2018", /*8*/
        "\2019", /*9*/
        "\001:", /*:*/
        "\001;", /*;*/
        "\001<", /*<*/
        "\001=", /*=*/
        "\001>", /*>*/
        "\001?", /*?*/
        "\001@", /*@*/
        "\201A", /*A*/
        "\201B", /*B*/
        "\201C", /*C*/
        "\201D", /*D*/
        "\201E", /*E*/
        "\201F", /*F*/
        "\201G", /*G*/
        "\201H", /*H*/
        "\201I", /*I*/
        "\201J", /*J*/
        "\201K", /*K*/
        "\201L", /*L*/
        "\201M", /*M*/
        "\201N", /*N*/
        "\201O", /*O*/
        "\201P", /*P*/
        "\201Q", /*Q*/
        "\201R", /*R*/
        "\201S", /*S*/
        "\201T", /*T*/
        "\201U", /*U*/
        "\201V", /*V*/
        "\201W", /*W*/
        "\201X", /*X*/
        "\201Y", /*Y*/
        "\201Z", /*Z*/
        "\001[", /*[*/
        "\001\\", /*\*/
        "\001]", /*]*/
        "\001^", /*^*/
        "\001_", /*_ dash*/
        "\001`", /*` open*/
        "\201a", /*a*/
        "\201b", /*b*/
        "\201c", /*c*/
        "\201d", /*d*/
        "\201e", /*e*/
        "\201f", /*f*/
        "\201g", /*g*/
        "\201h", /*h*/
        "\201i", /*i*/
        "\201j", /*j*/
        "\201k", /*k*/
        "\201l", /*l*/
        "\201m", /*m*/
        "\201n", /*n*/
        "\201o", /*o*/
        "\201p", /*p*/
        "\201q", /*q*/
        "\201r", /*r*/
        "\201s", /*s*/
        "\201t", /*t*/
        "\201u", /*u*/
        "\201v", /*v*/
        "\201w", /*w*/
        "\201x", /*x*/
        "\201y", /*y*/
        "\201z", /*z*/
        "\001{", /*{*/
        "\001|", /*|*/
        "\001}", /*}*/
        "\001~", /*~*/
        "\000\0", /*nar sp*/
        "\001-", /*hyphen*/
        "\001o\b+", /*bullet*/
        "\002\[]", /*square*/
        "\001-", /*3/4 em*/
        "\001_", /*rule*/
        "\000\0", /*1/4*/
        "\000\0", /*1/2*/
        "\000\0", /*3/4*/
        "\001-", /*minus*/
        "\202fi", /*fi*/
        "\202fl", /*fl*/
        "\202ff", /*ff*/
        "\203ffi", /*ffi*/
        "\203ffl", /*ffl*/
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
        "\000", /*alpha*/
        "\000", /*beta*/
        "\000", /*gamma*/
        "\000", /*delta*/
        "\000", /*epsilon*/
        "\000", /*zeta*/
        "\000", /*eta*/
        "\000", /*theta*/
        "\201i", /*iota*/
        "\201k", /*kappa*/
        "\000", /*lambda*/
        "\000", /*mu*/
        "\000", /*nu*/
        "\000", /*xi*/
        "\201o", /*omicron*/
        "\000", /*pi*/
        "\000", /*rho*/
        "\000", /*sigma*/
        "\000", /*tau*/
        "\201v", /*upsilon*/
        "\000", /*phi*/
        "\201x", /*chi*/
        "\000", /*psi*/
        "\000", /*omega*/
        "\000", /*Gamma*/
        "\000", /*Delta*/
        "\000", /*Theta*/
        "\000", /*Lambda*/
        "\000\0", /*Xi*/
        "\000", /*Pi*/
        "\000", /*Sigma*/
        "\000\0", /**/
        "\201Y", /*Upsilon*/
        "\000", /*Phi*/
        "\000", /*Psi*/
        "\000", /*Omega*/
        "\000\0", /*square root*/
        "\000\0", /*terminal sigma*/
        "\000\0", /*root en*/
        "\001>\b_", /*>=*/
        "\001<\b_", /*<=*/
        "\001=\b_", /*identically equal*/
        "\001-", /*equation minus*/
        "\001=\b~", /*approx =*/
        "\000\0", /*approximates*/
        "\001=\b/", /*not equal*/
        "\002->", /*right arrow*/
        "\002<-", /*left arrow*/
        "\001|\b^", /*up arrow*/
        "\000\0", /*down arrow*/
        "\001=", /*equation equal*/
        "\001x", /*multiply*/
        "\001/", /*divide*/
        "\001+\b_", /*plus-minus*/
        "\001U", /*cup (union)*/
        "\000\0", /*cap (intersection)*/
        "\000\0", /*subset of*/
        "\000\0", /*superset of*/
        "\000\0", /*improper subset*/
        "\000\0", /*improper superset*/
        "\002oo", /*infinity*/
        "\000", /*partial derivative*/
        "\000", /*gradient*/
        "\000", /*not*/
        "\000", /*integral sign*/
        "\000\0", /*proportional to*/
        "\000\0", /*empty set*/
        "\000\0", /*member of*/
        "\001+", /*equation plus*/
        "\001r\bO", /*registered*/
        "\001c\bO", /*copyright*/
        "\001|", /*box rule*/
        "\001c\b/", /*cent sign*/
        "\000\0", /*dbl dagger*/
        "\000\0", /*right hand*/
        "\001*", /*left hand*/
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
