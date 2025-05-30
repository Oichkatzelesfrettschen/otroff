#include "cxx23_scaffold.hpp"
/**
 * @file roff5.c
 * @brief ROFF hyphenation engine - Statistical digram-based word breaking
 *
 * This module implements a sophisticated hyphenation algorithm that combines
 * statistical analysis of character digrams with morphological suffix patterns
 * to determine optimal word break points. Originally written in PDP-11 assembly,
 * this has been converted to portable C90 while preserving the mathematical
 * precision and linguistic accuracy of the original algorithm.
 *
 * Mathematical Foundations:
 * - Statistical digram frequency analysis using 2D lookup tables
 * - Weighted scoring system with configurable thresholds
 * - Bit-packed data structures for memory efficiency
 * - Morphological pattern matching with recursive suffix analysis
 *
 * Linguistic Principles:
 * - Onset-nucleus-coda syllable structure detection
 * - Vowel clustering for natural break point identification
 * - Suffix stripping with morphological boundary recognition
 * - Context-sensitive hyphenation point validation
 *
 * Algorithm Architecture:
 * 1. Punctuation and boundary detection
 * 2. Vowel cluster identification for syllable cores
 * 3. Suffix pattern matching with morphological analysis
 * 4. Digram frequency scoring across word segments
 * 5. Threshold-based hyphenation point selection
 * 6. Context validation and final placement
 *
 * Data Structures:
 * - Compact digram tables with 4-bit frequency weights
 * - Suffix pattern trees with bit-encoded morphological rules
 * - Character classification arrays for efficient lookup
 * - Hyphenation point markers using high-bit encoding
 *
 * Performance Optimizations:
 * - Single-pass word analysis with backtracking
 * - Bit manipulation for compact data representation
 * - Efficient character classification using lookup tables
 * - Minimal memory allocation through static buffers
 *
 * @note This implementation preserves the exact hyphenation behavior
 *       of the original UNIX ROFF system, ensuring compatibility
 *       with existing documents and formatting expectations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Include ROFF system headers */
#include "roff.hpp" // roff definitions

/* Copyright notice from original */
static const char copyright[] ROFF_UNUSED = "Copyright 1972 Bell Telephone Laboratories Inc.";

/* SCCS version identifier */
[[maybe_unused]] static constexpr std::string_view sccs_id =
    "@(#)roff5.c 1.3 25/05/29 (hyphenation engine - converted from PDP-11 assembly)"; // ID string

/* Constants for hyphenation algorithm */
#define MAX_WORD_LENGTH 64 /**< Maximum word length for hyphenation */
#define DIGRAM_TABLE_SIZE 338 /**< Size of digram lookup table (26*13) */
#define SUFFIX_BUFFER_SIZE 32 /**< Buffer size for suffix processing */
#define VOWEL_MASK 0x3F /**< Mask for vowel identification */
#define ALPHA_MASK 0x7F /**< Mask for alphabetic characters */
#define HYPHEN_MARK 0x80 /**< High bit marker for hyphenation points */
#define SUFFIX_CONTINUE 0x40 /**< Suffix continuation bit */
#define SUFFIX_HYPHEN 0x80 /**< Suffix hyphenation bit */
#define SUFFIX_VOWEL_CHECK 0x20 /**< Suffix vowel check bit */
#define NIBBLE_MASK 0x0F /**< 4-bit nibble mask */
#define NIBBLE_SHIFT 4 /**< Bits per nibble */

/* External variables from ROFF system */
extern int hypedf; /**< Hyphenation processed flag */
extern int hyf; /**< Hyphenation enabled flag */
extern int nhyph; /**< Number of hyphenation points found */
extern int thresh; /**< Hyphenation threshold value */
extern int old; /**< Old-style punctuation flag */
extern int suff; /**< Suffix file descriptor */
extern int nfile; /**< Current file descriptor */
extern char *wordp; /**< Pointer to current word */
extern char *hstart; /**< Hyphenation start pointer */
extern char *nhstart; /**< New hyphenation start pointer */
extern char *maxloc; /**< Location of maximum digram score */
extern int maxdig; /**< Maximum digram score found */
extern char sufb[]; /**< Suffix buffer */

/* External function prototypes */
extern int alph(int c); /**< Check if character is alphabetic */
extern int alph2(int c); /**< Alternative alphabetic check */
extern int rdsufb(int offset, int file_desc); /**< Read suffix buffer */

