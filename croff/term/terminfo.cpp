#include "cxx23_scaffold.hpp"
#include <array>
#include <cstdint>
#include <string_view>

using namespace std::string_view_literals;

/**
 * @file terminfo.cpp
 * @brief Terminal capability definitions and configuration tables for text formatting systems
 * 
 * This file provides a comprehensive set of terminal capability tables for various terminal
 * types commonly used in text processing and document formatting systems. It implements a
 * modern C++17 approach to terminal abstraction with compile-time constant definitions.
 * 
 * The module defines terminal capabilities including:
 * - Resolution and spacing parameters for precise text positioning
 * - Control sequences for cursor movement and text formatting
 * - Character encoding tables for special symbols and typography
 * - Terminal initialization and reset sequences
 * 
 * Supported terminal types:
 * - DEC VT220: Legacy Digital terminal with basic ANSI capabilities
 * - DEC VT320: Enhanced Digital terminal with extended ANSI features
 * - xterm: Modern UTF-8 capable terminal emulator with Unicode support
 * - ANSI: Generic ANSI X3.64/ECMA-48/ISO 6429 compliant terminal
 * 
 * The design emphasizes type safety, compile-time evaluation, and backward compatibility
 * with legacy terminal systems while providing modern C++ interfaces.
 * 
 * @namespace terminal Contains all terminal-related types and configurations
 * @since C++17
 * @author Terminal capabilities implementation
 */
 * @brief Modern Terminal Driver Tables - Pure C++17 Implementation
 * @author Modern C++17 Engineering Team
 * @version 3.0
 * @date 2024
 *
 * @details Pure C++17 implementation of terminal capability tables.
 *          Completely redesigned with modern C++17 principles:
 *          - Full type safety with std::array and string_view
 *          - Constexpr evaluation for compile-time optimization
 *          - ANSI X3.64/ECMA-48/ISO 6429 escape sequence support
 *          - Exception-safe design patterns
 *          - Zero-cost abstractions
 */

namespace terminal {

     inline constexpr std::uint16_t INCH = 240;

     /**
 * @brief Terminal capability table structure
 * 
 * Modern C++17 replacement for legacy C struct with full type safety
 * and compile-time evaluation capabilities.
 */
     struct TerminalTable {
         std::uint16_t bset;
         std::uint16_t breset;
         std::uint16_t horizontal_resolution;
         std::uint16_t vertical_resolution;
         std::uint16_t newline_spacing;
         std::uint16_t character_width;
         std::uint16_t em_width;
         std::uint16_t halfline_spacing;
         std::uint16_t adjustment_spacing;

         std::string_view terminal_init;
         std::string_view terminal_reset;
         std::string_view newline;
         std::string_view halfline_reverse;
         std::string_view halfline_forward;
         std::string_view fullline_reverse;
         std::string_view bold_on;
         std::string_view bold_off;
         std::string_view plot_on;
         std::string_view plot_off;
         std::string_view cursor_up;
         std::string_view cursor_down;
         std::string_view cursor_right;
         std::string_view cursor_left;

         std::array<std::string_view, 224> character_table; // 256 - 32 = 224
     };

