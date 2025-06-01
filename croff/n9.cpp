#include "../cxx17_scaffold.hpp"
/**
 * @file n9.c
 * @brief Miscellaneous formatting functions for troff.
 *
 * This file contains a collection of functions responsible for handling
 * various troff commands related to drawing lines, overstriking characters,
 * building brackets, processing fields, and managing tab stops. These
 * functions typically manipulate the global character buffer `cbuf` to
 * assemble sequences of characters and motion commands for output by the
 * typesetting core.
 *
 * Copyright (c) 1976, 1977, 1979, 1982, 1983, 1984 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "tdef.hpp" /* Primary troff definitions, includes t.hpp indirectly or directly */
/* #include "t.h"  -- Usually included via tdef.h */

/* External variable declarations from other parts of the troff system */

/** @var cbuf
 *  @brief Character buffer for assembling output sequences.
 *  This buffer holds characters and motion commands before they are processed
 *  for final output. Functions in this file populate `cbuf`.
 */
extern int cbuf[];
/** @var cp
 *  @brief Pointer into cbuf.
 *  Typically points to the next available position in `cbuf` or to the
 *  start of a sequence to be processed.
 */
extern int *cp;
/** @var ch
 *  @brief Current character or global character state.
 *  Often holds the last character read by `getch()` or a similar input routine,
 *  or is used to pass character information between functions.
 */
extern int ch;
/** @var chbits
 *  @brief Character attribute bits (e.g., font, size).
 *  These bits are ORed with character codes to specify their properties.
 */
extern int chbits;
/** @var dfact
 *  @brief Scaling factor for units.
 *  Used by `quant()` to convert input numbers (often in ems, ens, etc.)
 *  to basic machine units.
 */
extern int dfact;
/** @var vflag
 *  @brief Vertical motion flag/indicator.
 *  Set when processing commands that involve vertical motion, like `\\L`.
 */
extern int vflag;
/** @var pts
 *  @brief Current point size.
 *  Used in various calculations, though not directly modified in all functions here.
 */
extern int pts;
/** @var fc
 *  @brief Field character for the `\\f` command.
 *  The delimiter character that marks the end of a field defined by `\\f`.
 *  Defaults to `IMP` (impossible character).
 */
extern int fc;
/** @var padc
 *  @brief Padding character for the `\\f` command.
 *  The character that, when encountered within a `\\f` field, indicates a
 *  point where padding should be inserted. Defaults to space.
 */
extern int padc;
/** @var tabtab
 *  @brief Array storing tab stop positions and types.
 *  Each entry contains a position (masked with `TMASK`) and a type
 *  (e.g., left, right, center).
 */
extern int tabtab[];
/** @var nlflg
 *  @brief Newline flag.
 *  Indicates if a newline character has been processed or is pending.
 *  Modified by `setfield` if a field is terminated by a newline.
 */
extern int nlflg;
/** @var lss
 *  @brief Line spacing setting (vertical distance).
 *  Used as a scaling factor (`dfact`) for vertical line drawing (`\\L`).
 */
extern int lss;
/** @var tabch
 *  @brief Global tab character (e.g., ASCII HT).
 *  When encountered, triggers tab processing.
 */
extern int tabch;
/** @var ldrch
 *  @brief Global leader character.
 *  When encountered, triggers leader processing.
 */
extern int ldrch;
/** @var tabc
 *  @brief Actual character used for tab fill.
 *  Typically a space, but can be changed. Combined with `chbits`.
 */
extern int tabc;
/** @var dotc
 *  @brief Actual character used for leader fill.
 *  Typically a period (dot), but can be changed. Combined with `chbits`.
 */
extern int dotc;
/** @var nchar
 *  @brief Number of characters for field fill or leaders.
 *  Set by `setfield` to indicate how many `rchar` characters to output.
 */
extern int nchar;
/** @var rchar
 *  @brief Repeating character for field fill or leaders.
 *  Set by `setfield` to the character (e.g., `tabc` or `dotc` with `chbits`)
 *  to be repeated `nchar` times.
 */
extern int rchar;
/** @var xxx
 *  @brief Unused global variable in the context of this file.
 *  Its purpose, if any, lies elsewhere in the troff system.
 */
extern int xxx;

/* Function Prototypes for C90 compatibility */
int setz(void);
void setline(void);
int eat(int delim_char);
void setov(void);
void setbra(void);
void setvline(void);
void casefc(void);
/**
 * @brief Process a field, handling tabs, leaders, and padding.
 *
 * This function is central to processing text fields, which can be delimited
 * by tab characters, leader characters, or explicit field delimiters set by `\f`.
 * It calculates the width of the field content, determines necessary padding
 * or leader characters, and sets global variables `nchar` and `rchar` to
 * control the output of fill characters by the main processing loop.
 *
 * @param field_type_char The character that triggered field processing (e.g., tab, leader).
 *                        This helps determine the type of field.
 * @return An integer status or value, often related to width or success.
 *         The exact return value's meaning depends on the calling context in troff.
 *         Typically, it might be 0 or a width.
 */
