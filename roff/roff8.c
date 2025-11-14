/**
 * @file roff8.c
 * @brief ROFF global variables and data segment - System state and configuration
 *
 * This module contains all global variables, configuration parameters, and
 * static data structures used throughout the ROFF typesetting system.
 * Originally defined in PDP-11 assembly as a data segment, this has been
 * converted to portable C90 with proper initialization and type safety.
 *
 * Data Organization:
 * - Formatting parameters (margins, line length, spacing)
 * - Page layout settings (page length, headers, footers)
 * - Text processing state (fill mode, adjustment, hyphenation)
 * - Buffer management (input/output buffers, file descriptors)
 * - Character translation tables and lookup arrays
 * - Runtime state variables (counters, flags, pointers)
 *
 * Memory Layout Considerations:
 * - Static initialization for constant data
 * - Proper alignment for structure members
 * - Buffer sizing based on original PDP-11 constraints
 * - String literals with embedded null terminators
 *
 * Assembly Translation Notes:
 * - .byte directives converted to array initializers
 * - .bss allocations converted to uninitialized globals
 * - .even alignment converted to proper C structure padding
 * - Octal constants preserved for compatibility
 *
 * @note This module establishes the foundational data structures that
 *       enable ROFF's sophisticated text processing capabilities while
 *       maintaining exact compatibility with the original system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /* For size_t

/* Include ROFF system headers */
#include "roff_c.h" /* roff definitions (now includes namespaces)




/* Copyright notice from original Bell Labs code */
static const char copyright[] ROFF_UNUSED  = "Copyright 1972 Bell Telephone Laboratories Inc.";

/* SCCS version identifier */
 static const char* sccs_id ROFF_UNUSED =
    "@(#)roff8.c 1.3 25/05/29 (global data - converted from PDP-11 assembly)"; /* ID string */

/* Buffer size constants */
#define LINE_SIZE 500 /**< Maximum line buffer size */
#define WORD_SIZE 200 /**< Maximum word buffer size */
#define SUFFIX_BUFFER_SIZE 20 /**< Suffix processing buffer size */
#define SUFFIX_FILE_SIZE 512 /**< Suffix file buffer size */
#define SUFFIX_TABLE_SIZE 52 /**< Suffix lookup table size (26*2) */
#define TRANSLATE_TABLE_SIZE 128 /**< Character translation table size */
#define TAB_TABLE_SIZE 10 /**< Tab stop table size */
#define INCLUDE_LIST_SIZE 8 /**< Include file list size */
#define FILENAME_SIZE 20 /**< Maximum filename length */

/*
 * =============================================================================
 * FORMATTING PARAMETERS AND CONFIGURATION
 * =============================================================================
 */

/**
 * @brief Slow processing mode flag.
 * Controls detailed processing for complex formatting operations.
 */
int slow = 1;

/**
 * @brief Page top offset in scaled units.
 * Defines the vertical position where page content begins.
 */
int pto = 9999;

/**
 * @brief Page offset (left margin) in character positions.
 * Controls horizontal positioning of all output on the page.
 */
int po = 0;

/**
 * @brief Line spacing value.
 * Current vertical spacing between output lines (1=single, 2=double, etc.).
 */
int ls = 1;

/**
 * @brief Previous line spacing value.
 * Saves the line spacing setting for restoration after temporary changes.
 */
int ls1 = 1;

/**
 * @brief Current page number.
 * Automatically incremented and available for header/footer substitution.
 */
int pn = 1;

/**
 * @brief Top margin size in lines.
 * Number of blank lines at the top of each page before content.
 */
int ma1 = 2;

/**
 * @brief Header margin size in lines.
 * Space between top margin and first line of body text.
 */
int ma2 = 2;

/**
 * @brief Footer margin size in lines.
 * Space between last line of body text and bottom margin.
 */
int ma3 = 1;

/**
 * @brief Bottom margin size in lines.
 * Number of blank lines at the bottom of each page after content.
 */
int ma4 = 3;

/**
 * @brief Line length in character positions.
 * Maximum width for justified text lines.
 */
int ll = 65;

/**
 * @brief Header line length in character positions.
 * Line length used specifically for header and footer formatting.
 */
int llh = 65;

/**
 * @brief Header/footer processing flag.
 * Controls whether headers and footers are generated (1=yes, 0=no).
 */
int hx = 1;

/**
 * @brief Page length in lines.
 * Total number of lines per page including margins.
 */
int pl = 66;

/**
 * @brief Text adjustment flag.
 * Controls line justification (1=justify, 0=left-align only).
 */
int ad = 1;

/**
 * @brief Fill mode flag.
 * Controls automatic line filling (1=fill, 0=no-fill).
 */
int fi = 1;

/**
 * @brief Command character.
 * Character that introduces ROFF control commands (default '.').
 */