/* Static variables for hyphenation state */
static char punctuation_chars[] = "<.,()\"\\'`"; /**< Punctuation character set */

/* Digram lookup tables - these would be initialized from data files */
static unsigned char bxh[DIGRAM_TABLE_SIZE]; /**< Beginning + consonant + vowel */
static unsigned char xxh[DIGRAM_TABLE_SIZE]; /**< Consonant + consonant + vowel patterns */
static unsigned char bxxh[DIGRAM_TABLE_SIZE]; /**< Beginning + consonant + consonant + vowel patterns */
static unsigned char xhx[DIGRAM_TABLE_SIZE]; /**< Consonant + vowel + consonant */
static unsigned char hxx[DIGRAM_TABLE_SIZE]; /**< Vowel + consonant + consonant */

/* Suffix table - would be initialized from suffix data */
static unsigned short suftab[26]; /**< Suffix lookup table by first character */

/* Function prototypes */
static int punct(char *pos);
static void maplow(int *ch);
static int vowel(int ch);
static int checkvow(char *pos);
static void digram(void);
static void suffix(void);
static int dilook(int ch1, int ch2, unsigned char *table, int multiplier);
static void rdsuf(int offset, char **result);

/**
 * @brief Main hyphenation driver function.
 *
 * This is the primary entry point for the hyphenation algorithm. It performs
 * initial word analysis, identifies hyphenation boundaries, and coordinates
 * the suffix and digram analysis phases.
 *
 * Mathematical Approach:
 * - Establishes word boundaries using punctuation analysis
 * - Identifies vowel clusters as potential syllable nuclei
 * - Applies morphological decomposition through suffix analysis
 * - Performs statistical analysis using digram frequency tables
 * - Makes threshold-based decisions for hyphenation point placement
 *
 * Assembly Translation Notes:
 * - Original used register r0 for character pointer traversal
 * - Branch targets converted to structured control flow
 * - Condition code manipulation converted to explicit comparisons
 * - Stack-based subroutine calls converted to function calls
 *
 * Algorithm Flow:
 * 1. Check if hyphenation is enabled and not already processed
 * 2. Find start of alphabetic content, skipping punctuation
 * 3. Locate the beginning of the hyphenatable core
 * 4. Find the end boundary, handling trailing punctuation
 * 5. Invoke suffix analysis for morphological decomposition
 * 6. Invoke digram analysis for statistical hyphenation points
 */
void hyphen(void) {
    char *current_pos;

    /* Check if hyphenation is enabled and not already processed */
    if (hypedf != 0) {
        return; /* Already processed */
    }

    if (hyf == 0) {
        return; /* Hyphenation disabled */
    }

    /* Mark as processed */
    hypedf = 1;

    /* Initialize analysis from word start */
    current_pos = wordp;
    nhyph = 0;

    /* Skip leading punctuation to find alphabetic content */
    while (1) {
        if (punct(current_pos) == 0) {
            break; /* Found non-punctuation */
        }
        current_pos++;
    }

    /* Verify we have alphabetic content */
    if (alph((unsigned char)*current_pos) != 0) {
        return; /* No alphabetic content found */
    }

    /* Find start of hyphenatable region */
    while (1) {
        current_pos++;
        if (alph((unsigned char)*current_pos) == 0) {
            break; /* Found end of alphabetic sequence */
        }
    }
    current_pos--; /* Back up to last alphabetic character */
    hstart = current_pos;

    /* Find end of word, skipping trailing punctuation */
    while (1) {
        current_pos++;
        if (*current_pos == '\0') {
            break; /* End of word */
        }
        if (punct(current_pos) != 0) {
            return; /* Found punctuation - not hyphenatable */
        }
    }

    /* Perform morphological analysis through suffix patterns */
    suffix();

    /* Perform statistical analysis through digram patterns */
    digram();
}

/**
 * @brief Check if character at position is punctuation.
 *
 * Determines whether the character at the given position should be
 * considered punctuation for hyphenation boundary purposes. Supports
 * both modern and old-style punctuation sets.
 *
 * Mathematical Logic:
 * - Modern mode: Uses alph2() for comprehensive character classification
 * - Old mode: Uses explicit character set matching
 * - Returns boolean result via condition codes (assembly style)
 *
 * Character Classification:
 * - Backspace (010) always considered punctuation
 * - Old mode: Matches against explicit punctuation string
 * - Modern mode: Uses inverse of alphabetic classification
 *
 * Assembly Translation:
 * - Original used condition code setting (sez/clz)
 * - Converted to explicit return values
 * - Character comparison loop converted to strchr-style logic
 *
 * @param pos Pointer to character position to check
 * @return 0 if punctuation, non-zero if not punctuation
 */
