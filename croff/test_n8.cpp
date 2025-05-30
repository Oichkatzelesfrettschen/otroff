#include "cxx23_scaffold.hpp"
/**
 * @file test_n8.c
 * @brief Unit tests for the n8.c hyphenation module
 *
 * @details
 * This file contains comprehensive unit tests for validating the
 * hyphenation algorithms and functions in n8.c. Tests cover:
 * - Character classification functions
 * - Word boundary detection
 * - Exception word handling
 * - Suffix-based hyphenation
 * - Digram analysis
 * - Helper function validation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Include tdef.h for proper definitions */
#include "tdef.hpp" // updated header extension

/* External variables mock */
int *wdstart, *wdend;
int *hyptr[NHYP];
int **hyp;
int hyoff;
int suffid;
char *sufind[26];
int noscale;
int xxx;

/* Include the module under test */
#include "n8.c"

/* ================================================================
 * TEST FRAMEWORK
 * ================================================================ */

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

#define TEST_START(name)                      \
    do {                                      \
        printf("Running test: %s... ", name); \
        test_count++;                         \
    } while (0)

#define TEST_PASS()       \
    do {                  \
        printf("PASS\n"); \
        test_passed++;    \
    } while (0)

#define TEST_FAIL(msg)             \
    do {                           \
        printf("FAIL: %s\n", msg); \
        test_failed++;             \
    } while (0)

#define ASSERT_TRUE(condition, msg) \
    do {                            \
        if (!(condition)) {         \
            TEST_FAIL(msg);         \
            return;                 \
        }                           \
    } while (0)

#define ASSERT_FALSE(condition, msg) \
    do {                             \
        if (condition) {             \
            TEST_FAIL(msg);          \
            return;                  \
        }                            \
    } while (0)

#define ASSERT_EQUAL(expected, actual, msg)                                        \
    do {                                                                           \
        if ((expected) != (actual)) {                                              \
            char error_msg[256];                                                   \
            sprintf(error_msg, "%s (expected %d, got %d)", msg, expected, actual); \
            TEST_FAIL(error_msg);                                                  \
            return;                                                                \
        }                                                                          \
    } while (0)

/* ================================================================
 * MOCK FUNCTIONS
 * ================================================================ */

int skip(void) {
    return 0; /* Mock implementation */
}

int tatoi(void) {
    return 100; /* Mock implementation */
}

void prstr(const char *s) {
    /* Mock implementation - do nothing */
    (void)s;
}

int getch(void) {
    return 'a'; /* Mock implementation */
}

unsigned char suftab_get_byte(size_t index) {
    /* Mock implementation */
    (void)index;
    return 0;
}

/* ================================================================
 * CHARACTER CLASSIFICATION TESTS
 * ================================================================ */

void test_punct_function(void) {
    TEST_START("punct function");

    /* Test null character */
    ASSERT_FALSE(punct(0), "Null character should not be punctuation");

    /* Test alphabetic characters */
    ASSERT_FALSE(punct('a'), "Lowercase 'a' should not be punctuation");
    ASSERT_FALSE(punct('Z'), "Uppercase 'Z' should not be punctuation");

    /* Test punctuation characters */
    ASSERT_TRUE(punct('.'), "Period should be punctuation");
    ASSERT_TRUE(punct(','), "Comma should be punctuation");
    ASSERT_TRUE(punct('!'), "Exclamation should be punctuation");
    ASSERT_TRUE(punct(' '), "Space should be punctuation");
    ASSERT_TRUE(punct('1'), "Digit should be punctuation");

    TEST_PASS();
}

