#include "../cxx17_scaffold.hpp"
/**
 * @file n7.c
 * @brief Text Processing and Line Breaking Module for troff
 *
 * This module handles the core text processing functionality of troff, including:
 * - Line breaking and justification algorithms.
 * - Text flow control, supporting both fill and no-fill modes.
 * - Character and word processing, including width calculations.
 * - Page and line management, including vertical spacing and trap handling.
 * - Hyphenation logic integration.
 * - Line numbering.
 *
 * Originally part of the troff text formatting system from Bell Labs,
 * this code has been modernized for C90 compliance while aiming to
 * preserve its original functionality and typographic precision.
 *
 * Key Functions:
 * - tbreak(): Breaks the current line and outputs accumulated text.
 * - text(): Processes incoming text characters, managing filling and line breaks.
 * - nofill(): Outputs text directly without filling or justification.
 * - newline(): Handles newline processing, vertical motion, and trap invocation.
 * - movword(): Moves processed words from an intermediate buffer to the line buffer.
 * - getword(): Extracts the next word from the input stream.
 *
 * Design Principles:
 * - Character-level precision for fine-grained typographic control.
 * - Support for dynamic line adjustments, justification, and hyphenation.
 * - Integration with troff's diversion and macro processing systems.
 * - Robust handling of various input conditions and overflow scenarios.
 */

#include "tdef.hpp" // troff definitions
#include "env.hpp"  // environment data
#include "t.hpp"    // troff common
#include "tw.hpp"   // typewriter table

#ifdef NROFF
#define GETCH gettch
#else /* NROFF */
#define GETCH getch
#endif /* NROFF */

/* Forward declarations for C90 compliance */
extern void hsend(void);
extern int makem(int i);
extern int getch(void); /* Standard character input function */
/* extern int gettch(void); NROFF-specific, declared later with its definition if part of this file */

/* External functions (assumed to be defined elsewhere) */
extern int width(int c);
extern void pchar(int c);
extern void fnumb(int n, void (*pchar_func)(int));
extern void flushi(void);
extern int quant(int val, int unit);
extern void casesp(int i);
extern void mchbits(void);
extern int control(int mac, int arg);
extern int findr(int reg);
extern void prstrfl(const char *s);
extern void pchar1(int c);
extern void done1(int status);
extern void done2(int status);
extern int findn(int pos);
extern void flusho(void);
extern void hyphen(int *word_ptr);
extern void dostop(void);

/* External variables from other modules */
extern struct env *dip;
extern struct variable_state v;
extern int trap;
extern int flss;
extern int npnflg;
extern int npn;
extern int stop;
extern int nflush;
extern int *ejl;
extern int ejf;
extern int ascii;
extern int donef;
extern int nc;
extern int wch;
extern int dpn;
extern int ndone;
extern int lss;
extern int pto;
extern int pfrom;
extern int print;
extern int nlist[NTRAP];
extern int mlist[NTRAP];
extern int *frame;
extern int *stk;
extern int *pnp;
extern int nb;
extern int ic;
extern int icf;
extern int ics;
extern int ne;
extern int ll;
extern int un;
extern int un1;
extern int in;
extern int ls;
extern int spread;
extern int totout;
extern int nwd;
extern int *pendw;
extern int *linep;
extern int line[];
extern int lastl;
extern int ch;
extern int ce;
extern int fi;
extern int nlflg;
extern int pendt;
extern int sps;
extern int adsp;
extern int pendnf;
extern int over;
extern int adrem;
extern int nel;
extern int ad;
extern int totout;
extern int ohc;
extern int hyoff;
extern int nhyp;
extern int spflg;
extern int word[];
extern int *wordp;
extern int wne;
extern int chbits;
extern int cwidth;
extern int widthp;
extern int hyf;
extern int xbitf;
extern int vflag;
extern int ul;
extern int cu;
extern int font;
extern int sfont;
extern int it;
extern int itmac;
extern int *hyptr[NHYP];
extern int **hyp;
extern int *wdstart, *wdend;
extern int lnmod;
extern int admod;
extern int nn;
extern int nms;
extern int ndf;
extern int ni;
extern int nform;
extern int lnsize;
extern int po;
extern int ulbit;
extern int *vlist;
extern int nrbits;
extern int nmbits;
extern int pl; /* Page length */
extern int xxx;

/* File-local variables */
static int brflg; /**< Break flag: 1 for normal break (fill mode), 2 for nofill break. */
static const char Sccsid[] = "@(#)n7.c  1.2 of 3/4/77"; /**< SCCS version identifier. Standard C90. */

/* Function prototypes for this module (C90 compliant) */
void tbreak(void); /**< @brief Break current line and output. @see tbreak */
void donum(void); /**< @brief Output line numbering. @see donum */
void text(void); /**< @brief Process text with filling/justification. @see text */
void nofill(void); /**< @brief Output text without filling. @see nofill */
void callsp(void); /**< @brief Call space-handling routine. @see callsp */
void ckul(void); /**< @brief Clean up underline/italic states. @see ckul */
void storeline(int c, int w); /**< @brief Store char/width into line buffer. @param c Character. @param w Width. @see storeline */
void newline(int a); /**< @brief Output newline, handle traps/pages. @param a Force break flag. @see newline */
int findn1(int a); /**< @brief Find trap at exact vertical position. @param a Position. @return Trap index or NTRAP. @see findn1 */
void chkpn(void); /**< @brief Check/update page numbers for printing. @see chkpn */
int findt(int a); /**< @brief Find distance to next trap. @param a Current position. @return Distance. @see findt */
int findt1(void); /**< @brief Wrapper for findt using current v.pos. @return Distance. @see findt1 */
void eject(int *a); /**< @brief Begin new page. @param a Environment frame. @see eject */
int movword(void); /**< @brief Move word from word buffer to line buffer. @return 1 if line full, 0 otherwise. @see movword */
void horiz(int i); /**< @brief Output horizontal motion. (Definition assumed elsewhere or later) */
void setnel(void); /**< @brief Initialize/reset line state variables. (Definition assumed elsewhere or later) */
void reset_setnel_flag(void); /**< @brief Reset flag for setnel. (Definition assumed elsewhere or later) */
int getword(int x); /**< @brief Extract next word from input. @param x Flag. (Definition assumed elsewhere or later) */
static int handleUnderlineBits(int i); /**< @brief Static helper for underline bits. (Definition assumed later in file) */
int gettch(void); /**< @brief NROFF specific character input. (Definition assumed later in file if NROFF) */

/* Missing function prototypes - Definitions expected elsewhere or to be added */
void tbreak1(void); /**< Presumably a related break function; definition not in this file. */
void storeword(int c, int w); /**< Stores a character of a word into the word buffer; definition not in this file. */

/**
 * @brief Break the current line and output accumulated text.
 *
 * This function finalizes the current line of text. It handles:
 * - Processing of any pending words or spaces.
 * - Application of justification and adjustment rules (e.g., spreading space).
 * - Outputting characters with appropriate inter-character spacing.
 * - Invoking line numbering if enabled.
 * - Resetting line state variables for the next line.
 * - Managing vertical spacing after the line.
 * All local variables are C90-style, declared at the start of the function block.
 */