int setfield(int field_type_char);

/* Prototypes for functions called from this file (assumed to be external) */
/* These should ideally be in a common header like tdef.h or t.h */
int getch(void);
int quant(int val, int factor_type);
int tatoi(void);
int width(int char_val);
int makem(int motion_val);
int skip(void);
int getch0(void);
void prstr(const char *s);

/**
 * @brief Get a character and set the ZBIT (zero-motion) flag.
 *
 * Reads a character using `getch()`. If the character read is not a motion
 * command (i.e., its `MOT` bit is not set), the `ZBIT` is ORed into the
 * character's value. `ZBIT` typically indicates that the character should be
 * printed without advancing the current horizontal or vertical position,
 * allowing for overstrikes or special placements.
 *
 * @return The character read, potentially modified with `ZBIT`.
 */
int setz(void) {
    int i;

    i = getch(); /* Read a character/command */
    if (!(i & MOT)) { /* If it's not a motion command */
        i |= ZBIT; /* Set the zero-motion bit */
    }
    return i;
}

/**
 * @brief Process the `\\l'...'` (line drawing) command.
 *
 * Draws a horizontal line of a specified length using a given character.
 * The command syntax is typically `\\l'N c'`, where:
 * - `N` is the length of the line. It can be a numeric value, possibly with
 *   a unit (e.g., `3m` for 3 ems). `tatoi()` and `quant()` handle its conversion.
 * - `c` is the character to draw the line with. If `c` is omitted or is the
 *   same as the delimiter, a default rule character (`0204` with `chbits`) is used.
 * - The apostrophe (`'`) is a common delimiter, but any character can be used.
 *
 * If `N` is negative, the line is drawn to the left by first outputting a
 * negative motion command.
 *
 * The function assembles commands into `cbuf`. `cp` is set to `cbuf` upon completion.
 * Buffer management ensures that writes do not exceed `cbuf` capacity.
 */
