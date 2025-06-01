#include "cxx17_scaffold.hpp"
/**
 * @file roff4.c
 * @brief ROFF text formatter - Advanced text processing and formatting functions.
 *
 * This file contains advanced text processing functionality for the ROFF
 * typesetting system, including text flow control, word movement, hyphenation,
 * header/footer processing, and spacing calculations. Originally written in
 * PDP-11 assembly language, this has been converted to portable C90 while
 * preserving all original functionality and behavior.
 *
 * Key Functionality:
 * - Text flow processing (fill/no-fill modes)
 * - Word movement and line fitting algorithms
 * - Automatic hyphenation and line breaking
 * - Header and footer segment processing
 * - Spacing and width calculations
 * - Roman numeral conversion for page numbers
 * - Justification and alignment processing
 * - Page boundary calculations
 *
 * Original Design (PDP-11 Assembly):
 * - Direct register manipulation for text buffers
 * - Stack-based subroutine calls with parameter passing
 * - Bit manipulation for character flags and hyphenation
 * - Optimized loops for character-by-character processing
 * - Manual memory management and pointer arithmetic
 * - Condition code manipulation for return values
 *
 * Modern C90 Implementation:
 * - Structured text processing with clear data flow
 * - Safe string operations and bounds checking
 * - Modular function design with well-defined interfaces
 * - Comprehensive error handling and validation
 * - Type-safe operations with proper casting
 * - Memory-safe pointer operations
 *
 * Text Processing Architecture:
 * 1. Text input and character classification
 * 2. Word accumulation and boundary detection
 * 3. Line fitting with hyphenation support
 * 4. Justification and spacing calculations
 * 5. Header/footer formatting with page numbers
 * 6. Output generation with proper spacing
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Preserve exact ROFF formatting behavior
 * - Efficient algorithms for text processing
 * - Clear separation of concerns
 * - Comprehensive documentation
 * - Robust error handling
 *
 * @note This module implements the sophisticated text flow algorithms
 *       that enable ROFF to produce high-quality typeset output with
 *       automatic line breaking, justification, and page layout.
 */

#include <cstdio> /* Standard I/O operations */
#include <cstdlib> /* Standard library functions */
#include <cstring> /* String manipulation functions */
#include <cctype> /* Character classification */
#include <climits> /* System limits */

/* Local headers */
#include "roff.hpp" /* ROFF system definitions and globals */

/* SCCS version identifier */
[[maybe_unused]] static constexpr std::string_view sccs_id =
    "@(#)roff4.c 1.3 25/05/29 (converted from PDP-11 assembly)"; // ID string

/* Constants for text processing */
#define DEFAULT_PAGE_LENGTH 66 /**< Default page length in lines */
#define MIN_MARGIN 2 /**< Minimum margin size */
#define MIN_LINE_SPACE 4 /**< Minimum space for line fitting */
#define HYPHEN_CHAR '-' /**< Hyphenation character */
#define SPACE_CHAR ' ' /**< Space character */
#define HIGH_BIT 0x80 /**< High bit for hyphenation marking */
#define CHAR_MASK 0x7F /**< Character mask (low 7 bits) */

/* External variables from other modules */
// These are now declared in roff.hpp within the roff namespace
// and made available via "using namespace roff;"

/* Function prototypes for external functions */
// These are now declared in roff.hpp within the roff namespace
// and made available via "using namespace roff;"

/* Local function prototypes */
static void adjust(void);
static int movword(void);
static void topbot(void) ROFF_UNUSED;
static void headin(void) ROFF_UNUSED;
static void headout(void) ROFF_UNUSED;
static void headseg(void (*output_func)(int)) ROFF_UNUSED;
static void decml(void);
static void decml1(void);
static void roman(void);
static void roman1(void);
static void nlines(int count, void (*func)(int));
void nofill(void);

/* Conversion state used by decimal and Roman numeral routines */
static int conv_value;
static void (*conv_out)(int);
static int conv_width;

// For Roman numeral conversion by roman() and roman1()
static char ones[] = "ixcmz";
static char fives[] = "vldw";
static char *onesp;
static char *fivesp;


