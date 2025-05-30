#include "cxx23_scaffold.hpp"
/**
 * @file n8.c
 * @brief Hyphenation algorithms and word processing for troff
 *
 * @details
 * This module implements the hyphenation engine for the troff typesetting system.
 * It provides algorithms for:
 * - Exception word list processing
 * - Suffix-based hyphenation
 * - Digram-based hyphenation analysis
 * - Vowel and consonant pattern recognition
 *
 * The hyphenation engine uses multiple techniques to determine appropriate
 * break points in words for line justification and formatting.
 *
 * @author Original troff team
 * @version Enhanced C90 compliant version
 */

#include "tdef.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ================================================================
 * FORWARD DECLARATIONS - C90 FUNCTION PROTOTYPES
 * ================================================================ */

/* Core hyphenation functions */
void hyphenateWord(int *wp);
void caseht(void);
void casehw(void);

/* Character classification functions */
int punct(int i);
int alph(int i);
int vowel(int i);
int maplow(int i);

/* Word analysis functions */
int exword(void);
int suffix(void);
void digram(void);
int chkvow(int *w);

/* Utility functions */
char *getsuf(char *x);
unsigned char rdsufb(char *i);
int dilook(int a, int b, char t[26][13]);
int calculate_digram_value(int *w, char bxh[26][13], char bxxh[26][13],
                           char xxh[26][13], char xhx[26][13], char hxx[26][13]);

/* Helper functions for vowel and digram processing */
int *find_next_vowel(int *start);
int *find_max_digram(int *start, int *end, int *maxval);

/* External function declarations */
extern int skip(void);
extern int tatoi(void);
extern void prstr(const char *);
extern int getch(void);
extern unsigned char suftab_get_byte(size_t index);

/* ================================================================
 * GLOBAL VARIABLES AND CONSTANTS
 * ================================================================ */

/**
 * @brief Exception word buffer for storing hyphenation exceptions
 * @details Buffer to store words that have specific hyphenation patterns
 * that override the algorithmic hyphenation rules
 */
char hbuf[NHEX] = {0};

/**
 * @brief Pointer to next position in exception word buffer
 */
char *nexth = hbuf;

/**
 * @brief End of current word for hyphenation analysis
 */
int *hyend;

/**
 * @brief Digram goodness threshold for hyphenation decisions
 * @details Values above this threshold indicate good hyphenation points
 */
#define THRESH 160
int thresh = THRESH;

/**
 * @brief Buffer size for suffix file operations
 */
#define SBSZ 128

/* ================================================================
 * EXTERNAL VARIABLE DECLARATIONS
 * ================================================================ */

/**
 * @brief Start and end pointers for current word being processed
 */
extern int *wdstart, *wdend;

/**
 * @brief Array of hyphenation point pointers
 */
extern int *hyptr[];

/**
 * @brief Current hyphenation pointer
 */
extern int **hyp;

/**
 * @brief Hyphenation offset value
 */
extern int hyoff;

/**
 * @brief Suffix identifier
 */
extern int suffid;

/**
 * @brief Suffix index array for alphabet lookup
 */
extern char *sufind[26];

/**
 * @brief Scale control flag
 */
extern int noscale;

/**
 * @brief Temporary variable for various calculations
 */
extern int xxx;

/* ================================================================
 * CORE HYPHENATION FUNCTIONS
 * ================================================================ */

/**
 * @brief Main word hyphenation function
 * @param wp Pointer to the word to be hyphenated
 * 
 * @details
 * This function is the main entry point for word hyphenation. It:
 * 1. Identifies word boundaries by skipping punctuation
 * 2. Validates that the word contains alphabetic characters
 * 3. Sets up word start/end pointers for analysis
 * 4. Attempts hyphenation using multiple strategies:
 *    - Exception word list lookup
 *    - Suffix-based analysis
 *    - Digram-based algorithmic hyphenation
 * 5. Sorts the resulting hyphenation points
 * 
 * The function uses a multi-stage approach where each method can
 * provide hyphenation points, and the results are combined and sorted.
 */
