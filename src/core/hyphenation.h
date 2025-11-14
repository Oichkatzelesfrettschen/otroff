/**
 * @file hyphenation.h
 * @brief Pure C hyphenation engine API - Statistical digram-based word breaking
 *
 * This is the pure C interface for the ROFF hyphenation algorithm originally
 * from 1977 PWB/UNIX. Converted from PDP-11 assembly to portable C17.
 *
 * Design Philosophy:
 * - Zero global variables - all state in explicit context structures
 * - Pure C17 for maximum portability and performance
 * - Explicit error handling via return codes
 * - Thread-safe by design (when contexts are not shared)
 *
 * @copyright Copyright 1972 Bell Telephone Laboratories Inc.
 * @copyright Modernization 2025
 */

#ifndef OTROFF_HYPHENATION_H
#define OTROFF_HYPHENATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Constants
 * ============================================================================ */

#define OTROFF_MAX_WORD_LENGTH 64      /**< Maximum word length for hyphenation */
#define OTROFF_DIGRAM_TABLE_SIZE 338   /**< Size of digram lookup table (26*13) */
#define OTROFF_SUFFIX_BUFFER_SIZE 512  /**< Buffer size for suffix data */

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief Return codes for hyphenation operations
 */
typedef enum {
    OTROFF_HYPHEN_OK = 0,              /**< Operation successful */
    OTROFF_HYPHEN_ERROR_INVALID_ARG = -1,  /**< Invalid argument */
    OTROFF_HYPHEN_ERROR_NO_ALPHA = -2, /**< No alphabetic content */
    OTROFF_HYPHEN_ERROR_TOO_SHORT = -3,/**< Word too short to hyphenate */
    OTROFF_HYPHEN_ERROR_IO = -4        /**< I/O error reading suffix data */
} otroff_hyphen_error_t;

/**
 * @brief Digram lookup tables
 *
 * These tables contain frequency weights for different character pair patterns
 * used in statistical hyphenation. Indexed by (ch1-'a')*13 + (ch2-'a').
 */
typedef struct {
    unsigned char bxh[OTROFF_DIGRAM_TABLE_SIZE];   /**< Beginning + consonant + vowel */
    unsigned char xxh[OTROFF_DIGRAM_TABLE_SIZE];   /**< Consonant + consonant + vowel */
    unsigned char bxxh[OTROFF_DIGRAM_TABLE_SIZE];  /**< Beginning + CC + vowel */
    unsigned char xhx[OTROFF_DIGRAM_TABLE_SIZE];   /**< Consonant + vowel + consonant */
    unsigned char hxx[OTROFF_DIGRAM_TABLE_SIZE];   /**< Vowel + consonant + consonant */
} otroff_digram_tables_t;

/**
 * @brief Suffix pattern table
 *
 * Lookup table for morphological suffix patterns, indexed by first character.
 */
typedef struct {
    unsigned short table[26];           /**< Suffix offsets by first char */
    char buffer[OTROFF_SUFFIX_BUFFER_SIZE]; /**< Suffix pattern data */
    size_t buffer_used;                 /**< Bytes used in buffer */
} otroff_suffix_table_t;

/**
 * @brief Hyphenation analysis state
 *
 * Contains all mutable state for hyphenation analysis. This replaces
 * global variables from the original implementation.
 */
typedef struct {
    /* Algorithm parameters */
    int threshold;                      /**< Minimum score for hyphenation */
    bool enabled;                       /**< Hyphenation enabled flag */

    /* Analysis state */
    char* word_start;                   /**< Start of word being analyzed */
    char* hyph_start;                   /**< Hyphenation analysis start position */
    char* next_hyph_start;              /**< Next hyphenation start position */
    char* max_position;                 /**< Position of maximum score */
    int max_score;                      /**< Maximum digram score found */
    int hyphen_count;                   /**< Number of hyphens inserted */
    bool processed;                     /**< Word already processed flag */

    /* File descriptor for suffix data */
    int suffix_fd;                      /**< File descriptor for suffix patterns */
} otroff_hyphen_state_t;

/**
 * @brief Complete hyphenation context
 *
 * Aggregates all data structures needed for hyphenation. Passed to all
 * hyphenation functions to eliminate global state.
 */