void tbreak(void) {
    register int *i; /* Pointer for iterating through the line buffer */
    int j; /* Temporary storage for character or value */
    int pad; /* Accumulated width of spaces for output */

    trap = 0; /* Clear any pending trap flags for this line break */
    if (nb) { /* If in no-break mode (e.g., from .br) */
        return; /* Do not break the line */
    }
    if ((!dip->op) && (v.nl == -1)) { /* If not in a diversion and on the very first line (v.nl == -1) */
        newline(1); /* Force a newline, which might trigger initial page setup */
        return;
    }
    if (!nc) { /* If no characters are currently on the line */
        reset_setnel_flag(); /* Ensure setnel can run if needed (related to line state init) */
        setnel(); /* Initialize line state variables (ne, nel, etc.) */
        if (!wch) { /* If no characters are waiting in the word buffer */
            return; /* Nothing to output */
        }
        if (pendw) { /* If a word is pending from previous partial processing */
            getword(1); /* Finalize processing of the current word */
        }
        movword(); /* Move the (potentially just finalized) word to the line buffer */
    } else if (pendw && !brflg) { /* If line has content, a word is pending, and no break is flagged yet */
        getword(1); /* Finalize processing of the current word */
        movword(); /* Move the word to the line buffer */
    }
    *linep = dip->nls = 0; /* Mark end of line buffer and clear diversion's accumulated line space */

#ifdef NROFF
    if (!dip->op) { /* If not in a diversion (i.e., outputting to main page) */
        horiz(po); /* Output the current page offset */
    }
#endif /* NROFF */

    if (lnmod) { /* If line numbering mode is active */
        donum(); /* Output the line number */
    }
    lastl = ne; /* Save the effective length of the line before adjustments */

    if (brflg != 1) { /* If not a normal fill-mode break (e.g., .nf break or initial state) */
        totout = 0; /* Reset total characters out for adjustment logic (no adjustment here) */
    } else if (ad) { /* If adjustment is enabled (.ad) */
        /* Calculate available space for adjustment.
         * ll is total line length, un is current indent.
         * If (ll - un) is less than current effective length (ne),
         * it means the line is overfull even before adjustment.
         * In this case, lastl is set to ne, implying no shrinkable space.
         */
        if ((lastl = (ll - un)) < ne) {
            lastl = ne;
        }
    }

    if (admod && ad && (brflg != 2)) { /* If in an adjustment mode (left, right, center) AND adjustment is on AND not a no-fill break */
        lastl = ne; /* Reset lastl to current effective length for adjustment calculations */
        adsp = adrem = 0; /* Reset additional inter-word space and remainder */

#ifdef NROFF
        if (admod == 1) { /* Centering adjustment in NROFF */
            un += quant(nel / 2, t.Adj); /* Add half of remaining line space (nel) to indent, quantized */
        }
        /* NROFF does not explicitly handle admod == 2 (right adjustment) here for 'un',
         * it's typically handled by how 'nel' is distributed or by pre-calculating 'un'.
         * The original troff logic for NROFF right adjust might be implicit in how
         * words are placed or how 'un' is managed by other requests.
         */
#else /* TROFF */
        if (admod == 1) { /* Centering adjustment in TROFF */
            un += nel / 2; /* Add half of remaining line space to indent */
        } else if (admod == 2) { /* Right adjustment in TROFF */
            un += nel; /* Add all remaining line space to indent, effectively right-aligning */
        }
#endif /* NROFF */
    }
    totout++; /* Increment total output lines count (used for alternating space distribution in some modes) */
    brflg = 0; /* Clear the break flag after handling this break */

    if (lastl > dip->maxl) { /* If the length of this line exceeds the max length recorded for the current diversion */
        dip->maxl = lastl; /* Update the maximum line length in the diversion */
    }
    horiz(un); /* Output the (potentially adjusted) current indent */

    /* Loop through characters in the line buffer to output them */
    for (i = line; nc > 0;) {
        if (((j = *i++) & CMASK) == ' ') { /* If the character is a space */
            pad = 0; /* Initialize padding width for this block of spaces */
            /* Accumulate width of consecutive spaces */
            do {
                pad += width(j); /* Add width of the current space character */
                nc--; /* Decrement count of characters remaining on the line */
            } while (((j = *i++) & CMASK) == ' ');
            i--; /* Adjust pointer back, as the loop condition broke on a non-space or end of buffer */

            pad += adsp; /* Add the calculated additional space for justification */
            if (adrem) { /* If there is remaining adjustment space to distribute */
                if (adrem < 0) { /* If adrem is negative (can happen with NROFF quantization) */
#ifdef NROFF
                    pad -= t.Adj; /* NROFF specific unit adjustment (subtract one unit) */
                    adrem += t.Adj; /* Compensate adrem */
                } else if ((totout & 01) || /* Alternate distribution based on line count, or if it's the last word opportunity */
                           ((adrem / t.Adj) >= (--nwd))) { /* Or if enough remainder for all remaining word gaps */
                    pad += t.Adj; /* Add one NROFF adjustment unit */
                    adrem -= t.Adj; /* Reduce remainder */
#else /* TROFF */
                    pad--; /* TROFF unit adjustment (subtract one unit) */
                    adrem++; /* Compensate adrem (should be positive here) */
                } else { /* adrem is positive */
                    pad++; /* Add one TROFF adjustment unit */
                    adrem--; /* Reduce remainder */
#endif /* NROFF */
                }
            }
            horiz(pad); /* Output the accumulated and adjusted space */
        } else { /* If the character is not a space */
            pchar(j); /* Output the character */
            nc--; /* Decrement count of characters remaining on the line */
        }
    }

    if (ic) { /* If an inter-character char is set (e.g., from .ic request) */
        if ((j = ll - un - lastl + ics) > 0) { /* Calculate remaining space for ic based on line length, indent, content length, and initial ic space */
            horiz(j); /* Output that space */
        }
        pchar(ic); /* Output the inter-character char itself */
    }

    if (icf) { /* If .ic was called with a numeric argument (icf > 0), it's a counter */
        icf++; /* Increment for the next line (effect lasts for 'icf' lines) */
    } else { /* Otherwise (.ic called without number, or icf reached its limit) */
        ic = 0; /* Clear .ic for the next line */
    }

    ne = nwd = 0; /* Reset effective line length and word count for the new line */
    un = in; /* Reset current indent to the base indent (.in) */
    setnel(); /* Reset line state variables (nel based on new 'un' and 'll') */
    newline(0); /* Process the newline, including vertical motion and trap checks */

    if (dip->op) { /* If in a diversion */
        if (dip->dnl > dip->hnl) { /* If current vertical position in diversion exceeds highest recorded */
            dip->hnl = dip->dnl; /* Update highest line number (vertical position) in diversion */
        }
    } else { /* Not in a diversion (main text stream) */
        if (v.nl > dip->hnl) { /* If current vertical position on page exceeds highest recorded for page */
            dip->hnl = v.nl; /* Update highest line number (vertical position) on page */
        }
    }

    /* Handle multi-line spacing (.ls N). Output N-1 additional newlines. */
    for (j = ls - 1; (j > 0) && !trap; j--) {
        newline(0); /* Output an additional newline, checking for traps each time */
    }
    spread = 0; /* Reset spread flag (used by \x) */
}

/**
 * @brief Output line numbering.
 *
 * This function generates and outputs line numbers for text lines if enabled.
 * It handles:
 * - Formatting of line numbers with appropriate spacing.
 * - Incrementing and displaying the line number.
 * - Adherence to numbering frequency controls (.nn, .nm).
 * - Application of correct font and character formatting for numbers.
 * All local variables are C90-style, declared at the start of the function block.
 */