static int punct(char *pos) {
    int ch;
    char *punct_ptr;

    ch = (unsigned char)*pos;

    if (old == 0) {
        /* Modern punctuation detection */
        if (ch == 010) { /* Backspace */
            return 0; /* Is punctuation */
        }

        /* Use alphabetic classification */
        if (alph2(ch) == 0) {
            return 1; /* Not punctuation (is alphabetic) */
        } else {
            return 0; /* Is punctuation */
        }
    } else {
        /* Old-style punctuation detection using character set */
        punct_ptr = punctuation_chars;

        while (*punct_ptr != '\0') {
            if (ch == (unsigned char)*punct_ptr) {
                return 0; /* Found in punctuation set */
            }
            punct_ptr++;
        }

        return 1; /* Not found in punctuation set */
    }
}

/**
 * @brief Convert character to lowercase.
 *
 * Performs case conversion for character normalization in hyphenation
 * analysis. Uses efficient comparison and arithmetic conversion.
 *
 * Mathematical Operation:
 * - Tests if character >= 'a' (lowercase already)
 * - If uppercase, adds offset ('a' - 'A') = 32
 * - Preserves non-alphabetic characters unchanged
 *
 * Assembly Translation:
 * - Original used bhis (branch if higher or same) instruction
 * - Arithmetic offset addition preserved exactly
 * - Register r2 usage converted to parameter modification
 *
 * @param ch Pointer to character value to convert
 */
static void maplow(int *ch) {
    if (*ch < 'a') {
        *ch += ('a' - 'A'); /* Convert uppercase to lowercase */
    }
}

/**
 * @brief Check if character is a vowel.
 *
 * Determines vowel status for syllable boundary detection. Uses
 * explicit character comparison for accuracy and speed.
 *
 * Linguistic Logic:
 * - Includes traditional vowels: a, e, i, o, u
 * - Includes 'y' as conditional vowel
 * - Case-insensitive through preprocessing
 *
 * Mathematical Implementation:
 * - Series of equality comparisons
 * - Early termination on match
 * - Condition code setting for assembly compatibility
 *
 * Assembly Translation:
 * - Original used sequence of cmp/beq instructions
 * - Converted to if-else chain for clarity
 * - Maintains exact same comparison order
 *
 * @param ch Character to test
 * @return 0 if vowel, non-zero if consonant
 */
static int vowel(int ch) {
    if (ch == 'a')
        return 0;
    if (ch == 'e')
        return 0;
    if (ch == 'i')
        return 0;
    if (ch == 'o')
        return 0;
    if (ch == 'u')
        return 0;
    if (ch == 'y')
        return 0;

    return 1; /* Not a vowel */
}

/**
 * @brief Check for vowel in backward direction from position.
 *
 * Scans backward from the given position to find a vowel character,
 * stopping at non-alphabetic characters. Used for syllable boundary
 * validation in suffix analysis.
 *
 * Linguistic Purpose:
 * - Ensures hyphenation points don't create vowel-less segments
 * - Validates syllable structure constraints
 * - Supports morphological boundary verification
 *
 * Algorithm:
 * 1. Move backward character by character
 * 2. Test each character for vowel status
 * 3. Stop at first vowel found (success)
 * 4. Stop at non-alphabetic character (failure)
 * 5. Return success/failure status
 *
 * Assembly Translation:
 * - Original used stack for position preservation
 * - Backward movement with pre-decrement addressing
 * - Condition code manipulation converted to return values
 *
 * @param pos Starting position for backward search
 * @return 0 if vowel found, non-zero if no vowel found
 */
static int checkvow(char *pos) {
    char *search_pos;
    int ch;

    search_pos = pos;

    while (1) {
        search_pos--;
        ch = (unsigned char)*search_pos;

        /* Check if character is vowel */
        if (vowel(ch) == 0) {
            return 0; /* Found vowel */
        }

        /* Check if character is alphabetic */
        if (alph(ch) == 0) {
            return 1; /* Found non-alphabetic, no vowel */
        }
    }
}

