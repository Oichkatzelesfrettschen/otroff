#include "../cxx17_scaffold.hpp"
/**
 * @file ne_symbols.c
 * @brief Symbol table and mathematical notation definitions for neqn
 *
 * @details
 * This module provides symbol table management and definitions for
 * mathematical notation symbols used by the neqn preprocessor.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ne.hpp"

/* ================================================================
 * BUILT-IN MATHEMATICAL SYMBOLS
 * ================================================================ */

/**
 * @brief Built-in symbol definitions
 */
typedef struct {
    const char *name;
    const char *terminal_output;
    const char *description;
} neqn_builtin_symbol_t;

static const neqn_builtin_symbol_t builtin_symbols[] = {
    /* Greek letters */
    {"alpha", "α", "Greek letter alpha"},
    {"beta", "β", "Greek letter beta"},
    {"gamma", "γ", "Greek letter gamma"},
    {"delta", "δ", "Greek letter delta"},
    {"epsilon", "ε", "Greek letter epsilon"},
    {"pi", "π", "Greek letter pi"},
    {"sigma", "σ", "Greek letter sigma"},
    {"theta", "θ", "Greek letter theta"},
    {"omega", "ω", "Greek letter omega"},

    /* Mathematical operators */
    {"+-", "±", "Plus-minus symbol"},
    {"-+", "∓", "Minus-plus symbol"},
    {"times", "×", "Multiplication symbol"},
    {"div", "÷", "Division symbol"},
    {"approx", "≈", "Approximately equal"},
    {"!=", "≠", "Not equal"},
    {"<=", "≤", "Less than or equal"},
    {">=", "≥", "Greater than or equal"},
    {"<<", "≪", "Much less than"},
    {">>", "≫", "Much greater than"},

    /* Set theory and logic */
    {"subset", "⊂", "Subset symbol"},
    {"supset", "⊃", "Superset symbol"},
    {"in", "∈", "Element of"},
    {"notin", "∉", "Not element of"},
    {"union", "∪", "Set union"},
    {"inter", "∩", "Set intersection"},
    {"and", "∧", "Logical AND"},
    {"or", "∨", "Logical OR"},
    {"not", "¬", "Logical NOT"},

    /* Calculus and analysis */
    {"integral", "∫", "Integral symbol"},
    {"sum", "∑", "Summation symbol"},
    {"prod", "∏", "Product symbol"},
    {"partial", "∂", "Partial derivative"},
    {"nabla", "∇", "Nabla (del) operator"},
    {"infinity", "∞", "Infinity symbol"},
    {"grad", "∇", "Gradient operator"},

    /* Arrows */
    {"->", "→", "Right arrow"},
    {"<-", "←", "Left arrow"},
    {"<->", "↔", "Left-right arrow"},
    {"=>", "⇒", "Right double arrow (implies)"},
    {"<==>", "⇔", "Left-right double arrow (iff)"},

    /* Miscellaneous */
    {"degree", "°", "Degree symbol"},
    {"prime", "′", "Prime symbol"},
    {"dagger", "†", "Dagger symbol"},
    {"section", "§", "Section symbol"},
    {"paragraph", "¶", "Paragraph symbol"},

    {nullptr, nullptr, nullptr} /* Terminator */
};

/* ================================================================
 * SYMBOL TABLE FUNCTIONS
 * ================================================================ */

/**
 * @brief Initialize built-in symbols in context
 */