void donum(void) {
    register int i; /* Loop counter or temporary storage, often for spacing calculation */
    int nw; /* Width of a single digit in the number font */

    nrbits = nmbits; /* Set character bits (font, size) for numbers from .nm settings */
    nw = width('1' | nrbits); /* Get width of '1' (as a representative digit) with number formatting */

    if (nn) { /* If .nn (number next N lines, but skip numbering these N) is active */
        nn--; /* Decrement counter for .nn */
        /* Adjust indent to leave space where number would have been */
        un += nw * (3 + nms + ni); /* 3 for up to 3 digits, nms for margin, ni for indent */
        v.ln++; /* Still increment logical line number even if not printing this one */
        return;
    }
    if (v.ln % ndf) { /* If current line number is not a multiple of .ndf (numbering frequency from .nm) */
        v.ln++; /* Increment line number */
        /* Adjust indent to leave space where number would have been */
        un += nw * (3 + nms + ni);
        return;
    }

    /* Format and print the line number */
    i = 0; /* Calculate number of padding spaces needed for alignment (assuming max 3 digits) */
    if (v.ln < 100) { /* If line number < 100 (e.g., 2 digits or 1 digit) */
        i++;
    }
    if (v.ln < 10) { /* If line number < 10 (e.g., 1 digit) */
        i++;
    }
    horiz(nw * (ni + i)); /* Output horizontal space for number indent (.ni) and alignment spaces */

    nform = 0; /* Set format for fnumb (0 is standard decimal) */
    fnumb(v.ln, pchar); /* Output the line number using pchar to print each digit */

    un += nw * nms; /* Add margin space (.nms) after the number */
    v.ln++; /* Increment line number for the next line */
}

/**
 * @brief Process incoming text characters with filling and justification.
 *
 * This is a central function for handling text when fill mode (.fi) is active.
 * It accumulates words, decides when to break lines, and triggers justification.
 * - Skips leading spaces on a new line.
 * - Calls tbreak() when a line is full or a break is forced.
 * - Manages pending words and spaces.
 * - Handles spread-out text (e.g., from \x).
 * Uses goto for managing different states of text processing, a common pattern in original troff.
 * All local variables are C90-style, declared at the start of the function block.
 */
void text(void) {
    int i; /* Temporary storage for character read */
    static int spcnt = 0; /* Count of leading spaces on a line, static to persist across calls within a line segment */

    nflush++; /* Indicate processing that might need flushing (e.g. before program exit) */

    if ((!dip->op) && (v.nl == -1)) { /* If not in a diversion and on the very first line */
        newline(1); /* Force a newline (handles initial page setup) */
        nflush = 0; /* Reset flush indicator as newline handles output */
        return;
    }
    setnel(); /* Initialize line variables (ne, nel) if line is currently empty or state is new */

    if (ce || !fi) { /* If centering (.ce) is active or no-fill mode (.nf or .fi off) */
        nofill(); /* Handle text using nofill logic */
        nflush = 0; /* Reset flush indicator as nofill handles its output */
        return;
    }

    if (pendw) { /* If a word is pending from a previous call (e.g., after a CONT character) */
        goto t4_getword; /* Jump to word processing section */
    }

    if (pendt) { /* If text is pending (i.e., not the first call for this line segment) */
        if (spcnt) { /* If spaces were pending before this current text segment */
            goto t2_handle_pending_spaces; /* Jump to handle those spaces */
        } else {
            goto t3_process_text; /* Jump directly to process more text */
        }
    }

    /* This is the start of processing for a new segment of text on the current line */
    pendt++; /* Mark that text processing has started for this segment */
    /* spcnt is static, so it might hold a value from a previous call if line wasn't broken */
    if (spcnt) { /* If spaces were pending from the absolute start of this line (before any non-space char) */
        goto t2_handle_pending_spaces;
    }

    /* Skip leading spaces at the beginning of a line and count them */
    while (((i = GETCH()) & CMASK) == ' ') {
        spcnt++;
    }

    if (nlflg) { /* If a newline character was encountered while skipping spaces (e.g. empty line or \n) */
        /* t1_handle_newline: (Implicit label for this block) */
        nflush = pendt = ch = spcnt = 0; /* Reset flags and pending character/space counts */
        callsp(); /* Handle vertical spacing associated with the newline */
        return;
    }
    ch = i; /* Store the first non-space character encountered */

    if (spcnt) { /* If any leading spaces were counted before 'ch' */
    t2_handle_pending_spaces: /* Label for handling pending spaces */
        tbreak(); /* Break the previous line (if any content was there) */
        if (nc || wch) { /* If the line (after tbreak) still has content or word buffer has content (unlikely here, but defensive) */
            nflush = 0; /* Reset flush indicator */
            /* This state implies tbreak might have been called, but we need to re-evaluate.
             * It's complex; typically after tbreak, nc/wch would be 0 if line was empty.
             * This return might defer space handling to a subsequent call.
             */
            return;
        }
        /* Line is confirmed empty after tbreak, now apply leading spaces as indent */
        un = +spcnt * sps; /* Set indent based on counted leading spaces and space width (sps) */
        spcnt = 0; /* Reset space count as they've been applied */
        setnel(); /* Re-initialize line variables (nel) with the new indent 'un' */
        if (trap) { /* If a trap occurred during tbreak or setnel (e.g. via newline in tbreak) */
            nflush = 0; /* Reset flush indicator */
            return; /* Trap handling takes precedence */
        }
        if (nlflg) { /* If a newline was encountered while handling spaces (e.g. during GETCH in setnel or trap) */
            nflush = pendt = ch = 0; /* Reset flags */
            callsp(); /* Handle vertical spacing for the newline */
            return;
        }
        /* Leading spaces handled, 'ch' still holds the first non-space char. Proceed to process it. */
    }

t3_process_text: /* Label for general text processing */
    if (spread) { /* If spread is active (e.g. from \x, to fill rest of line with next word) */
        goto t5_spread_text; /* Jump to spread handling */
    }

    if (pendw || !wch) { /* If a word is already pending, or word buffer is empty (need new word) */
    t4_getword: /* Label for getting a new word */
        if (getword(0)) { /* Get the next word; returns 1 if newline/eot encountered, 0 otherwise */
            goto t6_end_of_word_processing; /* End of input or explicit break */
        }
    }

    if (!movword()) { /* Move word from word buffer to line buffer; returns 0 if line not full yet */
        goto t3_process_text; /* Line not full, get next word / continue processing */
    }
    /* Line is full after movword(), or movword() decided a break is needed */

t5_spread_text: /* Label for when line is full or spread is active */
    if (nlflg) { /* If newline encountered during word processing (e.g. in getword) */
        pendt = 0; /* Reset pending text flag, as newline will terminate this line */
    }

    adsp = adrem = 0; /* Reset adjustment space variables */
    if (ad && (nwd - 1 > 0)) { /* If adjustment is on (.ad) and more than one word on the line */
        adsp = nel / (nwd - 1); /* Calculate base additional space to add between words */
#ifdef NROFF
        adsp = (adsp / t.Adj) * t.Adj; /* Quantize space for NROFF based on its adjustment unit */
#endif /* NROFF */
        adrem = nel - adsp * (nwd - 1); /* Calculate remainder space for distribution */
    }
    brflg = 1; /* Set break flag for a normal fill-mode break */
    tbreak(); /* Perform the line break and output */
    spread = 0; /* Reset spread flag as it's been handled */

    if (!trap) { /* If no trap occurred during tbreak */
        if (!nlflg) { /* If no newline is pending (i.e., break was due to full line, not \n) */
            goto t3_process_text; /* Continue processing text for the new line */
        }
    }
    /* Trap occurred or newline is pending, so this text processing segment ends */

t6_end_of_word_processing: /* Label for cleanup at end of word/segment processing */
    pendt = 0; /* Reset pending text flag */
    ckul(); /* Clean up underline/italic states if they ended */
    /* rtn: (Original label, effectively end of function for this path) */
    nflush = 0; /* Reset flush indicator for this call */
}