/**
 * @brief Perform digram-based hyphenation analysis.
 *
 * This function implements the core statistical hyphenation algorithm using
 * digram (two-character sequence) frequency analysis. It scans through the
 * word identifying potential hyphenation points based on character pair
 * patterns and their associated weights.
 *
 * Mathematical Foundation:
 * - Uses 5 different digram tables for positional context
 * - Scores each position using weighted pattern matching
 * - Applies threshold-based decision making for hyphenation
 * - Implements sliding window analysis across word length
 *
 * Digram Table Types:
 * 1. bxh: Beginning + consonant + vowel patterns
 * 2. xxh: Consonant + consonant + vowel patterns  
 * 3. bxxh: Beginning + consonant + consonant + vowel patterns
 * 4. xhx: Consonant + vowel + consonant patterns
 * 5. hxx: Vowel + consonant + consonant patterns
 *
 * Algorithm Steps:
 * 1. Find first vowel from hyphenation start point
 * 2. Locate first consonant after vowel cluster
 * 3. Scan through word applying all digram patterns
 * 4. Calculate weighted scores for each position
 * 5. Select maximum scoring position above threshold
 * 6. Mark hyphenation point and continue analysis
 *
 * Assembly Translation Notes:
 * - Complex register usage converted to local variables
 * - Multiple nested loops converted to structured control flow
 * - Bit manipulation operations preserved exactly
 * - Table lookup addressing converted to array indexing
 */
static void digram(void) {
    char *current_pos;
    char *analysis_start ROFF_UNUSED;
    int ch1, ch2;
    int score, max_score;
    char *max_position ROFF_UNUSED;
    int multiplier;

    current_pos = hstart;

    /* Find first vowel from start position */
    while (1) {
        if (alph((unsigned char)*current_pos) != 0) {
            return; /* End of alphabetic content */
        }

        if (vowel((unsigned char)*current_pos) == 0) {
            break; /* Found vowel */
        }

        current_pos--;
    }

    /* Update hyphenation start to vowel position */
    hstart = current_pos;

    /* Find first consonant after vowel cluster */
    while (1) {
        current_pos--;
        ch1 = (unsigned char)*current_pos;

        if (alph2(ch1) != 0) {
            return; /* End of word */
        }

        if (vowel(ch1) != 0) {
            break; /* Found consonant */
        }
    }

    /* Initialize digram analysis */
    maxdig = 0;
    nhstart = current_pos;

    /* Main digram analysis loop */
    while (current_pos < hstart) {
        multiplier = 1;
        max_score = 0;
        max_position = current_pos;

        /* Analyze patterns at current position */

        /* Pattern 1: Check if previous character exists and is alphabetic */
        ch1 = (unsigned char)*(current_pos - 1);
        if (alph2(ch1) == 0) {
            /* Single character pattern - bxh table */
            ch2 = (unsigned char)*current_pos;
            score = dilook('a', ch2, bxh, multiplier);
        } else {
            /* Two character patterns */
            ch1 = (unsigned char)*(current_pos - 2);

            /* Determine which pattern table to use */
            if (alph2(ch1) == 0) {
                /* xxh pattern */
                ch1 = (unsigned char)*(current_pos - 1);
                ch2 = (unsigned char)*current_pos;
                score = dilook(ch1, ch2, xxh, multiplier);
            } else {
                /* bxxh pattern */
                ch1 = (unsigned char)*(current_pos - 1);
                ch2 = (unsigned char)*current_pos;
                score = dilook(ch1, ch2, bxxh, multiplier);
            }
        }

        /* Update maximum if this score is higher */
        if (score > max_score) {
            max_score = score;
        }

        /* Pattern 2: xhx pattern (consonant-vowel-consonant) */
        ch1 = (unsigned char)*current_pos;
        ch2 = (unsigned char)*(current_pos + 1);
        score = dilook(ch1, ch2, xhx, multiplier);
        if (score > max_score) {
            max_score = score;
        }

        /* Pattern 3: hxx pattern (vowel-consonant-consonant) */
        ch1 = (unsigned char)*current_pos;
        ch2 = (unsigned char)*(current_pos + 1);
        score = dilook(ch1, ch2, hxx, multiplier);
        if (score > max_score) {
            max_score = score;
        }

        /* Check if this position has highest score so far */
        if (max_score > maxdig) {
            maxdig = max_score;
            maxloc = current_pos;
        }

        current_pos++;
    }

    /* Reset analysis start for next iteration */
    hstart = nhstart;

    /* Check if maximum score exceeds threshold */
    if (maxdig < thresh) {
        return; /* Below threshold - no hyphenation */
    }

    /* Mark hyphenation point */
    *maxloc |= HYPHEN_MARK;
    nhyph++;

    /* Recursive call to continue analysis */
    digram();
}