void setline(void) {
    int *cb_ptr; /* Pointer into cbuf */
    int length; /* Length of the line in basic units */
    int c_draw_char; /* Character to draw the line with, or command */
    int char_width; /* Width of the drawing character */
    int count; /* Number of times the character needs to be repeated */
    int delim_char; /* Delimiter character for the command */
    int remainder_len; /* Remaining length after fitting whole characters */
    int temp_offset; /* Temporary offset for centering partial lines */
    int temp_char_read; /* Temporary variable for getch() result */

    delim_char = getch();
    if (delim_char & MOT) { /* If delimiter itself is a motion, abort */
        return;
    }
    delim_char &= CMASK; /* Mask to get the actual character value */

    vflag = 0; /* This is horizontal motion */
    dfact = EM; /* Default scaling factor for length is EM */
    length = quant(tatoi(), HOR); /* Get and quantify the line length horizontally */
    dfact = 1; /* Reset scaling factor for subsequent operations */

    if (length == 0) { /* If length is zero, nothing to draw */
        eat(delim_char); /* Consume rest of input up to delimiter */
        return;
    }

    /* Get the character to draw with, or use default rule if delimiter is next */
    /* Skips FILLER characters before selecting the drawing character or default. */
    do {
        temp_char_read = getch();
        if ((temp_char_read & CMASK) == FILLER) {
            continue; /* Skip filler, get next char */
        }
        if ((temp_char_read & CMASK) == delim_char) {
            ch = temp_char_read; /* Store delimiter, possibly for later use by eat() */
            c_draw_char = 0204 | chbits; /* Default to a rule character (often a horizontal line) with current attributes */
        } else {
            c_draw_char = temp_char_read; /* Use the character read if it's not the delimiter */
        }
        break; /* Found a non-filler char or the delimiter was processed */
    } while (1);
    /* If c_draw_char was a specific character, it's now in 'c_draw_char'. eat() will consume the delimiter later. */

    char_width = width(c_draw_char); /* Get width of the drawing character */
    cb_ptr = cbuf; /* Initialize pointer to the character buffer */

    if (length < 0) { /* Negative length means draw to the left */
        if (cb_ptr < cbuf + NC - 1) { /* Ensure space for motion and terminator */
            *cb_ptr++ = makem(length); /* Output negative motion */
        }
        length = -length; /* Make length positive for calculations */
    }

    if (char_width <= 0) { /* Avoid division by zero or issues with non-positive width chars */
        /* If char has no width, or negative (unlikely but defensive),
           output the char (maybe it's a ZBIT char) and then the motion.
        */
        if (length > 0) {
            if (cb_ptr < cbuf + NC - 2) { /* Ensure space for char, motion, and terminator */
                *cb_ptr++ = c_draw_char | ZBIT; /* Output the char, ensure it doesn't move by itself */
                *cb_ptr++ = makem(length); /* Motion for the remaining length */
            } else if (cb_ptr < cbuf + NC - 1) { /* Only space for char and terminator */
                *cb_ptr++ = c_draw_char | ZBIT; /* Only space for char */
            }
        }
        count = 0;
        remainder_len = 0;
    } else {
        count = length / char_width; /* Number of full characters that fit */
        remainder_len = length % char_width; /* Remaining length */
    }

    /* If the line is shorter than one character width (count is 0, but length > 0) */
    if (count == 0 && length > 0) {
        temp_offset = (char_width - length) / 2; /* Calculate offset to center the character */
        /* Ensure space for 3 motion/char commands + terminator */
        if (cb_ptr < cbuf + NC - 3) {
            *cb_ptr++ = makem(-temp_offset); /* Move back to start centering */
            *cb_ptr++ = c_draw_char; /* Output the character */
            *cb_ptr++ = makem(-(char_width - length - temp_offset)); /* Adjust position */
        } else {
            /* Not enough space for full sequence, may result in partial output or none.
               Consider if a single character with ZBIT should be placed if space is minimal.
            */
        }
    } else {
        /* This block handles cases where count > 0 (line is one or more char_widths long)
           or when length was <= 0 initially (already returned or count/remainder_len are 0).
           Specifically, it handles drawing the main part of the line and any remainder.
        */

        /* Handle remainder for lines that are at least one character width or longer */
        if (remainder_len > 0) {
            /* Check space: 2 for (char, motion) or 1 for motion, plus RPT block (3) if count > 0, plus terminator (1) */
            /* Max needed: 2 (here for char+motion) + 3 (for RPT block) + 1 (NULL) = 6. So NC - 5 for cb_ptr.
               If count == 0 (but remainder_len > 0, implies char_width was > 0): 2 (here) + 1 (NULL) = 3. So NC - 2 for cb_ptr.
            */
            if (cb_ptr < cbuf + NC - ((count > 0) ? (2 + 3) : 2)) {
                switch (c_draw_char & CMASK) {
                case 0204: /* rule character */
                case 0224: /* underrule character */
                case 0276: /* root en character (often used in equations) */
                    *cb_ptr++ = c_draw_char | ZBIT;
                    *cb_ptr++ = makem(remainder_len);
                    break;
                default:
                    /* For other characters, the original code only outputted motion for the remainder.
                           This might be an oversight if the intent was to draw a partial char.
                           However, sticking to original behavior: just motion.
                        */
                    *cb_ptr++ = makem(remainder_len); /* Only one item for default */
                    break;
                }
            }
        }

        if (count > 0) { /* If one or more full characters are to be drawn */
            if (cb_ptr < cbuf + NC - 3) { /* Need space for RPT, count, char + terminator */
                *cb_ptr++ = RPT; /* Repeat command */
                *cb_ptr++ = count; /* Number of repetitions */
                *cb_ptr++ = c_draw_char; /* Character to repeat */
            }
        }
    }

    /* Common code for null-terminating cbuf and consuming delimiter */
    if (cb_ptr < cbuf + NC) { /* Ensure cb_ptr is within bounds before writing null terminator */
        *cb_ptr = 0; /* Null terminate the command sequence in cbuf */
    } else {
        /* If cb_ptr is at or beyond cbuf + NC, an overflow likely occurred.
           Force null termination at the very last valid position.
        */
        *(cbuf + NC - 1) = 0;
    }
    eat(delim_char); /* Consume rest of input up to the original delimiter */
    cp = cbuf; /* Set global character pointer to start of cbuf */
}

/**
 * @brief Consume input characters until a specified delimiter or newline.
 *
 * Reads characters using `getch()` one by one. The lower 7 bits (`CMASK`)
 * of each character are compared against the `delim_char` (also masked)
 * and against the newline character (`\n`). The loop continues as long as
 * neither the delimiter nor a newline is encountered.
 *
 * @param delim_char The delimiter character (already masked with `CMASK`)
 *                   to stop at.
 * @return The character (masked with `CMASK`) that terminated the scan
 *         (either `delim_char` or `\n`).
 */
int eat(int delim_char) {
    int i; /* Holds the character read from input */

    /* Loop until delimiter or newline is found */
    /* delim_char is assumed to be already masked with CMASK by the caller if necessary,
       though this function masks it again from getch() for safety.
    */
    while (((i = getch() & CMASK) != delim_char) && (i != '\n')) {
        /* Keep consuming characters, loop does nothing else */
    }
    return i; /* Return the terminating character (masked) */
}

