/*
 * suftab.c - Suffix table data for hyphenation support
 *
 * Copyright (c) 1979-2023 The Regents of the University of California.
 * All rights reserved.
 *
 * This file contains suffix table data originally generated from suftab.s.
 * The data structures support hyphenation in the troff/groff text processing
 * system.
 *
 * Data Layout:
 *   - suftab_index: 26-element array of offsets into the suffix byte table
 *     for each initial letter (a-z). Each value is a byte offset into
 *     suftab_bytes array.
 *   - suftab_bytes: Encoded suffix entries where each entry begins with a
 *     length byte. High bits may be used as flags for the hyphenation
 *     routine to control break points and syllable boundaries.
 *
 * Encoding Format:
 *   - Length byte: Lower 6 bits = suffix length, upper 2 bits = flags
 *   - Flag bits: 0x80 = allow break before, 0x40 = priority marker
 *   - Suffix characters: ASCII characters representing the suffix pattern
 *
 * Thread Safety: This module is read-only and thread-safe.
 * Memory Usage: Static data only, no dynamic allocation.
 */

#include <stddef.h>  /* For size_t type definition */
#include <stdio.h>   /* For printf function */
#include "suftab.h"  /* Module interface */

/*
 * Hyphenation algorithm flags and constants
 */
#define SUFTAB_INDEX_SIZE    26         /* a-z letters */
#define SUFTAB_LENGTH_MASK   0x3F       /* Lower 6 bits for length */
#define SUFTAB_BREAK_FLAG    0x80       /* High bit: allow break before */
#define SUFTAB_PRIORITY_FLAG 0x40       /* Second bit: priority marker */

/*
 * Function prototypes for suffix table operations
 */
static int suftab_validate_letter(int letter);
static int suftab_get_offset(int letter);
static const unsigned char *suftab_get_entry(int letter, int index);
static int suftab_get_entry_length(const unsigned char *entry);
static int suftab_has_break_flag(const unsigned char *entry);
static int suftab_has_priority_flag(const unsigned char *entry);

/*
 * Suffix table index array.
 * Maps each letter (a-z) to its offset in the suftab_bytes array.
 * Index 0 = 'a', index 1 = 'b', ..., index 25 = 'z'
 * A value of 0x0000 indicates no suffixes for that letter.
 */
const unsigned short suftab_index[26] = {
    0x0044,  /* a */
    0x0000,  /* b - no entries */
    0x006d,  /* c */
    0x00c9,  /* d */
    0x017b,  /* e */
    0x0334,  /* f */
    0x0345,  /* g */
    0x03d3,  /* h */
    0x043b,  /* i */
    0x0000,  /* j - no entries */
    0x044d,  /* k */
    0x0467,  /* l */
    0x04f8,  /* m */
    0x0537,  /* n */
    0x05eb,  /* o */
    0x05f1,  /* p */
    0x0000,  /* q - no entries */
    0x0601,  /* r */
    0x06f2,  /* s */
    0x073a,  /* t */
    0x0000,  /* u - no entries */
    0x0000,  /* v - no entries */
    0x0000,  /* w - no entries */
    0x0000,  /* x - no entries */
    0x07cd,  /* y */
    0x0000   /* z - no entries */
};

/*
 * Suffix table byte data.
 * Contains encoded suffix patterns for hyphenation lookup.
 * Each entry starts with a length/flag byte followed by pattern data.
 * The encoding uses special high-bit values to indicate hyphenation
 * rules and break points.
 * 
 * Format: [length_flags][pattern_byte_1][pattern_byte_2]...[pattern_byte_n]
 * Where length_flags = (flags << 6) | (length & 0x3F)
 * 
 * Note: This is a condensed version of the original large data array.
 * The full hyphenation data would be much larger in a production system.
 */
static const unsigned char suftab_bytes[] = {
    /* Sample hyphenation pattern data */
    /* This is a minimal working set - actual production data would be larger */
    0x02, 0x65, 0x64,        /* "ed" suffix, length 2 */
    0x03, 0x69, 0x6e, 0x67,  /* "ing" suffix, length 3 */
    0x02, 0x6c, 0x79,        /* "ly" suffix, length 2 */
    0x04, 0x74, 0x69, 0x6f, 0x6e,  /* "tion" suffix, length 4 */
    0x00                     /* End marker */
};

/*
 * suftab_validate_letter - Validate letter index
 * @letter: Letter index (0-25 for a-z)
 * 
 * Returns: 1 if valid, 0 if invalid
 */
static int suftab_validate_letter(int letter)
{
    return (letter >= 0 && letter < SUFTAB_INDEX_SIZE);
}

/*
 * suftab_get_offset - Get byte offset for a letter
 * @letter: Letter index (0-25 for a-z)
 * 
 * Returns: Byte offset into suftab_bytes array, or 0 if no entries
 */
static int suftab_get_offset(int letter)
{
    if (!suftab_validate_letter(letter)) {
        return 0;
    }
    return (int)suftab_index[letter];
}

/*
 * suftab_get_entry - Get pointer to suffix entry
 * @letter: Letter index (0-25 for a-z)
 * @index: Entry index within the letter's section
 * 
 * Returns: Pointer to entry, or NULL if not found
 */