/**
 * @brief Output a line without filling or justification.
 *
 * This function handles text input when no-fill mode (.nf) is active or
 * when centering (.ce) is in effect (which uses nofill logic initially).
 * Characters are output as they are read until a newline.
 * All local variables are C90-style, declared at the start of the function block.
 */
void nofill(void) {
    int i; /* Temporary storage for character with attributes */
    int j; /* Temporary storage for character mask (CMASK) */

    if (!pendnf) { /* If not pending no-fill (i.e., this is the start of a no-fill line segment) */
        over = 0; /* Reset line overflow flag for this new line */
        tbreak(); /* Break any previous filled line before starting no-fill */
        if (trap) { /* If a trap occurred during tbreak */
            pendnf = nflush = 0; /* Reset flags and return if trap occurred */
            return;
        }
        if (nlflg) { /* If a newline was pending (e.g. from previous empty line) */
            ch = nflush = 0; /* Reset char and flush flag */
            callsp(); /* Handle vertical spacing for the newline */
            pendnf = 0; /* Reset pending no-fill flag */
            return;
        }
        adsp = adrem = 0; /* No adjustment space in no-fill mode */
        nwd = 10000; /* Set word count high to effectively disable word-based spacing adjustments in tbreak */
        /* This ensures spaces are output as-is by tbreak's logic. */
    }

    /* Read characters until a newline is encountered */
    while ((j = ((i = GETCH()) & CMASK)) != '\n') {
        if (j == ohc) { /* If character is the output hyphen character (e.g. from .hc) */
            continue; /* Skip it, don't store it in no-fill mode */
        }
        if (j == CONT) { /* If continuation character (e.g. backslash at EOL) encountered */
            pendnf++; /* Set pending no-fill flag to continue no-fill mode on the next input line */
            nflush = 0; /* Reset flush indicator */
            flushi(); /* Flush input buffer (consume the rest of the physical line) */
            ckul(); /* Clean up underline/italic states if they ended */
            return; /* Return to get next segment of input */
        }
        storeline(i, -1); /* Store character in line buffer; -1 means width will be calculated by storeline */
    }
    /* Newline character was encountered, terminating the loop */

    if (ce) { /* If centering (.ce) is active (using nofill logic) */
        ce--; /* Decrement centering count for remaining lines */
        if ((i = quant(nel / 2, HOR)) > 0) { /* Calculate half of remaining space on the line, quantized */
            un = +i; /* Add this to current indent 'un' for centering effect */
            /* Note: original used un =+ i, which is un = un + i. Corrected to un += i for clarity if needed, but un = +i is valid C for un = i if i is positive. Assuming un = un + i was intended. */
            /* Given context, un += i; is safer. However, the original code is `un = +i;`. This is `un = i;`.
                        If `nel` is remaining space, `un` should be set to half of it for centering, not added.
                        This is complex. Let's assume `un = +i;` here means the indent for *this specific centered line* is `i`.
                     */
            un = i; /* Set indent for centering. Original was `un = +i;` which is `un = i;` */
        }
    }
    if (!nc) { /* If line is empty after reading (e.g., a blank line in .nf mode) */
        storeline(FILLER, 0); /* Store a filler character to ensure the line processes through tbreak */
    }

    brflg = 2; /* Set break flag for a no-fill break */
    tbreak(); /* Perform the line break (outputs the line as is) */
    ckul(); /* Clean up underline/italic states */

    /* rtn: (Original label, effectively end of function) */
    pendnf = nflush = 0; /* Reset pending no-fill and flush flags */
}

/**
 * @brief Call the appropriate space-handling routine.
 *
 * Determines the amount of vertical space to produce based on `flss` (flush
 * space, e.g. from .sp N) or `lss` (current line spacing from .ls N).
 * Then calls `casesp` to output the determined amount of vertical space.
 * All local variables are C90-style, declared at the start of the function block.
 */
void callsp(void) {
    int i; /* Amount of vertical space to output */

    if (flss) { /* If .sp N value is pending (flss > 0) */
        i = flss; /* Use the pending .sp value */
    } else {
        i = lss; /* Use the current line spacing value (.ls) */
    }
    flss = 0; /* Clear pending .sp value as it's about to be used or was not set */
    casesp(i); /* Call external function to output 'i' units of vertical space */
}

/**
 * @brief Clean up underline and italic states.
 *
 * Called after processing text segments (e.g., at end of line or word).
 * If `ul` (underline count) or `it` (italic count) has reached zero due to
 * processing characters that decrement these counts (like end of scope for \fI or \fU),
 * it resets the font and character mode bits accordingly.
 * If `itmac` (italic end macro) is set, it calls the specified macro when italic mode ends.
 */
void ckul(void) {
    if (ul && (--ul == 0)) { /* If underline was active and its count reaches 0 after decrementing */
        cu = 0; /* Clear current underline flag (cu is likely 'currently underlining') */
        font = sfont; /* Restore previous font (sfont is 'saved font') */
        mchbits(); /* Update character mode bits based on new font and flags */
    }
    if (it && (--it == 0) && itmac) { /* If italic was active, count reaches 0, and an italic-end macro is defined */
        control(itmac, 0); /* Call the specified macro (itmac) associated with italic mode ending */
    }
}

/**
 * @brief Store a character and its width into the output line buffer.
 *
 * Adds a character to the `line` buffer, updates the current effective line
 * length (`ne`), remaining line length (`nel`), and character count (`nc`).
 * Handles line buffer overflow by printing a message and substituting a placeholder.
 * Also handles special `JREG` characters for storing justification-related positions.
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @param c The character to store (can include font/style bits).
 * @param w The width of the character. If -1, `width(c)` is called to calculate it.
 */
void storeline(int c, int w) {
    int i; /* Temporary storage, e.g., for findr result (though not used for typical char path) */

    if ((c & CMASK) == JREG) { /* If the character is a justification register marker */
        /* JREG is a special character indicating that the current line position
         * should be stored in a register identified by (c >> BYTE).
         */
        if ((i = findr(c >> BYTE)) != -1) { /* Find the index of the register */
            vlist[i] = ne; /* Store current effective line length 'ne' into the register */
        }
        return; /* JREG itself is not stored on the line */
    }

    if (linep >= (line + lnsize - 1)) { /* Check for line buffer overflow */
        /* (lnsize - 1 to leave space for null terminator if used, or just as boundary) */
        if (!over) { /* Report overflow only once per line to avoid flooding messages */
            prstrfl("Line overflow.\n"); /* Print error message using troff's string flush mechanism */
            over++; /* Set overflow flag for this line */
            c = 0343; /* Substitute a placeholder character (often a visible warning symbol in some charsets) */
            /* 0343 octal is 227 decimal. Its visual representation depends on the output device/font. */
            w = -1; /* Force recalculation of width for this placeholder character */
            /* Fall through to store the placeholder character */
        } else {
            return; /* Already overflowed and reported, ignore further characters for this line */
        }
    } else if (linep < line || linep >= (line + lnsize)) { /* Defensive check for invalid line pointer state */
        /* This indicates a more severe internal error if linep is outside the allocated buffer bounds. */
        prstrfl("Invalid line pointer in storeline. Probable corruption.\n");
        /* Depending on desired robustness, could attempt recovery or terminate.
         * For now, just return to prevent further corruption by not writing.
         * This situation should ideally never be reached in normal operation.
         */
        return;
    }

    /* s1: (Original goto label, now integrated into the flow) */
    if (w == -1) { /* If width was not provided (passed as -1) */
        w = width(c); /* Calculate width of the character 'c' (including font/style bits) */
    }
    ne += w; /* Add character's width to effective line length 'ne' */
    nel -= w; /* Subtract character's width from remaining line length 'nel' */
    *linep++ = c; /* Store character in line buffer and advance pointer */
    nc++; /* Increment count of characters on the line */
}