/**
 * @brief Process the `\o'...'` (overstrike) command.
 *
 * Overstrikes a sequence of characters. The command syntax is `\o'c1c2c3...'`,
 * where `'` is a delimiter and `c1, c2, c3, ...` are the characters to be
 * overstruck. Reads up to `NOV` characters or until delimiter/newline.
 * Characters are sorted by width (descending), then output to `cbuf` with
 * centering motions. `cp` is set to `cbuf`.
 */
void setov(void) {
    int i, j, k;
    int *p_cbuf; /* Pointer into cbuf */
    int delim_char; /* Delimiter character */
    int o[NOV]; /* Array to store characters to be overstruck */
    int w[NOV]; /* Array to store widths of these characters */
    int temp_w, temp_o; /* For swapping during sort */
    int next_w; /* Width of the next character for motion calculation */

    delim_char = getch();
    if (delim_char & MOT) { /* If delimiter is a motion command, abort */
        return;
    }
    delim_char &= CMASK; /* Mask to get actual character */

    /* Read characters to be overstruck until delimiter, newline, or NOV limit */
    for (k = 0; k < NOV; k++) {
        i = getch();
        j = i & CMASK;
        if (j == delim_char || j == '\n') {
            ch = i; /* Store terminating char for eat() or other context */
            break; /* Stop if delimiter or newline */
        }
        o[k] = i; /* Store character (with attributes) */
        w[k] = width(i); /* Store its width */
    }
    /* k now holds the number of characters read */

    /* If k < NOV, it means the loop terminated before filling the arrays.
       Setting o[k] and w[k] to 0 can act as sentinels if needed by other logic,
       though the motion calculation below is robust without it.
    */
    if (k < NOV) { /* This check is fine, k is the count of items, so o[k] is the one after last. */
        o[k] = 0; /* Not strictly necessary for the current logic but harmless as sentinel */
        w[k] = 0; /* Width of sentinel is 0 */
    }
    /* Note: If k == NOV, o[NOV-1] and w[NOV-1] contain the last valid character/width.
       Accessing o[k] or w[k] would be out of bounds if k == NOV.
       The loop for (i=0; i<k; i++) is safe.
       The next_w calculation `w[i+1]` needs `i+1 < k`.
    */

    if (k == 0) { /* If no characters were read */
        eat(delim_char); /* Consume delimiter if it wasn't the one breaking the loop */
        return;
    }

    /* Sort characters by width in descending order (simple bubble sort) */
    /* This ensures the widest character (w[0]) dictates the overall width for centering. */
    for (j = 1; j != 0;) { /* Loop as long as swaps are made in a pass */
        j = 0; /* Reset swap flag for this pass */
        /* Iterate up to k-1 because we compare w[idx] and w[idx-1] */
        for (i = 1; i < k; i++) {
            if (w[i - 1] < w[i]) { /* If current char w[i] is wider than previous w[i-1] */
                j++; /* A swap occurred in this pass */
                /* Swap widths */
                temp_w = w[i];
                w[i] = w[i - 1];
                w[i - 1] = temp_w;
                /* Swap characters */
                temp_o = o[i];
                o[i] = o[i - 1];
                o[i - 1] = temp_o;
            }
        }
    }

    p_cbuf = cbuf; /* Initialize pointer to cbuf */
    /* Generate output sequence: char, backspace, char, backspace ... */
    /* Each character is output, followed by a motion to position for the next. */
    for (i = 0; i < k; i++) { /* Iterate through the k characters */
        /* Need space for char, motion, and potentially a final motion + terminator.
           Safest: ensure space for current char + motion.
        */
        if (p_cbuf >= cbuf + NC - 2) { /* Need at least 2 slots for char + motion */
            break; /* Not enough space */
        }
        *p_cbuf++ = o[i]; /* Output character o[i] */

        /* Determine width of next character for centering, or 0 if this is the last one */
        /* If i == k-1 (last char), then i+1 == k. w[k] was set to 0 if k < NOV.
           If k == NOV, then w[k] is out of bounds.
           So, next_w should be w[i+1] only if i+1 < k.
        */
        next_w = (i < k - 1) ? w[i + 1] : 0;
        /* Motion to center between current char (w[i]) and next char (next_w) */
        *p_cbuf++ = makem(-((w[i] + next_w) / 2));
    }

    /* Final adjustment: after all characters and intermediate motions,
       move forward by half the width of the widest character (w[0])
       to effectively center the entire overstrike sequence.
    */
    if (p_cbuf < cbuf + NC - 1) { /* Ensure space for this motion and terminator */
        *p_cbuf++ = makem(w[0] / 2);
    }

    if (p_cbuf < cbuf + NC) {
        *p_cbuf = 0; /* Null terminate cbuf sequence */
    } else {
        *(cbuf + NC - 1) = 0; /* Force null if overflow */
    }

    eat(delim_char); /* Consume the delimiter if it wasn't consumed in the loop */
    cp = cbuf; /* Set global character pointer */
}

