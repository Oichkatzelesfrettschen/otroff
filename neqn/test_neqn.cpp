#include "cxx23_scaffold.hpp"
/**
 * @file test_neqn.c
 * @brief Simple test program for neqn functionality
 *
 * This program provides basic unit tests and functional tests
 * for the neqn mathematical equation preprocessor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ne.h"

/* Test counter */
static int tests_run = 0;
static int tests_passed = 0;

/* Test macros */
#define TEST_START()                              \
    do {                                          \
        tests_run++;                              \
        printf("Running test: %s... ", __func__); \
    } while (0)
#define TEST_PASS()       \
    do {                  \
        tests_passed++;   \
        printf("PASS\n"); \
    } while (0)
#define TEST_FAIL(msg)             \
    do {                           \
        printf("FAIL: %s\n", msg); \
    } while (0)
#define TEST_ASSERT(cond)     \
    do {                      \
        if (!(cond)) {        \
            TEST_FAIL(#cond); \
            return;           \
        }                     \
    } while (0)

/* Test functions */
static void test_neqn_init(void) {
    TEST_START();

    /* Test initialization */
    TEST_ASSERT(neqn_init() == NEQN_SUCCESS);

    /* Test double initialization (should succeed) */
    TEST_ASSERT(neqn_init() == NEQN_SUCCESS);

    TEST_PASS();
}

static void test_neqn_version(void) {
    TEST_START();

    const char *version = neqn_get_version();
    TEST_ASSERT(version != NULL);
    TEST_ASSERT(strlen(version) > 0);

    printf("(version: %s) ", version);
    TEST_PASS();
}

static void test_context_creation(void) {
    neqn_context_t *context;

    TEST_START();

    /* Test context creation */
    context = neqn_context_create();
    TEST_ASSERT(context != NULL);

    /* Test context destruction */
    neqn_context_destroy(context);

    TEST_PASS();
}

static void test_token_creation(void) {
    neqn_token_t *token;

    TEST_START();

    /* Test token creation */
    token = neqn_token_create(NEQN_TOKEN_IDENTIFIER, "test", 4);
    TEST_ASSERT(token != NULL);
    TEST_ASSERT(token->type == NEQN_TOKEN_IDENTIFIER);
    TEST_ASSERT(strcmp(token->text, "test") == 0);
    TEST_ASSERT(token->length == 4);

    /* Test token destruction */
    neqn_token_destroy(token);

    TEST_PASS();
}

static void test_node_creation(void) {
    neqn_node_t *node;

    TEST_START();

    /* Test node creation */
    node = neqn_node_create(NEQN_NODE_IDENTIFIER, "variable");
    TEST_ASSERT(node != NULL);
    TEST_ASSERT(node->type == NEQN_NODE_IDENTIFIER);
    TEST_ASSERT(strcmp(node->content, "variable") == 0);

    /* Test node destruction */
    neqn_node_destroy(node);

    TEST_PASS();
}

static void test_utility_functions(void) {
    char *dup_str;
    char buffer[100];

    TEST_START();

    /* Test string duplication */
    dup_str = neqn_strdup("hello");
    TEST_ASSERT(dup_str != NULL);
    TEST_ASSERT(strcmp(dup_str, "hello") == 0);
    free(dup_str);

    /* Test safe string concatenation */
    strcpy(buffer, "hello");
    TEST_ASSERT(neqn_strcat_safe(buffer, " world", sizeof(buffer)) == NEQN_SUCCESS);
    TEST_ASSERT(strcmp(buffer, "hello world") == 0);

    /* Test hash function */
    TEST_ASSERT(neqn_hash_string("test") == neqn_hash_string("test"));

    TEST_PASS();
}

static void test_error_handling(void) {
    neqn_context_t *context;

    TEST_START();

    context = neqn_context_create();
    TEST_ASSERT(context != NULL);

    /* Test error reporting */
    neqn_error(context, NEQN_ERROR_SYNTAX, "Test error message");
    TEST_ASSERT(context->error_count == 1);

    /* Test warning reporting */
    neqn_warning(context, "Test warning message");
    TEST_ASSERT(context->warning_count == 1);

    neqn_context_destroy(context);

    TEST_PASS();
}

static void test_basic_processing(void) {
    neqn_context_t *context;
    int result;

    TEST_START();

    context = neqn_context_create();
    TEST_ASSERT(context != NULL);

    /* Test processing a simple line */
    result = neqn_process_line(context, "x + y");
    TEST_ASSERT(result == NEQN_SUCCESS);

    neqn_context_destroy(context);

    TEST_PASS();
}

/* Main test runner */
int main(void) {
    printf("Starting neqn test suite...\n\n");

    /* Initialize neqn system for testing */
    if (neqn_init() != NEQN_SUCCESS) {
        printf("Failed to initialize neqn system\n");
        return EXIT_FAILURE;
    }

    /* Run all tests */
    test_neqn_init();
    test_neqn_version();
    test_context_creation();
    test_token_creation();
    test_node_creation();
    test_utility_functions();
    test_error_handling();
    test_basic_processing();

    /* Print results */
    printf("\nTest Results:\n");
    printf("  Tests run: %d\n", tests_run);
    printf("  Tests passed: %d\n", tests_passed);
    printf("  Tests failed: %d\n", tests_run - tests_passed);

    if (tests_passed == tests_run) {
        printf("  All tests PASSED!\n");
        return EXIT_SUCCESS;
    } else {
        printf("  Some tests FAILED!\n");
        return EXIT_FAILURE;
    }
}