int cc = '.';

/**
 * @brief Output hyphenation character.
 * Character code used to represent hyphens in output (octal 200).
 */
int ohc = 0200;

/**
 * @brief Hyphenation flag.
 * Controls automatic hyphenation processing (1=enabled, 0=disabled).
 */
int hyf = 1;

/**
 * @brief Hyphenation processed flag.
 * Indicates whether current word has been processed for hyphenation.
 */
int hypedf = 0;

/**
 * @brief Hyphenation threshold value.
 * Minimum score required for automatic hyphenation point insertion.
 */
int thresh = 0240; /* Octal 240 = decimal 160 */

/**
 * @brief Tab character.
 * Character used for tab stop processing (default space).
 */
int tabc = ' ';

/*
 * =============================================================================
 * STATIC DATA TABLES AND LOOKUP ARRAYS
 * =============================================================================
 */

/**
 * @brief Tab stop positions table.
 * Predefined tab stops at 8-character intervals with null terminator.
 * Original assembly: .byte 8.,16.,24.,32.,40.,48.,56.,64.,72.,0
 */
int tabtab[TAB_TABLE_SIZE] = {
    8, 16, 24, 32, 40, 48, 56, 64, 72, 0};

/**
 * @brief Suffix file pathname.
 * Path to the external suffix pattern database for hyphenation.
 * Original assembly: </usr/lib/suftab\0>
 */
char suffil[] = "/usr/lib/suftab";

/**
 * @brief Roman numeral ones digits.
 * Character sequence for Roman numeral generation (units place).
 * Original assembly: <ixcm>
 */
char ones[] = "ixcm";

/**
 * @brief Roman numeral fives digits.
 * Character sequence for Roman numeral generation (fives place).
 * Original assembly: <vld>
 */
char fives[] = "vld";

/**
 * @brief Terminal device pathname.
 * Path to terminal device for interactive operations.
 * Original assembly: </dev/tty0\0>
 */
char ttyx[] = "/dev/tty0";

/**
 * @brief Temporary file base name.
 * Base pathname for temporary file creation during processing.
 * Original assembly: </tmp/rtma\0>
 */
char bfn[] = "/tmp/rtma";

/*
 * =============================================================================
 * FILE MANAGEMENT AND I/O STATE
 * =============================================================================
 */

/**
 * @brief Output file descriptor.
 * File descriptor for primary output stream (-1 indicates stdout).
 */
int ofile = -1;

/**
 * @brief Next buffer position.
 * Pointer offset for next available position in dynamic buffers.
 */
int nextb = 4;

/**
 * @brief Include list pointer.
 * Points to current position in include file processing list.
 */
int *ilistp = NULL; /* Will be set to ilist address during initialization */

/*
 * =============================================================================
 * RUNTIME STATE VARIABLES (BSS SECTION)
 * =============================================================================
 */

/**
 * @brief Old-style processing flag.
 * Controls compatibility mode for legacy document processing.
 */
int old;

/**
 * @brief Stop processing flag.
 * Emergency halt flag for error conditions or user interruption.
 */
int stop;

/**
 * @brief Garbage collection flag.
 * Controls memory cleanup and buffer reorganization operations.
 */
int garb;

/**
 * @brief Base filename buffer.
 * Storage for the base name of the current input file being processed.
 */
char bname[FILENAME_SIZE];

/**
 * @brief Next filename pointer.
 * Points to the next filename in the processing queue.
 */
char *nextf;

/**
 * @brief Next file index.
 * Numeric index for sequential file processing operations.
 */
int nx;

/**
 * @brief Input buffer file descriptor.
 * File descriptor for the primary input buffer stream.
 */
int ibf;

/**
 * @brief Secondary input buffer file descriptor.
 * File descriptor for auxiliary input operations (includes, etc.).
 */
int ibf1;

/**
 * @brief Skip count.
 * Number of lines or characters to skip in current processing operation.
 */
int skp;

/**
 * @brief Input pointer.
 * Current position pointer within the active input buffer.
 */
int ip;

/**
 * @brief Include file list.
 * Array storing file descriptors for nested include file processing.
 */
int ilist[INCLUDE_LIST_SIZE];

/**
 * @brief Include list end pointer.
 * Points to the end of the active include file list.
 */
int iliste;

/**
 * @brief Current column position.
 * Horizontal position counter for tab and alignment calculations.
 */
int column;

/**
 * @brief Output column position.
 * Current horizontal position in the output stream.
 */
int ocol;

/**
 * @brief Number of spaces.
 * Count of consecutive spaces for spacing and justification calculations.
 */
int nspace;

/**
 * @brief Justification factor.
 * Remainder value used in line justification distribution algorithm.
 */
int fac;

/**
 * @brief Justification quotient.
 * Base spacing value used in line justification calculations.
 */