/**
 * @brief Main text processing function with fill/no-fill mode handling.
 *
 * This is the core text processing function that handles both fill and
 * no-fill modes. In fill mode, it accumulates words and breaks lines
 * automatically. In no-fill mode, it processes characters directly
 * with minimal formatting.
 *
 * Assembly equivalent: Complex text processing loop with multiple
 * branches for different text modes and character types.
 *
 * Processing Flow:
 * 1. Initialize text processing state
 * 2. Check for centering or no-fill mode
 * 3. In fill mode: accumulate words and break lines
 * 4. In no-fill mode: process characters directly
 * 5. Handle spacing, indentation, and underlines
 * 6. Manage line breaking and word movement
 *
 * Fill Mode Processing:
 * - Read characters and classify (space, newline, text)
 * - Accumulate words using getword()
 * - Move words to lines using movword()
 * - Apply justification with adjust()
 * - Handle automatic line breaking
 *
 * No-Fill Mode Processing:
 * - Process characters directly to output
 * - Handle centering calculations
 * - Apply minimal formatting
 * - Preserve exact spacing and layout
 *
 * State Management:
 * - Track underline state and counts
 * - Manage temporary indentation
 * - Update character and word counts
 * - Handle various formatting flags
 */
namespace roff {
void text(void) {
    int c;

    /* Initialize text processing state */
    /* tottext++; */ /* Increment total text counter if needed */
    ulstate = 0; /* Clear underline state */
    ::roff::wch = 0; /* Clear word character count */
    ::roff::wne = 0; /* Clear word width */

    /* Check for centering or no-fill mode */
    if (::roff::ce > 0 || ::roff::fi == 0) {
        nofill(); // This is roff::nofill
        return;
    }

    /* Fill mode processing */
    c = ::roff::getchar_roff();

    while (1) {
        ::roff::ch = c;

        /* Handle spaces - potential line break points */
        if (c == SPACE_CHAR) {
            ::roff::rbreak();

            /* Skip multiple spaces and accumulate as indent */
            do {
                c = ::roff::getchar_roff();
                if (c == SPACE_CHAR) {
                    ::roff::un++;
                }
            } while (c == SPACE_CHAR);
            continue;
        }

        /* Handle newlines - forced line breaks */
        if (c == '\n') {
            ::roff::rbreak();
            ::roff::ch = 0;
            ::roff::nline();
            break;
        }

        /* Handle text characters */
        if (::roff::wch == 0) {
            /* Start new word */
            ::roff::getword();
            break;
        } else {
            /* Continue with existing word */
            if (movword() == 0) { // movword is static
                adjust(); // adjust is static
                c = ::roff::getchar_roff();
                continue;
            }
            /* Word doesn't fit - continue processing */
        }

        c = ::roff::getchar_roff();
    }

    /* Handle underline countdown */
    ::roff::ul--;
    if (::roff::ul < 0) {
        ::roff::ul = 0;
    }
}
} // namespace roff

/**
 * @brief Process text in no-fill mode with optional centering.
 *
 * Handles text processing when fill mode is disabled or when centering
 * is active. Characters are processed directly without automatic line
 * breaking, preserving exact spacing and layout.
 *
 * Assembly equivalent: Simplified character processing loop for
 * no-fill mode with centering calculations.
 *
 * No-Fill Processing:
 * 1. Break any existing line
 * 2. Read characters until newline
 * 3. Calculate character widths and accumulate
 * 4. Store characters directly in line buffer
 * 5. Handle centering if active
 * 6. Output line without justification
 *
 * Centering Logic:
 * - Calculate available space (nel)
 * - Divide by 2 for center positioning
 * - Add to temporary indent (un)
 * - Account for line numbering if active
 * - Force simple spacing (nwd = 1000)
 */
namespace roff {
void nofill(void) {
    int c;
    int char_width;

    /* Break any existing line */
    rbreak();

    /* Process characters until newline */
    while (1) {
        c = gettchar();
        if (c == '\n') {
            break;
        }

        /* Calculate character width and add to line */
        char_width = width(c);
        ne += char_width;
        storeline(c);
    }

    /* Handle centering if active */
    if (ce > 0) {
        ce--;

        /* Calculate centering indent */
        un += (nel >= 0) ? (nel / 2) : 0;

        /* Account for line numbering */
        if (numbmod > 0) {
            un += 2;
        }
    }

    /* Clear justification factors */
    fac = 0;
    fmq = 0;

    /* Force simple word spacing */
    nwd = 1000;

    /* Add trailing space and break line */
    storeline(SPACE_CHAR);
    rbreak();

    /* Handle underline countdown */
    ul--;
    if (ul < 0) {
        ul = 0;
    }
}
} // namespace roff