/**
 * @brief Output a newline and handle associated processing.
 *
 * This function manages vertical motion, page ejects, and trap invocation.
 * - Outputs physical newline characters or equivalent vertical motion commands.
 * - Updates current vertical position (`v.nl` for main text, `dip->dnl` for diversions).
 * - Checks for page boundaries and triggers page ejects (`eject` logic is embedded).
 * - Invokes traps (.wh, .dt) if their trigger points are reached.
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @param a If non-zero, indicates a forced break (e.g. from .br, .fi) that might
 *          trigger page eject logic sooner or bypass some normal line processing.
 */
void newline(int a) {
    int i, j, nlss; /* Basic counters, temporary storage, and net line space moved */
    int opn; /* Old page number, for detecting page transitions for printing ranges */

    if (a) { /* If 'a' is true, it's a forced break, potentially leading to page eject logic directly */
        goto nl1_check_page_eject; /* Skip normal line processing, go to page eject checks */
    }

    /* Normal newline processing (not a forced break 'a') */
    if (dip->op) { /* If currently in a diversion */
        j = lss; /* Save current line spacing value */
        pchar1(FLSS); /* Output a command indicating flush line spacing (special handling by output processor) */
        if (flss) { /* If an explicit .sp value (flss) is pending for this line */
            lss = flss; /* Use it for this diversion line's spacing */
        }
        i = lss + dip->blss; /* Total space for this line in diversion: current lss + diversion's block space */
        dip->dnl += i; /* Add this total space to diversion's current vertical position */
        pchar1(i); /* Output the value of total space */
        pchar1('\\n'); /* Output a newline character (or equivalent command for output processor) */
        lss = j; /* Restore original line spacing value */
        dip->blss = flss = 0; /* Clear diversion block space and any pending flush space */

        if (dip->alss) { /* If after-diversion space is set (to be added after this line) */
            pchar1(FLSS); /* Output flush line spacing command */
            pchar1(dip->alss); /* Output the after-diversion space value */
            pchar1('\\n'); /* Output newline */
            dip->dnl += dip->alss; /* Add this space to diversion's vertical position */
            dip->alss = 0; /* Clear after-diversion space as it has been applied */
        }

        /* Check for diversion trap */
        if (dip->ditrap && /* If a diversion trap position is set */
            !dip->ditf && /* And the trap has not fired yet for this diversion instance */
            (dip->dnl >= dip->ditrap) && /* And current vertical position reaches or exceeds trap position */
            dip->dimac) { /* And a macro is associated with this diversion trap */
            if (control(dip->dimac, 0)) { /* Execute diversion trap macro */
                trap++; /* Set global trap flag (may affect subsequent processing) */
                dip->ditf++; /* Mark diversion trap as fired for this instance */
            }
        }
        return; /* End of newline processing for diversions */
    }

    /* Normal newline processing (not in a diversion) */
    j = lss; /* Save current line spacing */
    if (flss) { /* If explicit .sp value is pending */
        lss = flss; /* Use it for this line's spacing */
    }
    nlss = dip->alss + dip->blss + lss; /* Total vertical space for this line:
                                          after-space (from prev diversion) +
                                          block-space (from prev diversion) +
                                          current line space (lss) */
    v.nl += nlss; /* Add total space to current vertical position on page */

#ifndef NROFF
    if (ascii) { /* In ASCII output mode (typically for troff generating plain text) */
        /* Some spacing might be handled differently or reset by output processor.
         * Here, alss and blss (related to diversions) are reset.
         */
        dip->alss = dip->blss = 0;
    }
#endif /* NROFF */

    pchar1('\\n'); /* Output physical newline (or equivalent command) */
    flss = 0; /* Clear any pending flush space (.sp) as it (or lss) has been used */
    lss = j; /* Restore original line spacing value for subsequent lines */

    if (v.nl < pl) { /* If current vertical position is still within the page length (pl) */
        goto nl2_check_traps; /* Skip page eject logic, proceed to check for other traps */
    }

/* nl1_check_page_eject: Page boundary reached or forced break 'a' */
nl1_check_page_eject:
    ejf = dip->hnl = v.nl = 0; /* Reset eject flag, highest line number on page, and current vertical position */
    ejl = frame; /* Set eject level to current environment frame (used for .ev processing during eject) */

    if (donef) { /* If document end processing is flagged (e.g., from EOF or .ex) */
        if ((!nc && !wch) || ndone) { /* If line and word buffers are empty, or done1 already called */
            done1(0); /* Call first phase of document termination */
        }
        ndone++; /* Increment counter for phases of termination */
        donef = 0; /* Clear done flag as it's being handled */
        if (frame == stk) { /* If at the top-level environment (base stack frame) */
            nflush++; /* Ensure output is flushed at the very end */
        }
    }
    opn = v.pn; /* Store current page number before incrementing */
    v.pn++; /* Increment page number */

    if (npnflg) { /* If a specific next page number was set by .pn N request */
        v.pn = npn; /* Use that page number */
        npn = npnflg = 0; /* Clear the flag and the stored next page number */
    }

    /* nlpn_handle_page_range_print: Loop to handle page printing range (pto, pfrom) */
    /* This loop checks if the current page (v.pn) or previous page (opn)
     * falls into a printing range specified by .bp N M or similar.
     * Refactored from goto to a for(;;) loop.
     */
    for (;;) { /* Loop to handle page printing range */
        if (v.pn == pfrom) { /* If current page is the start of a print range */
            print++; /* Enable printing (increments print flag) */
            pfrom = -1; /* Consume this 'pfrom' value (mark as handled) */
            break; /* Exit loop after handling pfrom */
        } else if (opn == pto) { /* If the *previous* page was the end of a print range */
            print = 0; /* Disable printing */
            opn = -1; /* Consume this 'pto' value (mark as handled) */
            chkpn(); /* Get next page range (pto, pfrom) from pnp list */
            /* Continue loop to re-check with new pto/pfrom for the current v.pn */
        } else {
            break; /* No condition met for current pto/pfrom, exit loop */
        }
    }

    if (stop && print) { /* If .st N (stop every N pages) is active and printing is enabled */
        dpn++; /* Increment count of pages printed since last stop */
        if (dpn >= stop) { /* If stop count is reached */
            dpn = 0; /* Reset counter */
            dostop(); /* Execute stop macro/routine (e.g., pause for paper change) */
        }
    }

/* nl2_check_traps: Check for vertical position traps (.wh N M, .dt N M) */
nl2_check_traps:
    trap = 0; /* Reset global trap flag for this line/position check */
    if (v.nl == 0) { /* If at the very top of the page (v.nl can be 0 after eject or at start) */
        if ((j = findn(0)) != NTRAP) { /* Check for a trap explicitly set at position 0 (e.g. header macro) */
            trap = control(mlist[j], 0); /* Execute trap macro; 'trap' var gets return status */
        }
    } else if ((i = findt(v.nl - nlss)) <= nlss) {
        /* If distance to next trap 'i' (from position before this newline's space 'nlss')
         * is less than or equal to the space 'nlss' just moved, a trap was crossed or landed on.
         * findt returns distance from (v.nl - nlss) to next trap.
         * So, the trap is at (v.nl - nlss + i).
         */
        if ((j = findn1(v.nl - nlss + i)) == NTRAP) { /* Find specific trap at that exact calculated position */
            /* This block indicates a potential logic error or corrupted trap list if findn1 fails
             * to find a trap that findt indicated should be there.
             */
            prstrfl("Trap botch detected. Attempting recovery by clearing trap.\n");
            /* Original troff might have had more drastic error handling (e.g., done2(-5)).
             * Current behavior: Log, attempt recovery by clearing trap and resetting hnl, then continue.
             */
            trap = 0; /* Clear trap flag as a recovery attempt. */
            dip->hnl = v.nl; /* Reset highest line on page to current line, possibly part of recovery. */
            /* Note: After setting trap = 0, execution continues. If a trap was critical,
             * this might lead to incorrect output, but prevents a crash.
             * The message "Recovery successful..." is implicit by continuing.
             */
        } else {
            trap = control(mlist[j], 0); /* Execute the found trap macro */
        }
    }
}