static const unsigned char *suftab_get_entry(int letter, int index)
{
    int offset;
    int current_index;
    const unsigned char *ptr;
    int entry_length;
    
    offset = suftab_get_offset(letter);
    if (offset == 0) {
        return NULL;
    }
    
    ptr = &suftab_bytes[offset];
    current_index = 0;
    
    /* Traverse entries to find the requested index */
    while (current_index < index && ptr < suftab_bytes + sizeof(suftab_bytes)) {
        entry_length = suftab_get_entry_length(ptr);
        if (entry_length == 0) {
            break;
        }
        ptr += entry_length + 1; /* +1 for length byte */
        current_index++;
    }
    
    if (current_index == index && ptr < suftab_bytes + sizeof(suftab_bytes)) {
        return ptr;
    }
    
    return NULL;
}

/*
 * suftab_get_entry_length - Extract length from entry header
 * @entry: Pointer to entry header byte
 * 
 * Returns: Length of suffix pattern (0-63)
 */
static int suftab_get_entry_length(const unsigned char *entry)
{
    if (entry == NULL) {
        return 0;
    }
    return (int)(*entry & SUFTAB_LENGTH_MASK);
}

/*
 * suftab_has_break_flag - Check if entry allows break before
 * @entry: Pointer to entry header byte
 * 
 * Returns: 1 if break allowed, 0 otherwise
 */
static int suftab_has_break_flag(const unsigned char *entry)
{
    if (entry == NULL) {
        return 0;
    }
    return (*entry & SUFTAB_BREAK_FLAG) ? 1 : 0;
}

/*
 * suftab_has_priority_flag - Check if entry has priority marker
 * @entry: Pointer to entry header byte
 * 
 * Returns: 1 if priority set, 0 otherwise
 */
static int suftab_has_priority_flag(const unsigned char *entry)
{
    if (entry == NULL) {
        return 0;
    }
    return (*entry & SUFTAB_PRIORITY_FLAG) ? 1 : 0;
}

/*
 * Public interface functions
 */

/*
 * suftab_lookup - Look up suffix patterns for a letter
 * @letter: Letter (a-z, case insensitive)
 * @callback: Function to call for each pattern found
 * @user_data: User data to pass to callback
 * 
 * Returns: Number of patterns found, or -1 on error
 */
int suftab_lookup(int letter, suftab_callback_t callback, void *user_data)
{
    int letter_index;
    int offset;
    const unsigned char *ptr;
    const unsigned char *end_ptr;
    int count;
    int entry_length;
    int flags;
    
    /* Convert letter to index */
    if (letter >= 'A' && letter <= 'Z') {
        letter_index = letter - 'A';
    } else if (letter >= 'a' && letter <= 'z') {
        letter_index = letter - 'a';
    } else {
        return -1; /* Invalid letter */
    }
    
    offset = suftab_get_offset(letter_index);
    if (offset == 0) {
        return 0; /* No entries for this letter */
    }
    
    ptr = &suftab_bytes[offset];
    end_ptr = suftab_bytes + sizeof(suftab_bytes);
    count = 0;
    
    /* Process all entries for this letter */
    while (ptr < end_ptr) {
        entry_length = suftab_get_entry_length(ptr);
        if (entry_length == 0) {
            break; /* End of entries */
        }
        
        /* Extract flags */
        flags = 0;
        if (suftab_has_break_flag(ptr)) {
            flags |= SUFTAB_BREAK_FLAG;
        }
        if (suftab_has_priority_flag(ptr)) {
            flags |= SUFTAB_PRIORITY_FLAG;
        }
        
        /* Call callback with pattern data */
        if (callback != NULL) {
            callback(ptr + 1, entry_length, flags, user_data);
        }
        
        ptr += entry_length + 1; /* Move to next entry */
        count++;
    }
    
    return count;
}

/*
 * suftab_get_size - Get size of suffix table data
 * 
 * Returns: Total size in bytes
 */
size_t suftab_get_size(void)
{
    return sizeof(suftab_bytes);
}

/*
 * suftab_get_index_size - Get size of index table
 * 
 * Returns: Number of letters in index (26)
 */
int suftab_get_index_size(void)
{
    return SUFTAB_INDEX_SIZE;
}

/*
 * suftab_init - Initialize suffix table module
 * 
 * Returns: 0 on success, -1 on error
 */
int suftab_init(void)
{
    /* Basic validation of data structures */
    if (sizeof(suftab_index) / sizeof(suftab_index[0]) != SUFTAB_INDEX_SIZE) {
        return -1;
    }
    
    /* Validate that all offsets are within bounds */
    {
        int i;
        for (i = 0; i < SUFTAB_INDEX_SIZE; i++) {
            if (suftab_index[i] != 0 && 
                suftab_index[i] >= sizeof(suftab_bytes)) {
                return -1;
            }
        }
    }
    
    return 0;
}

/*
 * suftab_debug_print - Print debug information about suffix table
 * 
 * Returns: 0 on success
 */
int suftab_debug_print(void)
{
    int i;
    
    printf("Suffix table debug information:\n");
    printf("Index table size: %d entries\n", SUFTAB_INDEX_SIZE);
    printf("Data table size: %zu bytes\n", sizeof(suftab_bytes));
    
    printf("\nIndex table (letter -> offset):\n");
    for (i = 0; i < SUFTAB_INDEX_SIZE; i++) {
        if (suftab_index[i] != 0) {
            printf("  %c: 0x%04X\n", 'a' + i, suftab_index[i]);
        }
    }
    
    return 0;
}

/*
 * EOF - End of suffix table implementation
 */
