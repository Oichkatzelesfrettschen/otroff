#include "../cxx17_scaffold.hpp"
/*
 * n10.c - Device interface for nroff/troff terminal output
 *
 * This file implements the low-level device interface for nroff/troff,
 * handling terminal or printer output, character rendering, cursor movement,
 * special effects (bold, underline), and basic plotting.
 *
 * Main Functions:
 * ---------------
 * void ptinit(void)
 *      Initialize the terminal/printer device, read terminal capabilities,
 *      set up device parameters, fonts, and terminal modes.
 *
 * void twdone(void)
 *      Restore terminal settings and clean up before exiting.
 *
 * void ptout(int i)
 *      Process a character or motion command, buffer it for output, and
 *      flush the line buffer on newline.
 *
 * void ptlead(void)
 *      Perform any pending vertical motion (wrapper for move()).
 *
 * void dostop(void)
 *      Halt processing and wait for a character from input (fd 2, usually stderr).
 *
 * Static Helper Functions:
 * ------------------------
 * static void ptout1(void)
 *      Output the contents of the oline buffer to the terminal, handling
 *      character rendering, font changes, bolding, underlining, and plotting.
 *
 * static char *plot(char *x)
 *      Process a plotting sequence from a character's definition, outputting
 *      elementary plotting commands (up, down, left, right).
 *
 * static void move(void)
 *      Perform accumulated horizontal and vertical motion, outputting the
 *      appropriate terminal control sequences.
 *
 * static void oputs(const char *s)
 *      Output a null-terminated string using oput for each character.
 *
 * Global Variables:
 * -----------------
 * int dtab         - Default tab stop distance.
 * int bdmode       - Bold mode status.
 * int plotmode     - Plot mode status.
 *
 * External Variables (defined elsewhere):
 * ---------------------------------------
 * int lss, xfont, esc, lead, ulfont, esct, sps, ics, ttysave, ttys[3], ptid, waitf, pipeflg, eqflg, hflg, tabtab[], xxx;
 * char obuf[], *obufp, termtab[];
 * struct env *dip;
 * int oline[], *olinep;
 *
 * Device Table Structure:
 * ----------------------
 * struct t (referenced as 't') contains terminal capability strings and device parameters,
 * such as initialization/restoration sequences, character width, plotting commands, etc.
 *
 * Notable Implementation Details:
 * -------------------------------
 * - Terminal capabilities are read from an external file (termtab), and pointers in the
 *   device table structure are relocated to point into the loaded string area.
 * - Character output is buffered per line, with motion and font changes handled inline.
 * - Special handling for bold and underline fonts, including device-specific sequences.
 * - Plotting sequences are interpreted and output as device commands for graphical characters.
 * - Horizontal motion is optimized using tabs if enabled.
 * - Terminal settings are manipulated directly using stty/gtty.
 *
 * SCCS Version:
 * -------------
 * static char Sccsid[] = "@(#)n10.c  1.3 of 4/26/77";
 */
#define NROFF 1
#include "tdef.hpp" /* Main definitions, likely includes t.hpp */
#include "env.hpp" /* Environment structure definition */
/* #include "t.hpp" -- Likely included by tdef.hpp */
#include "tw.hpp" /* Terminal/writer specific definitions */
#include <cstdint> // For uintptr_t
#include <cstdlib> /* For exit(), NULL */
#include <unistd.h> /* For read(), close(), open(), lseek(), sbrk() if setbrk is a wrapper */
#include <fcntl.h> /* For open() flags */
#include <sys/wait.h> /* For wait() */
#include "proto.hpp" /* Function prototypes for this project */
#include "troff_processor.hpp" // processor state

/* Explicit declaration to avoid implicit function warning */
void flusho(void);

/* Add missing function prototypes to avoid implicit declaration errors */
void prstr(const char *s);
int stty(int fd, int *ttys);
void oput(int c);

/*
 * n10.c
 *
 * Device interface for nroff/troff.
 * This file handles the low-level output to a terminal or printer device,
 * including character printing, cursor movement, and special effects like
 * bolding, underlining, and basic plotting.
 */

