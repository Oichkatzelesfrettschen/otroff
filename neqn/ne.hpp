/**
 * @file ne.h
 * @brief Main header file for the neqn mathematical equation preprocessor
 *
 * @details
 * This header provides the core type definitions, constants, and function
 * prototypes for the neqn (new equation) preprocessor. The neqn system is
 * a simplified version of the eqn preprocessor designed for terminal output.
 *
 * Historical Context:
 * - Originally developed at AT&T Bell Labs (1977)
 * - Based on eqn by Brian Kernighan and Lorinda Cherry
 * - Part of the original UNIX troff text processing system
 *
 * @author Original: AT&T Bell Labs (1977)
 * @author Modern C90 port: Various contributors
 * @version 2.0 (C90 compliant)
 * @date 2023
 *
 * @copyright
 * Copyright (c) 1979 The Regents of the University of California.
 * Copyright (c) 2023 Modern contributors.
 * All rights reserved.
 */

#pragma once
#include "../cxx17_scaffold.hpp" // common utilities

/* ================================================================
 * SYSTEM INCLUDES - C90 Standard Library Headers
 * ================================================================ */
#include <cstdio> /* Standard I/O operations */
#include <cstdlib> /* Memory allocation, program control */
#include <cstring> /* String manipulation functions */
#include <cctype> /* Character classification */
#include <cstdarg> /* Variable argument lists */
#include <climits> /* Implementation-defined limits */

/* ================================================================
 * COMPILER COMPATIBILITY AND FEATURE DETECTION
 * ================================================================ */

/* Ensure C90 compliance */
#ifndef __STDC__
#error "This code requires a C90-compliant compiler"
#endif

/* Handle different compiler conventions */
#ifdef __GNUC__
#define NEQN_UNUSED __attribute__((unused))
#define NEQN_NORETURN __attribute__((noreturn))
#else
#define NEQN_UNUSED
#define NEQN_NORETURN
#endif

/* ================================================================
 * FUNDAMENTAL CONSTANTS AND LIMITS
 * ================================================================ */

/**
 * @def NEQN_MAX_LINE_LENGTH
 * @brief Maximum length of a single input line in characters
 */
#define NEQN_MAX_LINE_LENGTH 1024

/**
 * @def NEQN_MAX_TOKEN_LENGTH
 * @brief Maximum length of a single token/symbol
 */
#define NEQN_MAX_TOKEN_LENGTH 256

/**
 * @def NEQN_MAX_DEPTH
 * @brief Maximum nesting depth for equations
 */
#define NEQN_MAX_DEPTH 32

/**
 * @def NEQN_MAX_ARGS
 * @brief Maximum number of arguments for functions/operators
 */
#define NEQN_MAX_ARGS 16

/**
 * @def NEQN_BUFFER_SIZE
 * @brief Standard buffer size for internal operations
 */
#define NEQN_BUFFER_SIZE 4096

/**
 * @def NEQN_HASH_SIZE
 * @brief Size of hash tables for symbol lookup
 */
#define NEQN_HASH_SIZE 127

/* ================================================================
 * ERROR CODES AND STATUS VALUES
 * ================================================================ */

/**
 * @brief Error codes returned by neqn functions
 */
typedef enum {
    NEQN_SUCCESS = 0, /**< Operation completed successfully */
    NEQN_ERROR_MEMORY = -1, /**< Memory allocation failure */
    NEQN_ERROR_SYNTAX = -2, /**< Syntax error in input */
    NEQN_ERROR_IO = -3, /**< Input/output error */
    NEQN_ERROR_OVERFLOW = -4, /**< Buffer or stack overflow */
    NEQN_ERROR_UNDERFLOW = -5, /**< Stack underflow */
    NEQN_ERROR_INVALID = -6, /**< Invalid argument or state */
    NEQN_ERROR_NOT_FOUND = -7, /**< Symbol or resource not found */
    NEQN_ERROR_RANGE = -8, /**< Value out of valid range */
    NEQN_ERROR_FORMAT = -9 /**< Invalid format specification */
} neqn_error_t;

