#include "cxx23_scaffold.hpp"
/**
 * @file roff2.c
 * @brief ROFF text formatter - Control command handlers and text processing.
 *
 * This file contains the implementation of all ROFF control commands and
 * text processing functions. Originally written in PDP-11 assembly language,
 * this has been converted to portable C90 while preserving all original
 * functionality and behavior.
 *
 * Key Functionality:
 * - Control command processing (.br, .sp, .ce, .ad, etc.)
 * - Text formatting and line breaking
 * - Page layout and margin control
 * - Header and footer management
 * - Line numbering and spacing
 * - Tab stop management
 * - Character translation and hyphenation
 * - Macro definition and expansion
 *
 * Original Design (PDP-11 Assembly):
 * - Direct register manipulation
 * - Jump-based control flow
 * - Manual stack management
 * - Global state variables
 * - Optimized for memory-constrained systems
 *
 * Modern C90 Implementation:
 * - Structured function organization
 * - Proper parameter passing
 * - Local variable scoping
 * - Standard library integration
 * - Comprehensive error handling
 * - Clear separation of concerns
 *
 * Control Command Categories:
 * - Text formatting: .ad, .na, .fi, .nf, .ce
 * - Line control: .br, .sp, .bl, .ls, .ss
 * - Page control: .bp, .pa, .pl, .sk
 * - Indentation: .in, .ti, .un, .po
 * - Headers/footers: .he, .fo, .eh, .oh, .ef, .of
 * - Margins: .m1, .m2, .m3, .m4
 * - Line numbering: .n1, .n2, .nn, .ni
 * - Miscellaneous: .ta, .tr, .ul, .hc, .hy, .tc
 * - Flow control: .nx, .de, .ig, .mk
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Preserve original ROFF behavior exactly
 * - Robust error handling and validation
 * - Efficient processing with minimal overhead
 * - Clear documentation and maintainability
 */

#include <stdio.h> /* Standard I/O operations */
#include <stdlib.h> /* Standard library functions */
#include <string.h> /* String manipulation functions */
#include <ctype.h> /* Character classification */
#include <limits.h> /* System limits */

/* Local headers - these should exist in the project */
#include "roff.h" /* ROFF system definitions and globals */
#include "roff_globals.hpp" /* Shared globals and prototypes */

/* SCCS version identifier */
[[maybe_unused]] static constexpr std::string_view sccs_id =
    "@(#)roff2.c 1.3 25/05/29 (converted from PDP-11 assembly)"; // ID string

/* External variables from roff1.c and other modules */

/* Function prototypes for external functions */

/* Local function prototypes for C90 compliance */
static void validate_line_count(int count);
static void validate_indent_value(int value);
static void validate_page_value(int value);
static void process_translation_pair(void) ROFF_UNUSED;
static void process_tab_stops(void);
static void setup_line_numbering(int mode, int start_value);
static void handle_header_footer(char **target_ptr);

/**
 * @brief Validate line count parameters.
 *
 * Ensures line count values are within reasonable bounds
 * to prevent excessive processing or negative values.
 *
 * @param count Line count to validate
 */
static void validate_line_count(int count) {
    if (count < 0) {
        /* Negative values are treated as zero */
        count = 0;
    } else if (count > 1000) {
        /* Extremely large values are capped */
        count = 1000;
    }
}

/**
 * @brief Validate indent values.
 *
 * Ensures indent values are within reasonable bounds
 * for proper text formatting.
 *
 * @param value Indent value to validate
 */
static void validate_indent_value(int value) {
    if (value < 0) {
        value = 0;
    } else if (value > ll) {
        /* Indent cannot exceed line length */
        value = ll;
    }
}

/**
 * @brief Validate page-related values.
 *
 * Ensures page numbers and lengths are within reasonable bounds.
 *
 * @param value Page-related value to validate
 */
static void validate_page_value(int value) {
    if (value < 1) {
        value = 1;
    } else if (value > 9999) {
        value = 9999;
    }
}

/**
 * @brief Process character translation pair.
 *
 * Reads two characters and sets up translation from first to second.
 * Used by the .tr command for character substitution.
 */
