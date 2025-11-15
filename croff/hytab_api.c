/*
 * hytab_api.c - Hyphenation API Implementation (Pure C17)
 *
 * Implements hyphenation weight calculation functions
 * using the hyphenation digram tables
 */

#include "hytab.h"
#include <string.h>

/**
 * Calculate hyphenation weight for a position in a word
 */
hyphen_weight_t calculate_hyphen_weight(
    const char *word,
    size_t word_len,
    size_t position,
    int at_beginning,
    int at_ending,
    int has_context
) {
    if (!word || position == 0 || position >= word_len) {
        return 0;
    }

    /* Get character indices */
    int left_idx = char_to_index(word[position - 1]);
    int right_idx = char_to_index(word[position]);

    if (left_idx < 0 || right_idx < 0) {
        return 0; /* Non-alphabetic characters */
    }

    hyphen_weight_t total_weight = 0;

    /* Primary digram weight */
    total_weight += get_digram_weight(left_idx, right_idx);

    /* Beginning context (position 1) */
    if (at_beginning && position == 1) {
        total_weight += get_beginning_weight(right_idx);
    }

    /* Ending context */
    if (at_ending) {
        total_weight += get_ending_weight(left_idx, right_idx);
    }

    /* Mixed context (existing hyphens nearby) */
    if (has_context) {
        total_weight += get_context_weight(left_idx, right_idx);
    }

    /* Fallback for uncertain cases */
    if (total_weight == 0) {
        total_weight = get_fallback_weight(left_idx, right_idx);
    }

    return total_weight;
}

/**
 * Determine if hyphenation should occur at a position
 */
int should_hyphenate_at(
    const char *word,
    size_t word_len,
    size_t position,
    hyphen_weight_t threshold
) {
    /* Basic validity checks */
    if (!word || position == 0 || position >= word_len) {
        return 0;
    }

    /* Don't hyphenate too close to boundaries */
    if (position < 2 || position > word_len - 2) {
        return 0;
    }

    /* Both characters must be alphabetic */
    int left_idx = char_to_index(word[position - 1]);
    int right_idx = char_to_index(word[position]);

    if (left_idx < 0 || right_idx < 0) {
        return 0;
    }

    /* Calculate weight and compare to threshold */
    int at_beginning = (position <= 2);
    int at_ending = (position >= word_len - 2);

    /* Check for existing hyphens nearby */
    int has_context = 0;
    for (size_t i = 0; i < position; i++) {
        if (word[i] == '-') {
            has_context = 1;
            break;
        }
    }

    hyphen_weight_t weight = calculate_hyphen_weight(
        word, word_len, position,
        at_beginning, at_ending, has_context
    );

    return weight > threshold;
}

/**
 * Legacy C interface for compatibility
 */
int hytab_get_weight(char first, char second) {
    int first_idx = char_to_index(first);
    int second_idx = char_to_index(second);

    if (first_idx < 0 || second_idx < 0) {
        return 0;
    }

    return (int)get_digram_weight(first_idx, second_idx);
}
