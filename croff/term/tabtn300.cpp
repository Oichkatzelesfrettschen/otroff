#include "../../cxx17_scaffold.hpp"
/*
 * tabtn300.cpp - Terminet300 nroff driving tables (C++23)
 *
 * This file defines the terminal table for the Terminet300 terminal, including width and code tables.
 * Refactored for C++23 with modern features, clarity, and maintainability.
 *
 * Usage:
 *   - The struct TermTab describes the terminal's capabilities and control sequences.
 *   - The table instance 't' is initialized with Terminet300-specific values.
 *   - The character code table (codetab) is embedded directly in the structure.
 *   - This file is intended for inclusion in nroff/roff terminal drivers.
 */

#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>

using namespace std::string_view_literals;

inline constexpr std::uint32_t INCH = 240U;

/*
 * struct TermTab - Terminal table structure for Terminet300
 *
 * Members describe terminal capabilities, control sequences, and character mappings.
 * All strings use string_view for zero-overhead string literals.
 * Character code table maps ASCII 32-255 to terminal-specific sequences.
 */
struct TermTab {
    std::uint32_t bset; // Bits to set on initialization
    std::uint32_t breset; // Bits to reset on initialization
    int Hor; // Horizontal resolution (units per space)
    int Vert; // Vertical resolution (units per line)
    int Newline; // Newline spacing (units)
    int Char; // Character width (units)
    int Em; // Em width (units)
    int Halfline; // Halfline spacing (units)
    int Adj; // Adjustment spacing (units)
    std::string_view twinit; // Terminal initialization string
    std::string_view twrest; // Terminal reset string
    std::string_view twnl; // Newline string
    std::string_view hlr; // Halfline reverse string
    std::string_view hlf; // Halfline forward string
    std::string_view flr; // Fullline reverse string
    std::string_view bdon; // Bold on string
    std::string_view bdoff; // Bold off string
    std::string_view ploton; // Plot mode on string
    std::string_view plotoff; // Plot mode off string
    std::string_view up; // Cursor up string
    std::string_view down; // Cursor down string
    std::string_view right; // Cursor right string
    std::string_view left; // Cursor left string
    std::array<std::string_view, 256 - 32> codetab; // Character code table
    int zzz; // Reserved/unused
};

/*
 * Table instance for Terminet300 terminal.
 * C++23 designated initializers provide clear field mapping.
 * The Terminet300 is a simple terminal with minimal control sequences.
 */
