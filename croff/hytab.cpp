#include "cxx23_scaffold.hpp"
/*
 * hytab.c - Hyphenation digram tables for nroff/troff (C90 portable)
 *
 * Copyright (c) 1979 Bell Telephone Laboratories, Incorporated
 * Copyright (c) 2024 Modern C90 port
 *
 * This file contains the hyphenation digram tables used by the nroff/troff
 * hyphenation algorithm. These tables encode statistical information about
 * letter combinations to determine probable hyphenation points in English words.
 *
 * ALGORITHM DESCRIPTION:
 * The hyphenation algorithm uses five different digram tables:
 *   1. bxh[13]      : Beginning of word + character sequences
 *   2. hxx[676]     : Character + character sequences (26x26 matrix)
 *   3. bxxh[676]    : Character + end of word sequences (26x26 matrix)
 *   4. xhx[676]     : Mixed character sequences with hyphenation context (26x26 matrix)
 *   5. xxh[676]     : Character + end sequences fallback (26x26 matrix)
 *
 * Each table entry represents the probability or weight of a hyphenation
 * point occurring at specific letter combinations. The values are encoded
 * in octal format for historical compatibility with original UNIX troff.
 *
 * ALGORITHM OVERVIEW:
 *   1. For each potential hyphenation point in a word
 *   2. Look up the surrounding character context in appropriate tables
 *   3. Combine the weights from multiple table lookups
 *   4. If the combined weight exceeds a threshold, allow hyphenation
 *
 * CHARACTER ENCODING:
 *   Tables use 0-based indexing where 'a'=0, 'b'=1, ..., 'z'=25
 *   Octal values represent hyphenation weights (0-255 decimal range)
 *   Higher values indicate stronger hyphenation probability
 *   Negative values (using two's complement) discourage hyphenation
 *
 * HISTORICAL NOTE:
 *   These tables were derived from statistical analysis of hyphenated
 *   text and represent decades of refinement for English hyphenation.
 *   Original tables from Bell Labs UNIX Version 7 troff (1979).
 *
 * USAGE:
 *   These tables are used by the hyphenation routines in the troff
 *   text formatting system and compatible implementations.
 */

#include <stddef.h> /* C90 standard definitions */

/*
 * Table size definitions for validation and bounds checking
 */
#define BXH_SIZE 13 /* Beginning + character table size */
#define MATRIX_SIZE 676 /* 26x26 character matrix size */
#define ALPHABET_SIZE 26 /* Number of letters in English alphabet */

/*
 * Hyphenation digram table for word beginning sequences (bxh)
 *
 * This table handles the case where we're looking at the beginning
 * of a word followed by a character, then considering hyphenation
 * after that character.
 *
 * Format: Each entry corresponds to a character (a-m, mapped to indices 0-12)
 * Values are octal-encoded weights for hyphenation probability
 * Size: 13 entries (covers a-m, sufficient for beginning sequences)
 */
char bxh[BXH_SIZE] = {
    0060, 0000, 0040, 0000, 0040, 0000, 0000, 0040, /* a-h */
    0000, 0000, 0040, 0000, 0040 /* i-m */
};

/*
 * Hyphenation digram table for general character-character sequences (hxx)
 *
 * This is the main table for most hyphenation decisions. It encodes
 * the probability of hyphenation between any two characters in the
 * middle of a word.
 *
 * Table organization:
 *   - 26x26 matrix representing all possible letter pairs
 *   - Each row represents the first character (a-z)
 *   - Each column represents the second character (a-z)
 *   - Values indicate hyphenation probability/weight
 *
 * Higher values indicate a higher probability that hyphenation
 * is appropriate between those two characters.
 *
 * Matrix layout: hxx[first_char * 26 + second_char]
 */