int fmq;

/**
 * @brief Number of header lines.
 * Count of lines allocated for header processing on current page.
 */
int nhl;

/**
 * @brief Number of characters remaining.
 * Available character count in current line for text placement.
 */
int nel;

/**
 * @brief Justification format modifier.
 * Controls specific justification behavior and spacing algorithms.
 */
int jfomod;

/**
 * @brief Word pointer.
 * Points to the current position within the word being processed.
 */
char *wordp;

/**
 * @brief Newline flag.
 * Indicates whether a newline character has been encountered.
 */
int nlflg;

/**
 * @brief Space flag.
 * Indicates the presence of spacing characters in current processing.
 */
int spaceflg;

/**
 * @brief Current character.
 * The character currently being processed by the text analysis engine.
 */
int ch;

/**
 * @brief Line pointer.
 * Points to the current position within the output line buffer.
 */
char *linep;

/**
 * @brief Underline flag.
 * Indicates whether underline processing is currently active.
 */
int undflg;

/**
 * @brief Word end position.
 * Marks the end boundary of the current word being processed.
 */
int wordend;

/**
 * @brief Maximum digram score.
 * Highest hyphenation weight found during current word analysis.
 */
int maxdig;

/**
 * @brief Maximum score location.
 * Pointer to the character position with the highest hyphenation score.
 */
char *maxloc;

/**
 * @brief Total output lines.
 * Running count of lines generated in the current formatting session.
 */
int totout;

/**
 * @brief Hyphenation start pointer.
 * Points to the beginning of the hyphenatable region in current word.
 */
char *hstart;

/**
 * @brief New hyphenation start pointer.
 * Updated start position for continued hyphenation analysis.
 */
char *nhstart;

/**
 * @brief Number of hyphens.
 * Count of hyphenation points found in the current word.
 */
int nhyph;

/**
 * @brief Argument count.
 * Number of command-line arguments passed to the program.
 */
int argc;

/**
 * @brief Argument pointer.
 * Points to the current command-line argument being processed.
 */
char **argp;

/**
 * @brief Input buffer pointer.
 * Points to the current position in the main input buffer.
 */
char *ibufp;

/**
 * @brief End of input buffer.
 * Points to the end boundary of the input buffer.
 */
char *eibuf;

/**
 * @brief Word width in units.
 * Calculated width of the current word in typographical units.
 */
int wne;

/**
 * @brief Current line number.
 * Vertical position counter within the current page.
 */
int nl;

/**
 * @brief Bottom line position.
 * Last usable line position on the current page.
 */
int bl;

/**
 * @brief Number of characters.
 * Character count in the current line being formatted.
 */
int nc;

/**
 * @brief Number of units.
 * Total width units used in the current line.
 */
int ne;

/**
 * @brief Line number counter.
 * Sequential line numbering for the entire document.
 */
int lnumber;

/**
 * @brief Line numbering mode.
 * Controls automatic line number generation and formatting.
 */
int numbmod;

/**
 * @brief Skip lines count.
 * Number of lines to skip for spacing and page break operations.
 */
int skip;

/**
 * @brief Number of words.
 * Count of words in the current line being formatted.
 */
int nwd;

/**
 * @brief Underline state.
 * Current state of underline processing (active/inactive).
 */
int ulstate;

/**
 * @brief Underline character.
 * Character used for underline representation in output.
 */
int ulc;

/**
 * @brief Backspace character.
 * Character used for backspace operations and overstriking.
 */
int bsc;

/**
 * @brief Number of spaces parameter.
 * Spacing parameter for various formatting operations.
 */
int nsp;

/**
 * @brief Numbering counter.
 * General-purpose counter for numbering and indexing operations.
 */
int nn;

/**
 * @brief Roman numeral flag.
 * Controls Roman vs. Arabic numeral generation for page numbers.
 */
int ro;

/**
 * @brief Print from position.
 * Starting position for partial line printing operations.
 */
int pfrom;

/**
 * @brief Number indent.
 * Indentation amount for line numbering display.
 */
int ni;

/**
 * @brief Ones place pointer.
 * Points to current position in Roman numeral ones array.
 */
char *onesp;

/**
 * @brief Fives place pointer.
 * Points to current position in Roman numeral fives array.
 */
char *fivesp;

/**
 * @brief Underline count.
 * Number of characters remaining to be underlined.
 */
int ul;

/**
 * @brief Center lines count.
 * Number of lines remaining to be centered.
 */
int ce;

/**
 * @brief Indent amount.
 * Current indentation setting for text formatting.
 */
int in;

/**
 * @brief Temporary indent.
 * Temporary indentation for single-line operations.
 */
int un;

/**
 * @brief Word character count.
 * Number of characters in the current word being processed.
 */
int wch;

