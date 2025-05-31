/**
 * @file roff.hpp
 * @brief ROFF System Definitions and Global Variables
 * @author Modern C++23 Implementation
 * @version 1.0
 * @date 2024
 *
 * @details Core ROFF system definitions, global state variables,
 *          and function declarations for the text formatting system.
 */

#pragma once

#include "cxx23_scaffold.hpp"

namespace roff {

// ============================================================================
// Global State Variables (externally defined in implementation files)
// ============================================================================

// Character and line processing
extern int nc;          ///< Number of characters in current line
extern int totout;      ///< Total output lines counter
extern int ls;          ///< Line spacing value
extern int pl;          ///< Page length in lines
extern int nl;          ///< Current line number on page
extern int bl;          ///< Bottom line number (computed)
extern int ma1, ma2, ma3, ma4;  ///< Margins: top, header, footer, bottom
extern int pn;          ///< Current page number
extern int skip;        ///< Lines to skip at next page break
extern int po;          ///< Page offset (left margin)
extern int un;          ///< Temporary indent value
extern int in;          ///< Permanent indent value

// Justification and formatting
extern int jfomod;      ///< Justification mode setting
extern int fac, fmq;    ///< Justification factors for spacing
extern int nel;         ///< Number of characters left in current line
extern int nwd;         ///< Number of words in current line
extern int ne;          ///< Number of characters in current line

// Line numbering
extern int numbmod;     ///< Line numbering mode (0=off, 1=mode1, 2=mode2)
extern int nn;          ///< Line number skip count
extern int ni;          ///< Line number indent spacing
extern int lnumber;     ///< Current line number for numbering
extern int ll;          ///< Line length setting

// Word processing
extern int ohc;         ///< Output hyphenation character
extern int wne;         ///< Word character count for width calculation
extern int wch;         ///< Word character count
extern int nhyph;       ///< Hyphenation point count in current word
extern int hypedf;      ///< Hyphenation detected flag
extern int spaceflg;    ///< Extra space flag for sentence spacing

// Processing control
extern int stop;        ///< Interactive stop mode flag
extern int pfrom, pto;  ///< Page range limits (from, to)
extern int hx;          ///< Header/footer processing flag
extern int skp;         ///< Skip processing flag for .ig command

// Buffer management
extern int ip;          ///< Include processing pointer
extern int nextb;       ///< Next buffer position for writing
extern int ibf, ibf1;   ///< Buffer file descriptors
extern int ofile, nfile; ///< File descriptors for operations
extern int sufoff;      ///< Suffix buffer offset for caching

// Buffer pointers
extern char* linep;     ///< Current position in line buffer
extern char* wordp;     ///< Current position in word buffer
extern char* ehead;     ///< Even page header string
extern char* efoot;     ///< Even page footer string
extern char* ohead;     ///< Odd page header string
extern char* ofoot;     ///< Odd page footer string

// Buffer arrays
extern char line[];     ///< Main line accumulation buffer
extern char word[];     ///< Word accumulation buffer
extern char sufbuf[];   ///< Suffix buffer for file caching
extern char garb[];     ///< Garbage buffer for input operations

// Include processing
extern int* ilistp;     ///< Include list stack pointer
extern int ilist[];     ///< Include list stack array

// Input processing
extern int ch;          ///< Current character from input
extern int nlflg;       ///< Newline flag for input processing

// ============================================================================
// Core Function Declarations
// ============================================================================

// Character I/O
int getchar_roff();
int gettchar();
void putchar_roff(int c);
void flush();

// Text processing
int width(int c);
void headout(char** header_ptr);
void decimal(int num, void (*output_func)(int));
void space(int count);
void fill();
void flushi();

// Line and page processing
void eject();
void skipcont();
void rbreak();
void jfo();
void donum();
void newline();
void nline();
void need(int lines);
void need2(int lines);
int number(int default_val);
int number1(int default_val);

// Word processing
void getword();
void setnel();
void storeword(int c);
void storeline(int c);

// Buffer management
void copyb();

// Utility functions
int min(int value);
void getname(char* name_buffer);
void istop();

// ============================================================================
// Constants
// ============================================================================

constexpr int WORD_SIZE = 64;          ///< Maximum word length
constexpr int LINE_SIZE = 256;         ///< Maximum line length
constexpr int SUFFIX_BUF_SIZE = 512;   ///< Suffix buffer size
constexpr int MAX_NAME_LEN = 18;       ///< Maximum filename length
constexpr int BLOCK_MASK = 077;        ///< Block alignment mask (octal 77)

} // namespace roff

// Make global variables accessible without namespace qualification
using namespace roff;
