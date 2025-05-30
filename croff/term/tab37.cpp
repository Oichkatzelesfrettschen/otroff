#include "cxx23_scaffold.hpp"
/*
 * tab37.cpp - TTY M37 nroff driving tables (C++23)
 *
 * This file defines the terminal table for the TTY M37, including width and code tables.
 * Refactored for C++23 compatibility and clarity.
 */

#include <cstdint>
#include <array>
#include <string_view>

namespace {
constexpr std::uint16_t INCH = 240;
constexpr std::size_t CODETAB_SIZE = 224; // 256 - 32 = 224
} // namespace

/* Terminal table structure for TTY M37 */
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
    std::array<std::string_view, CODETAB_SIZE> codetab;
};

/*
 * Table instance for TTY M37
 * All fields are initialized in C++23-compliant order.
 */
const termtab t{
    .bset = 0,
    .breset = 0,
    .Hor = INCH / 10,
    .Vert = INCH / 12,
    .Newline = INCH / 6,
    .Char = INCH / 10,
    .Em = INCH / 10,
    .Halfline = INCH / 12,
    .Adj = INCH / 10,
    .twinit = "",
    .twrest = "",
    .twnl = "\n",
    .hlr = "\0338",
    .hlf = "\0339",
    .flr = "\0337",
    .bdon = "",
    .bdoff = "",
    .ploton = "",
    .plotoff = "",
    .up = "",
    .down = "",
    .right = "",
    .left = "",
    .codetab = {{
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
        "\001\\", /*\\*/
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
        "\000", /*narrow sp*/
        "\001-", /*hyphen*/
        "\001o\b+", /*bullet*/
        "\002[]", /*square*/
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
        "\000", /*degree*/
        "\000", /*dagger*/
        "\000", /*section*/
        "\001'", /*foot mark*/
        "\001'", /*acute accent*/
        "\001`", /*grave accent*/
        "\001_", /*underrule*/
        "\001/", /*slash (longer)*/
        "\000", /*half narrow space*/
        "\001 ", /*non-paddable space*/
        "\001\016A\017", /*alpha*/
        "\001\016B\017", /*beta*/
        "\001\016\\\017", /*gamma*/
        "\001\016D\017", /*delta*/
        "\001\016S\017", /*epsilon*/
        "\001\016Q\017", /*zeta*/
        "\001\016N\017", /*eta*/
        "\001\016O\017", /*theta*/
        "\001i", /*iota*/
        "\001k", /*kappa*/
        "\001\016L\017", /*lambda*/
        "\001\016M\017", /*mu*/
        "\001\016@\017", /*nu*/
        "\001\016X\017", /*xi*/
        "\001o", /*omicron*/
        "\001\016J\017", /*pi*/
        "\001\016K\017", /*rho*/
        "\001\016Y\017", /*sigma*/
        "\001\016I\017", /*tau*/
        "\001v", /*upsilon*/
        "\001\016U\017", /*phi*/
        "\001x", /*chi*/
        "\001\016V\017", /*psi*/
        "\001\016C\017", /*omega*/
        "\001\016G\017", /*Gamma*/
        "\001\016W\017", /*Delta*/
        "\001\016T\017", /*Theta*/
        "\001\016E\017", /*Lambda*/
        "\000", /*Xi*/
        "\001\016P\017", /*Pi*/
        "\001\016R\017", /*Sigma*/
        "\000", /**/
        "\001Y", /*Upsilon*/
        "\001\016F\017", /*Phi*/
        "\001\016H\017", /*Psi*/
        "\001\016Z\017", /*Omega*/
        "\000", /*square root*/
        "\000", /*terminal sigma*/
        "\000", /*root en*/
        "\001>\b_", /*>=*/
        "\001<\b_", /*<=*/
        "\001=\b_", /*identically equal*/
        "\001-", /*equation minus*/
        "\001=\b~", /*approx =*/
        "\001\0339~\0338", /*approximates*/
        "\001=\b/", /*not equal*/
        "\002->", /*right arrow*/
        "\002<-", /*left arrow*/
        "\001|\b^", /*up arrow*/
        "\000", /*down arrow*/
        "\001=", /*equation equal*/
        "\001x", /*multiply*/
        "\001/", /*divide*/
        "\001+\b_", /*plus-minus*/
        "\001U", /*cup (union)*/
        "\000", /*cap (intersection)*/
        "\000", /*subset of*/
        "\000", /*superset of*/
        "\000", /*improper subset*/
        "\000", /*improper superset*/
        "\002oo", /*infinity*/
        "\001\016]\017", /*partial derivative*/
        "\001\016[\017", /*gradient*/
        "\001\016_\017", /*not*/
        "\001\016^\017", /*integral sign*/
        "\000", /*proportional to*/
        "\000", /*empty set*/
        "\000", /*member of*/
        "\001+", /*equation plus*/
        "\001\0338r\0339", /*registered*/
        "\001\0338c\0339", /*copyright*/
        "\001|", /*box rule */
        "\001c\b/", /*cent sign*/
        "\000", /*dbl dagger*/
        "\000", /*right hand*/
        "\001*", /*left hand*/
        "\001*", /*math * */
        "\000", /*bell system sign*/
        "\001|", /*or (was star)*/
        "\001O", /*circle*/
        "\001|", /*left top (of big curly)*/
        "\001|", /*left bottom*/
        "\001|", /*right top*/
        "\001|", /*right bot*/
        "\001|", /*left center of big curly bracket*/
        "\001|", /*right center of big curly bracket*/
        "\001|", /*bold vertical*/
        "\001|", /*left floor (left bot of big sq bract)*/
        "\001|", /*right floor (rb of ")*/
        "\001|", /*left ceiling (lt of ")*/
        "\001|" /*right ceiling (rt of ")*/
    }}};