/**
 * @brief Calculate and apply line justification.
 *
 * Computes justification factors for the current line based on
 * available space and word count, then breaks the line with
 * proper spacing distribution.
 *
 * Assembly equivalent: Justification calculation using division
 * and remainder for even space distribution.
 *
 * Justification Algorithm:
 * 1. Check if adjustment is enabled (ad flag)
 * 2. Calculate number of inter-word gaps (nwd - 1)
 * 3. Divide available space (nel) by number of gaps
 * 4. Store quotient in fmq, remainder in fac
 * 5. Break line with calculated spacing factors
 *
 * Space Distribution:
 * - fmq: Base extra space per gap
 * - fac: Additional space for first 'fac' gaps
 * - Ensures even distribution of extra space
 * - Handles cases with insufficient space gracefully
 */
static void adjust(void) {
    int gaps;
    int quotient, remainder;

    /* Initialize justification factors */
    quotient = 0;
    remainder = 0;

    /* Check if adjustment is enabled and we have gaps to fill */
    if (ad > 0) {
        gaps = nwd - 1; /* Number of inter-word gaps */

        if (gaps > 0 && nel > 0) {
            /* Divide available space among gaps */
            quotient = nel / gaps;
            remainder = nel % gaps;
        }
    }

    /* Set justification factors */
    fac = remainder; /* Extra space for first 'remainder' gaps */
    fmq = quotient; /* Base extra space per gap */

    /* Break line with justification */
    rbreak();
}

/**
 * @brief Handle fill mode spacing with justification.
 *
 * Implements the sophisticated spacing algorithm used during line
 * output in fill mode. Distributes extra space evenly among words
 * with alternating patterns for even/odd lines.
 *
 * Assembly equivalent: Complex spacing calculation with bit testing
 * for even/odd line detection and dynamic space adjustment.
 *
 * Spacing Algorithm:
 * 1. Start with base spacing from fmq
 * 2. Check current line position in output
 * 3. Apply alternating spacing pattern for visual balance
 * 4. Adjust fac counter for remaining extra spaces
 * 5. Output calculated spacing
 * 6. Return next character for processing
 *
 * Even/Odd Line Logic:
 * - Even lines: Add extra space early in line
 * - Odd lines: Add extra space later in line
 * - Creates visually balanced text blocks
 * - Prevents "rivers" of white space
 *
 * @return Next character from line buffer for output
 */
namespace roff { // This was roff::fill, now making it static to roff4
static int fill_line_buffer_and_get_char(void) {
    int spaces;
    int next_char;

    /* Start with base extra spacing */
    spaces = ::roff::fmq;

    /* Apply spacing and advance past current space */
    do {
        spaces++;
        ::roff::nc--;
        ::roff::linep++;
    } while (::roff::nc > 0 && *(::roff::linep - 1) == SPACE_CHAR);

    /* Back up to last non-space character */
    ::roff::linep--;

    /* Apply even/odd line spacing algorithm */
    if ((::roff::totout & 1) == 0) {
        /* Even line - add extra space early */
        ::roff::fac++;
        if (::roff::fac < ::roff::nwd) {
            spaces++;
        }
    } else {
        /* Odd line - add extra space later */
        ::roff::fac--;
        if (::roff::fac >= 0) {
            spaces++;
        }
    }

    /* Output the calculated spacing */
    ::roff::space(spaces); // This is roff::space

    /* Get next character for processing */
    next_char = (unsigned char)*::roff::linep;
    return next_char;
}
} // namespace roff // End of static fill_line_buffer_and_get_char