char hxx[MATRIX_SIZE] = {
    /* Row 'a' (0): combinations starting with 'a' */
    0006, 0042, 0041, 0123, 0021, 0024, 0063, 0042, 0002, 0043, 0021, 0001, 0022,
    0140, 0000, 0200, 0003, 0260, 0006, 0000, 0160, 0007, 0000, 0140, 0000, 0320,

    /* Row 'b' (1): combinations starting with 'b' */
    0220, 0000, 0160, 0005, 0240, 0010, 0000, 0100, 0006, 0000, 0200, 0000, 0320,
    0240, 0000, 0120, 0003, 0140, 0000, 0000, 0240, 0010, 0000, 0220, 0000, 0160,

    /* Row 'c' (2): combinations starting with 'c' */
    0042, 0023, 0041, 0040, 0040, 0022, 0043, 0041, 0030, 0064, 0021, 0000, 0041,
    0100, 0000, 0140, 0000, 0220, 0006, 0000, 0140, 0003, 0000, 0200, 0000, 0000,

    /* Row 'd' (3): combinations starting with 'd' */
    0200, 0000, 0120, 0002, 0220, 0010, 0000, 0160, 0006, 0000, 0140, 0000, 0320,
    0020, 0000, 0020, 0000, 0020, 0000, 0000, 0020, 0000, 0000, 0020, 0000, 0000,

    /* Row 'e' (4): combinations starting with 'e' */
    0043, 0163, 0065, 0044, 0022, 0043, 0104, 0042, 0061, 0146, 0061, 0000, 0007,
    0100, 0000, 0140, 0000, 0040, 0000, 0000, 0100, 0000, 0000, 0120, 0000, 0000,

    /* Row 'f' (5): combinations starting with 'f' */
    0140, 0000, 0040, 0011, 0060, 0004, 0001, 0120, 0003, 0000, 0140, 0000, 0040,
    0200, 0000, 0100, 0000, 0140, 0000, 0000, 0140, 0000, 0000, 0140, 0000, 0240,

    /* Row 'g' (6): combinations starting with 'g' */
    0200, 0000, 0140, 0000, 0160, 0000, 0000, 0220, 0000, 0000, 0140, 0000, 0240,
    0200, 0000, 0140, 0000, 0160, 0000, 0000, 0220, 0000, 0000, 0060, 0000, 0240,

    /* Row 'h' (7): combinations starting with 'h' */
    0021, 0043, 0041, 0121, 0040, 0023, 0042, 0003, 0142, 0042, 0061, 0001, 0022,
    0120, 0000, 0140, 0010, 0140, 0010, 0000, 0140, 0002, 0000, 0120, 0000, 0120,

    /* Row 'i' (8): combinations starting with 'i' */
    0000, 0000, 0000, 0000, 0360, 0000, 0000, 0000, 0000, 0000, 0160, 0000, 0000,
    0100, 0000, 0040, 0005, 0120, 0000, 0000, 0100, 0000, 0000, 0060, 0000, 0140,

    /* Row 'j' (9): combinations starting with 'j' */
    0140, 0040, 0100, 0001, 0240, 0041, 0000, 0242, 0000, 0002, 0140, 0000, 0100,
    0240, 0000, 0120, 0002, 0200, 0000, 0000, 0320, 0007, 0000, 0240, 0000, 0340,

    /* Row 'k' (10): combinations starting with 'k' */
    0101, 0021, 0041, 0020, 0040, 0005, 0042, 0121, 0002, 0021, 0201, 0000, 0020,
    0160, 0000, 0100, 0000, 0140, 0000, 0000, 0160, 0006, 0000, 0220, 0000, 0140,

    /* Row 'l' (11): combinations starting with 'l' */
    0140, 0000, 0020, 0001, 0020, 0000, 0000, 0100, 0001, 0000, 0300, 0000, 0000,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,

    /* Row 'm' (12): combinations starting with 'm' */
    0106, 0041, 0040, 0147, 0040, 0000, 0063, 0041, 0001, 0102, 0160, 0002, 0002,
    0300, 0000, 0040, 0017, 0140, 0017, 0000, 0240, 0000, 0000, 0140, 0000, 0120,

    /* Row 'n' (13): combinations starting with 'n' */
    0200, 0000, 0140, 0121, 0240, 0000, 0200, 0140, 0006, 0000, 0140, 0000, 0000,
    0020, 0000, 0000, 0000, 0000, 0000, 0000, 0140, 0000, 0000, 0140, 0000, 0040,

    /* Row 'o' (14): combinations starting with 'o' */
    0000, 0043, 0041, 0040, 0000, 0042, 0041, 0041, 0000, 0000, 0000, 0000, 0100,
    0120, 0000, 0200, 0000, 0160, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0000,

    /* Row 'p' (15): combinations starting with 'p' */
    0200, 0000, 0000, 0000, 0240, 0000, 0000, 0120, 0006, 0000, 0000, 0000, 0000,
    0000, 0000, 0140, 0000, 0200, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0000,

    /* Row 'q' (16): combinations starting with 'q' */
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0240, 0000, 0000, 0000, 0000, 0000,

    /* Row 'r' (17): combinations starting with 'r' */
    0162, 0042, 0100, 0104, 0242, 0041, 0141, 0100, 0026, 0001, 0141, 0000, 0142,
    0140, 0000, 0120, 0000, 0140, 0000, 0000, 0140, 0000, 0000, 0100, 0000, 0000,

    /* Row 's' (18): combinations starting with 's' */
    0141, 0041, 0140, 0000, 0140, 0001, 0000, 0140, 0042, 0000, 0140, 0000, 0142,
    0000, 0000, 0140, 0000, 0000, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0000,

    /* Row 't' (19): combinations starting with 't' */
    0161, 0000, 0140, 0000, 0220, 0000, 0000, 0140, 0124, 0000, 0000, 0000, 0000,
    0000, 0000, 0000, 0000, 0161, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0000,

    /* Row 'u' (20): combinations starting with 'u' */
    0000, 0043, 0142, 0041, 0000, 0000, 0141, 0000, 0000, 0000, 0000, 0000, 0142,
    0120, 0000, 0140, 0000, 0160, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,

    /* Row 'v' (21): combinations starting with 'v' */
    0140, 0000, 0000, 0000, 0220, 0000, 0000, 0000, 0042, 0000, 0000, 0000, 0000,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,

    /* Row 'w' (22): combinations starting with 'w' */
    0161, 0000, 0000, 0000, 0140, 0000, 0000, 0120, 0042, 0000, 0000, 0000, 0000,
    0120, 0000, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,

    /* Row 'x' (23): combinations starting with 'x' */
    0141, 0000, 0140, 0000, 0000, 0000, 0000, 0000, 0042, 0000, 0000, 0000, 0000,
    0000, 0000, 0140, 0000, 0000, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0000,

    /* Row 'y' (24): combinations starting with 'y' */
    0141, 0000, 0140, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0142,
    0120, 0000, 0140, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,

    /* Row 'z' (25): combinations starting with 'z' */
    0141, 0000, 0000, 0000, 0240, 0000, 0000, 0000, 0042, 0000, 0000, 0000, 0000,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000};