/**
 * @brief Process the `\b'...'` (bracket building) command.
 *
 * Builds a vertical "bracket" or pillar of characters. The command syntax is
 * `\b'c1c2c3...'`, where `'` is a delimiter and `c1, c2, c3, ...` are the
 * characters to be stacked vertically.
 *
 * Characters are read until the delimiter, newline, or buffer limit
 * is reached. Each character is stored in `cbuf` with `ZBIT` (to prevent
 * horizontal motion) followed by a downward motion command.
 *
 * - `down_motion`: In NROFF, this is `2 * t.Halfline`. In TROFF, it's `EM` units.
 *   This is the vertical distance each character is moved down from the previous one.
 *
 * If only one character is provided (e.g., `\b'c'`), it's treated as a normal
 * character by setting `ch` and returning.
 *
 * For multiple characters:
 *   - The sequence in `cbuf` starts at `cbuf[1]`.
 *   - `cbuf[0]` is reserved to store the total upward motion needed to return
 *     the vertical position to the original baseline after all characters
 *     have been "drawn". This total upward motion is also copied to the slot
 *     that would have held the motion command after the last character.
 *   - The `ZBIT` is cleared from the last character in the stack.
 *
 * `cp` is set to `cbuf`.
 */
void setbra(void) {
    int i, char_val_k; /* char_val_k stores (char & CMASK) */
    int *j_cbuf; /* Pointer into cbuf */
    int count; /* Count of characters in the bracket */
    int delim_char; /* Delimiter character */
    int down_motion; /* Vertical motion command for stacking */

    delim_char = getch();
    if (delim_char & MOT) { /* If delimiter is a motion command, abort */
        return;
    }
    delim_char &= CMASK; /* Mask to get actual character */

    j_cbuf = cbuf + 1; /* Start filling cbuf from the second element; cbuf[0] is for final up-motion. */
    count = 0;

    /* Define the downward motion per character */
#ifdef NROFF
    down_motion = (2 * t.Halfline) | MOT | VMOT; /* NROFF: 2 half-lines (one full line) downward */
#else
    /* TROFF: EM units are typically related to point size. This is one EM unit downward. */
    down_motion = EM | MOT | VMOT;
#endif

    /* Read characters for the bracket.
       Each (char, motion) pair takes 2 slots in cbuf.
       cbuf[0] is reserved for the final upward motion.
       A null terminator is needed at the end.
       The loop condition `j_cbuf <= (cbuf + NC - 3)` ensures space for a (char, motion) pair:
       - If j_cbuf points to cbuf+NC-3: char to [NC-3], motion to [NC-2].
       - j_cbuf then becomes cbuf+NC-1. The null terminator goes at cbuf[NC-1].
    */
    while (j_cbuf <= (cbuf + NC - 3)) { /* Max j_cbuf is cbuf+NC-3 for writing char, cbuf+NC-2 for motion */
        i = getch();
        char_val_k = i & CMASK;
        if (char_val_k == delim_char || char_val_k == '\n') {
            ch = i; /* Store terminating char for potential use by eat() or other context */
            break; /* Stop if delimiter or newline */
        }
        *j_cbuf++ = i | ZBIT; /* Store character with zero-motion (it will be moved vertically) */
        *j_cbuf++ = down_motion; /* Add downward motion command */
        count++;
    }
    /* j_cbuf now points to the position *after* the last down_motion written. */

    if (count == 0) { /* No characters read */
        eat(delim_char); /* Consume the delimiter if it wasn't the one that broke the loop */
        return;
    }
    /* If only one char, e.g., \b'c', effectively becomes just 'c'.
       The single character was stored at cbuf[1].
    */
    if (count == 1) {
        ch = *(cbuf + 1) & ~ZBIT; /* The single character is at cbuf[1], clear ZBIT as it's now normal */
        eat(delim_char); /* Consume the delimiter */
        return;
    }
    /* count > 1 here, meaning a true bracket is being built. */

    /* j_cbuf points *after* the last item written (which was a down_motion).
       The last down_motion (at j_cbuf-1) will be replaced by the total upward motion.
       cbuf[0] also stores this total upward motion.
    */
    j_cbuf--; /* Point to where the last down_motion was, to overwrite it. (e.g. cbuf + 1 + count*2 - 1) */

#ifdef NROFF
    /* Total upward motion for NROFF: count * (2 * t.Halfline) */
    *j_cbuf = *cbuf = (count * (2 * t.Halfline)) | MOT | NMOT | VMOT;
#else
    /* Total upward motion for TROFF: count * EM.
       The original code had (count * EM) / 2. This is preserved.
       This implies the baseline might be restored to the middle of the stack,
       or the initial down_motion was effectively half of what's used for total calculation.
       If down_motion was EM, and total up is (count*EM) / 2, it's unusual.
       Assuming (count * EM / 2) is the intended final vertical adjustment.
    */
    *j_cbuf = *cbuf = (count * EM / 2) | MOT | NMOT | VMOT;
#endif

    /* Clear ZBIT from the last actual character put in the stack.
       The total upward motion is at j_cbuf.
       The character before it (the last character of the \b sequence) is at j_cbuf-1.
       This character is at cbuf + 1 + (count-1)*2.
    */
    *(j_cbuf - 1) &= ~ZBIT;

    /* Null terminate the sequence in cbuf.
       The total upward motion is at j_cbuf. The next slot (j_cbuf+1) is for the null terminator.
       This should be (cbuf + 1 + count*2 -1) + 1 = cbuf + 1 + count*2.
       This position must be < cbuf + NC. Max value of j_cbuf before this was cbuf+NC-2 (if loop filled buffer).
       So j_cbuf+1 is cbuf+NC-1. This is safe.
    */
    *(j_cbuf + 1) = 0;

    eat(delim_char); /* Consume the delimiter */
    cp = cbuf; /* Set global character pointer */
}