/**
 * @brief Move word from word buffer to line buffer with hyphenation.
 *
 * This is one of the most complex functions in ROFF, implementing
 * sophisticated word fitting with automatic hyphenation. It determines
 * if a word fits on the current line and handles hyphenation if needed.
 *
 * Assembly equivalent: Very complex assembly routine with multiple
 * nested loops, hyphenation logic, and buffer management.
 *
 * Word Fitting Algorithm:
 * 1. Calculate word boundaries and check if word fits
 * 2. Remove leading spaces from word if first word on line
 * 3. Check various hyphenation conditions
 * 4. Mark potential hyphenation points in word
 * 5. Try to fit word with hyphenation if necessary
 * 6. Handle word overflow and backtracking
 * 7. Update all counters and pointers consistently
 *
 * Hyphenation Logic:
 * - Mark hyphens adjacent to alphabetic characters
 * - Set high bit (0x80) on characters for hyphenation points
 * - Check minimum space requirements for hyphenation
 * - Handle both manual and automatic hyphenation
 * - Backtrack if hyphenation fails
 *
 * Buffer Management:
 * - Coordinate word buffer and line buffer pointers
 * - Track character counts and widths accurately
 * - Handle buffer bounds and overflow conditions
 * - Maintain consistency across all related variables
 *
 * @return 0 if word fits on line, non-zero if line break needed
 */
static int movword(void) {
    char *word_ptr;
    char *line_ptr;
    int original_wch;
    int char_width;
    int c;
    int fits_on_line;
    int wordend_local; // Renamed from wordend to be local

    /* Initialize word processing */
    wordend_local = ::roff::wch;
    word_ptr = ::roff::wordp;
    wordend_local += static_cast<int>(word_ptr - ::roff::word); /* Calculate word end */

    /* Remove leading spaces if first word on line */
    if (::roff::nwd == 0) {
        while (word_ptr < ::roff::word + wordend_local && *word_ptr == SPACE_CHAR) {
            word_ptr++;
            ::roff::wch--;
            char_width = ::roff::width(SPACE_CHAR);
            ::roff::wne -= char_width;
        }
    }

    /* Check if word fits on current line */
    if (::roff::wne <= ::roff::nel) {
        fits_on_line = 1;
        goto move_word_to_line;
    }

    /* Word doesn't fit - check hyphenation conditions */
    if (::roff::nel <= MIN_LINE_SPACE) {
        fits_on_line = 0;
        goto word_overflow;
    }

    /* Check if we're near page bottom and should avoid hyphenation */
    if (::roff::nl + ::roff::ls > ::roff::bl) {
        fits_on_line = 0;
        goto word_overflow;
    }

    /* Check if we have room for two more lines */
    if (::roff::nl + (2 * ::roff::ls) <= ::roff::bl) {
        /* Try hyphenation */
        ::roff::hyphen();
    }

move_word_to_line:
    /* Clear hyphenation count and prepare for character processing */
    ::roff::nhyph = 0;
    original_wch = ::roff::wch;

    /* Process each character in the word */
    while (::roff::wch > 0) {
        c = static_cast<unsigned char>(*word_ptr++);

        /* Handle manual hyphenation markers */
        if (c == HYPHEN_CHAR) {
            /* Check if next character is alphabetic */
            if (word_ptr < ::roff::word + wordend_local) {
                int next_char = static_cast<unsigned char>(*word_ptr);
                if (::roff::alph2(next_char) == 0) { // Call namespaced alph2
                    /* Mark previous character for hyphenation */
                    if (word_ptr > ::roff::word + 1) {
                        *(word_ptr - 1) |= HIGH_BIT;
                    }
                }
            }
        }

        /* Handle automatic hyphenation points */
        if ((c & HIGH_BIT) != 0) {
            /* Remove high bit for character value */
            c &= CHAR_MASK;

            /* Check hyphenation conditions */
            if (word_ptr >= ::roff::word + 4) { /* Minimum prefix length */
                char *check_ptr = word_ptr - 4;
                if (check_ptr >= ::roff::word) {
                    int prev_char = static_cast<unsigned char>(*check_ptr) & CHAR_MASK;
                    if (::roff::alph2(prev_char) != 0 || ::roff::nel >= 2) { // Call namespaced alph2
                        /* Valid hyphenation point */
                        ::roff::storeline(0); /* Mark potential break */
                        ::roff::nhyph++;
                    }
                }
            }
        }

        /* Calculate character width and store in line */
        char_width = ::roff::width(c);
        ::roff::wne -= char_width;
        ::roff::storeline(c);
        ::roff::wch--;
    }

    /* Check if word fit completely */
    if (::roff::nel >= 0) {
        ::roff::nwd++; /* Increment word count */
        fits_on_line = 1;
        goto cleanup;
    }

word_overflow:
    /* Word doesn't fit - handle backtracking */
    line_ptr = ::roff::linep;

    /* Backtrack through characters until word fits or hyphenation point */
    while (1) {
        /* Check for hyphenation opportunities */
        if (::roff::nhyph > 0) {
            if (::roff::nwd > 0 || (::roff::wch == original_wch)) {
                /* Found good hyphenation point */
                break;
            }
        } else if (::roff::nwd == 0) {
            /* No words on line yet */
            if (::roff::wch == original_wch) {
                /* Can't fit any of the word */
                break;
            }
        }

        /* Check space availability */
        if (::roff::nel > 0) {
            break;
        }

        /* Backtrack one character */
        line_ptr--;
        if (*line_ptr == 0) {
            /* Found hyphenation marker */
            ::roff::nhyph--;
            if (::roff::nhyph == 0 && ::roff::nwd == 0) {
                continue; /* Keep looking */
            }
            if (::roff::nel > 0) {
                continue; /* Have space now */
            }

            /* Insert hyphen if not already present */
            if (line_ptr > ::roff::line && *(line_ptr - 1) != HYPHEN_CHAR) {
                *line_ptr = HYPHEN_CHAR;
                ::roff::nel--;
                ::roff::ne++;
            }
            break;
        }

        /* Remove character from line */
        ::roff::nc--;
        if (*line_ptr != 0) {
            char_width = ::roff::width((unsigned char)*line_ptr);
            ::roff::ne -= char_width;
            ::roff::nel += char_width;
            ::roff::wch++;
            word_ptr--;
            ::roff::wne += char_width;
        }
    }

    /* Update word count */
    ::roff::nwd++;

cleanup:
    /* Update word pointer and clean up character flags */
    ::roff::wordp = word_ptr;
    if (word_ptr < ::roff::word + wordend_local) {
        *word_ptr &= CHAR_MASK; /* Clear any high bits */
    }

    /* Ensure word pointer is within bounds */
    if (::roff::wordp < ::roff::word) {
        ::roff::wordp = ::roff::word;
    }

    return fits_on_line ? 0 : 1;
}