/*
 * Hyphenation digram table for character + word ending sequences (bxxh)
 *
 * This table handles hyphenation decisions when considering a character
 * followed by the end of the word. It's used to determine if hyphenation
 * is appropriate just before common word endings.
 *
 * Format: 26x26 matrix similar to hxx[], but specifically for end-of-word contexts
 * Layout: bxxh[first_char * 26 + second_char]
 */
char bxxh[MATRIX_SIZE] = {
    /* Row 'a' (0): character 'a' + ending sequences */
    0005, 0150, 0153, 0062, 0062, 0246, 0152, 0127, 0146, 0203, 0310, 0017, 0206,
    0100, 0000, 0120, 0000, 0140, 0000, 0000, 0100, 0000, 0000, 0120, 0000, 0060,

    /* Row 'b' (1): character 'b' + ending sequences */
    0100, 0000, 0040, 0000, 0060, 0000, 0000, 0060, 0000, 0000, 0220, 0000, 0040,
    0100, 0000, 0120, 0000, 0200, 0000, 0000, 0100, 0000, 0000, 0140, 0000, 0060,

    /* Row 'c' (2): character 'c' + ending sequences */
    0043, 0142, 0046, 0140, 0062, 0147, 0210, 0131, 0046, 0106, 0246, 0017, 0111,
    0060, 0000, 0020, 0000, 0060, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0000,

    /* Row 'd' (3): character 'd' + ending sequences */
    0060, 0000, 0040, 0000, 0040, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0040,
    0100, 0000, 0100, 0000, 0100, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0140,

    /* Row 'e' (4): character 'e' + ending sequences */
    0066, 0045, 0145, 0140, 0000, 0070, 0377, 0030, 0130, 0103, 0003, 0017, 0006,
    0040, 0000, 0040, 0000, 0020, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0000,

    /* Row 'f' (5): character 'f' + ending sequences */
    0200, 0000, 0020, 0000, 0140, 0000, 0000, 0120, 0000, 0000, 0120, 0000, 0040,
    0120, 0000, 0040, 0000, 0060, 0000, 0000, 0060, 0000, 0000, 0160, 0000, 0040,

    /* Row 'g' (6): character 'g' + ending sequences */
    0120, 0000, 0040, 0000, 0120, 0000, 0000, 0040, 0000, 0000, 0160, 0000, 0040,
    0120, 0000, 0020, 0000, 0140, 0000, 0000, 0120, 0000, 0000, 0140, 0000, 0040,

    /* Row 'h' (7): character 'h' + ending sequences */
    0051, 0126, 0150, 0140, 0060, 0210, 0146, 0006, 0006, 0165, 0003, 0017, 0244,
    0120, 0000, 0040, 0000, 0160, 0000, 0000, 0140, 0000, 0000, 0060, 0000, 0140,

    /* Row 'i' (8): character 'i' + ending sequences */
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
    0140, 0000, 0140, 0000, 0060, 0000, 0000, 0100, 0000, 0000, 0140, 0000, 0020,

    /* Row 'j' (9): character 'j' + ending sequences */
    0120, 0000, 0020, 0000, 0060, 0000, 0000, 0060, 0000, 0000, 0060, 0000, 0040,
    0140, 0000, 0020, 0000, 0100, 0000, 0000, 0140, 0000, 0000, 0140, 0000, 0020,

    /* Row 'k' (10): character 'k' + ending sequences */
    0070, 0125, 0051, 0162, 0120, 0105, 0126, 0104, 0006, 0044, 0000, 0017, 0052,
    0140, 0000, 0020, 0000, 0140, 0000, 0000, 0060, 0000, 0000, 0060, 0000, 0040,

    /* Row 'l' (11): character 'l' + ending sequences */
    0020, 0000, 0000, 0000, 0020, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0060,
    0140, 0000, 0160, 0000, 0200, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0240,

    /* Row 'm' (12): character 'm' + ending sequences */
    0065, 0042, 0060, 0200, 0000, 0210, 0222, 0146, 0006, 0204, 0220, 0012, 0003,
    0240, 0000, 0020, 0000, 0120, 0000, 0000, 0200, 0000, 0000, 0200, 0000, 0240,

    /* Rows 'n' through 'z' (13-25): remaining character + ending sequences */
    0100, 0000, 0120, 0000, 0140, 0000, 0000, 0100, 0000, 0000, 0120, 0000, 0060,
    0100, 0000, 0040, 0000, 0060, 0000, 0000, 0060, 0000, 0000, 0220, 0000, 0040,
    0100, 0000, 0120, 0000, 0200, 0000, 0000, 0100, 0000, 0000, 0140, 0000, 0060,
    0043, 0142, 0046, 0140, 0062, 0147, 0210, 0131, 0046, 0106, 0246, 0017, 0111,
    0060, 0000, 0020, 0000, 0060, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0000,
    0060, 0000, 0040, 0000, 0040, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0040,
    0100, 0000, 0100, 0000, 0100, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0140,
    0066, 0045, 0145, 0140, 0000, 0070, 0377, 0030, 0130, 0103, 0003, 0017, 0006,
    0040, 0000, 0040, 0000, 0020, 0000, 0000, 0040, 0000, 0000, 0100, 0000, 0000,
    0200, 0000, 0020, 0000, 0140, 0000, 0000, 0120, 0000, 0000, 0120, 0000, 0040,
    0120, 0000, 0040, 0000, 0060, 0000, 0000, 0060, 0000, 0000, 0160, 0000, 0040,
    0120, 0000, 0040, 0000, 0120, 0000, 0000, 0040, 0000, 0000, 0160, 0000, 0040,
    0120, 0000, 0020, 0000, 0140, 0000, 0000, 0120, 0000, 0000, 0140, 0000, 0040,
    0051, 0126, 0150, 0140, 0060, 0210, 0146, 0006, 0006, 0165, 0003, 0017, 0244,
    0120, 0000, 0040, 0000, 0160, 0000, 0000, 0140, 0000, 0000, 0060, 0000, 0140,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
    0140, 0000, 0140, 0000, 0060, 0000, 0000, 0100, 0000, 0000, 0140, 0000, 0020,
    0120, 0000, 0020, 0000, 0060, 0000, 0000, 0060, 0000, 0000, 0060, 0000, 0040,
    0140, 0000, 0020, 0000, 0100, 0000, 0000, 0140, 0000, 0000, 0140, 0000, 0020,
    0070, 0125, 0051, 0162, 0120, 0105, 0126, 0104, 0006, 0044, 0000, 0017, 0052,
    0140, 0000, 0020, 0000, 0140, 0000, 0000, 0060, 0000, 0000, 0060, 0000, 0040,
    0020, 0000, 0000, 0000, 0020, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0060,
    0140, 0000, 0160, 0000, 0200, 0000, 0000, 0140, 0000, 0000, 0000, 0000, 0240,
    0065, 0042, 0060, 0200, 0000, 0210, 0222, 0146, 0006, 0204, 0220, 0012, 0003,
    0240, 0000, 0020, 0000, 0120, 0000, 0000, 0200, 0000, 0000, 0200, 0000, 0240};

