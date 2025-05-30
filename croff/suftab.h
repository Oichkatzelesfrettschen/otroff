/*
 * suftab.h - Header file for suffix table hyphenation support
 *
 * Copyright (c) 1979-2023 The Regents of the University of California.
 * All rights reserved.
 *
 * This header defines the interface for the suffix table module used
 * in troff/groff text processing for hyphenation support. The module
 * provides lookup functions for finding hyphenation patterns based on
 * word suffixes.
 *
 * Thread Safety: This module is read-only and thread-safe after
 * initialization.
 */

#ifndef SUFTAB_H
#define SUFTAB_H

#include <stddef.h>  /* For size_t type */

/*
 * Public constants for suffix table operations
 */
#define SUFTAB_MAX_PATTERN_LENGTH  16    /* Maximum suffix pattern length */
#define SUFTAB_NUM_LETTERS         26    /* Number of letters (a-z) */

/*
 * Hyphenation flags - used in suffix pattern entries
 */
#define SUFTAB_FLAG_BREAK_BEFORE   0x80  /* Allow hyphen break before */
#define SUFTAB_FLAG_PRIORITY       0x40  /* High priority pattern */
#define SUFTAB_FLAG_NO_BREAK       0x00  /* No special flags */

/*
 * Return codes for suffix table functions
 */
#define SUFTAB_SUCCESS             0     /* Operation successful */
#define SUFTAB_ERROR              -1     /* General error */
#define SUFTAB_INVALID_LETTER     -2     /* Invalid letter parameter */
#define SUFTAB_NOT_FOUND          -3     /* Pattern not found */

/*
 * Function pointer type for pattern callback
 * Called for each suffix pattern found during lookup.
 *
 * @pattern: Pointer to pattern data (not null-terminated)
 * @length: Length of pattern in bytes
 * @flags: Hyphenation flags for this pattern
 * @user_data: User-provided data pointer
 */
typedef void (*suftab_callback_t)(const unsigned char *pattern,
                                  int length,
                                  int flags,
                                  void *user_data);

/* ================================================================
 * PUBLIC FUNCTION PROTOTYPES
 * ================================================================ */

/*
 * Primary lookup and query functions
 */

/*
 * suftab_lookup - Look up suffix patterns for a letter
 * @letter: Letter (a-z, case insensitive)
 * @callback: Function to call for each pattern found
 * @user_data: User data to pass to callback
 * 
 * Finds all suffix patterns that begin with the specified letter
 * and calls the callback function for each one found. The callback
 * receives the pattern data, length, flags, and user data.
 *
 * Returns: Number of patterns found, or negative error code
 */
int suftab_lookup(int letter, suftab_callback_t callback, void *user_data);

/*
 * suftab_get_size - Get size of suffix table data
 * 
 * Returns: Total size of suffix pattern data in bytes
 */
size_t suftab_get_size(void);

/*
 * suftab_get_index_size - Get size of index table
 * 
 * Returns: Number of letters in index table (normally 26)
 */
int suftab_get_index_size(void);

/*
 * Initialization and utility functions
 */

/*
 * suftab_init - Initialize suffix table module
 * 
 * Performs basic validation of the suffix table data structures
 * to ensure they are consistent and usable. This function should
 * be called once before using other suffix table functions.
 *
 * Returns: SUFTAB_SUCCESS on success, SUFTAB_ERROR on failure
 */
int suftab_init(void);

/*
 * suftab_debug_print - Print debug information about suffix table
 * 
 * Outputs diagnostic information about the suffix table structure,
 * including index table contents and data table size. Useful for
 * debugging and verification.
 *
 * Returns: SUFTAB_SUCCESS on success
 */
int suftab_debug_print(void);

/*
 * Data access functions - read-only access to internal structures
 */

/*
 * suftab_get_index - Get index table entry for a letter
 * @letter: Letter index (0-25 for a-z)
 * 
 * Returns: Byte offset into pattern data, or 0 if no patterns
 */
int suftab_get_index(int letter);

/*
 * suftab_validate_data - Validate suffix table data integrity
 * 
 * Performs comprehensive validation of the suffix table data
 * structures, checking for consistency and proper formatting.
 *
 * Returns: SUFTAB_SUCCESS if valid, SUFTAB_ERROR if corrupted
 */
int suftab_validate_data(void);

/* ================================================================
 * EXTERNAL DATA DECLARATIONS
 * ================================================================ */

/*
 * Public read-only access to suffix table index
 * Maps each letter (a-z) to its offset in the pattern data.
 * Index 0 = 'a', index 1 = 'b', ..., index 25 = 'z'
 */
extern const unsigned short suftab_index[SUFTAB_NUM_LETTERS];

/* ================================================================
 * INLINE UTILITY FUNCTIONS (C90 compatible)
 * ================================================================ */

/*
 * Note: C90 doesn't have inline keyword, so these are implemented
 * as regular functions in the source file. In a C99+ environment,
 * these could be made inline for better performance.
 */

/* ================================================================
 * COMPATIBILITY AND FEATURE MACROS
 * ================================================================ */

/*
 * Version information for API compatibility
 */
#define SUFTAB_VERSION_MAJOR  1
#define SUFTAB_VERSION_MINOR  0
#define SUFTAB_VERSION_PATCH  0

/*
 * Feature flags for compile-time configuration
 */
#ifdef SUFTAB_ENABLE_DEBUG
#define SUFTAB_DEBUG_ENABLED  1
#else
#define SUFTAB_DEBUG_ENABLED  0
#endif

/*
 * Memory usage estimation macros
 */
#define SUFTAB_ESTIMATED_INDEX_SIZE  (SUFTAB_NUM_LETTERS * sizeof(unsigned short))
#define SUFTAB_ESTIMATED_DATA_SIZE   2048  /* Approximate, varies by dataset */

#endif /* SUFTAB_H */

/*
 * EOF - End of suffix table header
 */