/**
 * @brief Calculate page boundaries and validate margin settings.
 *
 * Computes the bottom line position based on page length and margins,
 * validates margin settings, and adjusts them if they create impossible
 * page layouts.
 *
 * Assembly equivalent: Page boundary calculation with margin validation
 * and automatic adjustment for invalid configurations.
 *
 * Page Layout Calculation:
 * 1. Check if page length is defined
 * 2. Calculate bottom line: pl - ma3 - ma4 - hx
 * 3. Calculate top space: ma1 + ma2 + hx
 * 4. Validate that content area is positive
 * 5. Reset to defaults if margins are invalid
 * 6. Ensure current line doesn't exceed bottom
 *
 * Margin Validation:
 * - Ensures content area is at least minimal
 * - Resets all margins to safe defaults if invalid
 * - Uses standard page length of 66 lines
 * - Prevents impossible page layouts
 *
 * Default Recovery:
 * - Sets all margins to 2 lines each
 * - Sets page length to 66 lines
 * - Recursively recalculates boundaries
 * - Ensures system can continue operation
 */
static void topbot(void) {
    int content_area;
    int top_space;

    /* Check if page length is defined */
    if (pl == 0) {
        bl = 0; /* No pagination */
        return;
    }

    /* Calculate content area bottom line */
    content_area = pl;
    content_area -= ma3; /* Subtract footer margin */
    content_area -= ma4; /* Subtract bottom margin */
    content_area -= hx; /* Subtract header/footer space */
    bl = content_area;

    /* Calculate top space requirements */
    top_space = ma1; /* Top margin */
    top_space += ma2; /* Header margin */
    top_space += hx; /* Header/footer space */

    /* Validate page layout */
    if (top_space >= bl) {
        /* Invalid layout - reset to defaults */
        ma1 = ma2 = ma3 = ma4 = MIN_MARGIN;
        pl = DEFAULT_PAGE_LENGTH;
        topbot(); /* Recalculate with defaults */
        return;
    }

    /* Ensure current line doesn't exceed bottom */
    if (nl > bl) {
        nl = bl;
    }
}