typedef struct {
    otroff_hyphen_state_t state;        /**< Mutable analysis state */
    otroff_digram_tables_t* digrams;    /**< Digram frequency tables */
    otroff_suffix_table_t* suffixes;    /**< Suffix pattern table */
} otroff_hyphen_context_t;

/* ============================================================================
 * Context Management
 * ============================================================================ */

/**
 * @brief Create a new hyphenation context
 *
 * Allocates and initializes a hyphenation context with default settings.
 * The context must be freed with otroff_hyphen_context_destroy().
 *
 * @param threshold  Minimum score for hyphenation (typical: 100)
 * @return  Newly allocated context, or NULL on allocation failure
 */
otroff_hyphen_context_t* otroff_hyphen_context_create(int threshold);

/**
 * @brief Destroy a hyphenation context
 *
 * Frees all resources associated with the context.
 *
 * @param ctx  Context to destroy (may be NULL)
 */
void otroff_hyphen_context_destroy(otroff_hyphen_context_t* ctx);

/**
 * @brief Load digram tables from file
 *
 * Reads digram frequency tables from the specified file.
 *
 * @param ctx       Hyphenation context
 * @param filename  Path to digram table file
 * @return  OTROFF_HYPHEN_OK on success, error code otherwise
 */
int otroff_hyphen_load_digrams(otroff_hyphen_context_t* ctx, const char* filename);

/**
 * @brief Load suffix patterns from file
 *
 * Reads morphological suffix patterns from the specified file.
 *
 * @param ctx       Hyphenation context
 * @param filename  Path to suffix pattern file
 * @return  OTROFF_HYPHEN_OK on success, error code otherwise
 */
int otroff_hyphen_load_suffixes(otroff_hyphen_context_t* ctx, const char* filename);

/* ============================================================================
 * Hyphenation Operations
 * ============================================================================ */

/**
 * @brief Analyze word for hyphenation points
 *
 * Performs statistical digram analysis and morphological suffix matching
 * to determine optimal hyphenation points. Marks positions in the word
 * buffer by setting the high bit (0x80) of characters at hyphen points.
 *
 * Algorithm:
 * 1. Skip leading punctuation
 * 2. Verify alphabetic content exists
 * 3. Find hyphenatable region boundaries
 * 4. Apply suffix analysis for morphological decomposition
 * 5. Apply digram analysis for statistical scoring
 * 6. Mark positions exceeding threshold
 *
 * @param ctx       Hyphenation context
 * @param word      Word buffer (modified in-place to mark hyphen points)
 * @param word_len  Length of word
 * @return  Number of hyphenation points found, or negative error code
 */
int otroff_hyphen_analyze_word(otroff_hyphen_context_t* ctx,
                                char* word,
                                size_t word_len);

/**
 * @brief Enable or disable hyphenation
 *
 * @param ctx     Hyphenation context
 * @param enable  true to enable, false to disable
 */
void otroff_hyphen_set_enabled(otroff_hyphen_context_t* ctx, bool enable);

/**
 * @brief Set hyphenation threshold
 *
 * Higher thresholds result in fewer hyphenation points.
 * Typical values: 50-150, default 100.
 *
 * @param ctx        Hyphenation context
 * @param threshold  New threshold value
 */
void otroff_hyphen_set_threshold(otroff_hyphen_context_t* ctx, int threshold);

/**
 * @brief Reset analysis state
 *
 * Clears the "processed" flag and resets counters for a new word.
 *
 * @param ctx  Hyphenation context
 */
void otroff_hyphen_reset(otroff_hyphen_context_t* ctx);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * @brief Check if character is alphabetic
 *
 * Uses ROFF character classification (ASCII-based).
 *
 * @param ch  Character to test
 * @return  0 if alphabetic, non-zero otherwise
 */
int otroff_is_alpha(int ch);

/**
 * @brief Check if character is a vowel
 *
 * @param ch  Character to test (should be lowercase)
 * @return  0 if vowel, non-zero otherwise
 */
int otroff_is_vowel(int ch);

/**
 * @brief Convert character to lowercase
 *
 * Uses ROFF lowercase mapping (ASCII-based).
 *
 * @param ch  Pointer to character to convert (modified in-place)
 */
void otroff_to_lowercase(int* ch);

#ifdef __cplusplus
}
#endif

#endif /* OTROFF_HYPHENATION_H */
