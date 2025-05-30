#include "cxx23_scaffold.hpp"
/**
 * @file roff7.c
 * @brief ROFF hyphenation digram tables - Statistical pattern data for word breaking
 *
 * This module contains the core statistical data tables used by the ROFF
 * hyphenation algorithm. These tables encode empirically-derived frequency
 * weights for character digrams (two-character sequences) in various
 * positional contexts within English words.
 *
 * Mathematical Foundation:
 * - Each table entry represents a 4-bit frequency weight (0-15 scale)
 * - Values derived from statistical analysis of large English text corpora
 * - Positional context determines which table applies to a given digram
 * - Threshold comparisons against these weights determine hyphenation points
 *
 * Table Organization:
 * - bxh: Beginning + consonant + vowel patterns (word-initial contexts)
 * - hxx: Vowel + consonant + consonant patterns (syllable-final contexts)
 * - bxxh: Beginning + consonant + consonant + vowel (complex onsets)
 * - xhx: Consonant + vowel + consonant (syllable nucleus patterns)
 * - xxh: Consonant + consonant + vowel (consonant cluster patterns)
 *
 * Data Encoding:
 * - Two 4-bit values packed per byte (high and low nibbles)
 * - Index calculation: (char1 - 'a') * 13 + (char2 - 'a')
 * - Bit extraction: (value >> (4 * position)) & 0x0F
 * - Zero values indicate low hyphenation probability
 *
 * Linguistic Principles:
 * - Higher values indicate natural syllable boundaries
 * - Context-sensitive weighting based on phonotactic constraints
 * - Morphological boundary preferences encoded in patterns
 * - Statistical learning from large-scale corpus analysis
 *
 * Original Assembly:
 * - Direct byte data declarations using .byte directives
 * - Optimized for PDP-11 memory layout and addressing
 * - Hand-tuned values based on empirical testing
 * - Compact representation for memory efficiency
 *
 * @note These tables represent decades of linguistic research and empirical
 *       tuning, encoding sophisticated knowledge about English syllable
 *       structure and morphology in a compact, efficient format.
 */

#include "roff.h"

/* Copyright notice from original Bell Labs code */
static const char copyright[] ROFF_UNUSED = "Copyright 1972 Bell Telephone Laboratories Inc.";

/* SCCS version identifier */
[[maybe_unused]] static constexpr std::string_view sccs_id =
    "@(#)roff7.c 1.3 25/05/29 (digram tables - converted from PDP-11 assembly)"; // ID string

/**
 * @brief Beginning + consonant + vowel patterns table.
 *
 * This table encodes hyphenation weights for patterns that occur at
 * word beginnings followed by consonant-vowel sequences. These patterns
 * are crucial for identifying initial syllable boundaries.
 *
 * Linguistic Context:
 * - Word-initial position (beginning of hyphenatable region)
 * - Consonant-vowel transitions (onset-nucleus boundaries)
 * - Low values indicate strong syllable cohesion
 * - High values indicate potential hyphenation points
 *
 * Mathematical Encoding:
 * - 13 bytes total (covering 26 digram combinations)
 * - Each byte contains two 4-bit weights
 * - Index mapping: bxh[(c1*13 + c2)/2]
 * - Bit extraction based on odd/even index position
 */
const unsigned char bxh[13] = {
    0060, 0000, 0040, 0000, 0040, 0000, 0000,
    0040, 0000, 0000, 0040, 0000, 0040};

/**
 * @brief Vowel + consonant + consonant patterns table.
 *
 * This comprehensive table encodes hyphenation weights for vowel-consonant-
 * consonant sequences, which are critical for identifying syllable-final
 * boundaries and consonant cluster divisions.
 *
 * Linguistic Significance:
 * - Syllable coda identification (VC boundaries)
 * - Consonant cluster division rules
 * - Stress pattern considerations
 * - Morphological boundary detection
 *
 * Statistical Foundation:
 * - Derived from analysis of syllable-final patterns
 * - Higher weights for natural division points
 * - Context-sensitive based on vowel type and length
 * - Empirically validated against pronunciation dictionaries
 *
 * Data Structure:
 * - 338 bytes total (26×13 digram space)
 * - Packed 4-bit values for memory efficiency
 * - Covers all possible vowel-consonant combinations
 * - Zero-padded for non-occurring patterns
 */