/* External global variables, defined elsewhere */
extern int lss; /* Current line spacing */
extern TroffProcessor g_processor; /* Shared processor state */
extern int xfont; /* Current font selection */
extern int esc; /* Horizontal escapement */
extern int lead; /* Vertical leading */
extern struct env *dip; /* Pointer to current environment block */
extern int oline[]; /* Buffer for the current output line */
extern int *olinep; /* Pointer to current position in oline */
extern int ulfont; /* Underline font code */
extern int esct; /* Accumulated horizontal escapement for the current character */
extern int sps; /* Size of a space character */
extern int ics; /* Inter-character space */
extern int ttysave; /* Saved terminal settings */
extern int ttys[3]; /* Current terminal settings (for stty/gtty) */
extern char termtab[]; /* Path to terminal capability table file */
extern int ptid; /* Pipe file descriptor for output */
extern int waitf; /* Wait status for piped process */
extern int pipeflg; /* Flag: true if output is piped */
extern int eqflg; /* Flag: true if equation mode is active (adjusts horizontal resolution) */
extern int hflg; /* Flag: true if horizontal motion optimization (tabs) is enabled */
extern int tabtab[]; /* Array of tab stop positions */
extern int xxx; /* Unused? (common in old troff code for debugging) */

/* Global variables defined in this file */
int dtab; /* Default tab stop distance */
int bdmode; /* Bold mode status (0 = off, >0 = on) */
int plotmode; /* Plot mode status (0 = off, >0 = on) */

/* SCCS version control identifier */
static char Sccsid[] = "@(#)n10.c  1.3 of 4/26/77";

/* Forward declarations for local static helper functions */
static void ptout1(void);
static char *plot(char *x);
static void move(void);
static void oputs(const char *s);

/*
 * ptinit
 * Initializes the terminal/printer device.
 * Reads terminal capabilities from the file specified by 'termtab'.
 * Sets up initial device parameters, fonts, and terminal modes.
 */