constexpr TermTab t{
    .bset = 0U,
    .breset = 0U,
    .Hor = INCH / 10,
    .Vert = INCH / 6,
    .Newline = INCH / 6,
    .Char = INCH / 10,
    .Em = INCH / 10,
    .Halfline = INCH / 12,
    .Adj = INCH / 10,
    .twinit = ""sv,
    .twrest = ""sv,
    .twnl = "\n"sv,
    .hlr = ""sv,
    .hlf = ""sv,
    .flr = ""sv,
    .bdon = ""sv,
    .bdoff = ""sv,
    .ploton = ""sv,
    .plotoff = ""sv,
    .up = ""sv,
    .down = ""sv,
    .right = ""sv,
    .left = ""sv,
    .codetab = {
        "\001 "sv, /*space*/
        "\001!"sv, /*!*/
        "\001\""sv, /*"*/
        "\001#"sv, /*#*/
        "\001$"sv, /*$*/
        "\001%"sv, /*%*/
        "\001&"sv, /*&*/
        "\001'"sv, /*' close*/
        "\001("sv, /*(*/
        "\001)"sv, /*)*/
        "\001*"sv, /***/
        "\001+"sv, /*+*/
        "\001,"sv, /*,*/
        "\001-"sv, /*- hyphen*/
        "\001."sv, /*.*/
        "\001/"sv, /*/*/
        "\2010"sv, /*0*/
        "\2011"sv, /*1*/
        "\2012"sv, /*2*/
        "\2013"sv, /*3*/
        "\2014"sv, /*4*/
        "\2015"sv, /*5*/
        "\2016"sv, /*6*/
        "\2017"sv, /*7*/
        "\2018"sv, /*8*/
        "\2019"sv, /*9*/
        "\001:"sv, /*:*/
        "\001;"sv, /*;*/
        "\001<"sv, /*<*/
        "\001="sv, /*=*/
        "\001>"sv, /*>*/
        "\001?"sv, /*?*/
        "\001@"sv, /*@*/
        "\201A"sv, /*A*/
        "\201B"sv, /*B*/
        "\201C"sv, /*C*/
        "\201D"sv, /*D*/
        "\201E"sv, /*E*/
        "\201F"sv, /*F*/
        "\201G"sv, /*G*/
        "\201H"sv, /*H*/
        "\201I"sv, /*I*/
        "\201J"sv, /*J*/
        "\201K"sv, /*K*/
        "\201L"sv, /*L*/
        "\201M"sv, /*M*/
        "\201N"sv, /*N*/
        "\201O"sv, /*O*/
        "\201P"sv, /*P*/
        "\201Q"sv, /*Q*/
        "\201R"sv, /*R*/
        "\201S"sv, /*S*/
        "\201T"sv, /*T*/
        "\201U"sv, /*U*/
        "\201V"sv, /*V*/
        "\201W"sv, /*W*/
        "\201X"sv, /*X*/
        "\201Y"sv, /*Y*/
        "\201Z"sv, /*Z*/
        "\001["sv, /*[*/
        "\001\\"sv, /*\*/
        "\001]"sv, /*]*/
        "\001^"sv, /*^*/
        "\001_"sv, /*_ dash*/
        "\001`"sv, /*` open*/
        "\201a"sv, /*a*/
        "\201b"sv, /*b*/
        "\201c"sv, /*c*/
        "\201d"sv, /*d*/
        "\201e"sv, /*e*/
        "\201f"sv, /*f*/
        "\201g"sv, /*g*/
        "\201h"sv, /*h*/
        "\201i"sv, /*i*/
        "\201j"sv, /*j*/
        "\201k"sv, /*k*/
        "\201l"sv, /*l*/
        "\201m"sv, /*m*/
        "\201n"sv, /*n*/
        "\201o"sv, /*o*/
        "\201p"sv, /*p*/
        "\201q"sv, /*q*/
        "\201r"sv, /*r*/
        "\201s"sv, /*s*/
        "\201t"sv, /*t*/
        "\201u"sv, /*u*/
        "\201v"sv, /*v*/
        "\201w"sv, /*w*/
        "\201x"sv, /*x*/
        "\201y"sv, /*y*/
        "\201z"sv, /*z*/
        "\001{"sv, /*{*/
        "\001|"sv, /*|*/
        "\001}"sv, /*}*/
        "\001~"sv, /*~*/
        "\000\0"sv, /*narrow space*/
        "\001-"sv, /*hyphen*/
        "\001o\b+"sv, /*bullet*/
        "\002\[]"sv, /*square*/
        "\001-"sv, /*3/4 em*/
        "\001_"sv, /*rule*/
        "\000\0"sv, /*1/4*/
        "\000\0"sv, /*1/2*/
        "\000\0"sv, /*3/4*/
        "\001-"sv, /*minus*/
        "\202fi"sv, /*fi*/
        "\202fl"sv, /*fl*/
        "\202ff"sv, /*ff*/
        "\203ffi"sv, /*ffi*/
        "\203ffl"sv, /*ffl*/
        "\000\0"sv, /*degree*/
        "\000\0"sv, /*dagger*/
        "\000\0"sv, /*section*/
        "\001'"sv, /*foot mark*/
        "\001'"sv, /*acute accent*/
        "\001`"sv, /*grave accent*/
        "\001_"sv, /*underrule*/
        "\001/"sv, /*slash (longer)*/
        "\000\0"sv, /*half narrow space*/
        "\001 "sv, /*unpaddable space*/
        "\000"sv, /*alpha*/
        "\000"sv, /*beta*/
        "\000"sv, /*gamma*/
        "\000"sv, /*delta*/
        "\000"sv, /*epsilon*/
        "\000"sv, /*zeta*/
        "\000"sv, /*eta*/
        "\000"sv, /*theta*/
        "\201i"sv, /*iota*/
        "\201k"sv, /*kappa*/
        "\000"sv, /*lambda*/
        "\000"sv, /*mu*/
        "\000"sv, /*nu*/
        "\000"sv, /*xi*/
        "\201o"sv, /*omicron*/
        "\000"sv, /*pi*/
        "\000"sv, /*rho*/
        "\000"sv, /*sigma*/
        "\000"sv, /*tau*/
        "\201v"sv, /*upsilon*/
        "\000"sv, /*phi*/
        "\201x"sv, /*chi*/
        "\000"sv, /*psi*/
        "\000"sv, /*omega*/
        "\000"sv, /*Gamma*/
        "\000"sv, /*Delta*/
        "\000"sv, /*Theta*/
        "\000"sv, /*Lambda*/
        "\000\0"sv, /*Xi*/
        "\000"sv, /*Pi*/
        "\000"sv, /*Sigma*/
        "\000\0"sv, /**/
        "\201Y"sv, /*Upsilon*/
        "\000"sv, /*Phi*/
        "\000"sv, /*Psi*/
        "\000"sv, /*Omega*/
        "\000\0"sv, /*square root*/
        "\000\0"sv, /*terminal sigma*/
        "\000\0"sv, /*root en*/
        "\001>\b_"sv, /*>=*/
        "\001<\b_"sv, /*<=*/
        "\001=\b_"sv, /*identically equal*/
        "\001-"sv, /*equation minus*/
        "\001=\b~"sv, /*approx =*/
        "\000\0"sv, /*approximates*/
        "\001=\b/"sv, /*not equal*/
        "\002->"sv, /*right arrow*/
        "\002<-"sv, /*left arrow*/
        "\001|\b^"sv, /*up arrow*/
        "\000\0"sv, /*down arrow*/
        "\001="sv, /*equation equal*/
        "\001x"sv, /*multiply*/
        "\001/"sv, /*divide*/
        "\001+\b_"sv, /*plus-minus*/
        "\001U"sv, /*cup (union)*/
        "\000\0"sv, /*cap (intersection)*/
        "\000\0"sv, /*subset of*/
        "\000\0"sv, /*superset of*/
        "\000\0"sv, /*improper subset*/
        "\000\0"sv, /*improper superset*/
        "\002oo"sv, /*infinity*/
        "\000"sv, /*partial derivative*/
        "\000"sv, /*gradient*/
        "\000"sv, /*not*/
        "\000"sv, /*integral sign*/
        "\000\0"sv, /*proportional to*/
        "\000\0"sv, /*empty set*/
        "\000\0"sv, /*member of*/
        "\001+"sv, /*equation plus*/
        "\001r\bO"sv, /*registered*/
        "\001c\bO"sv, /*copyright*/
        "\001|"sv, /*box rule*/
        "\001c\b/"sv, /*cent sign*/
        "\000\0"sv, /*dbl dagger*/
        "\000\0"sv, /*right hand*/
        "\001*"sv, /*left hand*/
        "\001*"sv, /*math **/
        "\000\0"sv, /*bell system sign*/
        "\001|"sv, /*or*/
        "\001O"sv, /*circle*/
        "\001|"sv, /*left top*/
        "\001|"sv, /*left bottom*/
        "\001|"sv, /*right top*/
        "\001|"sv, /*right bot*/
        "\001|"sv, /*left center*/
        "\001|"sv, /*right center*/
        "\001|"sv, /*bold vertical*/
        "\001|"sv, /*left floor*/
        "\001|"sv, /*right floor*/
        "\001|"sv, /*left ceiling*/
        "\001|"sv /*right ceiling*/
    },
    .zzz = 0};