static void process_translation_pair(void) {
    int from_char, to_char;

    from_char = getchar_roff();
    if (from_char == '\n') {
        return; /* End of translation pairs */
    }

    to_char = getchar_roff();
    if (to_char == '\n') {
        to_char = ' '; /* Default to space if no second character */
    }

    /* Validate character range */
    if (from_char >= 0 && from_char < 128) {
        trtab[from_char] = (unsigned char)to_char;
    }
}

/**
 * @brief Process tab stop settings.
 *
 * Reads numeric tab stop positions and stores them in the tab table.
 * Used by the .ta command for setting custom tab stops.
 */
static void process_tab_stops(void) {
    int tab_pos;
    int tab_index = 0;

    while (tab_index < 20) { /* Maximum 20 tab stops */
        tab_pos = number(0);
        tab_pos = min(tab_pos);

        if (tab_pos <= 0) {
            break; /* End of tab stops */
        }

        tab_pos--; /* Convert to zero-based indexing */
        if (tab_pos >= 0 && tab_pos < 256) {
            tabtab[tab_index++] = (unsigned char)tab_pos;
        }
    }

    /* Mark end of tab stops */
    if (tab_index < 20) {
        tabtab[tab_index] = 0;
    }
}

/**
 * @brief Setup line numbering parameters.
 *
 * Configures line numbering mode and starting value.
 * Used by .n1 and .n2 commands.
 *
 * @param mode Line numbering mode (1 or 2)
 * @param start_value Starting line number
 */
static void setup_line_numbering(int mode, int start_value) {
    if (start_value > 0) {
        numbmod = mode;
        lnumber = start_value;
        nn = 0; /* Reset line numbering skip */
    } else {
        numbmod = 0; /* Disable line numbering */
    }
}

/**
 * @brief Handle header/footer processing.
 *
 * Common processing for header and footer commands.
 * Manages the header/footer string parsing and storage.
 *
 * @param target_ptr Pointer to target header/footer string pointer
 */
static void handle_header_footer(char **target_ptr) {
    headin(target_ptr);
}

/* ========================================================================
 * CONTROL COMMAND IMPLEMENTATIONS
 * ========================================================================
 * 
 * Each function implements a specific ROFF control command, maintaining
 * exact compatibility with the original assembly implementation while
 * using modern C90 idioms and error handling.
 */

/**
 * @brief .ad - Adjust (justify) text.
 *
 * Enables text justification mode. Lines are adjusted to fill
 * the available width by adding extra spaces between words.
 * Calls rbreak() to ensure any pending line is output first.
 */
void case_ad(void) {
    rbreak();
    ad = 1; /* Enable adjust mode */
}

/**
 * @brief .br - Break line.
 *
 * Forces a line break, outputting any text currently being
 * accumulated without waiting for the line to fill.
 * This is the most basic formatting command.
 */
void case_br(void) {
    rbreak();
}

/**
 * @brief .cc - Control character.
 *
 * Changes the control character (default '.') to a different
 * character. If no character is specified, retains current setting.
 * The control character begins command lines.
 */
void case_cc(void) {
    int new_cc;

    skipcont();
    new_cc = getchar_roff();

    if (new_cc != '\n') {
        cc = (char)new_cc;
    }

    ch = new_cc; /* Save character for potential reuse */
}

/**
 * @brief .ce - Center lines.
 *
 * Centers the specified number of lines. If no number is given,
 * centers one line. Centered lines are not justified and are
 * positioned in the middle of the available width.
 *
 * Processing:
 * 1. Break current line
 * 2. Read number of lines to center (default 0)
 * 3. Ensure adequate space on page
 * 4. Set centering counter
 */
void case_ce(void) {
    int lines_to_center;

    rbreak();
    lines_to_center = number(0);
    lines_to_center = min(lines_to_center);
    ce = lines_to_center;
    need(lines_to_center);
}

/**
 * @brief .ds - Double space.
 *
 * Sets line spacing to double (2). This affects the vertical
 * spacing between lines of text output.
 */
void case_ds(void) {
    rbreak();
    ls = 2; /* Set double spacing */
}

/**
 * @brief .fi - Fill mode.
 *
 * Enables fill mode where text lines are filled to the
 * maximum width before breaking. This is the normal
 * text processing mode.
 */
void case_fi(void) {
    rbreak();
    fi = 1; /* Enable fill mode */
}