int neqn_init_builtin_symbols(neqn_context_t *context) {
    int i;

    if (context == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    for (i = 0; builtin_symbols[i].name != nullptr; i++) {
        neqn_symbol_t *symbol = static_cast<neqn_symbol_t *>(malloc(sizeof(neqn_symbol_t)));
        if (symbol == nullptr) {
            return NEQN_ERROR_MEMORY;
        }

        symbol->name = neqn_strdup(builtin_symbols[i].name);
        symbol->value = neqn_strdup(builtin_symbols[i].terminal_output);
        symbol->tree = nullptr;
        symbol->line_defined = 0;
        symbol->is_builtin = 1;

        if (symbol->name == nullptr || symbol->value == nullptr) {
            free(symbol->name);
            free(symbol->value);
            free(symbol);
            return NEQN_ERROR_MEMORY;
        }

        /* Add to hash table */
        {
            unsigned int hash = neqn_hash_string(symbol->name);
            symbol->next = context->symbols[hash];
            context->symbols[hash] = symbol;
        }
    }

    return NEQN_SUCCESS;
}

/**
 * @brief Enhanced symbol lookup with built-in fallback
 */
neqn_symbol_t *neqn_symbol_lookup_enhanced(neqn_context_t *context, const char *name) {
    unsigned int hash;
    neqn_symbol_t *symbol;

    if (context == nullptr || name == nullptr) {
        return nullptr;
    }

    /* Look up in hash table */
    hash = neqn_hash_string(name);
    symbol = context->symbols[hash];

    while (symbol != nullptr) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }

    return nullptr;
}

/**
 * @brief Print all symbols for debugging
 */
void neqn_debug_print_symbols(neqn_context_t *context) {
    int i;
    neqn_symbol_t *symbol;

    if (context == nullptr) {
        return;
    }

    printf("=== Symbol Table ===\n");

    for (i = 0; i < NEQN_HASH_SIZE; i++) {
        symbol = context->symbols[i];
        while (symbol != nullptr) {
            printf("  %s = %s", symbol->name, symbol->value ? symbol->value : "(null)");
            if (symbol->is_builtin) {
                printf(" [built-in]");
            }
            printf("\n");
            symbol = symbol->next;
        }
    }

    printf("==================\n");
}

/**
 * @brief Enhanced symbol definition with conflict checking
 */
int neqn_symbol_define_enhanced(neqn_context_t *context,
                                const char *name,
                                const char *value) {
    neqn_symbol_t *existing;
    neqn_symbol_t *symbol;
    unsigned int hash;

    if (context == nullptr || name == nullptr) {
        return NEQN_ERROR_INVALID;
    }

    /* Check for existing symbol */
    existing = neqn_symbol_lookup_enhanced(context, name);
    if (existing != nullptr) {
        if (existing->is_builtin) {
            neqn_warning(context, "Redefining built-in symbol '%s'", name);
        }

        /* Update existing symbol */
        free(existing->value);
        existing->value = value ? neqn_strdup(value) : nullptr;
        existing->line_defined = context->line_number;
        return NEQN_SUCCESS;
    }

    /* Create new symbol */
    symbol = static_cast<neqn_symbol_t *>(malloc(sizeof(neqn_symbol_t)));
    if (symbol == nullptr) {
        return NEQN_ERROR_MEMORY;
    }

    symbol->name = neqn_strdup(name);
    symbol->value = value ? neqn_strdup(value) : nullptr;
    symbol->tree = nullptr;
    symbol->line_defined = context->line_number;
    symbol->is_builtin = 0;

    if (symbol->name == nullptr || (value != nullptr && symbol->value == nullptr)) {
        free(symbol->name);
        free(symbol->value);
        free(symbol);
        return NEQN_ERROR_MEMORY;
    }

    /* Add to hash table */
    hash = neqn_hash_string(name);
    symbol->next = context->symbols[hash];
    context->symbols[hash] = symbol;

    return NEQN_SUCCESS;
}

/* ================================================================
 * MATHEMATICAL FORMATTING FUNCTIONS
 * ================================================================ */

/**
 * @brief Format superscript for terminal output
 */
neqn_node_t *neqn_superscript(neqn_node_t *base, neqn_node_t *exponent) {
    neqn_node_t *result;
    char *formatted;
    size_t len;

    if (base == nullptr || exponent == nullptr) {
        return nullptr;
    }

    /* Calculate length needed */
    len = strlen(base->content) + strlen(exponent->content) + 4; /* "^{}" */
    formatted = static_cast<char *>(malloc(len));
    if (formatted == nullptr) {
        return nullptr;
    }

    /* Format as base^{exponent} */
    sprintf(formatted, "%s^{%s}", base->content, exponent->content);

    result = neqn_node_create(NEQN_NODE_SUPER, formatted);
    free(formatted);

    if (result != nullptr) {
        result->left = base;
        result->right = exponent;
    }

    return result;
}