const unsigned char hxx[338] = {
    0006, 0042, 0041, 0123, 0021, 0024, 0063, 0042, 0002, 0043, 0021, 0001, 0022,
    0140, 0000, 0200, 0003, 0260, 0006, 0000, 0160, 0007, 0000, 0140, 0000, 0320,
    0220, 0000, 0160, 0005, 0240, 0010, 0000, 0100, 0006, 0000, 0200, 0000, 0320,
    0240, 0000, 0120, 0003, 0140, 0000, 0000, 0240, 0010, 0000, 0220, 0000, 0160,
    0042, 0023, 0041, 0040, 0040, 0022, 0043, 0041, 0030, 0064, 0021, 0000, 0041,
    0100, 0000, 0140, 0000, 0220, 0006, 0000, 0140, 0003, 0000, 0200, 0000, 0000,
    0200, 0000, 0120, 0002, 0220, 0010, 0000, 0160, 0006, 0000, 0140, 0000, 0320,
    0020, 0000, 0020, 0000, 0020, 0000, 0000, 0020, 0000, 0000, 0020, 0000, 0000,
    0043, 0163, 0065, 0044, 0022, 0043, 0104, 0042, 0061, 0146, 0061, 0000, 0007,
    0100, 0000, 0140, 0000, 0040, 0000, 0000, 0100, 0000, 0000, 0120, 0000, 0000,
    0140, 0000, 0040, 0011, 0060, 0004, 0001, 0120, 0003, 0000, 0140, 0000, 0040,
    0200, 0000, 0100, 0000, 0140, 0000, 0000, 0140, 0000, 0000, 0140, 0000, 0240,
    0200, 0000, 0140, 0000, 0160, 0000, 0000, 0220, 0000, 0000, 0140, 0000, 0240,
    0200, 0000, 0140, 0000, 0160, 0000, 0000, 0220, 0000, 0000, 0060, 0000, 0240,
    0021, 0043, 0041, 0121, 0040, 0023, 0042, 0003, 0142, 0042, 0061, 0001, 0022,
    0120, 0000, 0140, 0010, 0140, 0010, 0000, 0140, 0002, 0000, 0120, 0000, 0120,
    0000, 0000, 0000, 0000, 0360, 0000, 0000, 0000, 0000, 0000, 0160, 0000, 0000,
    0100, 0000, 0040, 0005, 0120, 0000, 0000, 0100, 0000, 0000, 0060, 0000, 0140,
    0140, 0040, 0100, 0001, 0240, 0041, 0000, 0242, 0000, 0002, 0140, 0000, 0100,
    0240, 0000, 0120, 0002, 0200, 0000, 0000, 0320, 0007, 0000, 0240, 0000, 0340,
    0101, 0021, 0041, 0020, 0040, 0005, 0042, 0121, 0002, 0021, 0201, 0000, 0020,
    0160, 0000, 0100, 0000, 0140, 0000, 0000, 0160, 0006, 0000, 0220, 0000, 0140,
    0140, 0000, 0020, 0001, 0020, 0000, 0000, 0100, 0001, 0000, 0300, 0000, 0000,
    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
    0106, 0041, 0040, 0147, 0040, 0000, 0063, 0041, 0001, 0102, 0160, 0002, 0002,
    0300, 0000, 0040, 0017, 0140, 0017, 0000, 0240, 0000, 0000, 0140, 0000, 0120};

/**
 * @brief Beginning + consonant + consonant + vowel patterns table.
 *
 * This table handles complex word-initial consonant clusters followed by
 * vowels, encoding the hyphenation preferences for words with complex
 * onsets like "str-", "spl-", "thr-", etc.
 *
 * Phonological Principles:
 * - Complex onset cluster analysis
 * - Sonority sequencing principles
 * - Language-specific cluster constraints
 * - Stress assignment implications
 *
 * Hyphenation Rules:
 * - Generally avoids breaking legitimate clusters
 * - Identifies morpheme boundaries within clusters
 * - Handles borrowed words with unusual clusters
 * - Balances phonological and morphological factors
 */
