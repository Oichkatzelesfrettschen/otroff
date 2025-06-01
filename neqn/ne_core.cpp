#include "../cxx17_scaffold.hpp"
/**
 * @file ne_core.c
 * @brief Core implementation functions for the neqn preprocessor
 *
 * @details
 * This module implements the essential functionality for the neqn
 * mathematical equation preprocessor, including context management,
 * input/output handling, lexical analysis, and basic equation processing.
 *
 * @author Original: AT&T Bell Labs (1977)
 * @author Modern C90 port: Various contributors
 * @version 2.0 (C90 compliant)
 * @date 2023
 */

/* ================================================================
 * SYSTEM INCLUDES
 * ================================================================ */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <cstdarg>

/* ================================================================
 * PROJECT INCLUDES
 * ================================================================ */
#include "ne.hpp"              // main header
#include "os_abstraction.hpp"  // platform wrappers

/* ================================================================
 * MODULE CONSTANTS
 * ================================================================ */
#define NEQN_INITIAL_LINE_SIZE 256
#define NEQN_LINE_GROWTH_FACTOR 2

/* ================================================================
 * CONTEXT MANAGEMENT FUNCTIONS
 * ================================================================ */

/**
 * @brief Create a new neqn processing context
 */
neqn_context_t *neqn_context_create(void) {
    neqn_context_t *context;
    int i;

    context = malloc(sizeof(neqn_context_t));
    if (context == nullptr) {
        return nullptr;
    }

    /* Initialize all fields to safe defaults */
    memset(context, 0, sizeof(neqn_context_t));

    context->input = stdin;
    context->output = stdout;
    context->input_filename = nullptr;
    context->output_filename = nullptr;
    context->line_number = 0;
    context->column_number = 0;
    context->error_count = 0;
    context->warning_count = 0;
    context->debug_level = 0;
    context->strict_mode = 0;

    /* Initialize symbol table */
    for (i = 0; i < NEQN_HASH_SIZE; i++) {
        context->symbols[i] = nullptr;
    }

    /* Allocate initial line buffer */
    context->line_capacity = NEQN_INITIAL_LINE_SIZE;
    context->current_line = malloc(context->line_capacity);
    if (context->current_line == nullptr) {
        free(context);
        return nullptr;
    }

    return context;
}

/**
 * @brief Destroy a neqn processing context
 */
void neqn_context_destroy(neqn_context_t *context) {
    int i;
    neqn_symbol_t *sym, *next;

    if (context == nullptr) {
        return;
    }

    /* Close files if they're not standard streams */
    if (context->input != nullptr && context->input != stdin) {
        os_fclose(context->input);
    }

    if (context->output != nullptr && context->output != stdout) {
        os_fclose(context->output);
    }

    /* Free filename strings */
    if (context->input_filename != nullptr) {
        free(context->input_filename);
    }

    if (context->output_filename != nullptr) {
        free(context->output_filename);
    }

    /* Free line buffer */
    if (context->current_line != nullptr) {
        free(context->current_line);
    }

    /* Free symbol table */
    for (i = 0; i < NEQN_HASH_SIZE; i++) {
        sym = context->symbols[i];
        while (sym != nullptr) {
            next = sym->next;
            if (sym->name != nullptr) {
                free(sym->name);
            }
            if (sym->value != nullptr) {
                free(sym->value);
            }
            if (sym->tree != nullptr) {
                neqn_node_destroy(sym->tree);
            }
            free(sym);
            sym = next;
        }
    }

    free(context);
}

/**
 * @brief Set input file for context
 */