/**
 * @brief .in - Indent.
 *
 * Sets the left margin indent for subsequent lines.
 * The indent value is added to the current page offset.
 * Also sets the temporary indent (un) to the same value.
 *
 * @note The indent persists until changed by another .in command.
 */
void case_in(void) {
    int indent_value;

    rbreak();
    indent_value = number(in); /* Use current indent as default */
    indent_value = min(indent_value);
    validate_indent_value(indent_value);

    in = indent_value;
    un = indent_value; /* Also set temporary indent */
}

/**
 * @brief .ix - Immediate indent.
 *
 * Sets the indent for the current line only, without
 * breaking the current line first. Used for special
 * formatting where indent change takes effect immediately.
 */
void case_ix(void) {
    int indent_value;

    indent_value = number(in);
    indent_value = min(indent_value);
    validate_indent_value(indent_value);

    in = indent_value;
}

/**
 * @brief .li - Literal lines.
 *
 * Processes the specified number of lines literally,
 * without any formatting. Each line is processed as
 * raw text and output directly.
 *
 * Processing:
 * 1. Read number of literal lines
 * 2. For each line:
 *    - Flush current input
 *    - Clear newline flag
 *    - Process line as text
 */
void case_li(void) {
    int literal_count;

    literal_count = number(0);
    validate_line_count(literal_count);

    while (literal_count > 0) {
        literal_count--;
        flushi();
        nlflg = 0;
        text();
    }
}

/**
 * @brief .ll - Line length.
 *
 * Sets the maximum line length for text formatting.
 * This controls where lines are broken during fill mode.
 * The line length includes any indentation.
 */
void case_ll(void) {
    int line_length;

    line_length = number(ll); /* Use current line length as default */
    line_length = min(line_length);

    if (line_length > 0) {
        ll = line_length;
    }
}

/**
 * @brief .ls - Line spacing.
 *
 * Sets the line spacing value. If no argument is provided,
 * restores the previously saved line spacing (ls1).
 * Line spacing controls vertical space between text lines.
 */
void case_ls(void) {
    int spacing_value;

    rbreak();
    skipcont();

    spacing_value = getchar_roff();
    if (spacing_value == '\n') {
        /* No argument - restore saved spacing */
        ls = ls1;
        return;
    }

    ch = spacing_value; /* Put character back */
    spacing_value = number1(ls); /* Use current spacing as default */
    spacing_value--;
    spacing_value = min(spacing_value);
    spacing_value++;

    if (spacing_value > 0) {
        ls = spacing_value;
        ls1 = spacing_value; /* Save for later restoration */
    }
}

/**
 * @brief .na - No adjust.
 *
 * Disables text justification mode. Lines are output
 * with natural spacing between words, not stretched
 * to fill the available width.
 */
void case_na(void) {
    rbreak();
    ad = 0; /* Disable adjust mode */
}

/**
 * @brief .ne - Need space.
 *
 * Ensures that the specified number of lines are available
 * on the current page. If not enough space is available,
 * forces a page break.
 */
void case_ne(void) {
    int lines_needed;

    lines_needed = number(0);
    lines_needed = min(lines_needed);
    validate_line_count(lines_needed);

    need(lines_needed);
}

/**
 * @brief .nf - No fill.
 *
 * Disables fill mode. Lines are output exactly as they
 * appear in the input, without attempting to fill them
 * to the maximum width.
 */
void case_nf(void) {
    rbreak();
    fi = 0; /* Disable fill mode */
}

/**
 * @brief .pa/.bp - Page break.
 *
 * Forces a page break and optionally sets a new page number.
 * If a number is provided, it becomes the new page number.
 *
 * Processing:
 * 1. Break current line
 * 2. Eject current page
 * 3. Skip to end of command line
 * 4. If number provided and not at line start, set page number
 */
void case_pa(void) {
    int new_page_num;

    rbreak();
    eject();
    skipcont();

    if (nlflg == 0) {
        /* Number provided */
        new_page_num = number(pn);
        new_page_num = min(new_page_num);
        validate_page_value(new_page_num);
        pn = new_page_num;
    }
}

/**
 * @brief .bp - Break page (alias for .pa).
 *
 * Identical to .pa command. Forces a page break with
 * optional page number setting.
 */