     /**
 * @brief DEC VT220 Terminal Configuration
 * 
 * Digital VT220 with ANSI X3.64 escape sequences.
 * Optimized for legacy compatibility and reliability.
 */
     inline constexpr TerminalTable vt220_table{
         .bset = 0,
         .breset = 0,
         .horizontal_resolution = INCH / 10,
         .vertical_resolution = INCH / 6,
         .newline_spacing = INCH / 6,
         .character_width = INCH / 10,
         .em_width = INCH / 10,
         .halfline_spacing = INCH / 12,
         .adjustment_spacing = INCH / 10,

         .terminal_init = "\033[?7h\033[?1h\033[?3l\033[?4l\033[?5l\033[?6l\033[?8h\033[?25h"sv,
         .terminal_reset = "\033c\033[!p"sv,
         .newline = "\r\n"sv,
         .halfline_reverse = "\033M"sv,
         .halfline_forward = "\033D"sv,
         .fullline_reverse = "\033M"sv,
         .bold_on = "\033[1m"sv,
         .bold_off = "\033[0m"sv,
         .plot_on = ""sv,
         .plot_off = ""sv,
         .cursor_up = "\033[A"sv,
         .cursor_down = "\033[B"sv,
         .cursor_right = "\033[C"sv,
         .cursor_left = "\033[D"sv,

         .character_table = {{"\001 "sv, "\001!"sv, "\001\""sv, "\001#"sv, "\001$"sv, "\001%"sv, "\001&"sv, "\001'"sv,
                              "\001("sv, "\001)"sv, "\001*"sv, "\001+"sv, "\001,"sv, "\001-"sv, "\001."sv, "\001/"sv,
                              "\0010"sv, "\0011"sv, "\0012"sv, "\0013"sv, "\0014"sv, "\0015"sv, "\0016"sv, "\0017"sv,
                              "\0018"sv, "\0019"sv, "\001:"sv, "\001;"sv, "\001<"sv, "\001="sv, "\001>"sv, "\001?"sv,
                              "\001@"sv, "\001A"sv, "\001B"sv, "\001C"sv, "\001D"sv, "\001E"sv, "\001F"sv, "\001G"sv,
                              "\001H"sv, "\001I"sv, "\001J"sv, "\001K"sv, "\001L"sv, "\001M"sv, "\001N"sv, "\001O"sv,
                              "\001P"sv, "\001Q"sv, "\001R"sv, "\001S"sv, "\001T"sv, "\001U"sv, "\001V"sv, "\001W"sv,
                              "\001X"sv, "\001Y"sv, "\001Z"sv, "\001["sv, "\001\\"sv, "\001]"sv, "\001^"sv, "\001_"sv,
                              "\001`"sv, "\001a"sv, "\001b"sv, "\001c"sv, "\001d"sv, "\001e"sv, "\001f"sv, "\001g"sv,
                              "\001h"sv, "\001i"sv, "\001j"sv, "\001k"sv, "\001l"sv, "\001m"sv, "\001n"sv, "\001o"sv,
                              "\001p"sv, "\001q"sv, "\001r"sv, "\001s"sv, "\001t"sv, "\001u"sv, "\001v"sv, "\001w"sv,
                              "\001x"sv, "\001y"sv, "\001z"sv, "\001{"sv, "\001|"sv, "\001}"sv, "\001~"sv, "\000\0"sv,
                              "\001-"sv, "\001*"sv, "\001+"sv, "\001-"sv, "\001_"sv, "\0031/4"sv, "\0031/2"sv, "\0033/4"sv,
                              "\001-"sv, "\002fi"sv, "\002fl"sv, "\002ff"sv, "\003ffi"sv, "\003ffl"sv, "\001\260"sv, "\001\261"sv,
                              "\001\247"sv, "\001'"sv, "\001\264"sv, "\001\140"sv, "\001_"sv, "\001/"sv, "\000\0"sv, "\001 "sv,
                              "\001\341"sv, "\001\342"sv, "\001\343"sv, "\001\344"sv, "\001\345"sv, "\001\346"sv, "\001\347"sv, "\001\350"sv,
                              "\001\351"sv, "\001\352"sv, "\001\353"sv, "\001\354"sv, "\001\355"sv, "\001\356"sv, "\001\357"sv, "\001\360"sv,
                              "\001\361"sv, "\001\362"sv, "\001\363"sv, "\001\364"sv, "\001\365"sv, "\001\366"sv, "\001\367"sv, "\001\370"sv,
                              "\001\301"sv, "\001\304"sv, "\001\310"sv, "\001\313"sv, "\001\316"sv, "\001\320"sv, "\001\323"sv, "\000\0"sv,
                              "\001\325"sv, "\001\326"sv, "\001\327"sv, "\001\330"sv, "\001\221"sv, "\001\362"sv, "\000\0"sv, "\002>="sv,
                              "\002<="sv, "\002=="sv, "\001-"sv, "\002~="sv, "\002~"sv, "\002!="sv, "\002->"sv, "\002<-"sv,
                              "\001^"sv, "\001v"sv, "\001="sv, "\001x"sv, "\001/"sv, "\002+-"sv, "\001U"sv, "\000\0"sv,
                              "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\002oo"sv, "\001d"sv, "\001\\/"sv, "\001~"sv,
                              "\001S"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\001+"sv, "\002(R)"sv, "\002(C)"sv, "\001|"sv,
                              "\002ct"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\001*"sv, "\000\0"sv, "\001|"sv, "\001O"sv,
                              "\001|"sv, "\001|"sv, "\001|"sv, "\001|"sv, "\001|"sv, "\001|"sv, "\001|"sv, "\001|"sv,
                              "\001|"sv, "\001|"sv, "\001|"sv, "\001|"sv}}};