/**
 * @brief Process the `\L'...'` (vertical line drawing) command.
 *
 * Draws a vertical line of a specified length using a given character.
 * The command syntax is `\L'N c'`, where:
 * - `N` is the length of the vertical line. `tatoi()` and `quant()` convert it.
 *   `dfact` is set to `lss` (line spacing setting) for `quant()`.
 * - `c` is the character to draw with. If omitted or same as delimiter,
 *   a default box rule character (`0337` with `chbits`) is used.
 * - `'` is a common delimiter.
 *
 * If `N` is negative, the line is drawn upwards (`NMOT` flag).
 * The function sets `vflag` during processing.
 *
 * Line drawing steps:
 * 1. Determine `ver_step_motion_val`: `2 * t.Halfline` (NROFF) or `EM` (TROFF).
 * 2. Calculate `count` (number of full steps) and `remainder_motion`.
 * 3. Assemble into `cbuf`:
 *    - If `remainder_motion` exists and is non-zero, output `c | ZBIT` then `remainder_motion`.
 *    - For `count` times, output `c | ZBIT` then `ver_step_motion_cmd`.
 * 4. Clear `ZBIT` from the last character drawn.
 * 5. **Special Adjustment**: If drawing downwards (`!neg_motion_flag`), and if the
 *    last item added to `cbuf` was a motion command, it might be removed by
 *    decrementing `k_cbuf`. This effectively means the last character drawn
 *    establishes the final vertical position without an additional motion.
 *
 * `cp` is set to `cbuf`, and `vflag` is cleared.
 */