void ptinit(void) {
    int i; /* File descriptor or general integer */
    char **p; /* Pointer to iterate through t structure members */
    char *q; /* Pointer to allocated memory for terminal strings */
    int qsize; /* Size of the t structure's string area */
    char *x[8]; /* Temporary buffer, usage is unclear/possibly vestigial */
    int offset; /* Offset for relocating pointers in t structure */

    if ((i = open(termtab, O_RDONLY)) < 0) { /* O_RDONLY is 0 on many systems */
        prstr("Cannot open ");
        prstr(termtab);
        prstr("\n");
        exit(-1);
    }

    /* This read is unusual: it reads 16 bytes into an array of 8 char pointers.
     * Its specific purpose or effect on 'x' is unclear from this context,
     * and 'x' is only minimally used afterwards. May be vestigial or system-dependent.
     */
    read(i, x, 16);

    /* Calculate the size of the part of 't' structure containing char* members
     * that point to strings (terminal control sequences).
     * The multiplication by 2 might be a historical artifact or an error,
     * as pointer differences usually give element counts.
     * However, if &t.zzz and &t.bset are char** and point to the start/end
     * of an array of char* members, this calculates the byte size of that array.
     */
    qsize = reinterpret_cast<char *>(&t.zzz) - reinterpret_cast<char *>(&t.bset); /* Size of the pointer region in t */

    /* The original code had `qsize = 2 * (&t.zzz - &t.bset);`.
     * If t.zzz and t.bset are of type `char **`, then `&t.zzz - &t.bset` would
     * be the number of `char *` elements. Multiplying by 2 seems incorrect
     * if the intent was byte size unless `sizeof(char *)` was assumed to be 1
     * and it was actually number of words, or some other historical reason.
     * Assuming `(char*)&t.zzz - (char*)&t.bset` is the byte size of the region.
     * If the original calculation was indeed correct for its target, this might need review.
     * For now, using byte difference for clarity.
     */

    /* The following line `x[2] = -qsize;` is also unusual, assigning to part of
     * the 'x' array that was just read into. Its purpose is not clear.
     */
    /* x[2] = (char *)(long)-qsize; */ /* Retaining original logic if x[2] was a char* */
    /* However, the original was `x[2] = -qsize;` which implies x[2] was treated as an int or compatible type.
                                        This is highly platform-dependent and likely problematic.
                                        Given the limited context, this line is commented out as its original intent and type safety are questionable.
                                        If `x` is truly `char *x[8]`, then `x[2]` is a `char*`. Assigning an integer to it directly is a type error.
                                        If it was `int x[8]`, it would be different.
                                        The original code might have relied on specific compiler behavior or pointer/integer size equivalence.
                                     */

    /* Allocate memory for terminal control strings. setbrk is likely sbrk or similar. */
    if ((q = setbrk(qsize)) == reinterpret_cast<char *>(-1L)) { /* Check for allocation failure */
        prstr("Cannot allocate memory for termtab strings\n");
        close(i);
        exit(-1);
    }

    /* Read terminal control strings into the allocated buffer 'q'.
     * t.twinit is likely an offset within the termtab file.
     * 020 is an octal constant (16 decimal).
     */
    if (lseek(i, (long)t.twinit + 020, SEEK_SET) == -1) {
        prstr("Seek error in termtab\n");
        close(i);
        exit(-1);
    }
    if (read(i, q, qsize) != qsize) {
        prstr("Read error from termtab\n");
        close(i);
        exit(-1);
    }
    close(i); /* Close the termtab file */

    /* Relocate pointers in the t structure.
     * Members from t.twinit to t.zzz are assumed to be char* offsets
     * relative to the start of the string area in the file.
     * They are adjusted to be absolute pointers into the buffer 'q'.
     */
    offset = q - t.twinit; /* Calculate base offset for relocation, assuming t.twinit is char* */
    for (p = (char **)&t.twinit; p < (char **)&t.zzz; p++) {
        if (*p) { /* If the member (offset) is non-zero */
            *p = reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(*p) + offset); /* Adjust to be an absolute pointer */
        } else {
            /* If original offset was 0, point it to a safe default (e.g., end of t strings or an empty string)
             * Original code pointed to &t.zzz, which is unusual. A pointer to an empty string "" might be safer.
             * For now, preserving original logic: make it point to the address of t.zzz itself.
             * This is likely meant to be a sentinel or an effectively null string pointer.
             */
            *p = reinterpret_cast<char *>(&t.zzz);
        }
    }

    sps = EM; /* Default space size is 1 em */
    ics = EM * 2; /* Default inter-character space is 2 ems */
    dtab = 8 * t.Em; /* Default tab width is 8 ems */

    /* Initialize tab stops */
    for (i = 0; i < 16; i++) {
        tabtab[i] = dtab * (i + 1);
    }

    /* Set terminal modes if specified by t.bset or t.breset */
    if (t.bset || t.breset) {
        ttys[2] &= ~t.breset; /* Clear bits specified by t.breset */
        ttys[2] |= t.bset; /* Set bits specified by t.bset */
        stty(1, ttys); /* Apply new terminal settings to stdout */
    }

    oputs(t.twinit); /* Output terminal initialization string */

    /* If equation mode, adjust horizontal resolution */
    if (eqflg) {
        t.Adj = t.Hor; /* Set character adjustment unit to horizontal motion unit */
    }
}

/*
 * twdone
 * Restores terminal settings and cleans up before exiting.
 */
void twdone(void) {
    g_processor.outputPtr = g_processor.outputBuffer.data(); /* Reset buffer pointer */
    oputs(t.twrest); /* Output terminal restoration string */
    flusho(); /* Flush any remaining output */

    /* If output was piped, close pipe and wait for child process */
    if (pipeflg) {
        close(ptid); /* Close pipe file descriptor */
        wait(&waitf); /* Wait for piped process to terminate */
    }

    ttys[2] = ttysave; /* Restore original terminal settings */
    stty(1, ttys); /* Apply restored settings to stdout */
}

/*
 * ptout
 * Processes a character or motion command and adds it to the output line buffer.
 * If a newline character is encountered, the buffered line is flushed.
 * 'i' contains the character or command, potentially with motion/font bits.
 */