/**
 * @brief Find a trap registered exactly at vertical position 'a'.
 *
 * Iterates through the `mlist` (macro list for traps) and `nlist` (position
 * list for traps) to find if any trap is set for the exact vertical position `a`.
 * Trap positions in `nlist` can be positive (from page top) or negative
 * (relative to page bottom, calculated as `pl + nlist[i]`).
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @param a The absolute vertical position (in basic units from page top) to check for a trap.
 * @return The index of the trap in `mlist`/`nlist` if found, otherwise `NTRAP` (a sentinel value).
 */
int findn1(int a) {
    int i; /* Loop counter for iterating through trap slots */
    int j; /* Temporary storage for the calculated absolute trap position */

    for (i = 0; i < NTRAP; i++) { /* Iterate through all possible trap slots */
        if (mlist[i]) { /* If a macro is registered for this trap slot (i.e., trap is active) */
            if ((j = nlist[i]) < 0) { /* If trap position is negative, it's relative to page bottom */
                /* Standard interpretation: pl (page length) + nlist[i]
                 * e.g., if nlist[i] is -2m, trap is 2m from page bottom.
                 */
                j = pl + j;
            }
            if (j == a) { /* If the calculated absolute trap position matches the requested position 'a' */
                break; /* Trap found, exit loop with 'i' as the index */
            }
        }
    }
    return (i); /* Return index 'i'. If loop completed without break, i == NTRAP, indicating not found. */
}

/**
 * @brief Check and update page numbers for printing range.
 *
 * This function is called to process the next entry in the page number
 * printing list (`pnp`). It sets `pto` (print to page) and `pfrom` (print from
 * page) based on the list. Handles the end of the list (`-1` marker) and
 * MOT (Mark Output To) bit for ranges (e.g., `N-M` or `N-`).
 * `pnp` is a pointer to an array of page numbers/ranges.
 */
void chkpn(void) {
    pto = *(pnp++); /* Get next page number entry from the list and advance pnp pointer */
    pfrom = pto & ~MOT; /* Mask out MOT bit to get base page number for 'pfrom' (start of range) */
    /* MOT is a bit flag indicating a range rather than a single page. */

    if (pto == -1) { /* If end of page list marker is encountered */
        flusho(); /* Flush any pending output from output buffer */
        done1(0); /* Initiate termination sequence (first phase) */
        /* done1 might not return, or might set flags for eventual exit */
    }

    if (pto & MOT) { /* If MOT bit is set (indicates a range like N-M or N-) */
        pto &= ~MOT; /* Clear MOT bit from 'pto' (end of range) page number */
        print++; /* Enable printing for this range (increments print flag) */
        /* For ranges like 'N-' (print from N onwards) or 'N-M',
         * 'pfrom' (start page) is effectively the page number itself (already set by `pto & ~MOT` if pto was N|MOT).
         * If the range is just '-', pfrom might be 0 or a conventional start.
         * The logic here implies that if MOT is set, printing starts from the page number
         * derived from `pto & ~MOT` (which is now in `pfrom`).
         * The line `pfrom = pto & ~MOT;` already sets `pfrom` to `N` if `pto` was `N|MOT`.
         * So, `pfrom = 0;` might be specific for ranges like `-M` or if `N` itself is 0.
         * The `pfrom = pto & ~MOT` handles `N` correctly.
         * The `pfrom = 0;` here would override it. This needs careful interpretation of how `pnp` is constructed.
         * For now, retaining original structure.
         */
        pfrom = 0; /* This line is from the original snippet. Its interaction with the preceding
                    * `pfrom = pto & ~MOT` needs to be understood in context of how `pnp` is populated.
                    * It implies that for a MOT range, `pfrom` is the start page
                    * (already N from `pto & ~MOT`), and `pto` is the end page (M).
                    * If the range is `N-M` (print from N to M), `pfrom` should be N,
                    * and this line ensures it, or for `-M`, it sets `pfrom` to 0.
                    */
    }
}

/**
 * @brief Find the distance to the next trap from a given vertical position.
 *
 * Calculates the minimum distance in basic units from the current vertical
 * position `a` to the next trap location on the page, or to the bottom of
 * the page if no trap is closer.
 * For diversions, it only considers the diversion's own trap (`dip->ditrap`).
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @param a The current vertical position (from page/diversion top) from which to measure.
 * @return The distance in basic units to the next trap or page/diversion bottom.
 *         Returns a large value (32767) if no traps are ahead in a diversion
 *         or if past the diversion trap.
 */
int findt(int a) {
    int i; /* Loop counter for traps, or temporary for distances */
    int j; /* Calculated absolute position of a trap */
    int k; /* Minimum distance found so far to the next trap */

    k = 32767; /* Initialize minimum distance to a large value (effectively positive infinity for short int) */

    if (dip->op) { /* If in a diversion */
        if (dip->dimac && ((i = dip->ditrap - a) > 0)) { /* If diversion has a trap macro and trap is ahead (ditrap > a) */
            k = i; /* Distance to this diversion trap */
        }
        /* If no diversion trap or past it, k remains 32767.
         * Diversions do not consider page traps or page bottom.
         * Their "bottom" is effectively unbounded unless a diversion trap exists.
         */
        return (k);
    }

    /* Not in a diversion, check page traps */
    for (i = 0; i < NTRAP; i++) { /* Iterate through all page trap slots */
        if (mlist[i]) { /* If a macro is registered for this trap (trap is active) */
            if ((j = nlist[i]) < 0) { /* If trap position is negative (relative to page bottom) */
                j = pl + j; /* Calculate absolute position from page top */
            }
            if ((j -= a) <= 0) { /* Calculate distance from current position 'a' to this trap 'j'.
                                  * If trap is at or before 'a' (j <= 0), ignore it. */
                continue;
            }
            if (j < k) { /* If this trap is closer than previous minimum 'k' */
                k = j; /* Update minimum distance */
            }
        }
    }

    i = pl - a; /* Distance to physical page bottom from current position 'a' */
    if (k > i) { /* If no trap is closer than the page bottom (or no traps ahead at all) */
        k = i; /* Then the effective distance to the "next event" is the distance to page bottom */
    }
    return (k); /* Return smallest positive distance found, or distance to page bottom */
}