void setvline(void) {
    int length_val; /* Length of the vertical line in basic units */
    int c; /* Character to draw with (includes chbits) */
    int *k_cbuf; /* Pointer into cbuf */
    int count; /* Number of full vertical steps */
    int neg_motion_flag; /* Flag for negative (upward) motion: NMOT or 0 */
    int remainder_motion; /* Motion command for remaining partial step */
    int ver_step_motion_val; /* Value of one full vertical step, before makem() */
    int ver_step_motion_cmd; /* Motion command for one full vertical step */
    int delim_char; /* Delimiter character */

    delim_char = getch();
    if (delim_char & MOT) { /* If delimiter is a motion command, abort */
        return;
    }
    delim_char &= CMASK; /* Mask to get actual character */

    dfact = lss; /* Use line spacing scale factor for quant */
    vflag++; /* Indicate vertical motion processing */
    length_val = quant(tatoi(), VERT); /* Get and quantify vertical length */
    dfact = 1; /* Reset scale factor */

    if (length_val == 0) { /* If length is zero */
        eat(delim_char);
        vflag = 0;
        return;
    }

    /* Get character for line, or default if delimiter is next */
    c = getch();
    if ((c & CMASK) == delim_char) {
        ch = c; /* Store delimiter for eat() */
        c = 0337 | chbits; /* Default box rule character with current attributes */
    } else {
        /* 'c' is the drawing character. The delimiter will be consumed by eat() later. */
    }
    c |= ZBIT; /* Character should be zero-motion; vertical motion is separate */

    neg_motion_flag = 0;
    if (length_val < 0) {
        length_val = -length_val;
        neg_motion_flag = NMOT; /* Negative motion (upward) */
    }

#ifdef NROFF
    ver_step_motion_val = 2 * t.Halfline; /* Basic vertical step value for NROFF */
#else
    ver_step_motion_val = EM; /* Basic vertical step value for TROFF */
#endif

    if (ver_step_motion_val <= 0) { /* Avoid division by zero or non-positive step */
        count = 0;
        /* If step is invalid, make the remainder the full length */
        remainder_motion = makem(length_val) | neg_motion_flag | VMOT;
    } else {
        count = length_val / ver_step_motion_val; /* Number of full steps */
        remainder_motion = makem(length_val % ver_step_motion_val) | neg_motion_flag | VMOT; /* Motion for remainder */
    }
    ver_step_motion_cmd = makem(ver_step_motion_val) | neg_motion_flag | VMOT; /* Full step motion command */

    k_cbuf = cbuf;

    /* If there's a remainder motion with non-zero magnitude, output char and remainder motion first */
    /* Check magnitude part of motion (bits other than MOT, VMOT, NMOT) */
    if ((remainder_motion & ~(MOT | VMOT | NMOT)) != 0) {
        if (k_cbuf < (cbuf + NC - 2)) { /* Need 2 slots for (char, motion) + 1 for terminator */
            *k_cbuf++ = c;
            *k_cbuf++ = remainder_motion;
        } else if (k_cbuf < (cbuf + NC - 1)) { /* Only space for 1 char + terminator */
            *k_cbuf++ = c; /* Output char, motion will be lost */
        }
    }

    /* Output char and full step motion for 'count' times */
    /* Loop while count > 0 and there's space for a (char, motion) pair + null terminator */
    while (count-- > 0 && k_cbuf < (cbuf + NC - 2)) {
        *k_cbuf++ = c;
        *k_cbuf++ = ver_step_motion_cmd;
    }

    if (k_cbuf > cbuf) { /* If anything was added to cbuf */
        /* The character before the last motion (or the last char if no motion followed)
           should have its ZBIT cleared to allow normal horizontal spacing afterwards.
           k_cbuf points to the next free slot.
        */
        if ((k_cbuf - cbuf) >= 2 && (*(k_cbuf - 1) & MOT)) {
            /* At least char+motion written, and last item was a motion.
               The character is at k_cbuf-2.
            */
            *(k_cbuf - 2) &= ~ZBIT;
        } else if (!(*(k_cbuf - 1) & MOT)) {
            /* Last item written was a character (e.g., buffer filled after char, or only char written).
               The character is at k_cbuf-1.
            */
            *(k_cbuf - 1) &= ~ZBIT;
        }
    }

    /* Special adjustment for downward lines:
       If drawing downwards (!neg_motion_flag), and something was written to cbuf,
       and the very last item written was a motion command, then that motion is removed.
       This means the final vertical position is determined by the placement of the
       last character itself, not by a subsequent motion.
    */
    if (!neg_motion_flag && k_cbuf > cbuf && (*(k_cbuf - 1) & MOT)) {
        k_cbuf--; /* Effectively remove the last motion command by backing up the pointer.
                     The null terminator will overwrite it. */
    }

    if (k_cbuf < cbuf + NC) { /* Ensure k_cbuf is within bounds before writing null terminator */
        *k_cbuf = 0; /* Null terminate cbuf sequence */
    } else {
        /* If k_cbuf is at or beyond cbuf + NC, an overflow likely occurred.
           Force null termination at the very last valid position.
        */
        *(cbuf + NC - 1) = 0;
    }
    eat(delim_char); /* Consume the delimiter if it wasn't consumed in the loop */
    cp = cbuf; /* Set global character pointer */
    vflag = 0; /* Clear vertical motion indicator */
}

/**
 * @brief Set field and padding characters for the `\\f` command.
 *
 * Reads two characters following the `\\f` command. The first character
 * becomes the new field delimiter character (`fc`), and the second becomes
 * the new padding character (`padc`).
 * - `fc` stores the character code (masked with `CMASK`).
 * - `padc` stores the character code (masked with `CMASK`).
 *
 * If either character argument is a newline, a motion command, or missing (e.g., EOF),
 * the respective global variable is set to its default:
 * - `fc` defaults to `IMP` (an "impossible" character code, indicating no specific field char).
 * - `padc` defaults to space (`' '`).
 *
 * If an argument is a newline, the global `ch` is updated with the newline character
 * to allow the calling context (e.g., the main input loop) to recognize and process it.
 */
void casefc(void) {
    int char1; /* Temporary storage for the first character read (for fc) */
    int char2; /* Temporary storage for the second character read (for padc) */

    /* Get the first character for the field delimiter */
    char1 = getch();

    /* Check if the first character is a newline, motion, or EOF */
    if (((char1 & CMASK) == '\n') || (char1 & MOT) || (char1 == EOF)) {
        fc = IMP; /* Default field character */
        /* If the first char is invalid or ends input, padc also takes its default */
        padc = ' ';
        if ((char1 & CMASK) == '\n') {
            ch = char1; /* Make newline available to the caller via global ch */
        }
        /* If char1 was EOF, getch() might have set ch or it's handled by the main loop.
         * The primary goal here is to set fc/padc correctly and return.
         */
        return;
    }
    fc = char1 & CMASK; /* Set new field character (actual character code) */

    /* Get the second character for the padding character */
    char2 = getch();

    /* Check if the second character is a newline, motion, or EOF */
    if (((char2 & CMASK) == '\n') || (char2 & MOT) || (char2 == EOF)) {
        padc = ' '; /* Default padding character */
        if ((char2 & CMASK) == '\n') {
            ch = char2; /* Make newline available to the caller via global ch */
        }
        return;
    }
    padc = char2 & CMASK; /* Set new padding character (actual character code) */
}