void ptout(int i) {
    *olinep++ = i; /* Add item to the output line buffer */

    /* Prevent buffer overflow if line is too long */
    if (olinep >= &oline[LNSIZE]) {
        olinep--; /* Step back to prevent overflow */
    }

    /* If not a newline, just buffer it and return */
    if ((i & CMASK) != '\n') {
        return;
    }

    /* Process newline: flush the current line */
    olinep--; /* Remove the newline character itself from buffer, it's handled by t.twnl */

    /* Calculate vertical lead before printing the line */
    lead = dip->blss + lss - t.Newline; /* dip->blss: space before, lss: line space, t.Newline: device newline */
    dip->blss = 0; /* Reset space before */
    esct = esc = 0; /* Reset horizontal escapements */

    if (olinep > oline) { /* If there's content in the line buffer */
        move(); /* Perform any pending horizontal/vertical motion */
        ptout1(); /* Output the characters in the line buffer */
        oputs(t.twnl); /* Output device's newline sequence */
    } else { /* Empty line, just perform vertical motion */
        lead = t.Newline; /* Set lead to device's standard newline motion */
        move(); /* Perform the vertical motion */
    }
    lead = dip->alss; /* Set lead for space after the line */
    dip->alss = 0; /* Reset space after */
    olinep = oline; /* Reset line buffer pointer for the next line */
}

/*
 * oputs
 * Outputs a null-terminated string using oput for each character.
 * Assumes oput(char) is available and handles actual output.
 */
static void oputs(const char *s) {
    if (s == nullptr) { /* Defensive check for null pointer */
        return;
    }
    while (*s) {
        oput(*s++); /* oput() is assumed to be declared elsewhere (e.g., in proto.h) */
    }
}

/*
 * ptout1
 * Outputs the content of the oline buffer to the terminal.
 * Outputs the content of the oline buffer to the terminal.
 * Handles character rendering, font changes, bolding, underlining, and plotting.
 */
static void ptout1(void) {
    int i, k; /* i: current item from oline, k: loop counter/temporary */
    char *codep; /* Pointer to character code sequence in t.codetab */
    int *q; /* Pointer to iterate through oline */
    int w; /* Width of the character in device units */
    int j; /* Temporary for motion value */
    int phyw; /* Physical width of the character (for zero-width chars) */

    for (q = oline; q < olinep; q++) { /* Iterate over items in the line buffer */
        i = *q; /* Get current character/command */

        if (i & MOT) { /* If it's a motion command */
            j = i & ~MOTV; /* Extract motion magnitude */
            if (i & NMOT) { /* If negative motion */
                j = -j;
            }
            if (i & VMOT) { /* If vertical motion */
                lead += j; /* Accumulate vertical motion */
            } else { /* Horizontal motion */
                esc += j; /* Accumulate horizontal motion */
            }
            continue; /* Next item in oline */
        }

        k = (i & CMASK); /* Extract character code (mask out font/other bits) */

        /* Handle special characters (ASCII control codes or space) */
        if (k <= 040) { /* 040 is octal for space */
            switch (k) {
            case ' ': /* Space character */
                esc += t.Char; /* Advance by standard character width (or space width) */
                break;
                /* Other control characters could be handled here if needed */
            }
            continue; /* Next item in oline */
        }

        /* Regular printable character */
        codep = t.codetab[k - 32]; /* Get pointer to char's output sequence from table */
        /* k-32 assumes ASCII printable chars start at 32 */

        w = t.Char * (*codep++ & 0177); /* Get char width; first byte of sequence is width multiplier. */
        /* 0177 (octal) masks to 7 bits. */
        phyw = w; /* Store physical width */

        if (i & ZBIT) { /* If zero-width character flag is set */
            w = 0; /* Logical width is zero, but physical width (phyw) is used for underlining */
        }

        /* If there's pending motion or this char has a multi-byte sequence, perform motion first */
        if (*codep && (esc || lead)) {
            move();
        }

        esct += w; /* Accumulate character width to total horizontal escapement for the line */

        /* Handle font changes if any bits are set in the upper part of 'i' */
        if (i & 074000) { /* Octal mask for font bits */
            xfont = (i >> 9) & 03; /* Extract font code (0, 1, 2, or 3) */
        }

        /* Handle bold mode */
        if (*t.bdon & 0377) { /* Check if bold-on sequence exists (first char not null) */
            if (!bdmode && (xfont == 2)) { /* If not in bold mode and font is bold (font 2) */
                oputs(t.bdon); /* Output bold-on sequence */
                bdmode++; /* Set bold mode flag */
            }
            if (bdmode && (xfont != 2)) { /* If in bold mode and font is not bold */
                oputs(t.bdoff); /* Output bold-off sequence */
                bdmode = 0; /* Clear bold mode flag */
            }
        }

        /* Handle underlining */
        if (xfont == ulfont) { /* If current font is the underline font */
            for (k = phyw / t.Char; k > 0; k--) { /* Output '_' for width of char */
                oput('_');
            }
            for (k = phyw / t.Char; k > 0; k--) { /* Backspace to overwrite with char */
                oput('\b');
            }
        }

        /* Output character's control sequence */
        while (*codep != 0) { /* Loop through bytes of the character's sequence */
            if (*codep & 0200) { /* If high bit (0200 octal) is set, it's a plot sequence */
                codep = plot(codep); /* Process plot sequence */
                /* plot() returns pointer after the sequence it consumed */
                /* Assuming plot mode should be turned off after plotting sequence for a char */
                if (plotmode) { /* If plot mode was turned on by plot() */
                    oputs(t.plotoff); /* Turn off plot mode */
                    plotmode = 0;
                }
                oput(' '); /* Output a space to account for plotter movement (device dependent) */
            } else { /* Regular character byte in sequence */
                if (plotmode) { /* If plot mode is on (e.g. from previous char part) */
                    oputs(t.plotoff); /* Turn it off before printing normal char part */
                    plotmode = 0;
                }
                oput(*codep++); /* Output the byte */
            }
        }

        /* For zero-width characters, output backspaces to cover physical width */
        if (!w) { /* If logical width was zero */
            for (k = phyw / t.Char; k > 0; k--) {
                oput('\b');
            }
        }
    }
}