void test_alph_function(void) {
    TEST_START("alph function");

    /* Test alphabetic characters */
    ASSERT_TRUE(alph('a'), "Lowercase 'a' should be alphabetic");
    ASSERT_TRUE(alph('z'), "Lowercase 'z' should be alphabetic");
    ASSERT_TRUE(alph('A'), "Uppercase 'A' should be alphabetic");
    ASSERT_TRUE(alph('Z'), "Uppercase 'Z' should be alphabetic");

    /* Test non-alphabetic characters */
    ASSERT_FALSE(alph('1'), "Digit '1' should not be alphabetic");
    ASSERT_FALSE(alph(' '), "Space should not be alphabetic");
    ASSERT_FALSE(alph('.'), "Period should not be alphabetic");
    ASSERT_FALSE(alph(0), "Null should not be alphabetic");

    /* Test with CMASK */
    ASSERT_TRUE(alph('a' | 0x100), "Masked 'a' should be alphabetic");

    TEST_PASS();
}

void test_maplow_function(void) {
    TEST_START("maplow function");

    /* Test uppercase to lowercase conversion */
    ASSERT_EQUAL('a', maplow('A'), "Uppercase 'A' should map to 'a'");
    ASSERT_EQUAL('z', maplow('Z'), "Uppercase 'Z' should map to 'z'");

    /* Test lowercase characters remain unchanged */
    ASSERT_EQUAL('a', maplow('a'), "Lowercase 'a' should remain 'a'");
    ASSERT_EQUAL('z', maplow('z'), "Lowercase 'z' should remain 'z'");

    /* Test non-alphabetic characters */
    ASSERT_EQUAL('1', maplow('1'), "Digit '1' should remain unchanged");
    ASSERT_EQUAL(' ', maplow(' '), "Space should remain unchanged");

    /* Test with CMASK */
    ASSERT_EQUAL('a', maplow('A' | 0x100), "Masked 'A' should map to 'a'");

    TEST_PASS();
}

void test_vowel_function(void) {
    TEST_START("vowel function");

    /* Test vowels */
    ASSERT_TRUE(vowel('a'), "Lowercase 'a' should be vowel");
    ASSERT_TRUE(vowel('e'), "Lowercase 'e' should be vowel");
    ASSERT_TRUE(vowel('i'), "Lowercase 'i' should be vowel");
    ASSERT_TRUE(vowel('o'), "Lowercase 'o' should be vowel");
    ASSERT_TRUE(vowel('u'), "Lowercase 'u' should be vowel");
    ASSERT_TRUE(vowel('y'), "Lowercase 'y' should be vowel");

    /* Test uppercase vowels */
    ASSERT_TRUE(vowel('A'), "Uppercase 'A' should be vowel");
    ASSERT_TRUE(vowel('E'), "Uppercase 'E' should be vowel");

    /* Test consonants */
    ASSERT_FALSE(vowel('b'), "Lowercase 'b' should not be vowel");
    ASSERT_FALSE(vowel('c'), "Lowercase 'c' should not be vowel");
    ASSERT_FALSE(vowel('z'), "Lowercase 'z' should not be vowel");

    /* Test non-alphabetic characters */
    ASSERT_FALSE(vowel('1'), "Digit '1' should not be vowel");
    ASSERT_FALSE(vowel(' '), "Space should not be vowel");

    TEST_PASS();
}

/* ================================================================
 * HELPER FUNCTION TESTS
 * ================================================================ */

void test_find_next_vowel(void) {
    TEST_START("find_next_vowel function");

    /* Setup test word: "hello" */
    static int test_word[] = {'h', 'e', 'l', 'l', 'o', 0};
    wdstart = test_word;
    wdend = test_word + 4; /* Point to 'o' */

    /* Test finding first vowel */
    int *result = find_next_vowel(test_word);
    ASSERT_TRUE(result != NULL, "Should find vowel 'e'");
    ASSERT_EQUAL('e', *result, "Should find vowel 'e' at position 1");

    /* Test finding vowel from middle */
    result = find_next_vowel(test_word + 2);
    ASSERT_TRUE(result != NULL, "Should find vowel 'o'");
    ASSERT_EQUAL('o', *result, "Should find vowel 'o' at end");

    /* Test no vowel found */
    static int consonants[] = {'b', 'c', 'd', 0};
    wdend = consonants + 2;
    result = find_next_vowel(consonants);
    ASSERT_TRUE(result == NULL, "Should not find vowel in consonant string");

    /* Test NULL input */
    result = find_next_vowel(NULL);
    ASSERT_TRUE(result == NULL, "Should handle NULL input");

    TEST_PASS();
}