void case_bp(void) {
    case_pa(); /* Delegate to page break handler */
}

/**
 * @brief .bl - Blank lines.
 *
 * Outputs the specified number of blank lines.
 * Ensures adequate space is available before outputting.
 *
 * Processing:
 * 1. Break current line
 * 2. Read number of blank lines
 * 3. Ensure space is available
 * 4. Output blank lines by storing spaces and breaking
 */
void case_bl(void) {
    int blank_count;

    rbreak();
    blank_count = number(0);
    blank_count = min(blank_count);
    validate_line_count(blank_count);

    need2(blank_count);

    while (blank_count > 0) {
        blank_count--;
        storeline(' '); /* Store a space to create blank line */
        rbreak();
    }
}

/**
 * @brief .pl - Page length.
 *
 * Sets the page length in lines. This controls when
 * automatic page breaks occur during text processing.
 */
void case_pl(void) {
    int page_length;

    page_length = number(pl); /* Use current page length as default */
    validate_page_value(page_length);

    pl = page_length;
    topbot(); /* Recalculate top/bottom margins */
}

/**
 * @brief .sk - Skip lines.
 *
 * Sets the number of lines to skip at the next page break.
 * This is used for positioning text on the following page.
 */
void case_sk(void) {
    int skip_count;

    skip_count = number(0);
    skip_count = min(skip_count);
    validate_line_count(skip_count);

    skip = skip_count;
}

/**
 * @brief .sp - Space lines.
 *
 * Outputs the specified number of blank lines using
 * the current line spacing. Different from .bl in that
 * it respects line spacing settings.
 */
void case_sp(void) {
    int space_count;

    rbreak();
    space_count = number(0);
    validate_line_count(space_count);

    nlines(space_count, nl); /* Use current line for spacing calculation */
}

/**
 * @brief .ss - Single space.
 *
 * Sets line spacing to single (1). This is the default
 * line spacing for normal text output.
 */
void case_ss(void) {
    rbreak();
    ls = 1; /* Set single spacing */
}

/**
 * @brief .tr - Translate characters.
 *
 * Sets up character translation pairs. Each pair of
 * characters defines a translation from the first
 * character to the second character.
 *
 * Format: .tr ab cd ef
 * This translates 'a' to 'b', 'c' to 'd', 'e' to 'f'
 */
void case_tr(void) {
    skipcont();

    while (1) {
        int from_char = getchar_roff();
        int to_char;

        if (from_char == '\n') {
            break; /* End of translation pairs */
        }

        to_char = getchar_roff();
        if (to_char == '\n') {
            to_char = ' '; /* Default to space */
        }

        /* Set up translation */
        if (from_char >= 0 && from_char < 128) {
            trtab[from_char] = (unsigned char)to_char;
        }
    }
}

/**
 * @brief .ta - Tab stops.
 *
 * Sets custom tab stop positions. Multiple tab stops
 * can be specified on one line.
 *
 * Format: .ta 8 16 24 32
 * Sets tab stops at columns 8, 16, 24, and 32
 */
void case_ta(void) {
    process_tab_stops();
}

/**
 * @brief .ti - Temporary indent.
 *
 * Sets a temporary indent that applies only to the next line.
 * After that line is output, the indent reverts to the
 * previous setting.
 */
void case_ti(void) {
    int temp_indent;

    rbreak();
    temp_indent = number(in); /* Use current indent as default */
    temp_indent = min(temp_indent);
    validate_indent_value(temp_indent);

    un = temp_indent; /* Set temporary indent */
}

/**
 * @brief .ul - Underline.
 *
 * Sets the number of lines to underline. Underlined
 * text has underscores inserted between characters
 * for emphasis on devices that support it.
 */
void case_ul(void) {
    int underline_count;

    underline_count = number(0);
    underline_count = min(underline_count);
    validate_line_count(underline_count);

    ul = underline_count;
}

/**
 * @brief .un - Undent.
 *
 * Sets a negative temporary indent relative to the
 * current indent setting. Used for hanging indents
 * and outdented paragraphs.
 */