/* ================================================================
 * CORE DATA TYPES AND STRUCTURES
 * ================================================================ */

/**
 * @brief Token types for lexical analysis
 */
typedef enum {
    NEQN_TOKEN_EOF = 0, /**< End of file */
    NEQN_TOKEN_NEWLINE, /**< Newline character */
    NEQN_TOKEN_IDENTIFIER, /**< Variable or function name */
    NEQN_TOKEN_NUMBER, /**< Numeric literal */
    NEQN_TOKEN_STRING, /**< String literal */
    NEQN_TOKEN_OPERATOR, /**< Mathematical operator */
    NEQN_TOKEN_DELIMITER, /**< Punctuation/delimiter */
    NEQN_TOKEN_KEYWORD, /**< Reserved keyword */
    NEQN_TOKEN_ERROR /**< Lexical error */
} neqn_token_type_t;

/**
 * @brief Lexical token structure
 */
typedef struct neqn_token {
    neqn_token_type_t type; /**< Token type classification */
    char *text; /**< Token text content */
    size_t length; /**< Length of token text */
    int line_number; /**< Source line number */
    int column_number; /**< Column position in line */
} neqn_token_t;

/**
 * @brief Node types in the expression tree
 */
typedef enum {
    NEQN_NODE_NUMBER = 0, /**< Numeric constant */
    NEQN_NODE_IDENTIFIER, /**< Variable reference */
    NEQN_NODE_OPERATOR, /**< Binary/unary operator */
    NEQN_NODE_FUNCTION, /**< Function call */
    NEQN_NODE_GROUP, /**< Parenthesized expression */
    NEQN_NODE_SUPER, /**< Superscript */
    NEQN_NODE_SUB, /**< Subscript */
    NEQN_NODE_FRACTION, /**< Fraction (over/under) */
    NEQN_NODE_SQRT, /**< Square root */
    NEQN_NODE_MATRIX /**< Matrix/array */
} neqn_node_type_t;

/**
 * @brief Expression tree node structure
 */
typedef struct neqn_node {
    neqn_node_type_t type; /**< Node type */
    char *content; /**< Node content (text) */
    struct neqn_node *left; /**< Left child node */
    struct neqn_node *right; /**< Right child node */
    struct neqn_node *next; /**< Next sibling node */
    int precedence; /**< Operator precedence */
    int line_number; /**< Source line number */
} neqn_node_t;

/**
 * @brief Symbol table entry
 */
typedef struct neqn_symbol {
    char *name; /**< Symbol name */
    char *value; /**< Symbol value/definition */
    neqn_node_t *tree; /**< Parsed expression tree */
    struct neqn_symbol *next; /**< Hash chain link */
    int line_defined; /**< Line where symbol was defined */
    int is_builtin; /**< Built-in symbol flag */
} neqn_symbol_t;

/**
 * @brief Input/output context structure
 */
typedef struct neqn_context {
    FILE *input; /**< Input file stream */
    FILE *output; /**< Output file stream */
    char *input_filename; /**< Input file name */
    char *output_filename; /**< Output file name */
    int line_number; /**< Current line number */
    int column_number; /**< Current column number */
    int error_count; /**< Total error count */
    int warning_count; /**< Total warning count */
    neqn_symbol_t *symbols[NEQN_HASH_SIZE]; /**< Symbol table */
    char *current_line; /**< Current input line buffer */
    size_t line_capacity; /**< Line buffer capacity */
    int debug_level; /**< Debug output level */
    int strict_mode; /**< Strict parsing mode flag */
} neqn_context_t;

/* ================================================================
 * GLOBAL VARIABLES AND STATE
 * ================================================================ */

/* Note: In C90, external variables should be declared in headers
   and defined in exactly one source file */

extern neqn_context_t *neqn_current_context; /**< Current processing context */
extern int neqn_exit_status; /**< Program exit status */

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * CORE FUNCTION PROTOTYPES - Initialization and Cleanup
 * ================================================================ */

/**
 * @brief Initialize the neqn system
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_init(void);

/**
 * @brief Clean up and shutdown the neqn system
 */