/*
 * plot
 * Processes a plotting sequence from a character's definition.
 * Turns on plot mode and outputs elementary plotting commands (up, down, left, right).
 * 'x' points to the start of the plot sequence (byte with 0200 bit set).
 * Returns a pointer to the character after the consumed plot sequence.
 */
static char *plot(char *x) {
    int i; /* Number of steps for a plot command */
    char *j; /* Pointer to terminal command string (e.g., t.up) */
    char *k; /* Current position in the plot sequence string */

    if (!plotmode) {
        oputs(t.ploton); /* Turn on plot mode if not already on */
        plotmode = 1;
    }

    k = x;
    if ((*k & 0377) == 0200) { /* If current byte is the initial plot marker (0200 octal) */
        k++; /* Advance to the actual plot commands */
    }

    for (; *k; k++) { /* Loop through plot command bytes */
        if (*k & 0200) { /* If high bit set, it's a direction/count command */
            if (*k & 0100) { /* Bit 0100 octal distinguishes vertical/horizontal */
                if (*k & 0040) { /* Bit 0040 octal distinguishes direction (up/down or left/right) */
                    j = t.up; /* Up command string */
                } else {
                    j = t.down; /* Down command string */
                }
            } else {
                if (*k & 0040) {
                    j = t.left; /* Left command string */
                } else {
                    j = t.right; /* Right command string */
                }
            }
            if (!(i = *k & 0037)) { /* Extract count (low 5 bits, 0037 octal mask) */
                /* If count is 0, it might signify end of this plot sub-sequence */
                return (++k); /* Return pointer to byte after this command */
            }
            while (i--) { /* Output the command string 'i' times */
                oputs(j);
            }
        } else { /* Not a direction/count command, so it's a literal character to print in plot mode */
            oput(*k);
        }
    }
    return (k); /* Return pointer to the null terminator of the sequence */
}

/*
 * move
 * Performs accumulated horizontal (esc) and vertical (lead) motion.
 * Outputs appropriate terminal control sequences for movement.
 */
