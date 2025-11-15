/*
 * hytab.h - Hyphenation Tables Interface (Pure C17)
 *
 * Provides access to hyphenation digram tables for nroff/troff
 * Converted from C++ to pure C17
 */

#ifndef HYTAB_H
#define HYTAB_H

#include <stddef.h>
#include <stdint.h>

/*
 * Table size constants
 */
#define BXH_SIZE 13        /* Beginning + character table size */
#define MATRIX_SIZE 676    /* 26x26 character matrix (ALPHABET_SIZE^2) */
#define ALPHABET_SIZE 26   /* English alphabet size */

/*
 * Hyphenation weight type
 * Signed 8-bit value representing hyphenation probability
 * Positive values encourage hyphenation, negative discourage
 */
typedef int8_t hyphen_weight_t;

/*
 * Hyphenation table declarations
 * These are defined in hytab.c
 */
extern char bxh[BXH_SIZE];       /* Beginning + character sequences */
extern char hxx[MATRIX_SIZE];    /* General character pairs */
extern char bxxh[MATRIX_SIZE];   /* Character + end sequences */
extern char xhx[MATRIX_SIZE];    /* Mixed sequences with context */
extern char xxh[MATRIX_SIZE];    /* Fallback character + end */

/*
 * Table access helper macros
 * Convert character pairs to table indices
 */
#define HXX_INDEX(first, second)  ((first) * ALPHABET_SIZE + (second))
#define BXXH_INDEX(first, second) ((first) * ALPHABET_SIZE + (second))
#define XHX_INDEX(first, second)  ((first) * ALPHABET_SIZE + (second))
#define XXH_INDEX(first, second)  ((first) * ALPHABET_SIZE + (second))

/*
 * Character to index conversion (a=0, b=1, ..., z=25)
 * Returns -1 for non-alphabetic characters
 */
static inline int char_to_index(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= 'A' && c <= 'Z') return c - 'A';
    return -1;
}

/*
 * Macro version for compatibility
 */
#define CHAR_TO_INDEX(c) char_to_index(c)

/*
 * Hyphenation API functions
 */

/**
 * Get hyphenation weight for a digram (two-character sequence)
 * @param first First character index (0-25)
 * @param second Second character index (0-25)
 * @return Hyphenation weight, or 0 if indices invalid
 */
static inline hyphen_weight_t get_digram_weight(int first, int second) {
    if (first < 0 || first >= ALPHABET_SIZE ||
        second < 0 || second >= ALPHABET_SIZE) {
        return 0;
    }
    return (hyphen_weight_t)hxx[HXX_INDEX(first, second)];
}

/**
 * Get beginning context weight
 * @param char_idx Character index (0-12, limited to BXH_SIZE)
 * @return Hyphenation weight for word-beginning context
 */
static inline hyphen_weight_t get_beginning_weight(int char_idx) {
    if (char_idx < 0 || char_idx >= BXH_SIZE) {
        return 0;
    }
    return (hyphen_weight_t)bxh[char_idx];
}

/**
 * Get ending context weight
 * @param first First character index
 * @param second Second character index
 * @return Hyphenation weight for word-ending context
 */
static inline hyphen_weight_t get_ending_weight(int first, int second) {
    if (first < 0 || first >= ALPHABET_SIZE ||
        second < 0 || second >= ALPHABET_SIZE) {
        return 0;
    }
    return (hyphen_weight_t)bxxh[BXXH_INDEX(first, second)];
}

/**
 * Get mixed context weight (with existing hyphens nearby)
 * @param first First character index
 * @param second Second character index
 * @return Hyphenation weight with context
 */
static inline hyphen_weight_t get_context_weight(int first, int second) {
    if (first < 0 || first >= ALPHABET_SIZE ||
        second < 0 || second >= ALPHABET_SIZE) {
        return 0;
    }
    return (hyphen_weight_t)xhx[XHX_INDEX(first, second)];
}

/**
 * Get fallback weight
 * @param first First character index
 * @param second Second character index
 * @return Fallback hyphenation weight
 */
static inline hyphen_weight_t get_fallback_weight(int first, int second) {
    if (first < 0 || first >= ALPHABET_SIZE ||
        second < 0 || second >= ALPHABET_SIZE) {
        return 0;
    }
    return (hyphen_weight_t)xxh[XXH_INDEX(first, second)];
}

/**
 * Calculate hyphenation weight for a position in a word
 * @param word Pointer to word string
 * @param word_len Length of word
 * @param position Position to evaluate (0-based)
 * @param at_beginning True if near word start
 * @param at_ending True if near word end
 * @param has_context True if other hyphens exist nearby
 * @return Total hyphenation weight for this position
 */
hyphen_weight_t calculate_hyphen_weight(
    const char *word,
    size_t word_len,
    size_t position,
    int at_beginning,
    int at_ending,
    int has_context
);

/**
 * Determine if hyphenation should occur at a position
 * @param word Pointer to word string
 * @param word_len Length of word
 * @param position Position to evaluate
 * @param threshold Minimum weight required for hyphenation
 * @return 1 if should hyphenate, 0 otherwise
 */
int should_hyphenate_at(
    const char *word,
    size_t word_len,
    size_t position,
    hyphen_weight_t threshold
);

/**
 * Legacy C interface for compatibility
 * Returns hyphenation weight for a two-character sequence
 */
int hytab_get_weight(char first, char second);

#endif /* HYTAB_H */