int neqn_context_set_input(neqn_context_t *context, const char *filename) {
    FILE *new_input;

    if (context == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    /* Handle stdin case */
    if (filename == nullptr) {
        new_input = stdin;
    } else {
        new_input = os_fopen(filename, "r");
        if (new_input == nullptr) {
            return NEQN_ERROR_IO;
        }
    }

    /* Close previous input if not stdin */
    if (context->input != nullptr && context->input != stdin) {
        os_fclose(context->input);
    }

    /* Update context */
    context->input = new_input;

    /* Update filename */
    if (context->input_filename != nullptr) {
        free(context->input_filename);
        context->input_filename = nullptr;
    }

    if (filename != nullptr) {
        context->input_filename = neqn_strdup(filename);
        if (context->input_filename == nullptr) {
            return NEQN_ERROR_MEMORY;
        }
    }

    /* Reset line tracking */
    context->line_number = 0;
    context->column_number = 0;

    return NEQN_SUCCESS;
}

/**
 * @brief Set output file for context
 */
int neqn_context_set_output(neqn_context_t *context, const char *filename) {
    FILE *new_output;

    if (context == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    /* Handle stdout case */
    if (filename == nullptr) {
        new_output = stdout;
    } else {
        new_output = os_fopen(filename, "w");
        if (new_output == nullptr) {
            return NEQN_ERROR_IO;
        }
    }

    /* Close previous output if not stdout */
    if (context->output != nullptr && context->output != stdout) {
        os_fclose(context->output);
    }

    /* Update context */
    context->output = new_output;

    /* Update filename */
    if (context->output_filename != nullptr) {
        free(context->output_filename);
        context->output_filename = nullptr;
    }

    if (filename != nullptr) {
        context->output_filename = neqn_strdup(filename);
        if (context->output_filename == nullptr) {
            return NEQN_ERROR_MEMORY;
        }
    }

    return NEQN_SUCCESS;
}

/* ================================================================
 * INPUT/OUTPUT FUNCTIONS
 * ================================================================ */

/**
 * @brief Read next line from input (replaces getline)
 */
int neqn_read_line(neqn_context_t *context, char **buffer, size_t *capacity) {
    char *line_buffer;
    size_t buffer_size;
    size_t pos = 0;
    int ch;

    if (context == nullptr || buffer == nullptr || capacity == nullptr) {
        return -1;
    }

    /* Use context buffer if none provided */
    if (*buffer == nullptr || *capacity == 0) {
        *buffer = context->current_line;
        *capacity = context->line_capacity;
    }

    line_buffer = *buffer;
    buffer_size = *capacity;

    /* Read characters until newline or EOF */
    while ((ch = fgetc(context->input)) != EOF) {
        /* Expand buffer if needed */
        if (pos >= buffer_size - 1) {
            size_t new_size = buffer_size * NEQN_LINE_GROWTH_FACTOR;
            char *new_buffer = static_cast<char *>(realloc(line_buffer, new_size));
            if (new_buffer == nullptr) {
                return -1;
            }

            line_buffer = new_buffer;
            buffer_size = new_size;

            /* Update context if using context buffer */
            if (*buffer == context->current_line) {
                context->current_line = line_buffer;
                context->line_capacity = buffer_size;
            }

            *buffer = line_buffer;
            *capacity = buffer_size;
        }

        /* Store character */
        line_buffer[pos++] = static_cast<char>(ch);

        /* Check for end of line */
        if (ch == '\n') {
            break;
        }
    }

    /* Null-terminate */
    if (pos < buffer_size) {
        line_buffer[pos] = '\0';
    }

    /* Return number of characters read, or -1 on EOF with no characters */
    if (pos == 0 && ch == EOF) {
        return -1;
    }

    return (int)pos;
}

/**
 * @brief Write formatted output
 */
int neqn_write_output(neqn_context_t *context, const char *format, ...) {
    va_list args;
    int result;

    if (context == nullptr || format == nullptr) {
        return -1;
    }

    va_start(args, format);
    result = vfprintf(context->output, format, args);
    va_end(args);

    return result;
}

/**
 * @brief Process a single line of input
 */
int neqn_process_line(neqn_context_t *context, const char *line) {
    size_t pos = 0;
    neqn_token_t *token;
    neqn_token_t *tokens[NEQN_MAX_ARGS];
    size_t token_count = 0;
    neqn_node_t *tree;
    int result = NEQN_SUCCESS;
    size_t i;

    if (context == nullptr || line == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    /* Skip empty lines */
    if (strlen(line) == 0 || (strlen(line) == 1 && line[0] == '\n')) {
        neqn_write_output(context, "\n");
        return NEQN_SUCCESS;
    }

    /* Tokenize the line */
    while (pos < strlen(line) && token_count < NEQN_MAX_ARGS) {
        token = neqn_get_next_token(context, line, &pos);
        if (token == nullptr) {
            break;
        }

        if (token->type == NEQN_TOKEN_EOF) {
            neqn_token_destroy(token);
            break;
        }

        tokens[token_count++] = token;
    }

    /* Parse tokens into expression tree */
    if (token_count > 0) {
        tree = neqn_parse_expression(context, tokens, token_count);
        if (tree != nullptr) {
            /* Generate output */
            result = neqn_generate_output(context, tree);
            neqn_node_destroy(tree);
        } else {
            neqn_error(context, NEQN_ERROR_SYNTAX, "Failed to parse expression");
            result = NEQN_ERROR_SYNTAX;
        }
    }

    /* Clean up tokens */
    for (i = 0; i < token_count; i++) {
        neqn_token_destroy(tokens[i]);
    }

    return result;
}

/* ================================================================
 * TOKEN MANAGEMENT FUNCTIONS
 * ================================================================ */

/**
 * @brief Create a new token
 */
neqn_token_t *neqn_token_create(neqn_token_type_t type,
                                const char *text,
                                size_t length) {
    neqn_token_t *token;

    token = static_cast<neqn_token_t *>(malloc(sizeof(neqn_token_t)));
    if (token == nullptr) {
        return nullptr;
    }

    token->type = type;
    token->length = length;
    token->line_number = 0;
    token->column_number = 0;

    if (text != nullptr && length > 0) {
        token->text = static_cast<char *>(malloc(length + 1));
        if (token->text == nullptr) {
            free(token);
            return nullptr;
        }
        memcpy(token->text, text, length);
        token->text[length] = '\0';
    } else {
        token->text = nullptr;
    }

    return token;
}

/**
 * @brief Destroy a token
 */
void neqn_token_destroy(neqn_token_t *token) {
    if (token == nullptr) {
        return;
    }

    if (token->text != nullptr) {
        free(token->text);
    }

    free(token);
}

/**
 * @brief Get next token from input
 */
neqn_token_t *neqn_get_next_token(neqn_context_t *context,
                                  const char *line,
                                  size_t *position) {
    size_t pos;
    size_t start;
    neqn_token_type_t type;

    if (context == nullptr || line == nullptr || position == nullptr) {
        return nullptr;
    }

    pos = *position;

    /* Skip whitespace */
    while (pos < strlen(line) && isspace(line[pos]) && line[pos] != '\n') {
        pos++;
    }

    /* Check for end of line */
    if (pos >= strlen(line) || line[pos] == '\n') {
        *position = pos;
        return neqn_token_create(NEQN_TOKEN_EOF, nullptr, 0);
    }

    start = pos;

    /* Classify token */
    if (isalpha(line[pos])) {
        /* Identifier or keyword */
        while (pos < strlen(line) && (isalnum(line[pos]) || line[pos] == '_')) {
            pos++;
        }
        type = NEQN_TOKEN_IDENTIFIER;
    } else if (isdigit(line[pos])) {
        /* Number */
        while (pos < strlen(line) && (isdigit(line[pos]) || line[pos] == '.')) {
            pos++;
        }
        type = NEQN_TOKEN_NUMBER;
    } else if (line[pos] == '"') {
        /* String literal */
        pos++; /* Skip opening quote */
        while (pos < strlen(line) && line[pos] != '"' && line[pos] != '\n') {
            if (line[pos] == '\\' && pos + 1 < strlen(line)) {
                pos += 2; /* Skip escaped character */
            } else {
                pos++;
            }
        }
        if (pos < strlen(line) && line[pos] == '"') {
            pos++; /* Skip closing quote */
        }
        type = NEQN_TOKEN_STRING;
    } else {
        /* Operator or delimiter */
        pos++;
        type = NEQN_TOKEN_OPERATOR;
    }

    *position = pos;
    return neqn_token_create(type, line + start, pos - start);
}

/* ================================================================
 * EXPRESSION TREE FUNCTIONS
 * ================================================================ */

/**
 * @brief Create a new expression tree node
 */
neqn_node_t *neqn_node_create(neqn_node_type_t type, const char *content) {
    neqn_node_t *node;

    node = static_cast<neqn_node_t *>(malloc(sizeof(neqn_node_t)));
    if (node == nullptr) {
        return nullptr;
    }

    node->type = type;
    node->left = nullptr;
    node->right = nullptr;
    node->next = nullptr;
    node->precedence = 0;
    node->line_number = 0;

    if (content != nullptr) {
        node->content = neqn_strdup(content);
        if (node->content == nullptr) {
            free(node);
            return nullptr;
        }
    } else {
        node->content = nullptr;
    }

    return node;
}

/**
 * @brief Destroy an expression tree
 */
void neqn_node_destroy(neqn_node_t *node) {
    if (node == nullptr) {
        return;
    }

    /* Recursively destroy children */
    neqn_node_destroy(node->left);
    neqn_node_destroy(node->right);
    neqn_node_destroy(node->next);

    /* Free content */
    if (node->content != nullptr) {
        free(node->content);
    }

    free(node);
}

/**
 * @brief Parse tokens into expression tree (simplified implementation)
 */
neqn_node_t *neqn_parse_expression(neqn_context_t *context,
                                   neqn_token_t **tokens,
                                   size_t count) {
    neqn_node_t *root;
    size_t i;

    if (context == nullptr || tokens == nullptr || count == 0) {
        return nullptr;
    }

    /* Simple implementation: create a sequence of nodes */
    root = nullptr;

    for (i = 0; i < count; i++) {
        neqn_node_t *node;
        neqn_node_type_t node_type;

        /* Map token type to node type */
        switch (tokens[i]->type) {
        case NEQN_TOKEN_NUMBER:
            node_type = NEQN_NODE_NUMBER;
            break;
        case NEQN_TOKEN_IDENTIFIER:
            node_type = NEQN_NODE_IDENTIFIER;
            break;
        case NEQN_TOKEN_OPERATOR:
            node_type = NEQN_NODE_OPERATOR;
            break;
        default:
            node_type = NEQN_NODE_IDENTIFIER;
            break;
        }

        node = neqn_node_create(node_type, tokens[i]->text);
        if (node == nullptr) {
            neqn_node_destroy(root);
            return nullptr;
        }

        node->line_number = tokens[i]->line_number;

        /* Link nodes in sequence */
        if (root == nullptr) {
            root = node;
        } else {
            neqn_node_t *current = root;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = node;
        }
    }

    return root;
}

/* ================================================================
 * OUTPUT GENERATION FUNCTIONS
 * ================================================================ */

/**
 * @brief Generate output from expression tree
 */
int neqn_generate_output(neqn_context_t *context, neqn_node_t *tree) {
    neqn_node_t *current;

    if (context == nullptr || tree == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    /* Simple implementation: output nodes in sequence */
    current = tree;
    while (current != nullptr) {
        if (current->content != nullptr) {
            neqn_write_output(context, "%s", current->content);
            if (current->next != nullptr) {
                neqn_write_output(context, " ");
            }
        }
        current = current->next;
    }

    neqn_write_output(context, "\n");

    return NEQN_SUCCESS;
}

/* ================================================================
 * ERROR HANDLING FUNCTIONS
 * ================================================================ */

/**
 * @brief Report error with context information
 */
void neqn_error(neqn_context_t *context,
                neqn_error_t error_code,
                const char *format, ...) {
    va_list args;

    if (context != nullptr) {
        context->error_count++;

        fprintf(stderr, "neqn: ");
        if (context->input_filename != nullptr) {
            fprintf(stderr, "%s:", context->input_filename);
        }
        if (context->line_number > 0) {
            fprintf(stderr, "%d:", context->line_number);
        }
        fprintf(stderr, " error: ");

        if (format != nullptr) {
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
        }

        fprintf(stderr, "\n");
    }
}

/**
 * @brief Report warning with context information
 */
void neqn_warning(neqn_context_t *context, const char *format, ...) {
    va_list args;

    if (context != nullptr) {
        context->warning_count++;

        fprintf(stderr, "neqn: ");
        if (context->input_filename != nullptr) {
            fprintf(stderr, "%s:", context->input_filename);
        }
        if (context->line_number > 0) {
            fprintf(stderr, "%d:", context->line_number);
        }
        fprintf(stderr, " warning: ");

        if (format != nullptr) {
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
        }

        fprintf(stderr, "\n");
    }
}

/**
 * @brief Get error message for error code
 */
const char *neqn_error_message(neqn_error_t error_code) {
    switch (error_code) {
    case NEQN_SUCCESS:
        return "No error";
    case NEQN_ERROR_MEMORY:
        return "Memory allocation failure";
    case NEQN_ERROR_SYNTAX:
        return "Syntax error";
    case NEQN_ERROR_IO:
        return "Input/output error";
    case NEQN_ERROR_OVERFLOW:
        return "Buffer overflow";
    case NEQN_ERROR_UNDERFLOW:
        return "Stack underflow";
    case NEQN_ERROR_INVALID:
        return "Invalid argument";
    case NEQN_ERROR_NOT_FOUND:
        return "Not found";
    case NEQN_ERROR_RANGE:
        return "Out of range";
    case NEQN_ERROR_FORMAT:
        return "Invalid format";
    default:
        return "Unknown error";
    }
}

/* ================================================================
 * UTILITY FUNCTIONS
 * ================================================================ */

/**
 * @brief Safe string duplication with error checking
 */
char *neqn_strdup(const char *str) {
    char *copy;
    size_t len;

    if (str == nullptr) {
        return nullptr;
    }

    len = strlen(str);
    copy = static_cast<char *>(malloc(len + 1));
    if (copy == nullptr) {
        return nullptr;
    }

    memcpy(copy, str, len + 1);
    return copy;
}

/**
 * @brief Safe string concatenation with bounds checking
 */
int neqn_strcat_safe(char *dest, const char *src, size_t dest_size) {
    size_t dest_len;
    size_t src_len;

    if (dest == nullptr || src == nullptr || dest_size == 0) {
        return NEQN_ERROR_INVALID;
    }

    dest_len = strlen(dest);
    src_len = strlen(src);

    if (dest_len + src_len >= dest_size) {
        return NEQN_ERROR_OVERFLOW;
    }

    strcat(dest, src);
    return NEQN_SUCCESS;
}

/**
 * @brief Calculate hash value for string
 */
unsigned int neqn_hash_string(const char *str) {
    unsigned int hash = 0;

    if (str == nullptr) {
        return 0;
    }

    while (*str != '\0') {
        hash = hash * 31 + static_cast<unsigned char>(*str);
        str++;
    }

    return hash % NEQN_HASH_SIZE;
}

/* ================================================================
 * SYMBOL TABLE FUNCTIONS (Stub implementations)
 * ================================================================ */

/**
 * @brief Add symbol to symbol table
 */
int neqn_symbol_define(neqn_context_t *context,
                       const char *name,
                       const char *value) {
    /* Stub implementation */
    if (context == nullptr || name == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    return NEQN_SUCCESS;
}

/**
 * @brief Look up symbol in symbol table
 */
neqn_symbol_t *neqn_symbol_lookup(neqn_context_t *context, const char *name) {
    /* Stub implementation */
    if (context == nullptr || name == nullptr) {
        return nullptr;
    }

    return nullptr;
}

/**
 * @brief Remove symbol from symbol table
 */
int neqn_symbol_undefine(neqn_context_t *context, const char *name) {
    /* Stub implementation */
    if (context == nullptr || name == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    return NEQN_SUCCESS;
}

/* ================================================================
 * CONFIGURATION FUNCTIONS
 * ================================================================ */

/**
 * @brief Enable or disable strict parsing mode
 */
void neqn_set_strict_mode(neqn_context_t *context, int strict_mode) {
    if (context != nullptr) {
        context->strict_mode = strict_mode;
    }
}

/**
 * @brief Check if strict mode is enabled
 */
int neqn_is_strict_mode(neqn_context_t *context) {
    if (context == nullptr) {
        return 0;
    }

    return context->strict_mode;
}

/* ================================================================
 * END OF FILE - ne_core.c
 * ================================================================ */