void neqn_cleanup(void);

/**
 * @brief Get neqn version string
 * @return Pointer to static version string
 */
const char *neqn_get_version(void);

/**
 * @brief Set debug output level
 * @param level Debug level (0-3)
 */
void neqn_set_debug_level(int level);

/**
 * @brief Get current debug level
 * @return Current debug level
 */
int neqn_get_debug_level(void);

/* ================================================================
 * CONTEXT MANAGEMENT FUNCTIONS
 * ================================================================ */

/**
 * @brief Create a new neqn processing context
 * @return Pointer to new context, or NULL on failure
 */
neqn_context_t *neqn_context_create(void);

/**
 * @brief Destroy a neqn processing context
 * @param context Context to destroy
 */
void neqn_context_destroy(neqn_context_t *context);

/**
 * @brief Set input file for context
 * @param context Processing context
 * @param filename Input filename (NULL for stdin)
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_context_set_input(neqn_context_t *context, const char *filename);

/**
 * @brief Set output file for context
 * @param context Processing context
 * @param filename Output filename (NULL for stdout)
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_context_set_output(neqn_context_t *context, const char *filename);

/* ================================================================
 * INPUT/OUTPUT AND LINE PROCESSING FUNCTIONS
 * ================================================================ */

/**
 * @brief Read next line from input (replaces problematic getline)
 * @param context Processing context
 * @param buffer Pointer to line buffer (may be reallocated)
 * @param capacity Pointer to buffer capacity
 * @return Number of characters read, or -1 on EOF/error
 */
int neqn_read_line(neqn_context_t *context, char **buffer, size_t *capacity);

/**
 * @brief Write formatted output
 * @param context Processing context
 * @param format Printf-style format string
 * @param ... Variable arguments
 * @return Number of characters written, or -1 on error
 */
int neqn_write_output(neqn_context_t *context, const char *format, ...);

/**
 * @brief Process a single line of input
 * @param context Processing context
 * @param line Input line to process
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_process_line(neqn_context_t *context, const char *line);

/* ================================================================
 * LEXICAL ANALYSIS FUNCTIONS
 * ================================================================ */

/**
 * @brief Create a new token
 * @param type Token type
 * @param text Token text
 * @param length Text length
 * @return Pointer to new token, or NULL on failure
 */
neqn_token_t *neqn_token_create(neqn_token_type_t type,
                                const char *text,
                                size_t length);

/**
 * @brief Destroy a token
 * @param token Token to destroy
 */
void neqn_token_destroy(neqn_token_t *token);

/**
 * @brief Get next token from input
 * @param context Processing context
 * @param line Input line
 * @param position Current position in line (updated)
 * @return Pointer to next token, or NULL on EOF/error
 */
neqn_token_t *neqn_get_next_token(neqn_context_t *context,
                                  const char *line,
                                  size_t *position);

/* ================================================================
 * EXPRESSION TREE FUNCTIONS
 * ================================================================ */

/**
 * @brief Create a new expression tree node
 * @param type Node type
 * @param content Node content
 * @return Pointer to new node, or NULL on failure
 */
neqn_node_t *neqn_node_create(neqn_node_type_t type, const char *content);

/**
 * @brief Destroy an expression tree
 * @param node Root node of tree to destroy
 */
void neqn_node_destroy(neqn_node_t *node);

/**
 * @brief Parse tokens into expression tree
 * @param context Processing context
 * @param tokens Array of tokens
 * @param count Number of tokens
 * @return Root node of parsed tree, or NULL on failure
 */
neqn_node_t *neqn_parse_expression(neqn_context_t *context,
                                   neqn_token_t **tokens,
                                   size_t count);

/* ================================================================
 * SYMBOL TABLE FUNCTIONS
 * ================================================================ */

/**
 * @brief Initialize built-in mathematical symbols
 */
int neqn_init_builtin_symbols(neqn_context_t *context);

/**
 * @brief Enhanced symbol lookup with built-in fallback
 */
neqn_symbol_t *neqn_symbol_lookup_enhanced(neqn_context_t *context, const char *name);