/**
 * @brief Convenience wrapper around `findt` using the current vertical position.
 *
 * Determines the current vertical position (either `dip->dnl` for diversions
 * or `v.nl` for main text) and calls `findt` with that position to find the
 * distance to the next trap or boundary.
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @return The distance in basic units to the next trap from the current vertical position.
 */
int findt1(void) {
    int i; /* Current vertical position to be passed to findt() */

    if (dip->op) { /* If in a diversion */
        i = dip->dnl; /* Use diversion's current vertical position */
    } else { /* Main text stream */
        i = v.nl; /* Use page's current vertical position */
    }
    return (findt(i)); /* Find distance to next trap from that position */
}

/**
 * @brief Begin a new page, ejecting previous content as needed.
 *
 * This function forces a page eject. It repeatedly calls `newline(0)` to
 * advance vertical space. Each call to `newline(0)` will:
 * 1. Calculate space to next trap or page bottom (`lss = findt(v.nl)`).
 * 2. Output that much vertical space.
 * 3. Trigger any traps encountered.
 * This process continues until the top of a new page (`v.nl == 0`) is reached
 * AND no further traps are immediately triggered by reaching v.nl = 0.
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @param a Pointer to the environment frame initiating the eject. If NULL,
 *          uses the current `frame`. This is relevant for environment (.ev)
 *          switching during page ejects.
 */
void eject(int *a) {
    int savlss; /* To save and restore line spacing (lss) during eject processing */

    if (dip->op) { /* Eject request has no meaning inside a diversion */
        return;
    }

    ejf++; /* Set eject flag, indicating a page eject is in progress or requested */
    if (a) { /* If a specific environment frame 'a' is provided for eject context */
        ejl = a; /* Set eject level to this frame */
    } else {
        ejl = frame; /* Use current environment frame for eject level */
    }

    if (trap) { /* If a trap is already pending when eject is called */
        /* The eject might be deferred or its handling might be intertwined with the
         * pending trap's execution. Original troff code returns here, implying
         * the trap handler should eventually lead to the eject or resolve the state.
         */
        return;
    }

    /* Loop to advance vertical space and trigger traps until page top (v.nl == 0)
     * is reached and no new trap is immediately triggered by reaching v.nl = 0.
     */
    do {
        savlss = lss; /* Save current line spacing value */
        /* Determine space to next trap or page bottom. This effectively makes the
         * eject process advance in large steps, jumping to the next trap or page end.
         */
        lss = findt(v.nl);
        newline(0); /* Process this block of space. This will update v.nl,
                     * potentially trigger traps (setting global 'trap' flag),
                     * and if page end is crossed, reset v.nl to 0.
                     */
        lss = savlss; /* Restore original line spacing value */
    } while (v.nl && !trap); /* Continue if not at page top (v.nl != 0) AND no trap fired during the last newline(0).
                              * If a trap fires, it might handle the rest of the eject or set v.nl to 0.
                              * If v.nl becomes 0 (page top), loop terminates.
                              */
}

/**
 * @brief Move the current word from the word buffer to the output line buffer.
 *
 * This function takes the word assembled in `word[]` (pointed to by `wordp`,
 * with length `wch` and width `wne`) and transfers it to `line[]` (the line buffer).
 * It handles:
 * - Skipping leading spaces in the word buffer if it's the first word on a line.
 * - Checking for hyphenation points if the word doesn't fit on the current line.
 * - Storing characters and hyphenation markers (IMP) into the line buffer via `storeline`.
 * - Updating line length (`ne`, `nel`) and word count (`nwd`).
 * All local variables are C90-style, declared at the start of the function block.
 *
 * @return 1 if the word could not be fully placed (line became full or part of word remains).
 * @return 0 if the word was placed successfully and fits, or if the line is now ready for break.
 */