/**
 * @brief Format subscript for terminal output
 */
neqn_node_t *neqn_subscript(neqn_node_t *base, neqn_node_t *subscript) {
    neqn_node_t *result;
    char *formatted;
    size_t len;

    if (base == nullptr || subscript == nullptr) {
        return nullptr;
    }

    /* Calculate length needed */
    len = strlen(base->content) + strlen(subscript->content) + 4; /* "_{}" */
    formatted = static_cast<char *>(malloc(len));
    if (formatted == nullptr) {
        return nullptr;
    }

    /* Format as base_{subscript} */
    sprintf(formatted, "%s_{%s}", base->content, subscript->content);

    result = neqn_node_create(NEQN_NODE_SUB, formatted);
    free(formatted);

    if (result != nullptr) {
        result->left = base;
        result->right = subscript;
    }

    return result;
}

/**
 * @brief Format fraction for terminal output
 */
neqn_node_t *neqn_fraction(neqn_node_t *numerator, neqn_node_t *denominator) {
    neqn_node_t *result;
    char *formatted;
    size_t len;

    if (numerator == nullptr || denominator == nullptr) {
        return nullptr;
    }

    /* Calculate length needed */
    len = strlen(numerator->content) + strlen(denominator->content) + 8; /* "() / ()" */
    formatted = static_cast<char *>(malloc(len));
    if (formatted == nullptr) {
        return nullptr;
    }

    /* Format as (numerator) / (denominator) */
    sprintf(formatted, "(%s) / (%s)", numerator->content, denominator->content);

    result = neqn_node_create(NEQN_NODE_FRACTION, formatted);
    free(formatted);

    if (result != nullptr) {
        result->left = numerator;
        result->right = denominator;
    }

    return result;
}

/**
 * @brief Format square root for terminal output
 */
neqn_node_t *neqn_sqrt(neqn_node_t *expression) {
    neqn_node_t *result;
    char *formatted;
    size_t len;

    if (expression == nullptr) {
        return nullptr;
    }

    /* Calculate length needed */
    len = strlen(expression->content) + 10; /* "sqrt{}" */
    formatted = static_cast<char *>(malloc(len));
    if (formatted == nullptr) {
        return nullptr;
    }

    /* Format as sqrt{expression} */
    sprintf(formatted, "√(%s)", expression->content);

    result = neqn_node_create(NEQN_NODE_SQRT, formatted);
    free(formatted);

    if (result != nullptr) {
        result->left = expression;
    }

    return result;
}

/* ================================================================
 * ENHANCED OUTPUT FORMATTING
 * ================================================================ */

/**
 * @brief Format equation for terminal output with symbol substitution
 */
int neqn_format_equation(neqn_context_t *context,
                         neqn_node_t *tree,
                         char *buffer,
                         size_t capacity) {
    neqn_node_t *current;
    size_t pos = 0;

    if (context == nullptr || tree == nullptr || buffer == nullptr || capacity == 0) {
        return -1;
    }

    buffer[0] = '\0';

    current = tree;
    while (current != nullptr && pos < capacity - 1) {
        const char *output_text = current->content;
        neqn_symbol_t *symbol;

        /* Look up symbol for potential substitution */
        if (current->type == NEQN_NODE_IDENTIFIER) {
            symbol = neqn_symbol_lookup_enhanced(context, current->content);
            if (symbol != nullptr && symbol->value != nullptr) {
                output_text = symbol->value;
            }
        }

        /* Add to buffer */
        if (output_text != nullptr) {
            size_t text_len = strlen(output_text);
            if (pos + text_len < capacity - 1) {
                strcpy(buffer + pos, output_text);
                pos += text_len;

                /* Add space between tokens */
                if (current->next != nullptr && pos < capacity - 2) {
                    buffer[pos++] = ' ';
                    buffer[pos] = '\0';
                }
            }
        }

        current = current->next;
    }

    return (int)pos;
}

/* ================================================================
 * END OF FILE - ne_symbols.c
 * ================================================================ */