/**
 * @brief Print all symbols for debugging
 */
void neqn_debug_print_symbols(neqn_context_t *context);

/**
 * @brief Enhanced symbol definition with conflict checking
 */
int neqn_symbol_define_enhanced(neqn_context_t *context,
                                const char *name,
                                const char *value);

/* ================================================================
 * MATHEMATICAL FORMATTING FUNCTIONS
 * ================================================================ */

/**
 * @brief Format superscript for terminal output
 */
neqn_node_t *neqn_superscript(neqn_node_t *base, neqn_node_t *exponent);

/**
 * @brief Format subscript for terminal output
 */
neqn_node_t *neqn_subscript(neqn_node_t *base, neqn_node_t *subscript);

/**
 * @brief Format fraction for terminal output
 */
neqn_node_t *neqn_fraction(neqn_node_t *numerator, neqn_node_t *denominator);

/**
 * @brief Format square root for terminal output
 */
neqn_node_t *neqn_sqrt(neqn_node_t *expression);

/**
 * @brief Format equation for terminal output with symbol substitution
 */
int neqn_format_equation(neqn_context_t *context,
                         neqn_node_t *tree,
                         char *buffer,
                         size_t capacity);

/* ================================================================
 * SYMBOL TABLE FUNCTIONS (ORIGINAL)
 * ================================================================ */

/**
 * @brief Add symbol to symbol table
 * @param context Processing context
 * @param name Symbol name
 * @param value Symbol value
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_symbol_define(neqn_context_t *context,
                       const char *name,
                       const char *value);

/**
 * @brief Look up symbol in symbol table
 * @param context Processing context
 * @param name Symbol name
 * @return Pointer to symbol, or NULL if not found
 */
neqn_symbol_t *neqn_symbol_lookup(neqn_context_t *context, const char *name);

/**
 * @brief Remove symbol from symbol table
 * @param context Processing context
 * @param name Symbol name
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_symbol_undefine(neqn_context_t *context, const char *name);

/* ================================================================
 * CODE GENERATION AND OUTPUT FUNCTIONS
 * ================================================================ */

/**
 * @brief Generate output from expression tree
 * @param context Processing context
 * @param tree Expression tree root
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_generate_output(neqn_context_t *context, neqn_node_t *tree);

/**
 * @brief Format equation for terminal output
 * @param context Processing context
 * @param tree Expression tree
 * @param buffer Output buffer
 * @param capacity Buffer capacity
 * @return Number of characters written, or -1 on error
 */
int neqn_format_equation(neqn_context_t *context,
                         neqn_node_t *tree,
                         char *buffer,
                         size_t capacity);

/* ================================================================
 * ERROR HANDLING AND REPORTING FUNCTIONS
 * ================================================================ */

/**
 * @brief Report error with context information
 * @param context Processing context
 * @param error_code Error code
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void neqn_error(neqn_context_t *context,
                neqn_error_t error_code,
                const char *format, ...);

/**
 * @brief Report warning with context information
 * @param context Processing context
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void neqn_warning(neqn_context_t *context, const char *format, ...);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Pointer to static error message string
 */
const char *neqn_error_message(neqn_error_t error_code);

/* ================================================================
 * UTILITY AND HELPER FUNCTIONS
 * ================================================================ */

/**
 * @brief Register instance for tracking
 * @return Instance count, or -1 on error
 */
int neqn_register_instance(void);

/**
 * @brief Unregister instance
 * @return Instance count, or -1 on error
 */
int neqn_unregister_instance(void);

/**
 * @brief Report error to global tracking system
 * @param error_code Error code
 * @param message Optional error message
 */
void neqn_report_error(int error_code, const char *message);

/**
 * @brief Get total error count
 * @return Total number of errors encountered
 */
int neqn_get_error_count(void);

/**
 * @brief Safe string duplication with error checking
 * @param str String to duplicate
 * @return Pointer to duplicated string, or NULL on failure
 */
char *neqn_strdup(const char *str);