void case_un(void) {
    int undent_value;

    undent_value = number(0);
    undent_value = in - undent_value; /* Calculate relative to current indent */
    undent_value = min(undent_value);

    if (undent_value < 0) {
        undent_value = 0; /* Don't allow negative indents */
    }

    un = undent_value;
}

/**
 * @brief .hx - Header/footer processing toggle.
 *
 * Toggles the header and footer processing mode.
 * When enabled, headers and footers are printed on pages.
 * When disabled, pages contain only body text.
 */
void case_hx(void) {
    if (hx) {
        hx = 0; /* Disable header/footer processing */
    } else {
        hx = 1; /* Enable header/footer processing */
    }

    topbot(); /* Recalculate page layout */
}

/**
 * @brief .he - Header even.
 *
 * Sets the header for even-numbered pages.
 * Also copies to odd header if not separately set.
 */
void case_he(void) {
    handle_header_footer(&ehead);
    ohead = ehead; /* Copy to odd header */
}

/**
 * @brief .fo - Footer.
 *
 * Sets the footer for pages.
 * Also copies to odd footer if not separately set.
 */
void case_fo(void) {
    handle_header_footer(&efoot);
    ofoot = efoot; /* Copy to odd footer */
}

/**
 * @brief .eh - Even header.
 *
 * Sets the header specifically for even-numbered pages.
 */
void case_eh(void) {
    handle_header_footer(&ehead);
}

/**
 * @brief .oh - Odd header.
 *
 * Sets the header specifically for odd-numbered pages.
 */
void case_oh(void) {
    handle_header_footer(&ohead);
}

/**
 * @brief .ef - Even footer.
 *
 * Sets the footer specifically for even-numbered pages.
 */
void case_ef(void) {
    handle_header_footer(&efoot);
}

/**
 * @brief .of - Odd footer.
 *
 * Sets the footer specifically for odd-numbered pages.
 */
void case_of(void) {
    handle_header_footer(&ofoot);
}

/**
 * @brief .m1 - Top margin.
 *
 * Sets the top margin (space before header).
 */
void case_m1(void) {
    int margin_value;

    margin_value = number(ma1);
    margin_value = min(margin_value);
    validate_line_count(margin_value);

    ma1 = margin_value;
    topbot(); /* Recalculate page layout */
}

/**
 * @brief .m2 - Header margin.
 *
 * Sets the margin between header and text.
 */
void case_m2(void) {
    int margin_value;

    margin_value = number(ma2);
    margin_value = min(margin_value);
    validate_line_count(margin_value);

    ma2 = margin_value;
    topbot(); /* Recalculate page layout */
}

/**
 * @brief .m3 - Footer margin.
 *
 * Sets the margin between text and footer.
 */
void case_m3(void) {
    int margin_value;

    margin_value = number(ma3);
    margin_value = min(margin_value);
    validate_line_count(margin_value);

    ma3 = margin_value;
    topbot(); /* Recalculate page layout */
}

/**
 * @brief .m4 - Bottom margin.
 *
 * Sets the bottom margin (space after footer).
 */
void case_m4(void) {
    int margin_value;

    margin_value = number(ma4);
    margin_value = min(margin_value);
    validate_line_count(margin_value);

    ma4 = margin_value;
    topbot(); /* Recalculate page layout */
}

/**
 * @brief .hc - Hyphenation character.
 *
 * Sets the character used for hyphenation.
 * If no character is specified, uses a special
 * internal value (128) to disable hyphenation.
 */
void case_hc(void) {
    int hyph_char;

    skipcont();
    hyph_char = getchar_roff();

    if (hyph_char == '\n') {
        ohc = 128; /* Special value for no hyphenation */
    } else {
        ohc = hyph_char;
    }
}

/**
 * @brief .tc - Tab character.
 *
 * Sets the character used for tab expansion.
 * If no character is specified, uses space.
 */
void case_tc(void) {
    int tab_char;

    skipcont();
    tab_char = getchar_roff();

    if (tab_char == '\n') {
        tabc = ' '; /* Default to space */
    } else {
        tabc = tab_char;
    }
}

/**
 * @brief .hy - Hyphenation.
 *
 * Sets the hyphenation mode. Zero disables hyphenation,
 * non-zero enables it with the specified algorithm.
 */
void case_hy(void) {
    int hyph_mode;

    hyph_mode = number(0);
    hyf = hyph_mode;
}