/**
 * @brief Suffix file descriptor.
 * File descriptor for the hyphenation suffix pattern database.
 */
int suff;

/**
 * @brief Suffix buffer.
 * Small buffer for suffix pattern processing operations.
 */
char sufb[SUFFIX_BUFFER_SIZE];

/**
 * @brief Suffix file buffer.
 * Large buffer for reading suffix pattern data from external file.
 */
char sufbuf[SUFFIX_FILE_SIZE];

/**
 * @brief Suffix lookup table.
 * Hash table for rapid suffix pattern lookup by first character.
 */
short suftab[SUFFIX_TABLE_SIZE];

/**
 * @brief Input file descriptor.
 * File descriptor for the current input file being processed.
 */
int ifile;

/**
 * @brief Character buffer.
 * Single character buffer for input processing operations.
 */
int char_buf;

/**
 * @brief Current file descriptor.
 * File descriptor for the currently active input stream.
 */
int nfile;

/**
 * @brief Even page header.
 * Buffer pointer for even-numbered page header content.
 */
char *ehead;

/**
 * @brief Odd page header.
 * Buffer pointer for odd-numbered page header content.
 */
char *ohead;

/**
 * @brief Even page footer.
 * Buffer pointer for even-numbered page footer content.
 */
char *efoot;

/**
 * @brief Odd page footer.
 * Buffer pointer for odd-numbered page footer content.
 */
char *ofoot;

/**
 * @brief Character translation table.
 * 128-byte table for character set translation and mapping operations.
 */
char trtab[TRANSLATE_TABLE_SIZE];

/**
 * @brief Word buffer.
 * Primary buffer for accumulating and processing individual words.
 */
char word[WORD_SIZE];

/*
 * =============================================================================
 * PRIMARY PROCESSING BUFFERS
 * =============================================================================
 */

/**
 * @brief Static buffer pool start.
 * Marks the beginning of the static buffer allocation area.
 */
static char stbuf_marker ROFF_UNUSED;

/**
 * @brief Line buffer.
 * Primary buffer for accumulating and formatting output lines.
 * This is the most critical buffer in the ROFF system, where all
 * text formatting, justification, and spacing operations are performed.
 */
char line[LINE_SIZE];

/**
 * @brief Output buffer.
 * Secondary buffer for formatted output ready for printing or file output.
 * Referenced by the global obufp pointer for output operations.
 */
static char obuf[LINE_SIZE];

/**
 * @brief Output buffer pointer.
 * Points to the current position in the output buffer for character placement.
 */
char *obufp = obuf;

/**
 * @brief End marker.
 * Symbolic end of the data segment, used for memory management calculations.
 */
static char end_marker ROFF_UNUSED;

/*
 * =============================================================================
 * INITIALIZATION FUNCTIONS
 * =============================================================================
 */

/**
 * @brief Initialize global data structures.
 * 
 * Sets up pointers and relationships between global variables that
 * cannot be statically initialized in C90.
 */
void init_globals(void) {
    /* Initialize include list pointer */
    ilistp = ilist;

    /* Initialize Roman numeral pointers */
    onesp = ones;
    fivesp = fives;

    /* Initialize buffer pointers */
    wordp = word;
    linep = line;

    /* Initialize filename processing */
    nextf = bname;

    /* Clear critical state variables */
    old = 0;
    stop = 0;
    garb = 0;
    nlflg = 0;
    spaceflg = 0;
    undflg = 0;

    /* Initialize counters */
    column = 0;
    ocol = 0;
    nspace = 0;
    totout = 0;
    lnumber = 0;
    skip = 0;
    nwd = 0;
    ulstate = 0;
    ul = 0;
    ce = 0;
    in = 0;
    un = 0;
    wch = 0;
    nhyph = 0;

    /* Clear file descriptors */
    ibf = -1;
    ibf1 = -1;
    ifile = -1;
    suff = -1;
    nfile = -1;

    /* Clear buffer states */
    char_buf = 0;
    iliste = 0;
    nx = 0;
    ip = 0;

    /* Initialize pointers to null */
    argp = NULL;
    ibufp = NULL;
    eibuf = NULL;
    maxloc = NULL;
    hstart = NULL;
    nhstart = NULL;
    ehead = NULL;
    ohead = NULL;
    efoot = NULL;
    ofoot = NULL;
}

/**
 * @brief Get the size of the data segment.
 * 
 * Calculates the total size of the ROFF data segment for memory
 * management and debugging purposes.
 * 
 * @return Total size in bytes of the data segment
 */
size_t get_data_segment_size(void) {
    return ((char*)(&end_marker)) - ((char*)(&stbuf_marker));
}





/* Additional variables needed by stubs.c and other modules */
int sufoff = 0;       /* Suffix buffer offset */
char **argp = NULL;   /* Argument pointer */