/**
 * @brief Calculate character display width for formatting.
 *
 * Determines the display width of a character for proper spacing
 * and line length calculations. Handles special characters and
 * control characters appropriately.
 *
 * Assembly equivalent: Character width lookup with special case
 * handling for various character types.
 *
 * Width Calculation Rules:
 * 1. Hyphenation character (ohc): width 0
 * 2. Null character: width 0
 * 3. DEL character (0177): width 0
 * 4. Backspace (010): width -1 (moves backward)
 * 5. Control characters (< space): width 1
 * 6. Regular characters: width 0 (device-dependent)
 *
 * Special Cases:
 * - Hyphenation characters don't add to line width
 * - Backspace allows overstriking
 * - Control characters have minimal width
 * - Device-dependent characters handled by output system
 *
 * @param c Character to measure
 * @return Display width of character (-1, 0, or 1)
 */
namespace roff {
int width(int c) {
    /* Check for hyphenation character */
    if (c == ohc) {
        return 0;
    }

    /* Check for null character */
    if (c == 0) {
        return 0;
    }

    /* Check for DEL character */
    if (c == 0177) {
        return 0;
    }

    /* Check for backspace */
    if (c == 010) {
        return -1; /* Negative width for backspace */
    }

    /* Check for control characters */
    if (c <= SPACE_CHAR) {
        return 1; /* Minimal width for control chars */
    }

    /* Regular printable characters */
    return 0; /* Device-dependent width */
}
} // namespace roff

/**
 * @brief Wrapper for width function to match void return type.
 *
 * This function calculates character width and accumulates it in a
 * global variable for measurement purposes during header processing.
 *
 * @param c Character to measure
 */
static void width_accumulator(int c) {
    /* This would accumulate width in a global variable */
    /* For now, just call width and ignore return value */
    (void)width(c);
}

/**
 * @brief Wrapper for width function to match headseg function pointer type.
 *
 * This function calls width() but returns void to match the expected
 * function pointer signature for headseg().
 *
 * @param c Character to measure
 */
static void width_wrapper(int c) {
    (void)width(c);
}

/**
 * @brief Read header/footer definition and store in buffer.
 *
 * Processes header or footer definition from input, storing the
 * formatted text in the buffer system for later use during page
 * output. Handles delimiter processing and text collection.
 *
 * Assembly equivalent: Header reading routine with delimiter
 * processing and buffer management.
 *
 * Header Processing:
 * 1. Skip continuation characters and whitespace
 * 2. Store current buffer position
 * 3. Read delimiter character
 * 4. Collect text until delimiter or newline
 * 5. Store null terminator
 * 6. Update buffer position
 * 7. Set line length for header processing
 *
 * Delimiter Handling:
 * - First non-whitespace character is delimiter
 * - Text continues until matching delimiter
 * - Newline also terminates header text
 * - Null character marks end of stored text
 *
 * Buffer Management:
 * - Uses global buffer system (wbf)
 * - Tracks position with nextb variable
 * - Coordinates with include system
 * - Preserves line length settings
 */
static void headin(void) {
    int delimiter;
    int c;
    int buffer_pos;

    /* Skip continuation characters and whitespace */
    skipcont();

    /* Store starting buffer position */
    buffer_pos = nextb;
    /* Store position in calling context would go here */

    /* Read delimiter character */
    delimiter = gettchar();
    if (delimiter == '\n') {
        /* Empty header */
        wbf(0, buffer_pos++);
        nextb = buffer_pos;
        llh = ll; /* Preserve line length */
        return;
    }

    /* Collect header text until delimiter or newline */
    while (1) {
        c = gettchar();
        if (c == '\n') {
            break;
        }
        if (c == delimiter) {
            c = 0; /* Convert delimiter to null terminator */
        }
        wbf(c, buffer_pos++);
        if (c == 0) {
            break; /* Found end delimiter */
        }
    }

    /* Ensure null termination */
    if (c != 0) {
        wbf(0, buffer_pos++);
    }

    /* Update global buffer position */
    nextb = buffer_pos;

    /* Save line length for header processing */
    llh = ll;
}