/**
 * @brief Process a field, handling tabs, leaders, and padding.
 *
 * This function is central to processing text fields, which can be delimited
 * by tab characters, leader characters, or explicit field delimiters set by `\f`.
 * It calculates the width of the field content, determines necessary padding
 * or leader characters, and sets global variables `nchar` and `rchar` to
 * control the output of fill characters by the main processing loop.
 *
 * @param field_type_char The character that triggered field processing (e.g., tab, leader).
 *                        This helps determine the type of field.
 * @return An integer status or value, often related to width or success.
 *         The exact return value's meaning depends on the calling context in troff.
 *         Typically, it might be 0 or a width.
 */
int setfield(int field_type_char) {
    int *cb_ptr; /* Pointer into cbuf */
    int width_val; /* Calculated width of the field content */
    int c; /* Character read from input */
    int char_width; /* Width of the character used for the field */
    int count; /* Number of times to repeat the leader/tab character */
    int delim_char; /* Delimiter character for the field command */
    int temp_offset; /* Temporary offset for centering fields */
    int remainder_len; /* Remaining length after fitting whole characters */
    int field_type; /* Type of field: tab, leader, or explicit delimiter */

    delim_char = getch();
    if (delim_char & MOT) { /* If delimiter is a motion command, abort */
        return 0;
    }
    delim_char &= CMASK; /* Mask to get actual character */

    cb_ptr = cbuf; /* Initialize pointer to the character buffer */

    /* Determine field type and set initial character */
    if (delim_char == fc) {
        /* Explicit field delimiter */
        ch = delim_char; /* Store delimiter for potential use by eat() */
        field_type = 0; /* Field type 0: explicit delimiter */
    } else if (delim_char == tabch) {
        /* Tab character */
        field_type = 1; /* Field type 1: tab */
    } else if (delim_char == ldrch) {
        /* Leader character */
        field_type = 2; /* Field type 2: leader */
    } else {
        /* Invalid or unexpected character for field processing */
        eat(delim_char); /* Consume the character up to the delimiter */
        return 0;
    }

    /* Calculate the width of the field content based on the field type */
    switch (field_type) {
    case 0: /* Explicit delimiter */
        width_val = 0; /* Width is determined by the content between delimiters */
        break;
    case 1: /* Tab character */
        width_val = tabtab[(tabtab[0] & TMASK) >> 1] & TMASK; /* Get tab stop position */
        break;
    case 2: /* Leader character */
        width_val = 1; /* Default width for leader is 1 (can be adjusted) */
        break;
    default:
        width_val = 0;
        break;
    }

    /* Main loop: read characters and assemble the field content */
    while (cb_ptr < cbuf + NC - 1) { /* Ensure space for at least one character and terminator */
        c = getch();
        if (c == delim_char || c == '\n') {
            /* End of field: either the delimiter or a newline */
            ch = c; /* Store the terminating character (delimiter or newline) */
            break;
        }
        if (c == fc) {
            /* Nested field delimiter: treat as a normal character */
            *cb_ptr++ = c;
            continue;
        }
        if (c == tabch || c == ldrch) {
            /* Tab or leader character: handle according to field type */
            if (field_type == 1) {
                /* Tab character: set as the repeating character for the field */
                rchar = c | chbits;
                nchar = width_val / width(c);
                if (nchar <= 0) {
                    nchar = 1; /* Ensure at least one repetition */
                }
                *cb_ptr++ = RPT; /* Repeat command */
                *cb_ptr++ = nchar; /* Number of repetitions */
                *cb_ptr++ = rchar; /* Character to repeat */
            } else if (field_type == 2) {
                /* Leader character: set as the repeating character for the field */
                rchar = c | chbits;
                nchar = width_val / width(c);
                if (nchar <= 0) {
                    nchar = 1; /* Ensure at least one repetition */
                }
                *cb_ptr++ = RPT; /* Repeat command */
                *cb_ptr++ = nchar; /* Number of repetitions */
                *cb_ptr++ = rchar; /* Character to repeat */
            }
            continue;
        }
        /* Regular character: add to the field content */
        *cb_ptr++ = c;
    }

    /* Null terminate the field content in cbuf */
    if (cb_ptr < cbuf + NC) {
        *cb_ptr = 0;
    } else {
        *(cbuf + NC - 1) = 0; /* Force null termination at the last position */
    }

    eat(delim_char); /* Consume the rest of the input up to the delimiter */
    cp = cbuf; /* Set global character pointer to the start of cbuf */

    return width_val; /* Return the calculated width of the field */
}