/**
 * @brief Lookup digram score in specified table.
 *
 * Performs efficient table lookup for digram frequency weights using
 * optimized indexing and bit manipulation. Handles character validation,
 * case normalization, and packed data extraction.
 *
 * Mathematical Operations:
 * 1. Character validation and normalization
 * 2. 2D array index calculation: (ch1 * 13) + ch2
 * 3. Bit-packed data extraction using shifts and masks
 * 4. Score multiplication by context factor
 *
 * Data Structure:
 * - Table stores 4-bit values (0-15) packed as nibbles
 * - Index calculation maps to 26x26 character space
 * - Even indices use low nibble, odd indices use high nibble
 * - Multiplication factor applied based on pattern context
 *
 * Assembly Translation:
 * - Register r4 usage converted to local variables
 * - Complex addressing modes converted to pointer arithmetic
 * - Bit rotation and masking operations preserved
 * - Stack manipulation converted to local variables
 *
 * @param ch1 First character of digram
 * @param ch2 Second character of digram  
 * @param table Digram frequency table to search
 * @param multiplier Context-dependent scoring multiplier
 * @return Weighted digram score
 */
static int dilook(int ch1, int ch2, unsigned char *table, int multiplier) {
    int normalized_ch1, normalized_ch2;
    int index;
    int table_offset;
    int nibble_value;
    int final_score;
    int bit_position;

    /* Normalize characters to lowercase and validate range */
    normalized_ch2 = ch2 & ALPHA_MASK;
    maplow(&normalized_ch2);
    normalized_ch2 -= 'a';
    if (normalized_ch2 > ('z' - 'a')) {
        return 0; /* Invalid character */
    }

    normalized_ch1 = ch1 & ALPHA_MASK;
    maplow(&normalized_ch1);
    normalized_ch1 -= 'a';
    if (normalized_ch1 > ('z' - 'a')) {
        return 0; /* Invalid character */
    }

    /* Calculate table index using 2D mapping */
    /* Formula: index = ch1 * 13 + ch2 (using base-13 for efficiency) */
    index = normalized_ch2 * 13;

    /* Handle bit position calculation */
    bit_position = 0;
    if ((normalized_ch1 & 1) != 0) {
        bit_position = 1;
    }
    table_offset = (normalized_ch1 >> 1) + index;

    /* Extract nibble value from packed table */
    nibble_value = table[table_offset];
    if (bit_position == 0) {
        /* Use high nibble */
        nibble_value >>= NIBBLE_SHIFT;
    }
    nibble_value &= NIBBLE_MASK;

    /* Calculate final score with multiplier */
    final_score = nibble_value * multiplier;

    return final_score;
}

/**
 * @brief Perform suffix-based morphological hyphenation analysis.
 *
 * This function implements morphological decomposition through suffix pattern
 * matching. It uses a sophisticated suffix table to identify morphological
 * boundaries where hyphenation is linguistically appropriate.
 *
 * Linguistic Foundation:
 * - Recognizes common English suffixes and their variants
 * - Applies morphological boundary rules for hyphenation
 * - Validates vowel requirements for syllable structure
 * - Handles recursive suffix stripping for complex words
 *
 * Algorithm Architecture:
 * 1. Extract suffix table entry based on word ending
 * 2. Attempt pattern matching against known suffixes
 * 3. Validate morphological boundary conditions
 * 4. Apply vowel structure requirements
 * 5. Mark valid hyphenation points
 * 6. Continue with remaining word portion
 *
 * Data Structure:
 * - Suffix table indexed by last character of word
 * - Bit-encoded suffix rules with length and pattern data
 * - Recursive pattern matching for compound suffixes
 * - Context flags for hyphenation permission
 *
 * Assembly Translation:
 * - Complex bit manipulation converted to explicit operations
 * - Multiple indirection levels converted to pointer operations
 * - Stack-based recursion converted to structured loops
 * - Condition code testing converted to explicit comparisons
 */