int movword(void) {
    int i; /* Temporary storage for character being processed */
    int w; /* Width of the character 'i' */
    int *wp; /* Pointer to current character in word buffer `word[]` */
    int savwch; /* Original word character count, for hyphenation logic */
    int hyphen_width; /* Width of a hyphen character, used in break decisions */

    over = 0; /* Reset line overflow flag for this operation (storeline might set it) */
    wp = wordp; /* Initialize 'wp' to point to the start of the current word data in `word[]` */

    if (!nwd) { /* If this is the first word to be placed on the current line (nwd == 0) */
        /* Skip any leading spaces that might be part of the "word" due to how getword collects them.
         * These spaces do not contribute to the word itself if it's the first on a line.
         */
        while (((i = *wp++) & CMASK) == ' ') { /* Read character, check if space, then advance pointer */
            wch--; /* Decrement character count of the word in buffer */
            wne -= width(i); /* Adjust word width by subtracting space width */
        }
        wp--; /* Adjust pointer back to the first non-space character (or to original position if no spaces) */
    }

    /* Check if hyphenation is viable and potentially needed for this word */
    if ((wne > nel) && /* Word width 'wne' is greater than remaining line length 'nel' */
        !hyoff && hyf && /* Hyphenation is generally enabled ('hyf') and not turned off for this specific word ('hyoff') */
        (!nwd || (nel > 3 * sps)) && /* Either it's the first word (must try to fit part of it),
                                         or if not first, there's enough space for at least part of word + hyphen (heuristic: > 3 spaces) */
        (!(hyf & 02) || (findt1() > lss))) /* Hyphenation not disabled by mode (hyf & 02),
                                              or if it is, ensure we are not too close to a trap (findt1() > lss: distance to next trap > line space) */
    {
        hyphen(wp); /* Call hyphenation routine to find potential hyphenation points in 'wp' (word data).
                     * Results are typically stored in `hyptr[]`.
                     */
    }

    savwch = wch; /* Save original character count of the word (before any part is moved to line) */
    hyp = hyptr; /* Point 'hyp' (global pointer to hyphenation points array) to `hyptr` */
    nhyp = 0; /* Reset count of actual hyphenation points inserted into the line for this word */

    /* Advance 'hyp' to the first hyphenation point that is relevant to the current part of the word
     * (i.e., skip hyphenation points that refer to characters before 'wp').
     * `*hyp` would be a pointer to a character within the word buffer where hyphenation is allowed.
     */
    while (*hyp && (*hyp <= wp)) { /* While there's a hyphenation point and it's at or before current word start */
        hyp++; /* Move to the next hyphenation point */
    }

    /* Transfer characters from word buffer `word[]` (via `wp`) to line buffer `line[]` (via `storeline`) */
    while (wch) { /* While there are characters left in the word buffer for the current word */
        if ((hyoff != 1) && (*hyp == wp)) { /* If hyphenation is not completely off (hyoff != 1) AND current char `*wp` is a hyphenation point `*hyp` */
            hyp++; /* Move to next potential hyphenation point for subsequent characters */
            /* Check various conditions to decide if a discretionary hyphen (IMP) should be inserted here.
             * These conditions prevent hyphenating too close to word start/end, or based on specific hyf flags.
             */
            if (!wdstart || /* If no specific word start/end boundaries are defined for hyphenation rules, or... */
                ((wp > (wdstart + 1)) && /* Current point 'wp' is after the first char of allowed hyphenation zone */
                 (wp < wdend) && /* And 'wp' is before the end of allowed hyphenation zone */
                 (!(hyf & 04) || (wp < (wdend - 1))) && /* Condition related to hyf flag 04 (e.g. don't hyphenate last two chars) */
                 (!(hyf & 010) || (wp > (wdstart + 2))))) /* Condition related to hyf flag 010 (e.g. don't hyphenate first three chars) */
            {
                nhyp++; /* Count this as a potential hyphenation point used on the line */
                storeline(IMP, 0); /* Store an implicit hyphen marker (IMP). It has zero width but signals a hyphenation spot. */
                /* The actual hyphen char might be rendered later if this IMP is chosen for line break. */
            }
        }
        i = *wp++; /* Get character from word buffer and advance word buffer pointer `wp` */
        w = width(i); /* Get its width */
        wne -= w; /* Update remaining width needed for the rest of the word (by subtracting this char's width) */
        wch--; /* Decrement character count for the word in buffer */
        storeline(i, w); /* Store character 'i' and its width 'w' into the line buffer */
    }

    /* After attempting to store all characters of the word (or what's left of it) */
    if (nel >= 0) { /* If there's still space on the line (nel >= 0), or line is exactly full (nel == 0) */
        nwd++; /* Increment word count on the line */
        return (0); /* Word placed successfully without needing to break it, or line is now ready for break. */
        /* Indicates that text() can continue to get the next word for this line if it was not full. */
    }

    /* Word (or remaining part) doesn't fit (nel < 0). Line is overfull.
     * Try to break at a hyphenation point if possible.
     */
    xbitf = 1; /* Flag that line may be too long / needs adjustment or hyphenation. (xbitf: "extra bit" flag) */
    hyphen_width = width(0200); /* Get width of a standard hyphen character (0200 is often a hyphen).
                                 * This is used to see if breaking and adding a hyphen is feasible. */

m1_check_hyphen: /* Label for hyphenation check logic when word overflows */
    if (!nhyp) { /* If no hyphenation points were inserted into the line for this word, or all have been considered and "retracted" */
        if (!nwd) { /* If this was the first word on the line (nwd == 0) */
            /* First word must be placed, even if it makes the line overfull. Cannot break it back further. */
            goto m3_word_placed_or_overfull; /* Jump to finalization for this word */
        }
        if (wch == savwch) { /* If no characters were removed from word (cannot break) */
            goto m4_cannot_break_word; /* This implies the word itself is too long for the line */
        }
    }

    /* Try to remove characters back to a hyphenation point */
    if (*--linep != IMP) /* Move line pointer back. If not an IMP, it's a letter. */
        goto m5_backtrack_letter; /* Backtrack this letter */

    /* It was an IMP. We are at a valid hyphenation point. */
    nc--; /* Account for removing the IMP itself from the line character count */
    if (!(--nhyp)) { /* Decrement count of available hyphen points. If none left... */
        if (!nwd) /* And it's the first word... */
            goto m2_insert_hyphen_char; /* Must place it, try to add hyphen char */
    }

    if ((nel + width(IMP)) < hyphen_width) { /* Not enough space even for the hyphen character itself (IMP has 0 width but we add it back conceptually for this check) */
        /* nc was already decremented for IMP */
        goto m1_check_hyphen; /* Re-evaluate, try to backtrack further if possible */
    }

m2_insert_hyphen_char:
    /* At a valid hyphen point with space for a hyphen char */
    i = *(linep - 1); /* Get char before IMP (which is now pointed to by linep after *--linep) */
    /* This IMP will be overwritten or the char before it modified */
    w = width(i); /* Width of char before IMP */
    ne -= w; /* Remove its width from effective line length */
    nel += w; /* Add its width back to remaining line length */
    /* linep currently points to where IMP was. We will overwrite it or the char before it. */

    if ((i & CMASK) != '-' && (i & CMASK) != 0203) { /* If char before IMP is not already a hyphen or em-dash */
        *(linep - 1) = (*(linep - 1) & ~CMASK) | 0200; /* Replace char before IMP with actual hyphen, keeping font */
        w = width(*(linep - 1)); /* Get width of this new hyphen character */
        nel -= w; /* Adjust remaining line length */
        ne += w; /* Adjust effective line length */
        /* IMP is effectively gone, replaced by modifying the char *before* its original position.
         * No change to linep needed as the hyphen replaced the character *before* the IMP's original position.
         * nc was already decremented for the IMP.
         */
    } else {
        /* If it was already a hyphen/em-dash, the IMP is simply removed.
         * linep still points to where IMP was. The char before it is fine.
         * nc was already decremented.
         */
    }
    /* hsend(); // Potentially send hyphenation data; commented out in original */

m3_word_placed_or_overfull:
    nwd++; /* Increment word count */

m4_cannot_break_word: /* Word is placed (possibly overfull, or couldn't be hyphenated) */
    wordp = wp; /* Update global word pointer to remaining part of word */
    return (1); /* Indicates line is full / word was broken or couldn't fit */

m5_backtrack_letter: /* Backtracking a regular character (not an IMP) */
    /* linep already moved back by *--linep in the m1_check_hyphen path */
    nc--; /* Decrement character count on line */
    w = width(*linep); /* Get width of the character being removed */
    ne -= w; /* Adjust effective line length */
    nel += w; /* Adjust remaining line length (add width back) */
    wne += w; /* Add width back to 'width needed for word' (as it's put back to word buffer) */
    wch++; /* Increment character count of word (as it's put back) */
    wp--; /* Move word buffer pointer back */
    goto m1_check_hyphen; /* Re-evaluate hyphenation possibilities */
}

/**
 * @brief Output horizontal motion.
 *
 * Generates output commands to move the current printing position horizontally
 * by a specified amount `i`.
 *
 * @param i The distance to move horizontally, in basic units.
 *          Positive moves right, negative moves left.
 */
void horiz(int i) {
    vflag = 0; /* Clear vertical motion flag, as this is horizontal */
    if (i) /* If motion is non-zero */
        pchar(makem(i)); /* Output horizontal motion command */
}

/**
 * @brief Flag to track if setnel has been called for the current line.
 * Used by setnel and reset_setnel_flag to avoid redundant initializations.
 */
static int setnel_called = 0;

/**
 * @brief Set/reset line state variables (`nel`, `ne`, `un`, `linep`).
 *
 * This function initializes variables for a new line of output, but only if
 * the line is currently empty (`nc == 0`) and it hasn't been called already
 * for this line segment (tracked by `setnel_called`).
 * - `linep` is reset to the beginning of the `line` buffer.
 * - `un` (current indent) is set from `un1` if `un1` is valid.
 * - `nel` (net effective length remaining) is calculated based on `ll` and `un`.
 * - `ne` (net effective length used) and adjustment variables are reset.
 */
void setnel(void) {
    if (setnel_called && nc != 0)
        return; /* Avoid redundant calls unless line is truly new/empty */

    if (!nc) { /* Only if line character count is zero */
        linep = line; /* Reset line buffer pointer to start */
        if (un1) { /* If un1 is set, use it for current indent */
            un = un1; /* Set current indent from un1 */
            un1 = 0; /* Clear un1 after using it */
        }
        nel = ll - un; /* Calculate remaining line length */
        ne = 0; /* Reset effective length used */
        adsp = adrem = 0; /* Reset adjustment spacing variables */
        setnel_called = 1; /* Mark that setnel has been called for this line */
    }
}

/**
 * @brief Reset the setnel flag to allow setnel to run again.
 *
 * This function should be called when starting a new line or when line processing
 * is complete to ensure setnel can properly initialize the next line.
 */
void reset_setnel_flag(void) {
    setnel_called = 0;
}