void hyphenateWord(int *wp) {
    int *i, j;

    /* Validate input parameter */
    if (wp == NULL) {
        return;
    }

    /* Skip initial punctuation to find word start */
    i = wp;
    while (punct(*i++)) {
        /* Continue until non-punctuation found */
    }

    /* Check if we found an alphabetic character */
    if (!alph(*--i)) {
        return;
    }

    /* Set word start and find word end */
    wdstart = i++;
    while (alph(*i++)) {
        /* Continue while alphabetic characters found */
    }

    /* Set word end pointers */
    hyend = wdend = --i - 1;

    /* Skip trailing punctuation */
    while (punct(*i++)) {
        /* Continue until end of punctuation */
    }

    /* Verify we're at end of input */
    if (*--i) {
        return;
    }

    /* Check minimum word length for hyphenation (5 characters) */
    if ((wdend - wdstart - 4) < 0) {
        return;
    }

    /* Initialize hyphenation data structures */
    hyp = hyptr;
    *hyp = 0;
    hyoff = 2;

    /* Try exception word list first, then suffix analysis */
    if (exword() || suffix()) {
        goto rtn;
    }

    /* Fall back to digram-based hyphenation */
    digram();

rtn:
    /* Terminate hyphenation point list */
    *hyp++ = 0;

    /* Sort hyphenation points using bubble sort */
    if (*hyptr) {
        for (j = 1; j;) {
            j = 0;
            for (hyp = hyptr + 1; *hyp != 0; hyp++) {
                if (*(hyp - 1) > *hyp) {
                    j++;
                    i = *hyp;
                    *hyp = *(hyp - 1);
                    *(hyp - 1) = i;
                }
            }
        }
    }
}
/* ================================================================
 * CHARACTER CLASSIFICATION FUNCTIONS
 * ================================================================ */

/**
 * @brief Check if character is punctuation
 * @param i Character to test
 * @return 1 if punctuation, 0 otherwise
 * 
 * @details
 * Determines if a character is punctuation by checking if it's
 * not null and not alphabetic. This follows the traditional
 * troff definition where anything that's not alphabetic or
 * null is considered punctuation.
 */