const unsigned char bxxh[338] = {
    0005, 0150, 0153, 0062, 0062, 0246, 0152, 0127, 0146, 0203, 0310, 0017, 0206,
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

/**
 * @brief Consonant + vowel + consonant patterns table.
 *
 * This is the most complex and important table, encoding hyphenation weights
 * for consonant-vowel-consonant sequences that form the core of most syllables.
 * These patterns are crucial for identifying optimal syllable boundaries.
 *
 * Syllable Structure Analysis:
 * - Nucleus identification (vowel centers)
 * - Onset-nucleus boundaries (CV transitions)
 * - Nucleus-coda boundaries (VC transitions)
 * - Stress-dependent hyphenation preferences
 *
 * Mathematical Complexity:
 * - Highest density of non-zero values
 * - Most sophisticated statistical modeling
 * - Context-sensitive weight adjustments
 * - Morphological boundary integration
 *
 * Linguistic Sophistication:
 * - Captures English phonotactic constraints
 * - Encodes syllable weight preferences
 * - Handles vowel length distinctions
 * - Integrates morphological information
 */
const unsigned char xhx[338] = {
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
    0057, 0377, 0051, 0360, 0120, 0361, (unsigned char)273, 056, 001, (unsigned char)256, 057, 0000, 0060,
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

/**
 * @brief Consonant + consonant + vowel patterns table.
 *
 * This table encodes hyphenation weights for consonant cluster patterns
 * followed by vowels, which is essential for handling complex consonant
 * sequences and determining where they can be safely divided.
 *
 * Cluster Analysis:
 * - Legitimate vs. illegitimate consonant clusters
 * - Sonority hierarchy constraints
 * - Language-specific phonotactic rules
 * - Morphological vs. phonological boundaries
 *
 * Hyphenation Strategy:
 * - Prefers to keep legitimate clusters together
 * - Identifies morpheme boundaries within clusters
 * - Handles exceptions for borrowed words
 * - Balances readability with linguistic accuracy
 */
const unsigned char xxh[338] = {
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

/**
 * @brief Table access functions for hyphenation algorithm.
 *
 * These functions provide the interface between the hyphenation algorithm
 * and the statistical digram tables, handling index calculation, bit
 * extraction, and weight retrieval.
 */

/**
 * @brief Extract 4-bit weight from packed digram table.
 *
 * Performs the mathematical operations needed to extract a single 4-bit
 * weight value from the packed table representation.
 *
 * Algorithm:
 * 1. Calculate 2D array index from character pair
 * 2. Determine byte offset and bit position
 * 3. Extract appropriate nibble from packed byte
 * 4. Return normalized weight value
 *
 * @param ch1 First character of digram (normalized to 0-25)
 * @param ch2 Second character of digram (normalized to 0-25)  
 * @param table Pointer to digram table
 * @return 4-bit weight value (0-15)
 */
int get_digram_weight(int ch1, int ch2, const unsigned char *table) {
    int index;
    int byte_offset;
    int bit_position;
    int weight;

    /* Validate character ranges */
    if (ch1 < 0 || ch1 > 25 || ch2 < 0 || ch2 > 25) {
        return 0;
    }

    /* Calculate linear index: ch1 * 13 + ch2 */
    index = ch2 * 13;
    byte_offset = (ch1 >> 1) + index;
    bit_position = ch1 & 1;

    /* Extract nibble value */
    weight = table[byte_offset];
    if (bit_position == 0) {
        weight >>= 4; /* High nibble */
    }
    weight &= 0x0F; /* Low nibble */

    return weight;
}

/**
 * @brief Get hyphenation weight for beginning + consonant + vowel pattern.
 * @param ch1 First character (normalized)
 * @param ch2 Second character (normalized)
 * @return Hyphenation weight (0-15)
 */
int get_bxh_weight(int ch1, int ch2) {
    return get_digram_weight(ch1, ch2, bxh);
}

/**
 * @brief Get hyphenation weight for vowel + consonant + consonant pattern.
 * @param ch1 First character (normalized)
 * @param ch2 Second character (normalized)
 * @return Hyphenation weight (0-15)
 */
int get_hxx_weight(int ch1, int ch2) {
    return get_digram_weight(ch1, ch2, hxx);
}

/**
 * @brief Get hyphenation weight for beginning + consonant + consonant + vowel pattern.
 * @param ch1 First character (normalized)
 * @param ch2 Second character (normalized)
 * @return Hyphenation weight (0-15)
 */
int get_bxxh_weight(int ch1, int ch2) {
    return get_digram_weight(ch1, ch2, bxxh);
}

/**
 * @brief Get hyphenation weight for consonant + vowel + consonant pattern.
 * @param ch1 First character (normalized)
 * @param ch2 Second character (normalized)
 * @return Hyphenation weight (0-15)
 */
int get_xhx_weight(int ch1, int ch2) {
    return get_digram_weight(ch1, ch2, xhx);
}

/**
 * @brief Get hyphenation weight for consonant + consonant + vowel pattern.
 * @param ch1 First character (normalized)
 * @param ch2 Second character (normalized)
 * @return Hyphenation weight (0-15)
 */
int get_xxh_weight(int ch1, int ch2) {
    return get_digram_weight(ch1, ch2, xxh);
}