static void move(void) {
    int k; /* Loop counter for repeated motion commands */
    char *nl_seq; /* Newline-like sequence (e.g., newline, form feed, reverse line feed) */
    char *hl_seq; /* Half-line motion sequence */
    char *h_mot; /* Horizontal motion sequence (e.g., t.right for positive, t.left for negative) */
    char *v_mot; /* Vertical motion sequence (e.g., t.down for positive, t.up for negative) */
    int current_hpos; /* Current horizontal position before this move, for tab calculations */
    int dt; /* Distance to next tab stop */

    current_hpos = esct; /* esct is total accumulated horizontal motion on the line so far */

    /* Determine base newline sequence based on current horizontal position */
    if ((esct += esc)) { /* Add pending escapement to total; if total is non-zero after adding */
        nl_seq = ""; /* Effectively no newline action if horizontal position changes */
        /* Original: "\0" - an empty string is better */
    } else {
        nl_seq = "\n"; /* Standard newline if no net horizontal motion from origin */
        /* Original: "\n\0" - just "\n" is fine for string literal */
    }

    hl_seq = t.hlf; /* Default half-line feed sequence */
    h_mot = t.right; /* Default horizontal motion is right */
    v_mot = t.down; /* Default vertical motion is down */

    /* Handle vertical motion (lead) */
    if (lead) {
        if (lead < 0) { /* Negative lead (move up) */
            lead = -lead; /* Make lead positive for calculations */
            nl_seq = t.flr; /* Use reverse line feed sequence */
            /* Original logic: if(!esct)nl_seq = t.flr; else nl_seq = "\0";
             * This seems to imply reverse line feed only if at column 0.
             * The current nl_seq is already set based on esct.
             * If t.flr is the primary reverse motion, use it.
             */
            hl_seq = t.hlr; /* Use reverse half-line feed sequence */
            v_mot = t.up; /* Vertical motion is up */
        }

        /* Output full line motions */
        if (*nl_seq) { /* Check if the sequence string is not empty */
            k = lead / t.Newline; /* Number of full newlines */
            lead %= t.Newline; /* Remainder */
            while (k--) {
                oputs(nl_seq);
            }
        }
        /* Output half line motions */
        if (*hl_seq) { /* Check if the sequence string is not empty */
            k = lead / t.Halfline; /* Number of half newlines */
            lead %= t.Halfline; /* Remainder */
            while (k--) {
                oputs(hl_seq);
            }
        }
    }

    /* Handle horizontal motion (esc) */
    if (esc) {
        char *space_char_seq; /* Sequence for single character space/backspace */
        if (esc < 0) { /* Negative escapement (move left) */
            esc = -esc; /* Make esc positive for calculations */
            space_char_seq = "\b"; /* Use backspace for single unit left motion */
            h_mot = t.left; /* Horizontal motion is left */
        } else { /* Positive escapement (move right) */
            space_char_seq = " "; /* Use space for single unit right motion */
            /* Optimize with tabs if hflg is set and moving right */
            if (hflg) {
                while ((dt = dtab - (current_hpos % dtab)) <= esc) { /* dt is distance to next tab */
                    if (dt % t.Em) { /* If tab stop not multiple of Em, don't use tab */
                        break;
                    }
                    oput(TAB); /* Output tab character */
                    esc -= dt; /* Reduce remaining escapement */
                    current_hpos += dt; /* Update current horizontal position */
                }
            }
        }
        /* Output remaining motion in Em units */
        k = esc / t.Em; /* Number of Em-sized steps */
        esc %= t.Em; /* Remainder (sub-Em motion) */
        while (k--) {
            oputs(space_char_seq); /* Output space or backspace */
        }
    }

    /* Handle fine-grained plotting motion if plot mode is supported and residual esc/lead exists */
    if ((*t.ploton & 0377) && (esc || lead)) { /* If plot-on sequence exists and there's sub-unit motion */
        if (!plotmode) {
            oputs(t.ploton); /* Turn on plot mode */
            plotmode = 1;
        }
        /* Convert remaining esc and lead to device's finest horizontal/vertical units */
        if (t.Hor > 0)
            esc /= t.Hor;
        else
            esc = 0;
        if (t.Vert > 0)
            lead /= t.Vert;
        else
            lead = 0;

        while (esc--) { /* Output horizontal plot steps */
            oputs(h_mot);
        }
        while (lead--) { /* Output vertical plot steps */
            oputs(v_mot);
        }
        oputs(t.plotoff); /* Turn off plot mode */
        plotmode = 0;
    }
    esc = lead = 0; /* Reset pending motions */
}

/*
 * ptlead
 * Performs any pending vertical motion. Essentially a wrapper for move().
 */
void ptlead(void) {
    move();
}

/*
 * dostop
 * Halts processing and waits for a character from input (fd 2 - stderr).
 * Used for debugging or manual pagination.
 */
void dostop(void) {
    int junk; /* Dummy variable to satisfy read() */

    flusho(); /* Flush output buffer before stopping */
    /* Waits for any character to be typed on file descriptor 2 (stderr).
     * This is unusual; typically, one would read from stdin (fd 0) or the controlling terminal.
     * Reading from stderr might be intentional if stdin/stdout are redirected.
     */
    read(2, reinterpret_cast<char *>(&junk), 1); /* Read 1 byte into junk's memory location */
}