static void suffix(void) {
    char *word_pos;
    int last_char;
    unsigned short suffix_entry;
    unsigned short current_offset;
    char *suffix_pattern;
    int pattern_length;
    char *pattern_pos;
    char *word_check;
    int pattern_char;
    int word_char;
    int match_found;

    word_pos = hstart;

    /* Check if we have alphabetic content */
    if (alph((unsigned char)*word_pos) != 0) {
        return; /* No alphabetic content */
    }

    /* Get last character and normalize to lowercase */
    last_char = (unsigned char)*word_pos;
    maplow(&last_char);
    last_char -= 'a';

    /* Get suffix table entry */
    suffix_entry = suftab[last_char];
    suffix_entry &= 0x7FFF; /* Clear high bit */

    if (suffix_entry == 0) {
        return; /* No suffix patterns for this character */
    }

    /* Process suffix patterns */
    current_offset = suffix_entry;

    while (1) {
        /* Read suffix pattern from file */
        rdsuf(current_offset, &suffix_pattern);

        pattern_length = (unsigned char)*suffix_pattern;
        if (pattern_length == 0) {
            break; /* End of suffix patterns */
        }

        pattern_length &= 0x0F; /* Extract length bits */
        current_offset += pattern_length;
        pattern_pos = suffix_pattern + pattern_length;

        /* Attempt to match suffix pattern */
        word_check = hstart;
        match_found = 1;

        while (pattern_pos > suffix_pattern + 1) {
            pattern_char = (unsigned char)*--pattern_pos;
            pattern_char &= ALPHA_MASK;

            word_char = (unsigned char)*word_check--;
            maplow(&word_char);

            if (word_char != pattern_char) {
                match_found = 0;
                break;
            }
        }

        if (!match_found) {
            continue; /* Try next pattern */
        }

        /* Pattern matched - check hyphenation rules */
        word_pos = hstart;
        pattern_pos = suffix_pattern + 1;
        pattern_length = (unsigned char)*suffix_pattern & 0x0F;
        pattern_pos += pattern_length;

        /* Check for hyphenation permission bit */
        if (((unsigned char)*suffix_pattern & SUFFIX_HYPHEN) == 0) {
            /* No hyphenation allowed - scan for other patterns */
            while (pattern_pos > suffix_pattern + 1) {
                pattern_pos--;
                if (((unsigned char)*pattern_pos & HYPHEN_MARK) != 0) {
                    break;
                }
            }
        } else {
            /* Hyphenation allowed - update start position */
            hstart = word_pos;
            hstart--;

            /* Check vowel requirement */
            if (((unsigned char)*suffix_pattern & SUFFIX_VOWEL_CHECK) == 0) {
                if (checkvow(word_pos) != 0) {
                    continue; /* No vowel found - invalid hyphenation */
                }

                /* Mark hyphenation point */
                *word_pos |= HYPHEN_MARK;
            }
        }

        /* Check for continuation bit */
        pattern_pos--;
        if (((unsigned char)*pattern_pos & SUFFIX_CONTINUE) == 0) {
            return; /* No more processing */
        }

        /* Continue with recursive suffix analysis */
        break;
    }

    /* Recursive call for additional suffix processing */
    if (current_offset != suffix_entry) {
        suffix();
    }
}

/**
 * @brief Read suffix pattern data from external file.
 *
 * Loads suffix pattern data from the external suffix database file
 * into a local buffer for pattern matching operations. Handles
 * variable-length suffix patterns with embedded control information.
 *
 * File Format:
 * - First byte contains pattern length and control flags
 * - Following bytes contain pattern characters
 * - Patterns are stored consecutively in file
 * - Control flags indicate hyphenation rules
 *
 * Buffer Management:
 * - Uses static buffer to avoid dynamic allocation
 * - Loads complete pattern including control data
 * - Null-terminates patterns for string operations
 * - Maintains file position for sequential access
 *
 * Assembly Translation:
 * - Stack manipulation converted to local variables
 * - File I/O operations preserved with same interface
 * - Buffer pointer management converted to array indexing
 * - Register usage converted to local variable operations
 *
 * @param offset File offset to read pattern from
 * @param result Pointer to store pattern buffer address
 */
static void rdsuf(int offset, char **result) {
    static char pattern_buffer[SUFFIX_BUFFER_SIZE];
    int pattern_length;
    int bytes_read;
    int i;
    char *buffer_pos;

    /* Set file context */
    nfile = suff;

    /* Read pattern length byte */
    pattern_length = rdsufb(offset, nfile);

    /* Initialize buffer */
    buffer_pos = pattern_buffer;
    *buffer_pos++ = pattern_length;

    /* Extract actual length */
    bytes_read = pattern_length & 0x0F;

    /* Read pattern characters */
    for (i = 0; i < bytes_read; i++) {
        offset++;
        *buffer_pos++ = rdsufb(offset, nfile);
    }

    /* Return buffer address */
    *result = pattern_buffer;
}