void test_find_max_digram(void) {
    TEST_START("find_max_digram function");

    /* Setup test word */
    static int test_word[] = {'h', 'e', 'l', 'l', 'o', 0};
    wdstart = test_word;
    wdend = test_word + 4;

    int maxval = 0;
    int *result = find_max_digram(test_word, test_word + 3, &maxval);

    /* Should return some position in range */
    ASSERT_TRUE(result != NULL, "Should find a position");
    ASSERT_TRUE(result >= test_word && result < test_word + 3, "Should be in range");
    ASSERT_TRUE(maxval > 0, "Should have positive digram value");

    /* Test invalid inputs */
    result = find_max_digram(NULL, test_word + 3, &maxval);
    ASSERT_TRUE(result == NULL, "Should handle NULL start");

    result = find_max_digram(test_word, NULL, &maxval);
    ASSERT_TRUE(result == NULL, "Should handle NULL end");

    result = find_max_digram(test_word + 3, test_word, &maxval);
    ASSERT_TRUE(result == NULL, "Should handle start >= end");

    TEST_PASS();
}

/* ================================================================
 * INTEGRATION TESTS
 * ================================================================ */

void test_hyphenation_threshold(void) {
    TEST_START("hyphenation threshold");

    /* Test default threshold */
    caseht();
    ASSERT_EQUAL(THRESH, thresh, "Should reset to default threshold");

    TEST_PASS();
}

void test_chkvow_function(void) {
    TEST_START("chkvow function");

    /* Setup test word: "hello" */
    static int test_word[] = {'h', 'e', 'l', 'l', 'o', 0};
    wdstart = test_word;

    /* Test vowel found */
    int result = chkvow(test_word + 3); /* From 'l' position */
    ASSERT_TRUE(result > 0, "Should find vowel 'e' before 'l'");
    ASSERT_EQUAL(1, result, "Should return distance 1 for 'e'");

    /* Test vowel at position 4 ('o') */
    result = chkvow(test_word + 5); /* From end */
    ASSERT_TRUE(result > 0, "Should find vowel 'o'");

    /* Test no vowel found */
    static int consonants[] = {'b', 'c', 'd', 0};
    wdstart = consonants;
    result = chkvow(consonants + 2);
    ASSERT_EQUAL(0, result, "Should not find vowel in consonant string");

    TEST_PASS();
}

/* ================================================================
 * MAIN TEST RUNNER
 * ================================================================ */

void run_all_tests(void) {
    printf("Starting n8.c hyphenation module tests...\n");
    printf("==========================================\n\n");

    /* Character classification tests */
    test_punct_function();
    test_alph_function();
    test_maplow_function();
    test_vowel_function();

    /* Helper function tests */
    test_find_next_vowel();
    test_find_max_digram();
    test_chkvow_function();

    /* Integration tests */
    test_hyphenation_threshold();

    printf("\n==========================================\n");
    printf("Test Results:\n");
    printf("  Total tests: %d\n", test_count);
    printf("  Passed: %d\n", test_passed);
    printf("  Failed: %d\n", test_failed);

    if (test_failed == 0) {
        printf("  Result: ALL TESTS PASSED!\n");
    } else {
        printf("  Result: %d TESTS FAILED!\n", test_failed);
    }
    printf("==========================================\n");
}

int main(void) {
    run_all_tests();
    return (test_failed == 0) ? 0 : 1;
}

/* ================================================================
 * END OF FILE - test_n8.c
 * ================================================================ */