     /**
 * @brief DEC VT320 Terminal Configuration
 * 
 * Digital VT320 with extended ANSI features and enhanced capabilities.
 */
     inline constexpr TerminalTable vt320_table{
         .bset = 0,
         .breset = 0,
         .horizontal_resolution = INCH / 10,
         .vertical_resolution = INCH / 6,
         .newline_spacing = INCH / 6,
         .character_width = INCH / 10,
         .em_width = INCH / 10,
         .halfline_spacing = INCH / 12,
         .adjustment_spacing = INCH / 10,

         .terminal_init = "\033[?7h\033[?1h\033[?3l\033[?4l\033[?5l\033[?6l\033[?8h\033[?25h\033[62;1;2;6;8;9;15;44c"sv,
         .terminal_reset = "\033c\033[!p"sv,
         .newline = "\r\n"sv,
         .halfline_reverse = "\033M"sv,
         .halfline_forward = "\033D"sv,
         .fullline_reverse = "\033M"sv,
         .bold_on = "\033[1m"sv,
         .bold_off = "\033[0m"sv,
         .plot_on = ""sv,
         .plot_off = ""sv,
         .cursor_up = "\033[A"sv,
         .cursor_down = "\033[B"sv,
         .cursor_right = "\033[C"sv,
         .cursor_left = "\033[D"sv,

         .character_table = vt220_table.character_table};

     /**
 * @brief Modern xterm Terminal Configuration
 * 
 * UTF-8 capable terminal emulator with Unicode support.
 */
     inline constexpr TerminalTable xterm_table{
         .bset = 0,
         .breset = 0,
         .horizontal_resolution = INCH / 10,
         .vertical_resolution = INCH / 6,
         .newline_spacing = INCH / 6,
         .character_width = INCH / 10,
         .em_width = INCH / 10,
         .halfline_spacing = INCH / 12,
         .adjustment_spacing = INCH / 10,

         .terminal_init = "\033[!p\033[?3;4l\033[4l\033>"sv,
         .terminal_reset = "\033c"sv,
         .newline = "\r\n"sv,
         .halfline_reverse = "\033M"sv,
         .halfline_forward = "\033D"sv,
         .fullline_reverse = "\033M"sv,
         .bold_on = "\033[1m"sv,
         .bold_off = "\033[m"sv,
         .plot_on = ""sv,
         .plot_off = ""sv,
         .cursor_up = "\033[A"sv,
         .cursor_down = "\033[B"sv,
         .cursor_right = "\033[C"sv,
         .cursor_left = "\033[D"sv,

         .character_table = {{"\001 "sv, "\001!"sv, "\001\""sv, "\001#"sv, "\001$"sv, "\001%"sv, "\001&"sv, "\001'"sv,
                              "\001("sv, "\001)"sv, "\001*"sv, "\001+"sv, "\001,"sv, "\001-"sv, "\001."sv, "\001/"sv,
                              "\0010"sv, "\0011"sv, "\0012"sv, "\0013"sv, "\0014"sv, "\0015"sv, "\0016"sv, "\0017"sv,
                              "\0018"sv, "\0019"sv, "\001:"sv, "\001;"sv, "\001<"sv, "\001="sv, "\001>"sv, "\001?"sv,
                              "\001@"sv, "\001A"sv, "\001B"sv, "\001C"sv, "\001D"sv, "\001E"sv, "\001F"sv, "\001G"sv,
                              "\001H"sv, "\001I"sv, "\001J"sv, "\001K"sv, "\001L"sv, "\001M"sv, "\001N"sv, "\001O"sv,
                              "\001P"sv, "\001Q"sv, "\001R"sv, "\001S"sv, "\001T"sv, "\001U"sv, "\001V"sv, "\001W"sv,
                              "\001X"sv, "\001Y"sv, "\001Z"sv, "\001["sv, "\001\\"sv, "\001]"sv, "\001^"sv, "\001_"sv,
                              "\001`"sv, "\001a"sv, "\001b"sv, "\001c"sv, "\001d"sv, "\001e"sv, "\001f"sv, "\001g"sv,
                              "\001h"sv, "\001i"sv, "\001j"sv, "\001k"sv, "\001l"sv, "\001m"sv, "\001n"sv, "\001o"sv,
                              "\001p"sv, "\001q"sv, "\001r"sv, "\001s"sv, "\001t"sv, "\001u"sv, "\001v"sv, "\001w"sv,
                              "\001x"sv, "\001y"sv, "\001z"sv, "\001{"sv, "\001|"sv, "\001}"sv, "\001~"sv, "\000\0"sv,
                              "\001-"sv, "\001\342\200\242"sv, "\001\342\226\240"sv, "\001-"sv, "\001_"sv, "\302\274"sv, "\302\275"sv, "\302\276"sv,
                              "\001-"sv, "\357\254\201"sv, "\357\254\202"sv, "\357\254\200"sv, "\357\254\203"sv, "\357\254\204"sv, "\302\260"sv, "\342\200\240"sv,
                              "\302\247"sv, "\001'"sv, "\302\264"sv, "\001`"sv, "\001_"sv, "\001/"sv, "\000\0"sv, "\001 "sv,
                              "\316\261"sv, "\316\262"sv, "\316\263"sv, "\316\264"sv, "\316\265"sv, "\316\266"sv, "\316\267"sv, "\316\270"sv,
                              "\316\271"sv, "\316\272"sv, "\316\273"sv, "\316\274"sv, "\316\275"sv, "\316\276"sv, "\316\277"sv, "\317\200"sv,
                              "\317\201"sv, "\317\203"sv, "\317\204"sv, "\317\205"sv, "\317\206"sv, "\317\207"sv, "\317\210"sv, "\317\211"sv,
                              "\316\223"sv, "\316\224"sv, "\316\230"sv, "\316\233"sv, "\316\236"sv, "\316\240"sv, "\316\243"sv, "\000\0"sv,
                              "\316\245"sv, "\316\246"sv, "\316\250"sv, "\316\251"sv, "\342\210\232"sv, "\317\202"sv, "\000\0"sv, "\342\211\245"sv,
                              "\342\211\244"sv, "\342\211\241"sv, "\001-"sv, "\342\211\210"sv, "\342\211\210"sv, "\342\211\240"sv, "\342\206\222"sv, "\342\206\220"sv,
                              "\342\206\221"sv, "\342\206\223"sv, "\001="sv, "\303\227"sv, "\303\267"sv, "\302\261"sv, "\342\210\252"sv, "\342\210\251"sv,
                              "\342\212\206"sv, "\342\212\207"sv, "\342\212\210"sv, "\342\212\211"sv, "\342\210\236"sv, "\342\210\202"sv, "\342\210\207"sv, "\302\254"sv,
                              "\342\210\253"sv, "\342\210\235"sv, "\342\210\205"sv, "\342\210\210"sv, "\001+"sv, "\302\256"sv, "\302\251"sv, "\001|"sv,
                              "\302\242"sv, "\342\200\241"sv, "\342\230\233"sv, "\342\230\232"sv, "\342\230\205"sv, "\342\230\216"sv, "\342\210\250"sv, "\342\227\213"sv,
                              "\342\224\214"sv, "\342\224\224"sv, "\342\224\220"sv, "\342\224\230"sv, "\342\224\234"sv, "\342\224\244"sv, "\342\224\202"sv, "\342\214\212"sv,
                              "\342\214\213"sv, "\342\214\210"sv, "\342\214\211"sv}}};

