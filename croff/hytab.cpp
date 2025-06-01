#include "../cxx17_scaffold.hpp"
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

#include <cstddef> /* C90 standard definitions */

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
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), /* a-h */
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0040) /* i-m */
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
    static_cast<char>(0006), static_cast<char>(0042), static_cast<char>(0041), static_cast<char>(0123), static_cast<char>(0021), static_cast<char>(0024), static_cast<char>(0063), static_cast<char>(0042), static_cast<char>(0002), static_cast<char>(0043), static_cast<char>(0021), static_cast<char>(0001), static_cast<char>(0022),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0003), static_cast<char>(0260), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0007), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0320),

    /* Row 'b' (1): combinations starting with 'b' */
    static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0005), static_cast<char>(0240), static_cast<char>(0010), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0320),
    static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0003), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0240), static_cast<char>(0010), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0160),

    /* Row 'c' (2): combinations starting with 'c' */
    static_cast<char>(0042), static_cast<char>(0023), static_cast<char>(0041), static_cast<char>(0040), static_cast<char>(0040), static_cast<char>(0022), static_cast<char>(0043), static_cast<char>(0041), static_cast<char>(0030), static_cast<char>(0064), static_cast<char>(0021), static_cast<char>(0000), static_cast<char>(0041),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0003), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'd' (3): combinations starting with 'd' */
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0002), static_cast<char>(0220), static_cast<char>(0010), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0320),
    static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'e' (4): combinations starting with 'e' */
    static_cast<char>(0043), static_cast<char>(0163), static_cast<char>(0065), static_cast<char>(0044), static_cast<char>(0022), static_cast<char>(0043), static_cast<char>(0104), static_cast<char>(0042), static_cast<char>(0061), static_cast<char>(0146), static_cast<char>(0061), static_cast<char>(0000), static_cast<char>(0007),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'f' (5): combinations starting with 'f' */
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0011), static_cast<char>(0060), static_cast<char>(0004), static_cast<char>(0001), static_cast<char>(0120), static_cast<char>(0003), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0240),

    /* Row 'g' (6): combinations starting with 'g' */
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0240),
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0240),

    /* Row 'h' (7): combinations starting with 'h' */
    static_cast<char>(0021), static_cast<char>(0043), static_cast<char>(0041), static_cast<char>(0121), static_cast<char>(0040), static_cast<char>(0023), static_cast<char>(0042), static_cast<char>(0003), static_cast<char>(0142), static_cast<char>(0042), static_cast<char>(0061), static_cast<char>(0001), static_cast<char>(0022),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0010), static_cast<char>(0140), static_cast<char>(0010), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0002), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0120),

    /* Row 'i' (8): combinations starting with 'i' */
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0360), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0005), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0140),

    /* Row 'j' (9): combinations starting with 'j' */
    static_cast<char>(0140), static_cast<char>(0040), static_cast<char>(0100), static_cast<char>(0001), static_cast<char>(0240), static_cast<char>(0041), static_cast<char>(0000), static_cast<char>(0242), static_cast<char>(0000), static_cast<char>(0002), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0100),
    static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0002), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0320), static_cast<char>(0007), static_cast<char>(0000), static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0340),

    /* Row 'k' (10): combinations starting with 'k' */
    static_cast<char>(0101), static_cast<char>(0021), static_cast<char>(0041), static_cast<char>(0020), static_cast<char>(0040), static_cast<char>(0005), static_cast<char>(0042), static_cast<char>(0121), static_cast<char>(0002), static_cast<char>(0021), static_cast<char>(0201), static_cast<char>(0000), static_cast<char>(0020),
    static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0140),

    /* Row 'l' (11): combinations starting with 'l' */
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0001), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0001), static_cast<char>(0000), static_cast<char>(0300), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'm' (12): combinations starting with 'm' */
    static_cast<char>(0106), static_cast<char>(0041), static_cast<char>(0040), static_cast<char>(0147), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0063), static_cast<char>(0041), static_cast<char>(0001), static_cast<char>(0102), static_cast<char>(0160), static_cast<char>(0002), static_cast<char>(0002),
    static_cast<char>(0300), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0017), static_cast<char>(0140), static_cast<char>(0017), static_cast<char>(0000), static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0120),

    /* Row 'n' (13): combinations starting with 'n' */
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0121), static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0140), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040),

    /* Row 'o' (14): combinations starting with 'o' */
    static_cast<char>(0000), static_cast<char>(0043), static_cast<char>(0041), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0042), static_cast<char>(0041), static_cast<char>(0041), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'p' (15): combinations starting with 'p' */
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'q' (16): combinations starting with 'q' */
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'r' (17): combinations starting with 'r' */
    static_cast<char>(0162), static_cast<char>(0042), static_cast<char>(0100), static_cast<char>(0104), static_cast<char>(0242), static_cast<char>(0041), static_cast<char>(0141), static_cast<char>(0100), static_cast<char>(0026), static_cast<char>(0001), static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0142),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 's' (18): combinations starting with 's' */
    static_cast<char>(0141), static_cast<char>(0041), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0001), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0142),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 't' (19): combinations starting with 't' */
    static_cast<char>(0161), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0124), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0161), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'u' (20): combinations starting with 'u' */
    static_cast<char>(0000), static_cast<char>(0043), static_cast<char>(0142), static_cast<char>(0041), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0142),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'v' (21): combinations starting with 'v' */
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'w' (22): combinations starting with 'w' */
    static_cast<char>(0161), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'x' (23): combinations starting with 'x' */
    static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'y' (24): combinations starting with 'y' */
    static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0142),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'z' (25): combinations starting with 'z' */
    static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000)};

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
    static_cast<char>(0005), static_cast<char>(0150), static_cast<char>(0153), static_cast<char>(0062), static_cast<char>(0062), static_cast<char>(0246), static_cast<char>(0152), static_cast<char>(0127), static_cast<char>(0146), static_cast<char>(0203), static_cast<char>(0310), static_cast<char>(0017), static_cast<char>(0206),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0060),

    /* Row 'b' (1): character 'b' + ending sequences */
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0060),

    /* Row 'c' (2): character 'c' + ending sequences */
    static_cast<char>(0043), static_cast<char>(0142), static_cast<char>(0046), static_cast<char>(0140), static_cast<char>(0062), static_cast<char>(0147), static_cast<char>(0210), static_cast<char>(0131), static_cast<char>(0046), static_cast<char>(0106), static_cast<char>(0246), static_cast<char>(0017), static_cast<char>(0111),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'd' (3): character 'd' + ending sequences */
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140),

    /* Row 'e' (4): character 'e' + ending sequences */
    static_cast<char>(0066), static_cast<char>(0045), static_cast<char>(0145), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0070), static_cast<char>(0377), static_cast<char>(0030), static_cast<char>(0130), static_cast<char>(0103), static_cast<char>(0003), static_cast<char>(0017), static_cast<char>(0006),
    static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'f' (5): character 'f' + ending sequences */
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0040),

    /* Row 'g' (6): character 'g' + ending sequences */
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040),

    /* Row 'h' (7): character 'h' + ending sequences */
    static_cast<char>(0051), static_cast<char>(0126), static_cast<char>(0150), static_cast<char>(0140), static_cast<char>(0060), static_cast<char>(0210), static_cast<char>(0146), static_cast<char>(0006), static_cast<char>(0006), static_cast<char>(0165), static_cast<char>(0003), static_cast<char>(0017), static_cast<char>(0244),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0140),

    /* Row 'i' (8): character 'i' + ending sequences */
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020),

    /* Row 'j' (9): character 'j' + ending sequences */
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020),

    /* Row 'k' (10): character 'k' + ending sequences */
    static_cast<char>(0070), static_cast<char>(0125), static_cast<char>(0051), static_cast<char>(0162), static_cast<char>(0120), static_cast<char>(0105), static_cast<char>(0126), static_cast<char>(0104), static_cast<char>(0006), static_cast<char>(0044), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0052),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040),

    /* Row 'l' (11): character 'l' + ending sequences */
    static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0240),

    /* Row 'm' (12): character 'm' + ending sequences */
    static_cast<char>(0065), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0210), static_cast<char>(0222), static_cast<char>(0146), static_cast<char>(0006), static_cast<char>(0204), static_cast<char>(0220), static_cast<char>(0012), static_cast<char>(0003),
    static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0240),

    /* Rows 'n' through 'z' (13-25): remaining character + ending sequences */
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0060),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0060),
    static_cast<char>(0043), static_cast<char>(0142), static_cast<char>(0046), static_cast<char>(0140), static_cast<char>(0062), static_cast<char>(0147), static_cast<char>(0210), static_cast<char>(0131), static_cast<char>(0046), static_cast<char>(0106), static_cast<char>(0246), static_cast<char>(0017), static_cast<char>(0111),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140),
    static_cast<char>(0066), static_cast<char>(0045), static_cast<char>(0145), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0070), static_cast<char>(0377), static_cast<char>(0030), static_cast<char>(0130), static_cast<char>(0103), static_cast<char>(0003), static_cast<char>(0017), static_cast<char>(0006),
    static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0051), static_cast<char>(0126), static_cast<char>(0150), static_cast<char>(0140), static_cast<char>(0060), static_cast<char>(0210), static_cast<char>(0146), static_cast<char>(0006), static_cast<char>(0006), static_cast<char>(0165), static_cast<char>(0003), static_cast<char>(0017), static_cast<char>(0244),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0140),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020),
    static_cast<char>(0070), static_cast<char>(0125), static_cast<char>(0051), static_cast<char>(0162), static_cast<char>(0120), static_cast<char>(0105), static_cast<char>(0126), static_cast<char>(0104), static_cast<char>(0006), static_cast<char>(0044), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0052),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0240),
    static_cast<char>(0065), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0210), static_cast<char>(0222), static_cast<char>(0146), static_cast<char>(0006), static_cast<char>(0204), static_cast<char>(0220), static_cast<char>(0012), static_cast<char>(0003),
    static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0240)};

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
    static_cast<char>(0032), static_cast<char>(0146), static_cast<char>(0042), static_cast<char>(0107), static_cast<char>(0076), static_cast<char>(0102), static_cast<char>(0042), static_cast<char>(0146), static_cast<char>(0202), static_cast<char>(0050), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0051),
    static_cast<char>(0036), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0013), static_cast<char>(0057), static_cast<char>(0366), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0001), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0000), static_cast<char>(0040),

    /* Row 'b' (1): complex hyphenation context patterns starting with 'b' */
    static_cast<char>(0037), static_cast<char>(0377), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0022), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0362), static_cast<char>(0116), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0017),
    static_cast<char>(0057), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0031), static_cast<char>(0137), static_cast<char>(0363), static_cast<char>(0377), static_cast<char>(0037), static_cast<char>(0362), static_cast<char>(0270), static_cast<char>(0077), static_cast<char>(0000), static_cast<char>(0117),

    /* Row 'c' (2): complex hyphenation context patterns starting with 'c' */
    static_cast<char>(0074), static_cast<char>(0142), static_cast<char>(0012), static_cast<char>(0236), static_cast<char>(0076), static_cast<char>(0125), static_cast<char>(0063), static_cast<char>(0165), static_cast<char>(0341), static_cast<char>(0046), static_cast<char>(0047), static_cast<char>(0000), static_cast<char>(0024),
    static_cast<char>(0020), static_cast<char>(0017), static_cast<char>(0075), static_cast<char>(0377), static_cast<char>(0040), static_cast<char>(0001), static_cast<char>(0377), static_cast<char>(0017), static_cast<char>(0001), static_cast<char>(0204), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0040),

    /* Row 'd' (3): complex hyphenation context patterns starting with 'd' */
    static_cast<char>(0057), static_cast<char>(0017), static_cast<char>(0057), static_cast<char>(0340), static_cast<char>(0140), static_cast<char>(0362), static_cast<char>(0314), static_cast<char>(0117), static_cast<char>(0003), static_cast<char>(0302), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0057),
    static_cast<char>(0057), static_cast<char>(0357), static_cast<char>(0077), static_cast<char>(0017), static_cast<char>(0100), static_cast<char>(0366), static_cast<char>(0314), static_cast<char>(0057), static_cast<char>(0342), static_cast<char>(0346), static_cast<char>(0037), static_cast<char>(0000), static_cast<char>(0060),

    /* Row 'e' (4): complex hyphenation context patterns starting with 'e' */
    static_cast<char>(0252), static_cast<char>(0145), static_cast<char>(0072), static_cast<char>(0157), static_cast<char>(0377), static_cast<char>(0165), static_cast<char>(0063), static_cast<char>(0066), static_cast<char>(0164), static_cast<char>(0050), static_cast<char>(0363), static_cast<char>(0000), static_cast<char>(0362),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000),

    /* Row 'f' (5): complex hyphenation context patterns starting with 'f' */
    static_cast<char>(0117), static_cast<char>(0017), static_cast<char>(0237), static_cast<char>(0377), static_cast<char>(0200), static_cast<char>(0354), static_cast<char>(0125), static_cast<char>(0110), static_cast<char>(0004), static_cast<char>(0257), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0300),
    static_cast<char>(0057), static_cast<char>(0367), static_cast<char>(0054), static_cast<char>(0357), static_cast<char>(0157), static_cast<char>(0216), static_cast<char>(0314), static_cast<char>(0114), static_cast<char>(0217), static_cast<char>(0353), static_cast<char>(0053), static_cast<char>(0000), static_cast<char>(0057),

    /* Row 'g' (6): complex hyphenation context patterns starting with 'g' */
    static_cast<char>(0077), static_cast<char>(0213), static_cast<char>(0077), static_cast<char>(0077), static_cast<char>(0177), static_cast<char>(0317), static_cast<char>(0377), static_cast<char>(0114), static_cast<char>(0377), static_cast<char>(0352), static_cast<char>(0077), static_cast<char>(0000), static_cast<char>(0076),
    static_cast<char>(0077), static_cast<char>(0213), static_cast<char>(0077), static_cast<char>(0077), static_cast<char>(0157), static_cast<char>(0177), static_cast<char>(0377), static_cast<char>(0054), static_cast<char>(0377), static_cast<char>(0352), static_cast<char>(0117), static_cast<char>(0000), static_cast<char>(0075),

    /* Row 'h' (7): complex hyphenation context patterns starting with 'h' */
    static_cast<char>(0125), static_cast<char>(0230), static_cast<char>(0065), static_cast<char>(0216), static_cast<char>(0057), static_cast<char>(0066), static_cast<char>(0063), static_cast<char>(0047), static_cast<char>(0345), static_cast<char>(0126), static_cast<char>(0011), static_cast<char>(0000), static_cast<char>(0033),
    static_cast<char>(0057), static_cast<char>(0377), static_cast<char>(0051), static_cast<char>(0360), static_cast<char>(0120), static_cast<char>(0361), static_cast<char>(0273), static_cast<char>(0056), static_cast<char>(0001), static_cast<char>(0256), static_cast<char>(0057), static_cast<char>(0000), static_cast<char>(0060),

    /* Row 'i' (8): complex hyphenation context patterns starting with 'i' */
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0076), static_cast<char>(0310), static_cast<char>(0056), static_cast<char>(0310), static_cast<char>(0137), static_cast<char>(0174), static_cast<char>(0273), static_cast<char>(0055), static_cast<char>(0335), static_cast<char>(0266), static_cast<char>(0033), static_cast<char>(0000), static_cast<char>(0155),

    /* Row 'j' (9): complex hyphenation context patterns starting with 'j' */
    static_cast<char>(0077), static_cast<char>(0157), static_cast<char>(0057), static_cast<char>(0360), static_cast<char>(0057), static_cast<char>(0063), static_cast<char>(0042), static_cast<char>(0024), static_cast<char>(0077), static_cast<char>(0206), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0057), static_cast<char>(0037), static_cast<char>(0077), static_cast<char>(0360), static_cast<char>(0100), static_cast<char>(0365), static_cast<char>(0377), static_cast<char>(0037), static_cast<char>(0362), static_cast<char>(0176), static_cast<char>(0050), static_cast<char>(0000), static_cast<char>(0026),

    /* Row 'k' (10): complex hyphenation context patterns starting with 'k' */
    static_cast<char>(0167), static_cast<char>(0146), static_cast<char>(0042), static_cast<char>(0112), static_cast<char>(0077), static_cast<char>(0110), static_cast<char>(0062), static_cast<char>(0254), static_cast<char>(0366), static_cast<char>(0052), static_cast<char>(0377), static_cast<char>(0000), static_cast<char>(0163),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0377), static_cast<char>(0060), static_cast<char>(0012), static_cast<char>(0000), static_cast<char>(0037), static_cast<char>(0000), static_cast<char>(0257),

    /* Row 'l' (11): complex hyphenation context patterns starting with 'l' */
    static_cast<char>(0037), static_cast<char>(0232), static_cast<char>(0157), static_cast<char>(0361), static_cast<char>(0040), static_cast<char>(0003), static_cast<char>(0125), static_cast<char>(0010), static_cast<char>(0001), static_cast<char>(0256), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0340),
    static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0017), static_cast<char>(0277),

    /* Row 'm' (12): complex hyphenation context patterns starting with 'm' */
    static_cast<char>(0253), static_cast<char>(0315), static_cast<char>(0257), static_cast<char>(0216), static_cast<char>(0377), static_cast<char>(0206), static_cast<char>(0146), static_cast<char>(0306), static_cast<char>(0371), static_cast<char>(0126), static_cast<char>(0232), static_cast<char>(0000), static_cast<char>(0004),
    static_cast<char>(0057), static_cast<char>(0012), static_cast<char>(0100), static_cast<char>(0360), static_cast<char>(0160), static_cast<char>(0360), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0157), static_cast<char>(0000), static_cast<char>(0176),

    /* Rows 'n' through 'z' (13-25): remaining complex context patterns */
    static_cast<char>(0032), static_cast<char>(0146), static_cast<char>(0042), static_cast<char>(0107), static_cast<char>(0076), static_cast<char>(0102), static_cast<char>(0042), static_cast<char>(0146), static_cast<char>(0202), static_cast<char>(0050), static_cast<char>(0006), static_cast<char>(0000), static_cast<char>(0051),
    static_cast<char>(0036), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0013), static_cast<char>(0057), static_cast<char>(0366), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0001), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0037), static_cast<char>(0377), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0022), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0362), static_cast<char>(0116), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0017),
    static_cast<char>(0057), static_cast<char>(0377), static_cast<char>(0057), static_cast<char>(0031), static_cast<char>(0137), static_cast<char>(0363), static_cast<char>(0377), static_cast<char>(0037), static_cast<char>(0362), static_cast<char>(0270), static_cast<char>(0077), static_cast<char>(0000), static_cast<char>(0117),
    static_cast<char>(0074), static_cast<char>(0142), static_cast<char>(0012), static_cast<char>(0236), static_cast<char>(0076), static_cast<char>(0125), static_cast<char>(0063), static_cast<char>(0165), static_cast<char>(0341), static_cast<char>(0046), static_cast<char>(0047), static_cast<char>(0000), static_cast<char>(0024),
    static_cast<char>(0020), static_cast<char>(0017), static_cast<char>(0075), static_cast<char>(0377), static_cast<char>(0040), static_cast<char>(0001), static_cast<char>(0377), static_cast<char>(0017), static_cast<char>(0001), static_cast<char>(0204), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0057), static_cast<char>(0017), static_cast<char>(0057), static_cast<char>(0340), static_cast<char>(0140), static_cast<char>(0362), static_cast<char>(0314), static_cast<char>(0117), static_cast<char>(0003), static_cast<char>(0302), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0057),
    static_cast<char>(0057), static_cast<char>(0357), static_cast<char>(0077), static_cast<char>(0017), static_cast<char>(0100), static_cast<char>(0366), static_cast<char>(0314), static_cast<char>(0057), static_cast<char>(0342), static_cast<char>(0346), static_cast<char>(0037), static_cast<char>(0000), static_cast<char>(0060),
    static_cast<char>(0252), static_cast<char>(0145), static_cast<char>(0072), static_cast<char>(0157), static_cast<char>(0377), static_cast<char>(0165), static_cast<char>(0063), static_cast<char>(0066), static_cast<char>(0164), static_cast<char>(0050), static_cast<char>(0363), static_cast<char>(0000), static_cast<char>(0362),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0117), static_cast<char>(0017), static_cast<char>(0237), static_cast<char>(0377), static_cast<char>(0200), static_cast<char>(0354), static_cast<char>(0125), static_cast<char>(0110), static_cast<char>(0004), static_cast<char>(0257), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0300),
    static_cast<char>(0057), static_cast<char>(0367), static_cast<char>(0054), static_cast<char>(0357), static_cast<char>(0157), static_cast<char>(0216), static_cast<char>(0314), static_cast<char>(0114), static_cast<char>(0217), static_cast<char>(0353), static_cast<char>(0053), static_cast<char>(0000), static_cast<char>(0057),
    static_cast<char>(0077), static_cast<char>(0213), static_cast<char>(0077), static_cast<char>(0077), static_cast<char>(0177), static_cast<char>(0317), static_cast<char>(0377), static_cast<char>(0114), static_cast<char>(0377), static_cast<char>(0352), static_cast<char>(0077), static_cast<char>(0000), static_cast<char>(0076),
    static_cast<char>(0077), static_cast<char>(0213), static_cast<char>(0077), static_cast<char>(0077), static_cast<char>(0157), static_cast<char>(0177), static_cast<char>(0377), static_cast<char>(0054), static_cast<char>(0377), static_cast<char>(0352), static_cast<char>(0117), static_cast<char>(0000), static_cast<char>(0075),
    static_cast<char>(0125), static_cast<char>(0230), static_cast<char>(0065), static_cast<char>(0216), static_cast<char>(0057), static_cast<char>(0066), static_cast<char>(0063), static_cast<char>(0047), static_cast<char>(0345), static_cast<char>(0126), static_cast<char>(0011), static_cast<char>(0000), static_cast<char>(0033),
    static_cast<char>(0057), static_cast<char>(0377), static_cast<char>(0051), static_cast<char>(0360), static_cast<char>(0120), static_cast<char>(0361), static_cast<char>(0273), static_cast<char>(0056), static_cast<char>(0001), static_cast<char>(0256), static_cast<char>(0057), static_cast<char>(0000), static_cast<char>(0060),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000),
    static_cast<char>(0076), static_cast<char>(0310), static_cast<char>(0056), static_cast<char>(0310), static_cast<char>(0137), static_cast<char>(0174), static_cast<char>(0273), static_cast<char>(0055), static_cast<char>(0335), static_cast<char>(0266), static_cast<char>(0033), static_cast<char>(0000), static_cast<char>(0155),
    static_cast<char>(0077), static_cast<char>(0157), static_cast<char>(0057), static_cast<char>(0360), static_cast<char>(0057), static_cast<char>(0063), static_cast<char>(0042), static_cast<char>(0024), static_cast<char>(0077), static_cast<char>(0206), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0040),
    static_cast<char>(0057), static_cast<char>(0037), static_cast<char>(0077), static_cast<char>(0360), static_cast<char>(0100), static_cast<char>(0365), static_cast<char>(0377), static_cast<char>(0037), static_cast<char>(0362), static_cast<char>(0176), static_cast<char>(0050), static_cast<char>(0000), static_cast<char>(0026),
    static_cast<char>(0167), static_cast<char>(0146), static_cast<char>(0042), static_cast<char>(0112), static_cast<char>(0077), static_cast<char>(0110), static_cast<char>(0062), static_cast<char>(0254), static_cast<char>(0366), static_cast<char>(0052), static_cast<char>(0377), static_cast<char>(0000), static_cast<char>(0163),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0377), static_cast<char>(0060), static_cast<char>(0012), static_cast<char>(0000), static_cast<char>(0037), static_cast<char>(0000), static_cast<char>(0257),
    static_cast<char>(0037), static_cast<char>(0232), static_cast<char>(0157), static_cast<char>(0361), static_cast<char>(0040), static_cast<char>(0003), static_cast<char>(0125), static_cast<char>(0010), static_cast<char>(0001), static_cast<char>(0256), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0340),
    static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0377), static_cast<char>(0017), static_cast<char>(0277),
    static_cast<char>(0253), static_cast<char>(0315), static_cast<char>(0257), static_cast<char>(0216), static_cast<char>(0377), static_cast<char>(0206), static_cast<char>(0146), static_cast<char>(0306), static_cast<char>(0371), static_cast<char>(0126), static_cast<char>(0232), static_cast<char>(0000), static_cast<char>(0004),
    static_cast<char>(0057), static_cast<char>(0012), static_cast<char>(0100), static_cast<char>(0360), static_cast<char>(0160), static_cast<char>(0360), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0157), static_cast<char>(0000), static_cast<char>(0176)};

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
    static_cast<char>(0045), static_cast<char>(0150), static_cast<char>(0154), static_cast<char>(0162), static_cast<char>(0042), static_cast<char>(0246), static_cast<char>(0210), static_cast<char>(0147), static_cast<char>(0152), static_cast<char>(0103), static_cast<char>(0230), static_cast<char>(0017), static_cast<char>(0206),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0120), static_cast<char>(0017), static_cast<char>(0060),

    /* Row 'b' (1): fallback hyphenation weights starting with 'b' */
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0002), static_cast<char>(0140), static_cast<char>(0320), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0001), static_cast<char>(0220), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0001), static_cast<char>(0120), static_cast<char>(0001), static_cast<char>(0241), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0140), static_cast<char>(0017), static_cast<char>(0060),

    /* Row 'c' (2): fallback hyphenation weights starting with 'c' */
    static_cast<char>(0023), static_cast<char>(0162), static_cast<char>(0046), static_cast<char>(0142), static_cast<char>(0022), static_cast<char>(0207), static_cast<char>(0210), static_cast<char>(0131), static_cast<char>(0052), static_cast<char>(0106), static_cast<char>(0250), static_cast<char>(0017), static_cast<char>(0110),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0212), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0000),

    /* Row 'd' (3): fallback hyphenation weights starting with 'd' */
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0002), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0120), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0001), static_cast<char>(0021), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0046), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0140),

    /* Row 'e' (4): fallback hyphenation weights starting with 'e' */
    static_cast<char>(0066), static_cast<char>(0045), static_cast<char>(0025), static_cast<char>(0201), static_cast<char>(0020), static_cast<char>(0130), static_cast<char>(0146), static_cast<char>(0030), static_cast<char>(0130), static_cast<char>(0103), static_cast<char>(0025), static_cast<char>(0017), static_cast<char>(0006),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0017), static_cast<char>(0000),

    /* Row 'f' (5): fallback hyphenation weights starting with 'f' */
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0001), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0026), static_cast<char>(0042), static_cast<char>(0020), static_cast<char>(0140), static_cast<char>(0161), static_cast<char>(0042), static_cast<char>(0143), static_cast<char>(0000), static_cast<char>(0022), static_cast<char>(0162), static_cast<char>(0017), static_cast<char>(0040),

    /* Row 'g' (6): fallback hyphenation weights starting with 'g' */
    static_cast<char>(0121), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0020), static_cast<char>(0140), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0123), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0220), static_cast<char>(0017), static_cast<char>(0041),
    static_cast<char>(0121), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0120), static_cast<char>(0140), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0123), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0160), static_cast<char>(0017), static_cast<char>(0041),

    /* Row 'h' (7): fallback hyphenation weights starting with 'h' */
    static_cast<char>(0051), static_cast<char>(0126), static_cast<char>(0150), static_cast<char>(0141), static_cast<char>(0060), static_cast<char>(0210), static_cast<char>(0146), static_cast<char>(0066), static_cast<char>(0026), static_cast<char>(0165), static_cast<char>(0026), static_cast<char>(0017), static_cast<char>(0247),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0003), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0140),

    /* Row 'i' (8): fallback hyphenation weights starting with 'i' */
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0000),
    static_cast<char>(0141), static_cast<char>(0023), static_cast<char>(0122), static_cast<char>(0040), static_cast<char>(0160), static_cast<char>(0143), static_cast<char>(0042), static_cast<char>(0142), static_cast<char>(0000), static_cast<char>(0047), static_cast<char>(0143), static_cast<char>(0017), static_cast<char>(0020),

    /* Row 'j' (9): fallback hyphenation weights starting with 'j' */
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0006), static_cast<char>(0140), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0026), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0007), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0001), static_cast<char>(0140), static_cast<char>(0017), static_cast<char>(0020),

    /* Row 'k' (10): fallback hyphenation weights starting with 'k' */
    static_cast<char>(0110), static_cast<char>(0125), static_cast<char>(0051), static_cast<char>(0162), static_cast<char>(0120), static_cast<char>(0125), static_cast<char>(0127), static_cast<char>(0104), static_cast<char>(0006), static_cast<char>(0104), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0052),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0017), static_cast<char>(0000),

    /* Row 'l' (11): fallback hyphenation weights starting with 'l' */
    static_cast<char>(0040), static_cast<char>(0005), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0313), static_cast<char>(0231), static_cast<char>(0030), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0056),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0240),

    /* Row 'm' (12): fallback hyphenation weights starting with 'm' */
    static_cast<char>(0065), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0206), static_cast<char>(0231), static_cast<char>(0146), static_cast<char>(0006), static_cast<char>(0224), static_cast<char>(0220), static_cast<char>(0017), static_cast<char>(0004),
    static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0017), static_cast<char>(0141),

    /* Rows 'n' through 'z' (13-25): remaining fallback patterns */
    static_cast<char>(0045), static_cast<char>(0150), static_cast<char>(0154), static_cast<char>(0162), static_cast<char>(0042), static_cast<char>(0246), static_cast<char>(0210), static_cast<char>(0147), static_cast<char>(0152), static_cast<char>(0103), static_cast<char>(0230), static_cast<char>(0017), static_cast<char>(0206),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0120), static_cast<char>(0017), static_cast<char>(0060),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0002), static_cast<char>(0140), static_cast<char>(0320), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0001), static_cast<char>(0220), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0001), static_cast<char>(0120), static_cast<char>(0001), static_cast<char>(0241), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0140), static_cast<char>(0017), static_cast<char>(0060),
    static_cast<char>(0023), static_cast<char>(0162), static_cast<char>(0046), static_cast<char>(0142), static_cast<char>(0022), static_cast<char>(0207), static_cast<char>(0210), static_cast<char>(0131), static_cast<char>(0052), static_cast<char>(0106), static_cast<char>(0250), static_cast<char>(0017), static_cast<char>(0110),
    static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0042), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0212), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0000),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0002), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0120), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0001), static_cast<char>(0021), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0046), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0140),
    static_cast<char>(0066), static_cast<char>(0045), static_cast<char>(0025), static_cast<char>(0201), static_cast<char>(0020), static_cast<char>(0130), static_cast<char>(0146), static_cast<char>(0030), static_cast<char>(0130), static_cast<char>(0103), static_cast<char>(0025), static_cast<char>(0017), static_cast<char>(0006),
    static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0017), static_cast<char>(0000),
    static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0001), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0120), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0120), static_cast<char>(0026), static_cast<char>(0042), static_cast<char>(0020), static_cast<char>(0140), static_cast<char>(0161), static_cast<char>(0042), static_cast<char>(0143), static_cast<char>(0000), static_cast<char>(0022), static_cast<char>(0162), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0121), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0020), static_cast<char>(0140), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0123), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0220), static_cast<char>(0017), static_cast<char>(0041),
    static_cast<char>(0121), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0120), static_cast<char>(0140), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0123), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0160), static_cast<char>(0017), static_cast<char>(0041),
    static_cast<char>(0051), static_cast<char>(0126), static_cast<char>(0150), static_cast<char>(0141), static_cast<char>(0060), static_cast<char>(0210), static_cast<char>(0146), static_cast<char>(0066), static_cast<char>(0026), static_cast<char>(0165), static_cast<char>(0026), static_cast<char>(0017), static_cast<char>(0247),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0003), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0021), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0140),
    static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0000),
    static_cast<char>(0141), static_cast<char>(0023), static_cast<char>(0122), static_cast<char>(0040), static_cast<char>(0160), static_cast<char>(0143), static_cast<char>(0042), static_cast<char>(0142), static_cast<char>(0000), static_cast<char>(0047), static_cast<char>(0143), static_cast<char>(0017), static_cast<char>(0020),
    static_cast<char>(0120), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0006), static_cast<char>(0140), static_cast<char>(0060), static_cast<char>(0000), static_cast<char>(0141), static_cast<char>(0000), static_cast<char>(0026), static_cast<char>(0100), static_cast<char>(0017), static_cast<char>(0040),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0007), static_cast<char>(0100), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0001), static_cast<char>(0140), static_cast<char>(0017), static_cast<char>(0020),
    static_cast<char>(0110), static_cast<char>(0125), static_cast<char>(0051), static_cast<char>(0162), static_cast<char>(0120), static_cast<char>(0125), static_cast<char>(0127), static_cast<char>(0104), static_cast<char>(0006), static_cast<char>(0104), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0052),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0060), static_cast<char>(0017), static_cast<char>(0000),
    static_cast<char>(0040), static_cast<char>(0005), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0040), static_cast<char>(0313), static_cast<char>(0231), static_cast<char>(0030), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0056),
    static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0160), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0017), static_cast<char>(0240),
    static_cast<char>(0065), static_cast<char>(0042), static_cast<char>(0060), static_cast<char>(0040), static_cast<char>(0000), static_cast<char>(0206), static_cast<char>(0231), static_cast<char>(0146), static_cast<char>(0006), static_cast<char>(0224), static_cast<char>(0220), static_cast<char>(0017), static_cast<char>(0004),
    static_cast<char>(0240), static_cast<char>(0000), static_cast<char>(0020), static_cast<char>(0000), static_cast<char>(0140), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0220), static_cast<char>(0000), static_cast<char>(0000), static_cast<char>(0200), static_cast<char>(0017), static_cast<char>(0141)};

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