/**
 * @brief Safe string concatenation with bounds checking
 * @param dest Destination buffer
 * @param src Source string
 * @param dest_size Size of destination buffer
 * @return NEQN_SUCCESS on success, error code on failure
 */
int neqn_strcat_safe(char *dest, const char *src, size_t dest_size);

/**
 * @brief Calculate hash value for string
 * @param str String to hash
 * @return Hash value
 */
unsigned int neqn_hash_string(const char *str);

/* ================================================================
 * BACKWARD COMPATIBILITY FUNCTIONS
 * ================================================================ */

/**
 * @brief Legacy module initialization (deprecated)
 * @deprecated Use neqn_init() instead
 */
void neqn_module_init(void);

/* ================================================================
 * DEBUGGING AND DIAGNOSTIC FUNCTIONS
 * ================================================================ */

/**
 * @brief Print expression tree for debugging
 * @param tree Expression tree root
 * @param indent Indentation level
 */
void neqn_debug_print_tree(neqn_node_t *tree, int indent);

/**
 * @brief Print symbol table for debugging
 * @param context Processing context
 */
void neqn_debug_print_symbols(neqn_context_t *context);

/**
 * @brief Validate expression tree integrity
 * @param tree Expression tree root
 * @return NEQN_SUCCESS if valid, error code on problems
 */
int neqn_validate_tree(neqn_node_t *tree);

/* ================================================================
 * MATHEMATICAL FUNCTION PROTOTYPES
 * ================================================================ */

/**
 * @brief Process mathematical fraction (over/under)
 * @param numerator Numerator expression
 * @param denominator Denominator expression
 * @return Formatted fraction node
 */
neqn_node_t *neqn_fraction(neqn_node_t *numerator, neqn_node_t *denominator);

/**
 * @brief Process square root expression
 * @param expression Expression under radical
 * @return Formatted square root node
 */
neqn_node_t *neqn_sqrt(neqn_node_t *expression);

/**
 * @brief Process superscript expression
 * @param base Base expression
 * @param exponent Superscript expression
 * @return Formatted superscript node
 */
neqn_node_t *neqn_superscript(neqn_node_t *base, neqn_node_t *exponent);

/**
 * @brief Process subscript expression
 * @param base Base expression
 * @param subscript Subscript expression
 * @return Formatted subscript node
 */
neqn_node_t *neqn_subscript(neqn_node_t *base, neqn_node_t *subscript);

/* ================================================================
 * CONFIGURATION AND FEATURE CONTROL
 * ================================================================ */

/**
 * @brief Enable or disable strict parsing mode
 * @param context Processing context
 * @param strict_mode Non-zero to enable strict mode
 */
void neqn_set_strict_mode(neqn_context_t *context, int strict_mode);

/**
 * @brief Check if strict mode is enabled
 * @param context Processing context
 * @return Non-zero if strict mode enabled
 */
int neqn_is_strict_mode(neqn_context_t *context);

/* ================================================================
 * END OF HEADER - Multiple inclusion protection
 * ================================================================ */

#ifdef __cplusplus
} // extern "C"
#endif

/* ================================================================
 * DOCUMENTATION NOTES
 * ================================================================
 *
 * This header file provides a complete interface for the neqn
 * mathematical equation preprocessor. Key design principles:
 *
 * 1. C90 Compliance: All code follows C90 standards for maximum
 *    portability across different systems and compilers.
 *
 * 2. Memory Safety: All functions that allocate memory have
 *    corresponding cleanup functions. Buffer sizes are explicitly
 *    managed to prevent overflows.
 *
 * 3. Error Handling: Comprehensive error reporting with specific
 *    error codes and detailed diagnostic messages.
 *
 * 4. Backward Compatibility: Legacy function names are preserved
 *    but marked as deprecated to ease migration.
 *
 * 5. Extensibility: The modular design allows for easy addition
 *    of new mathematical operators and functions.
 *
 * 6. Thread Safety: While not thread-safe by default, the design
 *    allows for future thread-safety enhancements through context
 *    isolation.
 *
 * For usage examples and detailed documentation, see the individual
 * source files and the project documentation.
 *
 * ================================================================
 */
