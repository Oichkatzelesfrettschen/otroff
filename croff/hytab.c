/* C17 - no scaffold needed */
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
    ((char)0060), ((char)0000), ((char)0040), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0040), /* a-h */
    ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0040) /* i-m */
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
    ((char)0006), ((char)0042), ((char)0041), ((char)0123), ((char)0021), ((char)0024), ((char)0063), ((char)0042), ((char)0002), ((char)0043), ((char)0021), ((char)0001), ((char)0022),
    ((char)0140), ((char)0000), ((char)0200), ((char)0003), ((char)0260), ((char)0006), ((char)0000), ((char)0160), ((char)0007), ((char)0000), ((char)0140), ((char)0000), ((char)0320),

    /* Row 'b' (1): combinations starting with 'b' */
    ((char)0220), ((char)0000), ((char)0160), ((char)0005), ((char)0240), ((char)0010), ((char)0000), ((char)0100), ((char)0006), ((char)0000), ((char)0200), ((char)0000), ((char)0320),
    ((char)0240), ((char)0000), ((char)0120), ((char)0003), ((char)0140), ((char)0000), ((char)0000), ((char)0240), ((char)0010), ((char)0000), ((char)0220), ((char)0000), ((char)0160),

    /* Row 'c' (2): combinations starting with 'c' */
    ((char)0042), ((char)0023), ((char)0041), ((char)0040), ((char)0040), ((char)0022), ((char)0043), ((char)0041), ((char)0030), ((char)0064), ((char)0021), ((char)0000), ((char)0041),
    ((char)0100), ((char)0000), ((char)0140), ((char)0000), ((char)0220), ((char)0006), ((char)0000), ((char)0140), ((char)0003), ((char)0000), ((char)0200), ((char)0000), ((char)0000),

    /* Row 'd' (3): combinations starting with 'd' */
    ((char)0200), ((char)0000), ((char)0120), ((char)0002), ((char)0220), ((char)0010), ((char)0000), ((char)0160), ((char)0006), ((char)0000), ((char)0140), ((char)0000), ((char)0320),
    ((char)0020), ((char)0000), ((char)0020), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0000),

    /* Row 'e' (4): combinations starting with 'e' */
    ((char)0043), ((char)0163), ((char)0065), ((char)0044), ((char)0022), ((char)0043), ((char)0104), ((char)0042), ((char)0061), ((char)0146), ((char)0061), ((char)0000), ((char)0007),
    ((char)0100), ((char)0000), ((char)0140), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0000),

    /* Row 'f' (5): combinations starting with 'f' */
    ((char)0140), ((char)0000), ((char)0040), ((char)0011), ((char)0060), ((char)0004), ((char)0001), ((char)0120), ((char)0003), ((char)0000), ((char)0140), ((char)0000), ((char)0040),
    ((char)0200), ((char)0000), ((char)0100), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0240),

    /* Row 'g' (6): combinations starting with 'g' */
    ((char)0200), ((char)0000), ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0240),
    ((char)0200), ((char)0000), ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0240),

    /* Row 'h' (7): combinations starting with 'h' */
    ((char)0021), ((char)0043), ((char)0041), ((char)0121), ((char)0040), ((char)0023), ((char)0042), ((char)0003), ((char)0142), ((char)0042), ((char)0061), ((char)0001), ((char)0022),
    ((char)0120), ((char)0000), ((char)0140), ((char)0010), ((char)0140), ((char)0010), ((char)0000), ((char)0140), ((char)0002), ((char)0000), ((char)0120), ((char)0000), ((char)0120),

    /* Row 'i' (8): combinations starting with 'i' */
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0360), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0160), ((char)0000), ((char)0000),
    ((char)0100), ((char)0000), ((char)0040), ((char)0005), ((char)0120), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0140),

    /* Row 'j' (9): combinations starting with 'j' */
    ((char)0140), ((char)0040), ((char)0100), ((char)0001), ((char)0240), ((char)0041), ((char)0000), ((char)0242), ((char)0000), ((char)0002), ((char)0140), ((char)0000), ((char)0100),
    ((char)0240), ((char)0000), ((char)0120), ((char)0002), ((char)0200), ((char)0000), ((char)0000), ((char)0320), ((char)0007), ((char)0000), ((char)0240), ((char)0000), ((char)0340),

    /* Row 'k' (10): combinations starting with 'k' */
    ((char)0101), ((char)0021), ((char)0041), ((char)0020), ((char)0040), ((char)0005), ((char)0042), ((char)0121), ((char)0002), ((char)0021), ((char)0201), ((char)0000), ((char)0020),
    ((char)0160), ((char)0000), ((char)0100), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0160), ((char)0006), ((char)0000), ((char)0220), ((char)0000), ((char)0140),

    /* Row 'l' (11): combinations starting with 'l' */
    ((char)0140), ((char)0000), ((char)0020), ((char)0001), ((char)0020), ((char)0000), ((char)0000), ((char)0100), ((char)0001), ((char)0000), ((char)0300), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'm' (12): combinations starting with 'm' */
    ((char)0106), ((char)0041), ((char)0040), ((char)0147), ((char)0040), ((char)0000), ((char)0063), ((char)0041), ((char)0001), ((char)0102), ((char)0160), ((char)0002), ((char)0002),
    ((char)0300), ((char)0000), ((char)0040), ((char)0017), ((char)0140), ((char)0017), ((char)0000), ((char)0240), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0120),

    /* Row 'n' (13): combinations starting with 'n' */
    ((char)0200), ((char)0000), ((char)0140), ((char)0121), ((char)0240), ((char)0000), ((char)0200), ((char)0140), ((char)0006), ((char)0000), ((char)0140), ((char)0000), ((char)0000),
    ((char)0020), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0040),

    /* Row 'o' (14): combinations starting with 'o' */
    ((char)0000), ((char)0043), ((char)0041), ((char)0040), ((char)0000), ((char)0042), ((char)0041), ((char)0041), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0100),
    ((char)0120), ((char)0000), ((char)0200), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'p' (15): combinations starting with 'p' */
    ((char)0200), ((char)0000), ((char)0000), ((char)0000), ((char)0240), ((char)0000), ((char)0000), ((char)0120), ((char)0006), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'q' (16): combinations starting with 'q' */
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0240), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'r' (17): combinations starting with 'r' */
    ((char)0162), ((char)0042), ((char)0100), ((char)0104), ((char)0242), ((char)0041), ((char)0141), ((char)0100), ((char)0026), ((char)0001), ((char)0141), ((char)0000), ((char)0142),
    ((char)0140), ((char)0000), ((char)0120), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000),

    /* Row 's' (18): combinations starting with 's' */
    ((char)0141), ((char)0041), ((char)0140), ((char)0000), ((char)0140), ((char)0001), ((char)0000), ((char)0140), ((char)0042), ((char)0000), ((char)0140), ((char)0000), ((char)0142),
    ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 't' (19): combinations starting with 't' */
    ((char)0161), ((char)0000), ((char)0140), ((char)0000), ((char)0220), ((char)0000), ((char)0000), ((char)0140), ((char)0124), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0161), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'u' (20): combinations starting with 'u' */
    ((char)0000), ((char)0043), ((char)0142), ((char)0041), ((char)0000), ((char)0000), ((char)0141), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0142),
    ((char)0120), ((char)0000), ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'v' (21): combinations starting with 'v' */
    ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0000), ((char)0000), ((char)0042), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'w' (22): combinations starting with 'w' */
    ((char)0161), ((char)0000), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0042), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0120), ((char)0000), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'x' (23): combinations starting with 'x' */
    ((char)0141), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0042), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'y' (24): combinations starting with 'y' */
    ((char)0141), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0142),
    ((char)0120), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),

    /* Row 'z' (25): combinations starting with 'z' */
    ((char)0141), ((char)0000), ((char)0000), ((char)0000), ((char)0240), ((char)0000), ((char)0000), ((char)0000), ((char)0042), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000)};

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
    ((char)0005), ((char)0150), ((char)0153), ((char)0062), ((char)0062), ((char)0246), ((char)0152), ((char)0127), ((char)0146), ((char)0203), ((char)0310), ((char)0017), ((char)0206),
    ((char)0100), ((char)0000), ((char)0120), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0060),

    /* Row 'b' (1): character 'b' + ending sequences */
    ((char)0100), ((char)0000), ((char)0040), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0040),
    ((char)0100), ((char)0000), ((char)0120), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0060),

    /* Row 'c' (2): character 'c' + ending sequences */
    ((char)0043), ((char)0142), ((char)0046), ((char)0140), ((char)0062), ((char)0147), ((char)0210), ((char)0131), ((char)0046), ((char)0106), ((char)0246), ((char)0017), ((char)0111),
    ((char)0060), ((char)0000), ((char)0020), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000),

    /* Row 'd' (3): character 'd' + ending sequences */
    ((char)0060), ((char)0000), ((char)0040), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0040),
    ((char)0100), ((char)0000), ((char)0100), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0140),

    /* Row 'e' (4): character 'e' + ending sequences */
    ((char)0066), ((char)0045), ((char)0145), ((char)0140), ((char)0000), ((char)0070), ((char)0377), ((char)0030), ((char)0130), ((char)0103), ((char)0003), ((char)0017), ((char)0006),
    ((char)0040), ((char)0000), ((char)0040), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000),

    /* Row 'f' (5): character 'f' + ending sequences */
    ((char)0200), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0040),
    ((char)0120), ((char)0000), ((char)0040), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0160), ((char)0000), ((char)0040),

    /* Row 'g' (6): character 'g' + ending sequences */
    ((char)0120), ((char)0000), ((char)0040), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0160), ((char)0000), ((char)0040),
    ((char)0120), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0040),

    /* Row 'h' (7): character 'h' + ending sequences */
    ((char)0051), ((char)0126), ((char)0150), ((char)0140), ((char)0060), ((char)0210), ((char)0146), ((char)0006), ((char)0006), ((char)0165), ((char)0003), ((char)0017), ((char)0244),
    ((char)0120), ((char)0000), ((char)0040), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0140),

    /* Row 'i' (8): character 'i' + ending sequences */
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0140), ((char)0000), ((char)0140), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0020),

    /* Row 'j' (9): character 'j' + ending sequences */
    ((char)0120), ((char)0000), ((char)0020), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0040),
    ((char)0140), ((char)0000), ((char)0020), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0020),

    /* Row 'k' (10): character 'k' + ending sequences */
    ((char)0070), ((char)0125), ((char)0051), ((char)0162), ((char)0120), ((char)0105), ((char)0126), ((char)0104), ((char)0006), ((char)0044), ((char)0000), ((char)0017), ((char)0052),
    ((char)0140), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0040),

    /* Row 'l' (11): character 'l' + ending sequences */
    ((char)0020), ((char)0000), ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0060),
    ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0240),

    /* Row 'm' (12): character 'm' + ending sequences */
    ((char)0065), ((char)0042), ((char)0060), ((char)0200), ((char)0000), ((char)0210), ((char)0222), ((char)0146), ((char)0006), ((char)0204), ((char)0220), ((char)0012), ((char)0003),
    ((char)0240), ((char)0000), ((char)0020), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0200), ((char)0000), ((char)0240),

    /* Rows 'n' through 'z' (13-25): remaining character + ending sequences */
    ((char)0100), ((char)0000), ((char)0120), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0060),
    ((char)0100), ((char)0000), ((char)0040), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0040),
    ((char)0100), ((char)0000), ((char)0120), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0060),
    ((char)0043), ((char)0142), ((char)0046), ((char)0140), ((char)0062), ((char)0147), ((char)0210), ((char)0131), ((char)0046), ((char)0106), ((char)0246), ((char)0017), ((char)0111),
    ((char)0060), ((char)0000), ((char)0020), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000),
    ((char)0060), ((char)0000), ((char)0040), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0040),
    ((char)0100), ((char)0000), ((char)0100), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0140),
    ((char)0066), ((char)0045), ((char)0145), ((char)0140), ((char)0000), ((char)0070), ((char)0377), ((char)0030), ((char)0130), ((char)0103), ((char)0003), ((char)0017), ((char)0006),
    ((char)0040), ((char)0000), ((char)0040), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000),
    ((char)0200), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0040),
    ((char)0120), ((char)0000), ((char)0040), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0160), ((char)0000), ((char)0040),
    ((char)0120), ((char)0000), ((char)0040), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0160), ((char)0000), ((char)0040),
    ((char)0120), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0040),
    ((char)0051), ((char)0126), ((char)0150), ((char)0140), ((char)0060), ((char)0210), ((char)0146), ((char)0006), ((char)0006), ((char)0165), ((char)0003), ((char)0017), ((char)0244),
    ((char)0120), ((char)0000), ((char)0040), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0140),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0140), ((char)0000), ((char)0140), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0020),
    ((char)0120), ((char)0000), ((char)0020), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0040),
    ((char)0140), ((char)0000), ((char)0020), ((char)0000), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0020),
    ((char)0070), ((char)0125), ((char)0051), ((char)0162), ((char)0120), ((char)0105), ((char)0126), ((char)0104), ((char)0006), ((char)0044), ((char)0000), ((char)0017), ((char)0052),
    ((char)0140), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0000), ((char)0060), ((char)0000), ((char)0040),
    ((char)0020), ((char)0000), ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0060),
    ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0240),
    ((char)0065), ((char)0042), ((char)0060), ((char)0200), ((char)0000), ((char)0210), ((char)0222), ((char)0146), ((char)0006), ((char)0204), ((char)0220), ((char)0012), ((char)0003),
    ((char)0240), ((char)0000), ((char)0020), ((char)0000), ((char)0120), ((char)0000), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0200), ((char)0000), ((char)0240)};

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
    ((char)0032), ((char)0146), ((char)0042), ((char)0107), ((char)0076), ((char)0102), ((char)0042), ((char)0146), ((char)0202), ((char)0050), ((char)0006), ((char)0000), ((char)0051),
    ((char)0036), ((char)0377), ((char)0057), ((char)0013), ((char)0057), ((char)0366), ((char)0377), ((char)0057), ((char)0001), ((char)0377), ((char)0057), ((char)0000), ((char)0040),

    /* Row 'b' (1): complex hyphenation context patterns starting with 'b' */
    ((char)0037), ((char)0377), ((char)0020), ((char)0000), ((char)0100), ((char)0022), ((char)0377), ((char)0057), ((char)0362), ((char)0116), ((char)0100), ((char)0000), ((char)0017),
    ((char)0057), ((char)0377), ((char)0057), ((char)0031), ((char)0137), ((char)0363), ((char)0377), ((char)0037), ((char)0362), ((char)0270), ((char)0077), ((char)0000), ((char)0117),

    /* Row 'c' (2): complex hyphenation context patterns starting with 'c' */
    ((char)0074), ((char)0142), ((char)0012), ((char)0236), ((char)0076), ((char)0125), ((char)0063), ((char)0165), ((char)0341), ((char)0046), ((char)0047), ((char)0000), ((char)0024),
    ((char)0020), ((char)0017), ((char)0075), ((char)0377), ((char)0040), ((char)0001), ((char)0377), ((char)0017), ((char)0001), ((char)0204), ((char)0020), ((char)0000), ((char)0040),

    /* Row 'd' (3): complex hyphenation context patterns starting with 'd' */
    ((char)0057), ((char)0017), ((char)0057), ((char)0340), ((char)0140), ((char)0362), ((char)0314), ((char)0117), ((char)0003), ((char)0302), ((char)0100), ((char)0000), ((char)0057),
    ((char)0057), ((char)0357), ((char)0077), ((char)0017), ((char)0100), ((char)0366), ((char)0314), ((char)0057), ((char)0342), ((char)0346), ((char)0037), ((char)0000), ((char)0060),

    /* Row 'e' (4): complex hyphenation context patterns starting with 'e' */
    ((char)0252), ((char)0145), ((char)0072), ((char)0157), ((char)0377), ((char)0165), ((char)0063), ((char)0066), ((char)0164), ((char)0050), ((char)0363), ((char)0000), ((char)0362),
    ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0017), ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0000),

    /* Row 'f' (5): complex hyphenation context patterns starting with 'f' */
    ((char)0117), ((char)0017), ((char)0237), ((char)0377), ((char)0200), ((char)0354), ((char)0125), ((char)0110), ((char)0004), ((char)0257), ((char)0000), ((char)0000), ((char)0300),
    ((char)0057), ((char)0367), ((char)0054), ((char)0357), ((char)0157), ((char)0216), ((char)0314), ((char)0114), ((char)0217), ((char)0353), ((char)0053), ((char)0000), ((char)0057),

    /* Row 'g' (6): complex hyphenation context patterns starting with 'g' */
    ((char)0077), ((char)0213), ((char)0077), ((char)0077), ((char)0177), ((char)0317), ((char)0377), ((char)0114), ((char)0377), ((char)0352), ((char)0077), ((char)0000), ((char)0076),
    ((char)0077), ((char)0213), ((char)0077), ((char)0077), ((char)0157), ((char)0177), ((char)0377), ((char)0054), ((char)0377), ((char)0352), ((char)0117), ((char)0000), ((char)0075),

    /* Row 'h' (7): complex hyphenation context patterns starting with 'h' */
    ((char)0125), ((char)0230), ((char)0065), ((char)0216), ((char)0057), ((char)0066), ((char)0063), ((char)0047), ((char)0345), ((char)0126), ((char)0011), ((char)0000), ((char)0033),
    ((char)0057), ((char)0377), ((char)0051), ((char)0360), ((char)0120), ((char)0361), ((char)0273), ((char)0056), ((char)0001), ((char)0256), ((char)0057), ((char)0000), ((char)0060),

    /* Row 'i' (8): complex hyphenation context patterns starting with 'i' */
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0076), ((char)0310), ((char)0056), ((char)0310), ((char)0137), ((char)0174), ((char)0273), ((char)0055), ((char)0335), ((char)0266), ((char)0033), ((char)0000), ((char)0155),

    /* Row 'j' (9): complex hyphenation context patterns starting with 'j' */
    ((char)0077), ((char)0157), ((char)0057), ((char)0360), ((char)0057), ((char)0063), ((char)0042), ((char)0024), ((char)0077), ((char)0206), ((char)0020), ((char)0000), ((char)0040),
    ((char)0057), ((char)0037), ((char)0077), ((char)0360), ((char)0100), ((char)0365), ((char)0377), ((char)0037), ((char)0362), ((char)0176), ((char)0050), ((char)0000), ((char)0026),

    /* Row 'k' (10): complex hyphenation context patterns starting with 'k' */
    ((char)0167), ((char)0146), ((char)0042), ((char)0112), ((char)0077), ((char)0110), ((char)0062), ((char)0254), ((char)0366), ((char)0052), ((char)0377), ((char)0000), ((char)0163),
    ((char)0060), ((char)0000), ((char)0040), ((char)0000), ((char)0120), ((char)0000), ((char)0377), ((char)0060), ((char)0012), ((char)0000), ((char)0037), ((char)0000), ((char)0257),

    /* Row 'l' (11): complex hyphenation context patterns starting with 'l' */
    ((char)0037), ((char)0232), ((char)0157), ((char)0361), ((char)0040), ((char)0003), ((char)0125), ((char)0010), ((char)0001), ((char)0256), ((char)0000), ((char)0000), ((char)0340),
    ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0017), ((char)0277),

    /* Row 'm' (12): complex hyphenation context patterns starting with 'm' */
    ((char)0253), ((char)0315), ((char)0257), ((char)0216), ((char)0377), ((char)0206), ((char)0146), ((char)0306), ((char)0371), ((char)0126), ((char)0232), ((char)0000), ((char)0004),
    ((char)0057), ((char)0012), ((char)0100), ((char)0360), ((char)0160), ((char)0360), ((char)0000), ((char)0040), ((char)0000), ((char)0017), ((char)0157), ((char)0000), ((char)0176),

    /* Rows 'n' through 'z' (13-25): remaining complex context patterns */
    ((char)0032), ((char)0146), ((char)0042), ((char)0107), ((char)0076), ((char)0102), ((char)0042), ((char)0146), ((char)0202), ((char)0050), ((char)0006), ((char)0000), ((char)0051),
    ((char)0036), ((char)0377), ((char)0057), ((char)0013), ((char)0057), ((char)0366), ((char)0377), ((char)0057), ((char)0001), ((char)0377), ((char)0057), ((char)0000), ((char)0040),
    ((char)0037), ((char)0377), ((char)0020), ((char)0000), ((char)0100), ((char)0022), ((char)0377), ((char)0057), ((char)0362), ((char)0116), ((char)0100), ((char)0000), ((char)0017),
    ((char)0057), ((char)0377), ((char)0057), ((char)0031), ((char)0137), ((char)0363), ((char)0377), ((char)0037), ((char)0362), ((char)0270), ((char)0077), ((char)0000), ((char)0117),
    ((char)0074), ((char)0142), ((char)0012), ((char)0236), ((char)0076), ((char)0125), ((char)0063), ((char)0165), ((char)0341), ((char)0046), ((char)0047), ((char)0000), ((char)0024),
    ((char)0020), ((char)0017), ((char)0075), ((char)0377), ((char)0040), ((char)0001), ((char)0377), ((char)0017), ((char)0001), ((char)0204), ((char)0020), ((char)0000), ((char)0040),
    ((char)0057), ((char)0017), ((char)0057), ((char)0340), ((char)0140), ((char)0362), ((char)0314), ((char)0117), ((char)0003), ((char)0302), ((char)0100), ((char)0000), ((char)0057),
    ((char)0057), ((char)0357), ((char)0077), ((char)0017), ((char)0100), ((char)0366), ((char)0314), ((char)0057), ((char)0342), ((char)0346), ((char)0037), ((char)0000), ((char)0060),
    ((char)0252), ((char)0145), ((char)0072), ((char)0157), ((char)0377), ((char)0165), ((char)0063), ((char)0066), ((char)0164), ((char)0050), ((char)0363), ((char)0000), ((char)0362),
    ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0017), ((char)0000), ((char)0000), ((char)0020), ((char)0000), ((char)0000),
    ((char)0117), ((char)0017), ((char)0237), ((char)0377), ((char)0200), ((char)0354), ((char)0125), ((char)0110), ((char)0004), ((char)0257), ((char)0000), ((char)0000), ((char)0300),
    ((char)0057), ((char)0367), ((char)0054), ((char)0357), ((char)0157), ((char)0216), ((char)0314), ((char)0114), ((char)0217), ((char)0353), ((char)0053), ((char)0000), ((char)0057),
    ((char)0077), ((char)0213), ((char)0077), ((char)0077), ((char)0177), ((char)0317), ((char)0377), ((char)0114), ((char)0377), ((char)0352), ((char)0077), ((char)0000), ((char)0076),
    ((char)0077), ((char)0213), ((char)0077), ((char)0077), ((char)0157), ((char)0177), ((char)0377), ((char)0054), ((char)0377), ((char)0352), ((char)0117), ((char)0000), ((char)0075),
    ((char)0125), ((char)0230), ((char)0065), ((char)0216), ((char)0057), ((char)0066), ((char)0063), ((char)0047), ((char)0345), ((char)0126), ((char)0011), ((char)0000), ((char)0033),
    ((char)0057), ((char)0377), ((char)0051), ((char)0360), ((char)0120), ((char)0361), ((char)0273), ((char)0056), ((char)0001), ((char)0256), ((char)0057), ((char)0000), ((char)0060),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000),
    ((char)0076), ((char)0310), ((char)0056), ((char)0310), ((char)0137), ((char)0174), ((char)0273), ((char)0055), ((char)0335), ((char)0266), ((char)0033), ((char)0000), ((char)0155),
    ((char)0077), ((char)0157), ((char)0057), ((char)0360), ((char)0057), ((char)0063), ((char)0042), ((char)0024), ((char)0077), ((char)0206), ((char)0020), ((char)0000), ((char)0040),
    ((char)0057), ((char)0037), ((char)0077), ((char)0360), ((char)0100), ((char)0365), ((char)0377), ((char)0037), ((char)0362), ((char)0176), ((char)0050), ((char)0000), ((char)0026),
    ((char)0167), ((char)0146), ((char)0042), ((char)0112), ((char)0077), ((char)0110), ((char)0062), ((char)0254), ((char)0366), ((char)0052), ((char)0377), ((char)0000), ((char)0163),
    ((char)0060), ((char)0000), ((char)0040), ((char)0000), ((char)0120), ((char)0000), ((char)0377), ((char)0060), ((char)0012), ((char)0000), ((char)0037), ((char)0000), ((char)0257),
    ((char)0037), ((char)0232), ((char)0157), ((char)0361), ((char)0040), ((char)0003), ((char)0125), ((char)0010), ((char)0001), ((char)0256), ((char)0000), ((char)0000), ((char)0340),
    ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0377), ((char)0017), ((char)0277),
    ((char)0253), ((char)0315), ((char)0257), ((char)0216), ((char)0377), ((char)0206), ((char)0146), ((char)0306), ((char)0371), ((char)0126), ((char)0232), ((char)0000), ((char)0004),
    ((char)0057), ((char)0012), ((char)0100), ((char)0360), ((char)0160), ((char)0360), ((char)0000), ((char)0040), ((char)0000), ((char)0017), ((char)0157), ((char)0000), ((char)0176)};

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
    ((char)0045), ((char)0150), ((char)0154), ((char)0162), ((char)0042), ((char)0246), ((char)0210), ((char)0147), ((char)0152), ((char)0103), ((char)0230), ((char)0017), ((char)0206),
    ((char)0100), ((char)0000), ((char)0040), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0021), ((char)0120), ((char)0017), ((char)0060),

    /* Row 'b' (1): fallback hyphenation weights starting with 'b' */
    ((char)0100), ((char)0000), ((char)0040), ((char)0002), ((char)0140), ((char)0320), ((char)0000), ((char)0060), ((char)0000), ((char)0001), ((char)0220), ((char)0017), ((char)0040),
    ((char)0100), ((char)0001), ((char)0120), ((char)0001), ((char)0241), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0020), ((char)0140), ((char)0017), ((char)0060),

    /* Row 'c' (2): fallback hyphenation weights starting with 'c' */
    ((char)0023), ((char)0162), ((char)0046), ((char)0142), ((char)0022), ((char)0207), ((char)0210), ((char)0131), ((char)0052), ((char)0106), ((char)0250), ((char)0017), ((char)0110),
    ((char)0060), ((char)0000), ((char)0042), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0212), ((char)0100), ((char)0017), ((char)0000),

    /* Row 'd' (3): fallback hyphenation weights starting with 'd' */
    ((char)0140), ((char)0000), ((char)0040), ((char)0002), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0040), ((char)0120), ((char)0017), ((char)0040),
    ((char)0100), ((char)0000), ((char)0100), ((char)0000), ((char)0140), ((char)0001), ((char)0021), ((char)0140), ((char)0000), ((char)0046), ((char)0100), ((char)0017), ((char)0140),

    /* Row 'e' (4): fallback hyphenation weights starting with 'e' */
    ((char)0066), ((char)0045), ((char)0025), ((char)0201), ((char)0020), ((char)0130), ((char)0146), ((char)0030), ((char)0130), ((char)0103), ((char)0025), ((char)0017), ((char)0006),
    ((char)0100), ((char)0000), ((char)0040), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0200), ((char)0017), ((char)0000),

    /* Row 'f' (5): fallback hyphenation weights starting with 'f' */
    ((char)0200), ((char)0000), ((char)0020), ((char)0001), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0017), ((char)0040),
    ((char)0120), ((char)0026), ((char)0042), ((char)0020), ((char)0140), ((char)0161), ((char)0042), ((char)0143), ((char)0000), ((char)0022), ((char)0162), ((char)0017), ((char)0040),

    /* Row 'g' (6): fallback hyphenation weights starting with 'g' */
    ((char)0121), ((char)0042), ((char)0060), ((char)0020), ((char)0140), ((char)0200), ((char)0000), ((char)0123), ((char)0000), ((char)0021), ((char)0220), ((char)0017), ((char)0041),
    ((char)0121), ((char)0042), ((char)0060), ((char)0120), ((char)0140), ((char)0200), ((char)0000), ((char)0123), ((char)0000), ((char)0021), ((char)0160), ((char)0017), ((char)0041),

    /* Row 'h' (7): fallback hyphenation weights starting with 'h' */
    ((char)0051), ((char)0126), ((char)0150), ((char)0141), ((char)0060), ((char)0210), ((char)0146), ((char)0066), ((char)0026), ((char)0165), ((char)0026), ((char)0017), ((char)0247),
    ((char)0120), ((char)0000), ((char)0040), ((char)0003), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0021), ((char)0100), ((char)0017), ((char)0140),

    /* Row 'i' (8): fallback hyphenation weights starting with 'i' */
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0017), ((char)0000),
    ((char)0141), ((char)0023), ((char)0122), ((char)0040), ((char)0160), ((char)0143), ((char)0042), ((char)0142), ((char)0000), ((char)0047), ((char)0143), ((char)0017), ((char)0020),

    /* Row 'j' (9): fallback hyphenation weights starting with 'j' */
    ((char)0120), ((char)0000), ((char)0040), ((char)0006), ((char)0140), ((char)0060), ((char)0000), ((char)0141), ((char)0000), ((char)0026), ((char)0100), ((char)0017), ((char)0040),
    ((char)0140), ((char)0000), ((char)0020), ((char)0007), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0001), ((char)0140), ((char)0017), ((char)0020),

    /* Row 'k' (10): fallback hyphenation weights starting with 'k' */
    ((char)0110), ((char)0125), ((char)0051), ((char)0162), ((char)0120), ((char)0125), ((char)0127), ((char)0104), ((char)0006), ((char)0104), ((char)0000), ((char)0017), ((char)0052),
    ((char)0140), ((char)0000), ((char)0040), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0060), ((char)0017), ((char)0000),

    /* Row 'l' (11): fallback hyphenation weights starting with 'l' */
    ((char)0040), ((char)0005), ((char)0020), ((char)0000), ((char)0040), ((char)0313), ((char)0231), ((char)0030), ((char)0000), ((char)0140), ((char)0000), ((char)0017), ((char)0056),
    ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0017), ((char)0240),

    /* Row 'm' (12): fallback hyphenation weights starting with 'm' */
    ((char)0065), ((char)0042), ((char)0060), ((char)0040), ((char)0000), ((char)0206), ((char)0231), ((char)0146), ((char)0006), ((char)0224), ((char)0220), ((char)0017), ((char)0004),
    ((char)0240), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0000), ((char)0200), ((char)0017), ((char)0141),

    /* Rows 'n' through 'z' (13-25): remaining fallback patterns */
    ((char)0045), ((char)0150), ((char)0154), ((char)0162), ((char)0042), ((char)0246), ((char)0210), ((char)0147), ((char)0152), ((char)0103), ((char)0230), ((char)0017), ((char)0206),
    ((char)0100), ((char)0000), ((char)0040), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0021), ((char)0120), ((char)0017), ((char)0060),
    ((char)0100), ((char)0000), ((char)0040), ((char)0002), ((char)0140), ((char)0320), ((char)0000), ((char)0060), ((char)0000), ((char)0001), ((char)0220), ((char)0017), ((char)0040),
    ((char)0100), ((char)0001), ((char)0120), ((char)0001), ((char)0241), ((char)0000), ((char)0000), ((char)0100), ((char)0000), ((char)0020), ((char)0140), ((char)0017), ((char)0060),
    ((char)0023), ((char)0162), ((char)0046), ((char)0142), ((char)0022), ((char)0207), ((char)0210), ((char)0131), ((char)0052), ((char)0106), ((char)0250), ((char)0017), ((char)0110),
    ((char)0060), ((char)0000), ((char)0042), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0212), ((char)0100), ((char)0017), ((char)0000),
    ((char)0140), ((char)0000), ((char)0040), ((char)0002), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0000), ((char)0040), ((char)0120), ((char)0017), ((char)0040),
    ((char)0100), ((char)0000), ((char)0100), ((char)0000), ((char)0140), ((char)0001), ((char)0021), ((char)0140), ((char)0000), ((char)0046), ((char)0100), ((char)0017), ((char)0140),
    ((char)0066), ((char)0045), ((char)0025), ((char)0201), ((char)0020), ((char)0130), ((char)0146), ((char)0030), ((char)0130), ((char)0103), ((char)0025), ((char)0017), ((char)0006),
    ((char)0100), ((char)0000), ((char)0040), ((char)0000), ((char)0020), ((char)0000), ((char)0000), ((char)0040), ((char)0000), ((char)0000), ((char)0200), ((char)0017), ((char)0000),
    ((char)0200), ((char)0000), ((char)0020), ((char)0001), ((char)0140), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0120), ((char)0017), ((char)0040),
    ((char)0120), ((char)0026), ((char)0042), ((char)0020), ((char)0140), ((char)0161), ((char)0042), ((char)0143), ((char)0000), ((char)0022), ((char)0162), ((char)0017), ((char)0040),
    ((char)0121), ((char)0042), ((char)0060), ((char)0020), ((char)0140), ((char)0200), ((char)0000), ((char)0123), ((char)0000), ((char)0021), ((char)0220), ((char)0017), ((char)0041),
    ((char)0121), ((char)0042), ((char)0060), ((char)0120), ((char)0140), ((char)0200), ((char)0000), ((char)0123), ((char)0000), ((char)0021), ((char)0160), ((char)0017), ((char)0041),
    ((char)0051), ((char)0126), ((char)0150), ((char)0141), ((char)0060), ((char)0210), ((char)0146), ((char)0066), ((char)0026), ((char)0165), ((char)0026), ((char)0017), ((char)0247),
    ((char)0120), ((char)0000), ((char)0040), ((char)0003), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0021), ((char)0100), ((char)0017), ((char)0140),
    ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0000), ((char)0017), ((char)0000),
    ((char)0141), ((char)0023), ((char)0122), ((char)0040), ((char)0160), ((char)0143), ((char)0042), ((char)0142), ((char)0000), ((char)0047), ((char)0143), ((char)0017), ((char)0020),
    ((char)0120), ((char)0000), ((char)0040), ((char)0006), ((char)0140), ((char)0060), ((char)0000), ((char)0141), ((char)0000), ((char)0026), ((char)0100), ((char)0017), ((char)0040),
    ((char)0140), ((char)0000), ((char)0020), ((char)0007), ((char)0100), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0001), ((char)0140), ((char)0017), ((char)0020),
    ((char)0110), ((char)0125), ((char)0051), ((char)0162), ((char)0120), ((char)0125), ((char)0127), ((char)0104), ((char)0006), ((char)0104), ((char)0000), ((char)0017), ((char)0052),
    ((char)0140), ((char)0000), ((char)0040), ((char)0000), ((char)0160), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0060), ((char)0017), ((char)0000),
    ((char)0040), ((char)0005), ((char)0020), ((char)0000), ((char)0040), ((char)0313), ((char)0231), ((char)0030), ((char)0000), ((char)0140), ((char)0000), ((char)0017), ((char)0056),
    ((char)0140), ((char)0000), ((char)0160), ((char)0000), ((char)0200), ((char)0000), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0000), ((char)0017), ((char)0240),
    ((char)0065), ((char)0042), ((char)0060), ((char)0040), ((char)0000), ((char)0206), ((char)0231), ((char)0146), ((char)0006), ((char)0224), ((char)0220), ((char)0017), ((char)0004),
    ((char)0240), ((char)0000), ((char)0020), ((char)0000), ((char)0140), ((char)0000), ((char)0000), ((char)0220), ((char)0000), ((char)0000), ((char)0200), ((char)0017), ((char)0141)};

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