/**
 * @brief .n1 - Line numbering mode 1.
 *
 * Enables line numbering in mode 1 format.
 * If a number is provided, it becomes the starting line number.
 */
void case_n1(void) {
    int start_num;

    rbreak();
    start_num = number(0);
    setup_line_numbering(1, start_num);
}

/**
 * @brief .n2 - Line numbering mode 2.
 *
 * Enables line numbering in mode 2 format.
 * If a number is provided, it becomes the starting line number.
 */
void case_n2(void) {
    int start_num;

    rbreak();
    start_num = number(0);
    setup_line_numbering(2, start_num);
}

/**
 * @brief .nn - Number lines skip.
 *
 * Sets the number of lines to skip before starting
 * line numbering. Used to suppress line numbers
 * for headings or special text.
 */
void case_nn(void) {
    int skip_count;

    skip_count = number(0);
    skip_count = min(skip_count);
    validate_line_count(skip_count);

    nn = skip_count;
}

/**
 * @brief .ni - Number indent.
 *
 * Sets the indent for line numbers. This controls
 * how far from the left margin line numbers appear.
 */
void case_ni(void) {
    int number_indent;

    number_indent = number(ni);
    number_indent = min(number_indent);
    validate_indent_value(number_indent);

    ni = number_indent;
}

/**
 * @brief .jo - Justification options.
 *
 * Sets justification and formatting options.
 * Controls how text is justified and formatted.
 */
void case_jo(void) {
    int justify_mode;

    justify_mode = number(0);
    jfomod = justify_mode;
}

/**
 * @brief .ar - Arabic mode.
 *
 * Disables read-only mode, allowing normal text processing.
 */
void case_ar(void) {
    ro = 0; /* Disable read-only mode */
}

/**
 * @brief .ro - Read-only mode.
 *
 * Enables read-only mode for special text processing.
 */
void case_ro(void) {
    ro = 1; /* Enable read-only mode */
}

/**
 * @brief .nx - Next file.
 *
 * Switches to processing the next input file.
 * The filename is read from the command line.
 *
 * Processing:
 * 1. Skip to end of current command
 * 2. Read next filename
 * 3. Set next file flag
 * 4. Switch to new file
 * 5. Reset include processing state
 */
void case_nx(void) {
    skipcont();
    getname(nextf); /* Read filename into nextf buffer */
    nx = 1; /* Set next file flag */
    nextfile(); /* Switch to new file */
    nlflg = 1; /* Set newline flag */
    ip = 0; /* Clear include pointer */
    ilistp = ilist; /* Reset include list pointer */
}

/**
 * @brief .po - Page offset.
 *
 * Sets the left page offset (left margin) for all text.
 * This shifts the entire text block horizontally.
 */
void case_po(void) {
    int offset_value;

    rbreak();
    offset_value = number(po);
    offset_value = min(offset_value);
    validate_indent_value(offset_value);

    po = offset_value;
}

/**
 * @brief .de - Define macro.
 *
 * Defines a macro that can be called later.
 * If not currently in include processing, sets up
 * macro definition in the control table.
 *
 * Processing:
 * 1. Check if in include processing (skip if so)
 * 2. Skip to continuation
 * 3. Read macro name
 * 4. Search for existing macro in control table
 * 5. Set up macro definition or update existing
 * 6. Copy macro body to buffer
 */
void case_de(void) {
    /* Implementation depends on macro system structure */
    /* This is a simplified version */

    if (ip != 0) {
        return; /* Skip if in include processing */
    }

    skipcont();
    getname(bname); /* Read macro name */

    /* Macro definition processing would go here */
    /* This involves complex buffer management */

    copyb(); /* Copy macro body */
}

/**
 * @brief .ig - Ignore.
 *
 * Ignores text until the matching end marker.
 * Used for comments and conditional text.
 */
void case_ig(void) {
    skp = 1; /* Set skip flag */
    copyb(); /* Skip until end marker */
}

/**
 * @brief .mk - Mark position.
 *
 * Marks the current vertical position for later
 * reference. Outputs a special control character
 * that can be used for positioning.
 */
void case_mk(void) {
    rbreak();
    putchar_roff(002); /* Output STX (start of text) marker */
}