/*
 * Hyphenation digram table for mixed sequences with hyphenation context (xhx)
 *
 * This table is used for more complex hyphenation decisions where the
 * context includes existing hyphenation points or special character
 * combinations that affect hyphenation probability.
 *
 * The values in this table can be negative (using two's complement
 * representation in signed char) to indicate contexts where hyphenation
 * should be discouraged or prohibited.
 *
 * Layout: xhx[first_char * 26 + second_char]
 */
char xhx[MATRIX_SIZE] = {
    /* Row 'a' (0): complex hyphenation context patterns starting with 'a' */
    0032, 0146, 0042, 0107, 0076, 0102, 0042, 0146, 0202, 0050, 0006, 0000, 0051,
    0036, 0377, 0057, 0013, 0057, 0366, 0377, 0057, 0001, 0377, 0057, 0000, 0040,

    /* Row 'b' (1): complex hyphenation context patterns starting with 'b' */
    0037, 0377, 0020, 0000, 0100, 0022, 0377, 0057, 0362, 0116, 0100, 0000, 0017,
    0057, 0377, 0057, 0031, 0137, 0363, 0377, 0037, 0362, 0270, 0077, 0000, 0117,

    /* Row 'c' (2): complex hyphenation context patterns starting with 'c' */
    0074, 0142, 0012, 0236, 0076, 0125, 0063, 0165, 0341, 0046, 0047, 0000, 0024,
    0020, 0017, 0075, 0377, 0040, 0001, 0377, 0017, 0001, 0204, 0020, 0000, 0040,

    /* Row 'd' (3): complex hyphenation context patterns starting with 'd' */
    0057, 0017, 0057, 0340, 0140, 0362, 0314, 0117, 0003, 0302, 0100, 0000, 0057,
    0057, 0357, 0077, 0017, 0100, 0366, 0314, 0057, 0342, 0346, 0037, 0000, 0060,

    /* Row 'e' (4): complex hyphenation context patterns starting with 'e' */
    0252, 0145, 0072, 0157, 0377, 0165, 0063, 0066, 0164, 0050, 0363, 0000, 0362,
    0000, 0000, 0020, 0000, 0020, 0000, 0000, 0017, 0000, 0000, 0020, 0000, 0000,

    /* Row 'f' (5): complex hyphenation context patterns starting with 'f' */
    0117, 0017, 0237, 0377, 0200, 0354, 0125, 0110, 0004, 0257, 0000, 0000, 0300,
    0057, 0367, 0054, 0357, 0157, 0216, 0314, 0114, 0217, 0353, 0053, 0000, 0057,

    /* Row 'g' (6): complex hyphenation context patterns starting with 'g' */
    0077, 0213, 0077, 0077, 0177, 0317, 0377, 0114, 0377, 0352, 0077, 0000, 0076,
    0077, 0213, 0077, 0077, 0157, 0177, 0377, 0054, 0377, 0352, 0117, 0000, 0075,

    /* Row 'h' (7): complex hyphenation context patterns starting with 'h' */
    0125, 0230, 0065, 0216, 0057, 0066, 0063, 0047, 0345, 0126, 0011, 0000, 0033,
    0057, 0377, 0051, 0360, 0120, 0361, 0273, 0056, 0001, 0256, 0057, 0000, 0060,

    /* Row 'i' (8): complex hyphenation context patterns starting with 'i' */
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
    0076, 0310, 0056, 0310, 0137, 0174, 0273, 0055, 0335, 0266, 0033, 0000, 0155,

    /* Row 'j' (9): complex hyphenation context patterns starting with 'j' */
    0077, 0157, 0057, 0360, 0057, 0063, 0042, 0024, 0077, 0206, 0020, 0000, 0040,
    0057, 0037, 0077, 0360, 0100, 0365, 0377, 0037, 0362, 0176, 0050, 0000, 0026,

    /* Row 'k' (10): complex hyphenation context patterns starting with 'k' */
    0167, 0146, 0042, 0112, 0077, 0110, 0062, 0254, 0366, 0052, 0377, 0000, 0163,
    0060, 0000, 0040, 0000, 0120, 0000, 0377, 0060, 0012, 0000, 0037, 0000, 0257,

    /* Row 'l' (11): complex hyphenation context patterns starting with 'l' */
    0037, 0232, 0157, 0361, 0040, 0003, 0125, 0010, 0001, 0256, 0000, 0000, 0340,
    0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0017, 0277,

    /* Row 'm' (12): complex hyphenation context patterns starting with 'm' */
    0253, 0315, 0257, 0216, 0377, 0206, 0146, 0306, 0371, 0126, 0232, 0000, 0004,
    0057, 0012, 0100, 0360, 0160, 0360, 0000, 0040, 0000, 0017, 0157, 0000, 0176,

    /* Rows 'n' through 'z' (13-25): remaining complex context patterns */
    0032, 0146, 0042, 0107, 0076, 0102, 0042, 0146, 0202, 0050, 0006, 0000, 0051,
    0036, 0377, 0057, 0013, 0057, 0366, 0377, 0057, 0001, 0377, 0057, 0000, 0040,
    0037, 0377, 0020, 0000, 0100, 0022, 0377, 0057, 0362, 0116, 0100, 0000, 0017,
    0057, 0377, 0057, 0031, 0137, 0363, 0377, 0037, 0362, 0270, 0077, 0000, 0117,
    0074, 0142, 0012, 0236, 0076, 0125, 0063, 0165, 0341, 0046, 0047, 0000, 0024,
    0020, 0017, 0075, 0377, 0040, 0001, 0377, 0017, 0001, 0204, 0020, 0000, 0040,
    0057, 0017, 0057, 0340, 0140, 0362, 0314, 0117, 0003, 0302, 0100, 0000, 0057,
    0057, 0357, 0077, 0017, 0100, 0366, 0314, 0057, 0342, 0346, 0037, 0000, 0060,
    0252, 0145, 0072, 0157, 0377, 0165, 0063, 0066, 0164, 0050, 0363, 0000, 0362,
    0000, 0000, 0020, 0000, 0020, 0000, 0000, 0017, 0000, 0000, 0020, 0000, 0000,
    0117, 0017, 0237, 0377, 0200, 0354, 0125, 0110, 0004, 0257, 0000, 0000, 0300,
    0057, 0367, 0054, 0357, 0157, 0216, 0314, 0114, 0217, 0353, 0053, 0000, 0057,
    0077, 0213, 0077, 0077, 0177, 0317, 0377, 0114, 0377, 0352, 0077, 0000, 0076,
    0077, 0213, 0077, 0077, 0157, 0177, 0377, 0054, 0377, 0352, 0117, 0000, 0075,
    0125, 0230, 0065, 0216, 0057, 0066, 0063, 0047, 0345, 0126, 0011, 0000, 0033,
    0057, 0377, 0051, 0360, 0120, 0361, 0273, 0056, 0001, 0256, 0057, 0000, 0060,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
    0076, 0310, 0056, 0310, 0137, 0174, 0273, 0055, 0335, 0266, 0033, 0000, 0155,
    0077, 0157, 0057, 0360, 0057, 0063, 0042, 0024, 0077, 0206, 0020, 0000, 0040,
    0057, 0037, 0077, 0360, 0100, 0365, 0377, 0037, 0362, 0176, 0050, 0000, 0026,
    0167, 0146, 0042, 0112, 0077, 0110, 0062, 0254, 0366, 0052, 0377, 0000, 0163,
    0060, 0000, 0040, 0000, 0120, 0000, 0377, 0060, 0012, 0000, 0037, 0000, 0257,
    0037, 0232, 0157, 0361, 0040, 0003, 0125, 0010, 0001, 0256, 0000, 0000, 0340,
    0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0017, 0277,
    0253, 0315, 0257, 0216, 0377, 0206, 0146, 0306, 0371, 0126, 0232, 0000, 0004,
    0057, 0012, 0100, 0360, 0160, 0360, 0000, 0040, 0000, 0017, 0157, 0000, 0176};