     /**
 * @brief Generic ANSI Terminal Configuration
 * 
 * ANSI X3.64/ECMA-48/ISO 6429 compliant terminal for maximum compatibility.
 */
     inline constexpr TerminalTable ansi_table{
         .bset = 0,
         .breset = 0,
         .horizontal_resolution = INCH / 10,
         .vertical_resolution = INCH / 6,
         .newline_spacing = INCH / 6,
         .character_width = INCH / 10,
         .em_width = INCH / 10,
         .halfline_spacing = INCH / 12,
         .adjustment_spacing = INCH / 10,

         .terminal_init = "\033[0m\033[?25h"sv,
         .terminal_reset = "\033c"sv,
         .newline = "\r\n"sv,
         .halfline_reverse = "\033M"sv,
         .halfline_forward = "\033D"sv,
         .fullline_reverse = "\033M"sv,
         .bold_on = "\033[1m"sv,
         .bold_off = "\033[22m"sv,
         .plot_on = ""sv,
         .plot_off = ""sv,
         .cursor_up = "\033[A"sv,
         .cursor_down = "\033[B"sv,
         .cursor_right = "\033[C"sv,
         .cursor_left = "\033[D"sv,

         .character_table = {
             {"\001 "sv, "\001!"sv, "\001\""sv, "\001#"sv, "\001$"sv, "\001%"sv, "\001&"sv, "\001'"sv,
              "\001("sv, "\001)"sv, "\001*"sv, "\001+"sv, "\001,"sv, "\001-"sv, "\001."sv, "\001/"sv,
              "\0010"sv, "\0011"sv, "\0012"sv, "\0013"sv, "\0014"sv, "\0015"sv, "\0016"sv, "\0017"sv,
              "\0018"sv, "\0019"sv, "\001:"sv, "\001;"sv, "\001<"sv, "\001="sv, "\001>"sv, "\001?"sv,
              "\x001@"sv, "\001A"sv, "\001B"sv, "\001C"sv, "\001D"sv, "\001E"sv, "\001F"sv, "\001G"sv,
              "\001H"sv, "\001I"sv, "\001J"sv, "\001K"sv, "\001L"sv, "\001M"sv, "\001N"sv, "\001O"sv,
              "\001P"sv, "\001Q"sv, "\001R"sv, "\001S"sv, "\001T"sv, "\001U"sv, "\001V"sv, "\001W"sv,
              "\001X"sv, "\001Y"sv, "\001Z"sv, "\001["sv, "\001\\"sv, "\001]"sv, "\001^"sv, "\001_"sv,
              "\001`"sv, "\001a"sv, "\001b"sv, "\001c"sv, "\001d"sv, "\001e"sv, "\001f"sv, "\001g"sv,
              "\001\316", /*Xi*/
              "\001\320", /*Pi*/
              "\001\323", /*Sigma*/
              "\000\0", /**/
              "\001\325", /*Upsilon*/
              "\001\326", /*Phi*/
              "\001h"sv, "\001i"sv, "\001j"sv, "\001k"sv, "\001l"sv, "\001m"sv, "\001n"sv, "\001o"sv, Psi * / "\001p"sv, "\001q"sv, "\001r"sv, "\001s"sv, "\001t"sv, "\001u"sv, "\001v"sv, "\001w"sv, Omega * / "\001x"sv, "\001y"sv, "\001z"sv, "\001{"sv, "\001|"sv, "\001}"sv, "\001~"sv, "\000\0"sv, are root * / "\001-"sv, "\001*"sv, "\001+"sv, "\001-"sv, "\001_"sv, "\0031/4"sv, "\0031/2"sv, "\0033/4"sv, , /*terminal sigma*/
              "\001-"sv, "\002fi"sv, "\002fl"sv, "\002ff"sv, "\003ffi"sv, "\003ffl"sv, "\001o"sv, "\001+"sv, */ "\001S"sv, "\001'"sv, "\001'"sv, "\001`"sv, "\001_"sv, "\001/"sv, "\000\0"sv, "\001 "sv, /
                                                                                                                                                                                                             "\001a"sv,
              "\001b"sv, "\001g"sv, "\001d"sv, "\001e"sv, "\001z"sv, "\001h"sv, "\001t"sv, /
                                                                                               "\001i"sv,
              "\001k"sv, "\001l"sv, "\001m"sv, "\001n"sv, "\001x"sv, "\001o"sv, "\001p"sv, ically equal * / "\001r"sv, "\001s"sv, "\001t"sv, "\001u"sv, "\001f"sv, "\001c"sv, "\001p"sv, "\001w"sv, us * / "\001G"sv, "\001D"sv, "\001T"sv, "\001L"sv, "\001X"sv, "\001P"sv, "\001S"sv, "\000\0"sv,
              "\001U"sv, "\001F"sv, "\001P"sv, "\001W"sv, "\001/"sv, "\001s"sv, "\000\0"sv, "\002>="sv, imates * / "\002<="sv, "\002=="sv, "\001-"sv, "\002~="sv, "\002~"sv, "\002!="sv, "\002->"sv, "\002<-"sv, not equal * / "\001^"sv, "\001v"sv, "\001="sv, "\001x"sv, "\001/"sv, "\002+-"sv, "\001U"sv, "\000\0"sv, right arrow * / "\000\0"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\002oo"sv, "\001d"sv, "\001\\/"sv, "\001~"sv,
              "\001S"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\001+"sv, "\002(R)"sv, "\002(C)"sv, "\001|"sv,
              "\002ct"sv, "\000\0"sv, "\000\0"sv, "\000\0"sv, "\001*"sv, "\000\0"sv, "\001|"sv, "\001O"sv, row * / "\001+"sv, "\001+"sv, "\001+"sv, "\001+"sv, "\001+"sv, "\001+"sv, "\001|"sv, "\001_"sv, qual * / "\001-"sv, "\001|"sv, "\001|"sv * / }}};us*/

/**
 * @brief Default terminal configuration
 * set*/
 * Points to ANSI table for maximum compatibility across systems.roper subset*/
 */ superset*/
inline constexpr const TerminalTable& default_terminal = ansi_table;
     infinity * /
         rtial * /
 } // namespace terminal