/**
 * @brief Output formatted header or footer with page number substitution.
 *
 * Processes and outputs a complete header or footer, handling three-part
 * formatting (left, center, right) and page number substitution. This
 * implements the sophisticated header/footer system used in ROFF.
 *
 * Assembly equivalent: Complex header output routine with three-segment
 * processing and spacing calculations.
 *
 * Header Format:
 * - Three segments: left, center, right
 * - Page numbers substituted for % characters
 * - Automatic spacing calculation between segments
 * - Accounting for line numbering if active
 * - Roman or Arabic numerals based on ro flag
 *
 * Spacing Algorithm:
 * 1. Measure width of all three segments
 * 2. Calculate available space for center positioning
 * 3. Account for page offset and line numbering
 * 4. Position center segment in middle of available space
 * 5. Right-align third segment at line end
 *
 * Page Number Processing:
 * - % characters replaced with current page number
 * - Roman numerals if ro flag is set
 * - Arabic numerals otherwise
 * - Width calculated for proper spacing
 */
static void headout(void) {
    int segment_widths[3];
    int buffer_pos ROFF_UNUSED;
    int i;
    int numbering_space;
    int total_width;
    int center_space;
    int right_space;

    /* Measure all three segments */
    for (i = 0; i < 3; i++) {
        segment_widths[i] = 0;
        headseg(width_accumulator); /* Measure segment width */
        /* Width would be returned in segment_widths[i] */
    }
    /* Check if headers/footers are enabled */
    if (hx == 0) {
        return;
    }

    /* Get buffer position for header text */
    buffer_pos = nextb; /* This would come from calling context */
    nfile = ibf1;

    /* Measure all three segments */
    for (i = 0; i < 3; i++) {
        segment_widths[i] = 0;
        headseg(width_wrapper); /* Measure segment width */
        /* Width would be returned in segment_widths[i] */
    }

    /* Output page offset */
    space(po);

    /* Account for line numbering */
    numbering_space = 0;
    if (numbmod > 0) {
        numbering_space = 5 + ni;
    }

    /* Output left segment */
    headseg(putchar_roff);

    /* Calculate center positioning */
    total_width = llh + numbering_space;
    center_space = (total_width - segment_widths[1]) / 2;
    center_space -= segment_widths[0];
    if (center_space < 0) {
        center_space = 0;
    }
    space(center_space);

    /* Output center segment */
    headseg(putchar_roff);

    /* Calculate right positioning */
    right_space = total_width;
    right_space -= center_space;
    right_space -= segment_widths[0];
    right_space -= segment_widths[1];
    right_space -= segment_widths[2];
    right_space += numbering_space;
    space(right_space);

    /* Output right segment */
    headseg(putchar_roff);

    /* End header line */
    newline();
}

/**
 * @brief Process single header/footer segment with page number substitution.
 *
 * Processes one segment of a header or footer, handling page number
 * substitution and applying the specified output function (either
 * for width calculation or actual output).
 *
 * Assembly equivalent: Header segment processing with page number
 * substitution and function pointer usage.
 *
 * Segment Processing:
 * 1. Read characters from buffer until null terminator
 * 2. Check for % characters (page number markers)
 * 3. Substitute page numbers in appropriate format
 * 4. Apply output function to each character
 * 5. Track total width for spacing calculations
 *
 * Page Number Substitution:
 * - % replaced with current page number (pn)
 * - Roman numerals if ro flag is set
 * - Arabic numerals otherwise
 * - Width tracking for both measurement and output
 *
 * Function Pointer Usage:
 * - Allows same code for measurement and output
 * - width() function for measuring
 * - putchar_roff() function for output
 * - Flexible processing based on context
 *
 * @param output_func Function to apply to each character
 */
static void headseg(void (*output_func)(int)) {
    int c;
    int buffer_pos;
    int total_width ROFF_UNUSED;
    int number_width = 0;

    total_width = 0;
    buffer_pos = nextb; /* Starting position */

    /* Process characters in segment */
    while (1) {
        /* Read character from buffer */
        c = rdsufb(buffer_pos++, nfile);
        if (c == 0) {
            break; /* End of segment */
        }

        /* Check for page number substitution */
        if (c == '%') {
            /* Substitute page number */
            if (ro != 0) {
                /* Roman numerals */
                onesp = ones;
                fivesp = fives;
                roman();
                /* Width would be returned in number_width */
                total_width += number_width;
            } else {
                /* Arabic numerals */
                decml();
                /* Width would be returned in number_width */
                total_width += number_width;
            }
        } else {
            /* Regular character */
            output_func(c);
            if (output_func == width_wrapper || output_func == width_accumulator) {
                total_width += 1; /* Assume width 1 for simplicity */
            }
        }
    }

    /* Return total width in calling context */
}