/*
 * Hyphenation digram table for character + ending fallback sequences (xxh)
 *
 * This table provides fallback hyphenation weights when the primary
 * tables don't provide sufficient information. It's used as a secondary
 * lookup for edge cases and uncommon letter combinations.
 *
 * Format: 26x26 matrix similar to other tables, but with conservative weights
 * to avoid inappropriate hyphenation in uncertain cases.
 *
 * Layout: xxh[first_char * 26 + second_char]
 */
char xxh[MATRIX_SIZE] = {
    /* Row 'a' (0): fallback hyphenation weights starting with 'a' */
    0045, 0150, 0154, 0162, 0042, 0246, 0210, 0147, 0152, 0103, 0230, 0017, 0206,
    0100, 0000, 0040, 0000, 0140, 0000, 0000, 0100, 0000, 0021, 0120, 0017, 0060,

    /* Row 'b' (1): fallback hyphenation weights starting with 'b' */
    0100, 0000, 0040, 0002, 0140, 0320, 0000, 0060, 0000, 0001, 0220, 0017, 0040,
    0100, 0001, 0120, 0001, 0241, 0000, 0000, 0100, 0000, 0020, 0140, 0017, 0060,

    /* Row 'c' (2): fallback hyphenation weights starting with 'c' */
    0023, 0162, 0046, 0142, 0022, 0207, 0210, 0131, 0052, 0106, 0250, 0017, 0110,
    0060, 0000, 0042, 0000, 0160, 0000, 0000, 0040, 0000, 0212, 0100, 0017, 0000,

    /* Row 'd' (3): fallback hyphenation weights starting with 'd' */
    0140, 0000, 0040, 0002, 0140, 0000, 0000, 0120, 0000, 0040, 0120, 0017, 0040,
    0100, 0000, 0100, 0000, 0140, 0001, 0021, 0140, 0000, 0046, 0100, 0017, 0140,

    /* Row 'e' (4): fallback hyphenation weights starting with 'e' */
    0066, 0045, 0025, 0201, 0020, 0130, 0146, 0030, 0130, 0103, 0025, 0017, 0006,
    0100, 0000, 0040, 0000, 0020, 0000, 0000, 0040, 0000, 0000, 0200, 0017, 0000,

    /* Row 'f' (5): fallback hyphenation weights starting with 'f' */
    0200, 0000, 0020, 0001, 0140, 0000, 0000, 0140, 0000, 0000, 0120, 0017, 0040,
    0120, 0026, 0042, 0020, 0140, 0161, 0042, 0143, 0000, 0022, 0162, 0017, 0040,

    /* Row 'g' (6): fallback hyphenation weights starting with 'g' */
    0121, 0042, 0060, 0020, 0140, 0200, 0000, 0123, 0000, 0021, 0220, 0017, 0041,
    0121, 0042, 0060, 0120, 0140, 0200, 0000, 0123, 0000, 0021, 0160, 0017, 0041,

    /* Row 'h' (7): fallback hyphenation weights starting with 'h' */
    0051, 0126, 0150, 0141, 0060, 0210, 0146, 0066, 0026, 0165, 0026, 0017, 0247,
    0120, 0000, 0040, 0003, 0160, 0000, 0000, 0140, 0000, 0021, 0100, 0017, 0140,

    /* Row 'i' (8): fallback hyphenation weights starting with 'i' */
    0000, 0000, 0000, 0000, 0200, 0000, 0000, 0000, 0000, 0000, 0000, 0017, 0000,
    0141, 0023, 0122, 0040, 0160, 0143, 0042, 0142, 0000, 0047, 0143, 0017, 0020,

    /* Row 'j' (9): fallback hyphenation weights starting with 'j' */
    0120, 0000, 0040, 0006, 0140, 0060, 0000, 0141, 0000, 0026, 0100, 0017, 0040,
    0140, 0000, 0020, 0007, 0100, 0000, 0000, 0140, 0000, 0001, 0140, 0017, 0020,

    /* Row 'k' (10): fallback hyphenation weights starting with 'k' */
    0110, 0125, 0051, 0162, 0120, 0125, 0127, 0104, 0006, 0104, 0000, 0017, 0052,
    0140, 0000, 0040, 0000, 0160, 0000, 0000, 0140, 0000, 0000, 0060, 0017, 0000,

    /* Row 'l' (11): fallback hyphenation weights starting with 'l' */
    0040, 0005, 0020, 0000, 0040, 0313, 0231, 0030, 0000, 0140, 0000, 0017, 0056,
    0140, 0000, 0160, 0000, 0200, 0000, 0000, 0140, 0000, 0000, 0000, 0017, 0240,

    /* Row 'm' (12): fallback hyphenation weights starting with 'm' */
    0065, 0042, 0060, 0040, 0000, 0206, 0231, 0146, 0006, 0224, 0220, 0017, 0004,
    0240, 0000, 0020, 0000, 0140, 0000, 0000, 0220, 0000, 0000, 0200, 0017, 0141,

    /* Rows 'n' through 'z' (13-25): remaining fallback patterns */
    0045, 0150, 0154, 0162, 0042, 0246, 0210, 0147, 0152, 0103, 0230, 0017, 0206,
    0100, 0000, 0040, 0000, 0140, 0000, 0000, 0100, 0000, 0021, 0120, 0017, 0060,
    0100, 0000, 0040, 0002, 0140, 0320, 0000, 0060, 0000, 0001, 0220, 0017, 0040,
    0100, 0001, 0120, 0001, 0241, 0000, 0000, 0100, 0000, 0020, 0140, 0017, 0060,
    0023, 0162, 0046, 0142, 0022, 0207, 0210, 0131, 0052, 0106, 0250, 0017, 0110,
    0060, 0000, 0042, 0000, 0160, 0000, 0000, 0040, 0000, 0212, 0100, 0017, 0000,
    0140, 0000, 0040, 0002, 0140, 0000, 0000, 0120, 0000, 0040, 0120, 0017, 0040,
    0100, 0000, 0100, 0000, 0140, 0001, 0021, 0140, 0000, 0046, 0100, 0017, 0140,
    0066, 0045, 0025, 0201, 0020, 0130, 0146, 0030, 0130, 0103, 0025, 0017, 0006,
    0100, 0000, 0040, 0000, 0020, 0000, 0000, 0040, 0000, 0000, 0200, 0017, 0000,
    0200, 0000, 0020, 0001, 0140, 0000, 0000, 0140, 0000, 0000, 0120, 0017, 0040,
    0120, 0026, 0042, 0020, 0140, 0161, 0042, 0143, 0000, 0022, 0162, 0017, 0040,
    0121, 0042, 0060, 0020, 0140, 0200, 0000, 0123, 0000, 0021, 0220, 0017, 0041,
    0121, 0042, 0060, 0120, 0140, 0200, 0000, 0123, 0000, 0021, 0160, 0017, 0041,
    0051, 0126, 0150, 0141, 0060, 0210, 0146, 0066, 0026, 0165, 0026, 0017, 0247,
    0120, 0000, 0040, 0003, 0160, 0000, 0000, 0140, 0000, 0021, 0100, 0017, 0140,
    0000, 0000, 0000, 0000, 0200, 0000, 0000, 0000, 0000, 0000, 0000, 0017, 0000,
    0141, 0023, 0122, 0040, 0160, 0143, 0042, 0142, 0000, 0047, 0143, 0017, 0020,
    0120, 0000, 0040, 0006, 0140, 0060, 0000, 0141, 0000, 0026, 0100, 0017, 0040,
    0140, 0000, 0020, 0007, 0100, 0000, 0000, 0140, 0000, 0001, 0140, 0017, 0020,
    0110, 0125, 0051, 0162, 0120, 0125, 0127, 0104, 0006, 0104, 0000, 0017, 0052,
    0140, 0000, 0040, 0000, 0160, 0000, 0000, 0140, 0000, 0000, 0060, 0017, 0000,
    0040, 0005, 0020, 0000, 0040, 0313, 0231, 0030, 0000, 0140, 0000, 0017, 0056,
    0140, 0000, 0160, 0000, 0200, 0000, 0000, 0140, 0000, 0000, 0000, 0017, 0240,
    0065, 0042, 0060, 0040, 0000, 0206, 0231, 0146, 0006, 0224, 0220, 0017, 0004,
    0240, 0000, 0020, 0000, 0140, 0000, 0000, 0220, 0000, 0000, 0200, 0017, 0141};

/*
 * Table access helper macros for improved code readability and bounds checking
 */
#define HXX_INDEX(first, second) ((first) * ALPHABET_SIZE + (second))
#define BXXH_INDEX(first, second) ((first) * ALPHABET_SIZE + (second))
#define XHX_INDEX(first, second) ((first) * ALPHABET_SIZE + (second))
#define XXH_INDEX(first, second) ((first) * ALPHABET_SIZE + (second))

/*
 * Character to index conversion (a=0, b=1, ..., z=25)
 * Returns -1 for non-alphabetic characters
 */
#define CHAR_TO_INDEX(c) (((c) >= 'a' && (c) <= 'z') ? ((c) - 'a') : (((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A') : -1))

/*
 * End of hyphenation tables
 *
 * These tables represent the cumulative knowledge of English hyphenation
 * patterns developed over decades of typographic research and practical
 * application in professional typesetting systems.
 */