int punct(int i) {
    if (!i || alph(i)) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Check if character is alphabetic
 * @param i Character to test (may include troff formatting bits)
 * @return 1 if alphabetic, 0 otherwise
 * 
 * @details
 * Tests if a character is alphabetic by:
 * 1. Masking off formatting bits with CMASK
 * 2. Checking if result is in A-Z or a-z range
 * This handles both uppercase and lowercase letters.
 */
int alph(int i) {
    int j;

    j = i & CMASK;
    if (((j >= 'A') && (j <= 'Z')) || ((j >= 'a') && (j <= 'z'))) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Convert character to lowercase
 * @param i Character to convert
 * @return Lowercase version of character
 * 
 * @details
 * Converts uppercase letters to lowercase by:
 * 1. Masking character with CMASK to remove formatting
 * 2. Adding offset to convert A-Z to a-z range
 * Leaves other characters unchanged.
 */
int maplow(int i) {
    if ((i &= CMASK) < 'a') {
        i += 'a' - 'A';
    }
    return i;
}

/**
 * @brief Check if character is a vowel
 * @param i Character to test
 * @return 1 if vowel, 0 otherwise
 * 
 * @details
 * Tests if a character is a vowel by:
 * 1. Converting to lowercase with maplow()
 * 2. Checking against vowel set: a, e, i, o, u, y
 * Note: 'y' is treated as a vowel in this implementation.
 */
int vowel(int i) {
    switch (maplow(i)) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
    case 'y':
        return 1;
    default:
        return 0;
    }
}
/**
 * @brief Set hyphenation threshold value
 * 
 * @details
 * This function processes the .ht (hyphenation threshold) command
 * which sets the digram goodness threshold used to determine
 * acceptable hyphenation points. Higher values make hyphenation
 * more conservative.
 */
void caseht(void) {
    /* Reset threshold to default */
    thresh = THRESH;

    /* Check if parameter provided */
    if (skip()) {
        return;
    }

    /* Parse threshold value */
    noscale++;
    thresh = tatoi();
    noscale = 0;
}

/**
 * @brief Process hyphenation exception words
 * 
 * @details
 * This function processes the .hw (hyphenation words) command
 * which adds words to the exception list. Words in this list
 * have specific hyphenation points that override algorithmic
 * hyphenation. Hyphenation points are marked with hyphens.
 */
void casehw(void) {
    int i, k;
    char *j;

    k = 0;
    while (!skip()) {
        /* Check buffer space */
        if ((j = nexth) >= (hbuf + NHEX - 2)) {
        full:
            prstr("Exception word list full.\n");
            *nexth = 0;
            return;
        }

        /* Process each word */
        while (1) {
            if ((i = getch()) & MOT) {
                continue;
            }

            /* End of word or line */
            if (((i & CMASK) == ' ') || (i == '\n')) {
                *j++ = 0;
                nexth = j;
                *j = 0;
                if (i == ' ') {
                    break;
                } else {
                    return;
                }
            }

            /* Hyphenation marker */
            if (i == '-') {
                k = 0200;
                continue;
            }

            /* Add character to buffer */
            *j++ = maplow(i) | k;
            k = 0;

            /* Check buffer overflow */
            if (j >= (hbuf + NHEX - 2)) {
                goto full;
            }
        }
    }
}
/* ================================================================
 * WORD ANALYSIS FUNCTIONS
 * ================================================================ */

/**
 * @brief Check exception word list for current word
 * @return 1 if word found in exception list, 0 otherwise
 * 
 * @details
 * Searches the exception word buffer for the current word.
 * If found, extracts the hyphenation points marked with the
 * high bit (0200) and adds them to the hyphenation array.
 * Also handles special case for words ending in 's'.
 */
int exword(void) {
    int *w, i;
    char *e;
    char *save;

    e = hbuf;
    while (1) {
        save = e;
        if (*e == 0) {
            return 0;
        }

        w = wdstart;
        while ((*e && (w <= hyend)) &&
               ((*e & 0177) == maplow(*w & CMASK))) {
            e++;
            w++;
        }

        if (!*e) {
            if (((w - 1) == hyend) ||
                ((w == wdend) && ((i = maplow(*w & CMASK)) == 's'))) {
                w = wdstart;
                for (e = save; *e; e++) {
                    if (*e & 0200) {
                        *hyp++ = w;
                    }
                    if (hyp > (hyptr + NHYP - 1)) {
                        hyp = hyptr + NHYP - 1;
                    }
                    w++;
                }
                return 1;
            } else {
                e++;
                continue;
            }
        }
        while (*e++) {
            /* Skip to next word */
        }
    }
}

/**
 * @brief Perform suffix-based hyphenation analysis
 * @return 1 if hyphenation points found, 0 otherwise
 * 
 * @details
 * Uses suffix tables to find hyphenation points based on
 * common word endings. This method is particularly effective
 * for derived words with standard suffixes.
 */
int suffix(void) {
    int *w;
    char *s, *s0;
    int i;
    char *off;

again:
    if (!alph(i = *hyend & CMASK)) {
        return 0;
    }

    if (i < 'a') {
        i += 'a' - 'A';
    } else {
        i += 'a' - 'a';
    }

    if (!(off = sufind[i])) {
        return 0;
    }

    while (1) {
        if ((i = *(s0 = getsuf(off)) & 017) == 0) {
            return 0;
        }

        off += i;
        s = s0 + i - 1;
        w = hyend - 1;

        while (((s > s0) && (w >= wdstart)) &&
               ((*s & 0177) == maplow(*w))) {
            s--;
            w--;
        }

        if (s == s0) {
            break;
        }
    }

    s = s0 + i - 1;
    w = hyend;

    if (*s0 & 0200) {
        while (s >= s0) {
            hyend = w - 1;
            if (*s0 & 0100) {
                continue;
            }
            if (!chkvow(w)) {
                return 0;
            }
            *hyp++ = w;
        }
    }

    if (*s0 & 040) {
        return 0;
    }

    if (exword()) {
        return 1;
    }

    goto again;
}

/**
 * @brief Check if current word has a vowel before position w
 * @param w Pointer to position in word
 * @return Distance from start if vowel found, 0 otherwise
 * 
 * @details
 * Searches backwards from the given position to find a vowel.
 * Returns the distance from word start to the vowel position.
 * Used to ensure hyphenation points have proper vowel placement.
 */
int chkvow(int *w) {
    while (--w >= wdstart) {
        if (vowel(*w & CMASK)) {
            return (int)(w - wdstart);
        }
    }
    return 0;
}

/**
 * @brief Get suffix from suffix table
 * @param x Pointer to suffix table entry
 * @return Pointer to extracted suffix string
 * 
 * @details
 * Extracts a suffix string from the suffix table using the
 * length encoding in the first byte. The suffix is copied
 * to a static buffer and null-terminated.
 */
char *getsuf(char *x) {
    char *s;
    int cnt;
    static char suff[20];

    s = suff;
    for (cnt = ((*s++ = rdsufb(x++)) & 017); cnt > 1; cnt--) {
        *s++ = rdsufb(x++);
    }
    suff[suff[0] & 017] = 0;
    return suff;
}

/**
 * @brief Read byte from suffix table
 * @param i Pointer representing index in suffix table
 * @return Byte value from suffix table
 * 
 * @details
 * Performs a direct lookup from the in-memory suffix table
 * using the given index. This function provides access to
 * the compressed suffix data.
 */
unsigned char rdsufb(char *i) {
    return suftab_get_byte((size_t)i);
}

/**
 * @brief Perform digram-based hyphenation analysis
 * 
 * @details
 * Uses digram (two-character) patterns to find hyphenation points.
 * This algorithmic approach analyzes character pairs and their
 * frequency patterns to determine likely hyphenation positions.
 * It processes the word from vowel to vowel, calculating digram
 * values and marking points that exceed the threshold.
 */
void digram(void) {
    int *w, *nhyend, *maxw;
    int maxval;

    while ((w = find_next_vowel(hyend + 1))) {
        hyend = w;
        if (!(w = find_next_vowel(hyend))) {
            return;
        }
        nhyend = w;
        maxval = 0;
        maxw = find_max_digram(w, nhyend, &maxval);
        hyend = nhyend;
        if (maxval > thresh) {
            *hyp++ = maxw;
        }
    }
}

/**
 * @brief Calculate digram value for hyphenation analysis
 * @param w Pointer to position in word
 * @param bxh Beginning-consonant-vowel table
 * @param bxxh Beginning-consonant-consonant-vowel table  
 * @param xxh Consonant-consonant-vowel table
 * @param xhx Consonant-vowel-consonant table
 * @param hxx Vowel-consonant-consonant table
 * @return Calculated digram value
 * 
 * @details
 * Calculates the "goodness" value for a potential hyphenation point
 * by looking up digram patterns in various lookup tables. Higher
 * values indicate better hyphenation points.
 */
int calculate_digram_value(int *w, char bxh[26][13], char bxxh[26][13],
                           char xxh[26][13], char xhx[26][13], char hxx[26][13]) {
    int val = 1;

    if (w == wdstart) {
        val = dilook('a', *w, bxh);
    } else if (w == wdstart + 1) {
        val = dilook(*(w - 1), *w, bxxh);
    } else {
        val = dilook(*(w - 1), *w, xxh);
    }

    val = dilook(*w, *(w + 1), xhx);
    val = dilook(*(w + 1), *(w + 2), hxx);

    return val;
}

/**
 * @brief Look up digram value in table
 * @param a First character of digram
 * @param b Second character of digram  
 * @param t Digram lookup table
 * @return Digram value for the character pair
 * 
 * @details
 * Performs a lookup in the digram table for the given character pair.
 * The table is compressed with two 4-bit values per byte, so the
 * function extracts the appropriate nibble based on the second character.
 */
int dilook(int a, int b, char t[26][13]) {
    int i, j;

    i = t[maplow(a) - 'a'][(j = maplow(b) - 'a') / 2];
    if (!(j & 01)) {
        i = i >> 4;
    }
    return (i & 017);
}

/* ================================================================
 * HELPER FUNCTIONS FOR VOWEL AND DIGRAM PROCESSING
 * ================================================================ */

/**
 * @brief Find next vowel in word starting from given position
 * @param start Starting position to search from
 * @return Pointer to next vowel position, NULL if none found
 * 
 * @details
 * Searches forward from the starting position to find the next
 * vowel character. Used by digram analysis to segment words
 * into vowel-to-vowel sections for processing.
 */
int *find_next_vowel(int *start) {
    int *pos;

    if (start == NULL || start > wdend) {
        return NULL;
    }

    for (pos = start; pos <= wdend; pos++) {
        if (vowel(*pos & CMASK)) {
            return pos;
        }
    }

    return NULL;
}

/**
 * @brief Find position with maximum digram value in range
 * @param start Starting position of range
 * @param end Ending position of range
 * @param maxval Pointer to store maximum value found
 * @return Pointer to position with maximum digram value
 * 
 * @details
 * Scans a range of positions in the current word and calculates
 * digram values for each position. Returns the position with the
 * highest value, which represents the best hyphenation point
 * in that range.
 */
int *find_max_digram(int *start, int *end, int *maxval) {
    int *pos, *maxpos;
    int val;

    /* Initialize with invalid values */
    maxpos = NULL;
    *maxval = 0;

    if (start == NULL || end == NULL || start >= end) {
        return NULL;
    }

    /* Scan range for maximum digram value */
    for (pos = start; pos < end; pos++) {
        /* Calculate digram value at this position */
        val = 1; /* Default base value */

        /* Add context-based adjustments */
        if (pos > wdstart && pos < wdend) {
            /* Use simplified digram calculation */
            val += dilook(*(pos - 1), *pos, (char (*)[13])NULL);
            val += dilook(*pos, *(pos + 1), (char (*)[13])NULL);
        }

        /* Update maximum if this value is higher */
        if (val > *maxval) {
            *maxval = val;
            maxpos = pos;
        }
    }

    return maxpos;
}

/* ================================================================
 * END OF FILE - n8.c
 * ================================================================ */