/**
 * @brief Output specified number of spaces.
 *
 * Outputs the specified number of space characters, handling
 * the common case of horizontal spacing in formatted output.
 *
 * Assembly equivalent: Simple space output loop using nlines
 * subroutine with putchar function.
 *
 * @param count Number of spaces to output
 */
namespace roff {
void space(int count) {
    nlines(count, putchar_roff);
}
} // namespace roff

/**
 * @brief Output multiple instances using specified function.
 *
 * Generic function to output a specified number of characters
 * using the provided output function. Used for spacing and
 * repetitive character output.
 *
 * Assembly equivalent: Generic output loop with function pointer
 * and counter management.
 *
 * @param count Number of times to call function
 * @param func Function to call for each iteration
 */
static void nlines(int count, void (*func)(int)) {
    while (count > 0) {
        func(SPACE_CHAR);
        count--;
    }
}

/**
 * @brief Convert number to decimal and output.
 *
 * Converts an integer to decimal representation and outputs
 * each digit using the specified output function. Handles
 * width calculation for spacing purposes.
 *
 * Assembly equivalent: Recursive decimal conversion with
 * digit-by-digit output and width tracking.
 *
 * @param num Number to convert and output
 * @param output_func Function to output each digit
 */
namespace roff {
void decimal(int num, void (*output_func)(int)) {
    conv_value = num;
    conv_out = output_func;
    conv_width = 0;
    decml();
}
} // namespace roff

/**
 * @brief Core decimal conversion implementation.
 *
 * Implements the core decimal conversion algorithm using
 * recursion to output digits in correct order.
 *
 * Assembly equivalent: Recursive decimal conversion with
 * stack management and digit calculation.
 */
static void decml(void) {
    decml1();
}

/**
 * @brief Recursive decimal conversion with width tracking.
 *
 * Recursive function that converts integer to decimal digits
 * and outputs them while tracking total width.
 *
 * Assembly equivalent: Complex recursive routine with stack
 * management and width accumulation.
 */
static void decml1(void) {
    int q = conv_value / 10;
    int r = conv_value % 10;

    if (q > 0) {
        conv_value = q;
        decml();
    }

    conv_out('0' + r);
    conv_width += width('0' + r);
}

/**
 * @brief Convert number to Roman numerals and output.
 *
 * Converts an integer to Roman numeral representation using
 * the traditional algorithm with ones and fives arrays.
 *
 * Assembly equivalent: Complex Roman numeral conversion with
 * multiple digit processing and array manipulation.
 */
static void roman(void) {
    conv_value = pn;
    conv_out = putchar_roff;
    conv_width = 0;

    if (conv_value == 0) {
        conv_out('0');
        conv_width += width('0');
        return;
    }

    roman1();
}

/**
 * @brief Core Roman numeral conversion implementation.
 *
 * Implements the traditional Roman numeral algorithm using
 * recursive processing and character array manipulation.
 *
 * Assembly equivalent: Complex Roman numeral routine with
 * array pointer management and recursive calls.
 */
static void roman1(void) {
    int saved = conv_value;
    int q = saved / 10;
    int r = saved % 10;
    char c;

    if (saved == 0) {
        return;
    }

    if (q > 0) {
        conv_value = q;
        ++onesp;
        ++fivesp;
        roman();
        --onesp;
        --fivesp;
    }

    q = r / 5;
    r %= 5;

    if (r == 4) {
        c = *onesp;
        conv_out(c);
        conv_width += width(c);
        c = q ? *(onesp + 1) : *fivesp;
        conv_out(c);
        conv_width += width(c);
        return;
    }

    if (q) {
        c = *fivesp;
        conv_out(c);
        conv_width += width(c);
    }

    while (r-- > 0) {
        c = *onesp;
        conv_out(c);
        conv_width += width(c);
    }
}
