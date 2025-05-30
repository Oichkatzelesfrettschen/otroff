#include "cxx23_scaffold.hpp"
/**
 * @file ne3.c
 * @brief NEQN equation typesetting - Part 3: Text processing and symbol translation.
 *
 * This file contains the text processing engine for the NEQN typesetting system.
 * It handles the translation of textual input into appropriate troff commands,
 * including mathematical symbols, Greek letters, operators, and special characters.
 * The module provides a comprehensive symbol table mapping human-readable names
 * to their corresponding troff character sequences.
 *
 * Key Functionality:
 * - Symbol table lookup for mathematical operators and Greek letters
 * - Character-by-character translation with context awareness
 * - Special handling for multi-character sequences (>=, <=, ->, etc.)
 * - Font switching for mathematical functions (sin, cos, log, etc.)
 * - Proper spacing and formatting for mathematical expressions
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Comprehensive symbol coverage for mathematical typesetting
 * - Efficient lookup mechanisms for performance
 * - Robust error handling and buffer management
 * - Extensible symbol table design
 */

#include "ne.hpp" /* NEQN type definitions and global declarations */
#include <stdio.h> /* Standard I/O for printf (troff command generation) */
#include <stdlib.h> /* Standard library functions */
#include <string.h> /* String manipulation functions */

/* SCCS version identifier */
static const char sccs_id[] = "@(#)ne3.c 1.3 25/05/29";

/**
 * @brief Symbol translation table entry structure.
 *
 * Defines the structure for mapping human-readable symbol names
 * to their corresponding troff character sequences. This enables
 * users to write mathematical expressions using descriptive names
 * rather than remembering obscure troff escape sequences.
 */
struct symbol_entry {
    char *res; /**< Human-readable symbol name (e.g., "alpha", ">=") */
    char *resval; /**< Corresponding troff escape sequence (e.g., "\\(*a", "\\(>=") */
};

/**
 * @brief Comprehensive symbol translation table.
 *
 * This table maps mathematical symbols, Greek letters, operators,
 * and special functions to their troff representations. The table
 * is organized into logical groups for maintainability:
 * 
 * 1. Mathematical operators and relations
 * 2. Special symbols and punctuation
 * 3. Greek alphabet (lowercase and uppercase)
 * 4. Mathematical functions and constants
 *
 * Symbol Categories:
 * - Relational operators: >=, <=, ==, !=, etc.
 * - Mathematical symbols: infinity, partial, prime, etc.
 * - Greek letters: alpha through chi, both cases where applicable
 * - Functions: sin, cos, tan, log, exp, etc.
 * - Special notation: ellipsis, arrows, dots, etc.
 */
static struct symbol_entry restab[] = {
    /* Mathematical operators and relations */
    {">=", "\\(>="}, /* Greater than or equal */
    {"<=", "\\(<="}, /* Less than or equal */
    {"==", "\\(=="}, /* Identically equal */
    {"!=", "\\(!="}, /* Not equal */
    {"+-", "\\(+-"}, /* Plus-minus */
    {"->", "\\(->"}, /* Right arrow */
    {"<-", "\\(<-"}, /* Left arrow */

    /* Special mathematical symbols */
    {"inf", "\\(if"}, /* Infinity symbol (short form) */
    {"infinity", "\\(if"}, /* Infinity symbol (long form) */
    {"partial", "\\(pd"}, /* Partial derivative symbol */
    {"half", "\\fR\\(12\\fP"}, /* One-half fraction */
    {"prime", "\\(fm"}, /* Prime symbol for derivatives */
    {"approx", "~\\b\\d~\\u"}, /* Approximately equal */
    {"nothing", ""}, /* Empty/null symbol */
    {"cdot", "\\v'-.5'.\\v'.5'"}, /* Centered dot */
    {"times", "\\|\\(mu\\|"}, /* Multiplication cross */
    {"del", "\\(gr"}, /* Del/nabla operator (short) */
    {"grad", "\\(gr"}, /* Gradient operator (long) */

    /* Ellipsis and continuation symbols */
    {"...", "\\v'-.3m'\\|\\|.\\|\\|.\\|\\|.\\|\\|\\v'.3m'"}, /* Horizontal ellipsis */
    {",...,", ",\\|\\|.\\|\\|.\\|\\|.\\|\\|,\\|"}, /* Comma ellipsis comma */

    /* Greek alphabet - lowercase */
    {"alpha", "\\(*a"}, /* α - alpha */
    {"beta", "\\(*b"}, /* β - beta */
    {"gamma", "\\(*g"}, /* γ - gamma */
    {"delta", "\\(*d"}, /* δ - delta */
    {"epsilon", "\\(*e"}, /* ε - epsilon */
    {"zeta", "\\(*z"}, /* ζ - zeta */
    {"eta", "\\(*y"}, /* η - eta */
    {"theta", "\\(*h"}, /* θ - theta */
    {"iota", "\\(*i"}, /* ι - iota */
    {"kappa", "\\(*k"}, /* κ - kappa */
    {"lambda", "\\(*l"}, /* λ - lambda */
    {"mu", "\\(*m"}, /* μ - mu */
    {"nu", "\\(*n"}, /* ν - nu */
    {"xi", "\\(*c"}, /* ξ - xi */
    {"omicron", "\\(*o"}, /* ο - omicron */
    {"pi", "\\(*p"}, /* π - pi */
    {"rho", "\\(*r"}, /* ρ - rho */
    {"sigma", "\\(*s"}, /* σ - sigma */
    {"tau", "\\(*t"}, /* τ - tau */
    {"upsilon", "\\(*u"}, /* υ - upsilon */
    {"phi", "\\(*f"}, /* φ - phi */
    {"chi", "\\(*x"}, /* χ - chi */
    {"psi", "\\(*q"}, /* ψ - psi */
    {"omega", "\\(*w"}, /* ω - omega */

    /* Greek alphabet - uppercase */
    {"GAMMA", "\\(*G"}, /* Γ - Gamma */
    {"DELTA", "\\(*D"}, /* Δ - Delta */
    {"THETA", "\\(*H"}, /* Θ - Theta */
    {"LAMBDA", "\\(*L"}, /* Λ - Lambda */
    {"XI", "\\(*C"}, /* Ξ - Xi */
    {"PI", "\\(*P"}, /* Π - Pi */
    {"SIGMA", "\\(*S"}, /* Σ - Sigma */
    {"UPSILON", "\\(*U"}, /* Υ - Upsilon */
    {"PHI", "\\(*F"}, /* Φ - Phi */
    {"PSI", "\\(*Q"}, /* Ψ - Psi */
    {"OMEGA", "\\(*W"}, /* Ω - Omega */

    /* Mathematical functions and logical operators */
    {"and", "\\fRand\\fP"}, /* Logical AND in roman font */
    {"for", "\\fRfor\\fP"}, /* "For" in mathematical context */
    {"if", "\\fRif\\fP"}, /* "If" in mathematical context */
    {"Re", "\\fRRe\\fP"}, /* Real part function */
    {"Im", "\\fRIm\\fP"}, /* Imaginary part function */

    /* Trigonometric functions */
    {"sin", "\\fRsin\\fP"}, /* Sine function */
    {"cos", "\\fRcos\\fP"}, /* Cosine function */
    {"tan", "\\fRtan\\fP"}, /* Tangent function */
    {"arc", "\\fRarc\\fP"}, /* Arc prefix for inverse trig */

    /* Hyperbolic functions */
    {"sinh", "\\fRsinh\\fP"}, /* Hyperbolic sine */
    {"cosh", "\\fRcosh\\fP"}, /* Hyperbolic cosine */
    {"tanh", "\\fRtanh\\fP"}, /* Hyperbolic tangent */
    {"coth", "\\fRcoth\\fP"}, /* Hyperbolic cotangent */

    /* Logarithmic and exponential functions */
    {"log", "\\fRlog\\fP"}, /* Logarithm function */
    {"ln", "\\fRln\\fP"}, /* Natural logarithm */
    {"exp", "\\fRexp\\fP"}, /* Exponential function */

    /* Mathematical limits and extrema */
    {"lim", "\\fRlim\\fP"}, /* Limit function */
    {"max", "\\fRmax\\fP"}, /* Maximum function */
    {"min", "\\fRmin\\fP"}, /* Minimum function */
    {"det", "\\fRdet\\fP"}, /* Determinant function */

    /* Sentinel entry to mark end of table */
    {NULL, NULL}};

/* External variables - these should be defined in ne.h or other modules */
extern int yyval; /* Result of parsing rule, often an object handle */
extern int eht[]; /* Effective height of objects */
extern int ebase[]; /* Effective baseline of objects */
extern int ps; /* Current point size */
extern int dbg; /* Debug flag for diagnostic output */

/* External function declarations - these should be in ne.h */
extern int oalloc(void); /* Allocate object handle */
extern int VERT(int n); /* Convert to vertical units */
extern int lookup(char *name, void *table); /* Symbol table lookup */
extern void error(int level, char *fmt, ...); /* Error reporting */

/* Constants for buffer management */
#define CSSIZE 400 /**< Maximum size for converted character buffer */
#define BUFFER_MARGIN 20 /**< Safety margin for buffer operations */

/* Global variables for character conversion state */
static int csp; /**< Current position in character output buffer */
static int psp; /**< Current position in input parsing */
static char cs[CSSIZE + BUFFER_MARGIN]; /**< Character conversion output buffer */

/* Error severity levels */
#define FATAL 1 /**< Fatal error level for error() function */

/* Local function prototypes for C90 compliance */
static void validate_buffer_space(int needed);
static void safe_append_char(int c);
static void safe_append_string(const char *str);

/* Function prototypes for external interface */
void text(int t, char *p1);
void trans(int c, char *p1);
void shim(void);
void roman(int c);
void name4(int c1, int c2);

/**
 * @brief Validate available buffer space and prevent overflow.
 *
 * Checks if there is sufficient space in the character conversion buffer
 * for the requested number of characters. Triggers a fatal error if
 * buffer would overflow, preventing memory corruption.
 *
 * @param needed Number of characters that need to be added to buffer
 */
static void validate_buffer_space(int needed) {
    if (csp + needed >= CSSIZE) {
        error(FATAL, "character conversion buffer overflow: need %d, have %d available",
              needed, CSSIZE - csp);
    }
}

/**
 * @brief Safely append a single character to the conversion buffer.
 *
 * Adds a character to the output buffer with bounds checking to prevent
 * buffer overflow. This is a safer alternative to direct buffer access.
 *
 * @param c Character to append to the buffer
 */
static void safe_append_char(int c) {
    validate_buffer_space(1);
    cs[csp++] = (char)c;
}

/**
 * @brief Safely append a string to the conversion buffer.
 *
 * Adds a null-terminated string to the output buffer with bounds checking.
 * Prevents buffer overflow while copying string content.
 *
 * @param str Null-terminated string to append
 */
static void safe_append_string(const char *str) {
    int len;
    int i;

    if (!str)
        return; /* Handle null pointer gracefully */

    len = strlen(str);
    validate_buffer_space(len);

    for (i = 0; i < len; i++) {
        cs[csp++] = str[i];
    }
}

/**
 * @brief Process a text token and convert it to appropriate troff commands.
 *
 * This is the main text processing function that handles different types
 * of input tokens and converts them to their troff representations. It
 * supports several token types and performs symbol table lookups for
 * mathematical notation.
 *
 * Token Types Handled:
 * - 'q': Quoted text (used literally)
 * - '~': Non-breaking space
 * - '^': Null/empty content
 * - '\t': Tab character
 * - Default: Regular text requiring character-by-character translation
 *
 * Processing Flow:
 * 1. Validate input parameters
 * 2. Allocate object handle for result
 * 3. Set standard dimensions for text objects
 * 4. Determine processing method based on token type
 * 5. Perform symbol lookup or character translation
 * 6. Generate troff string definition
 *
 * @param t Token type character indicating processing method
 * @param p1 Pointer to input text string to be processed
 */
void text(int t, char *p1) {
    int i, c; /* Loop variables and character storage */
    char *j; /* Pointer to result string */
    static char space_str[] = "\\ "; /* Non-breaking space string */
    static char empty_str[] = ""; /* Empty string */
    static char tab_str[] = "\\t"; /* Tab escape sequence */

    /* Input validation */
    if (!p1) {
        if (dbg)
            printf(".\tERROR: text: null input string\n");
        return;
    }

    /* Allocate object handle for the text result */
    yyval = oalloc();
    if (yyval <= 0) {
        if (dbg)
            printf(".\tERROR: text: failed to allocate object handle\n");
        return;
    }

    /* Set standard dimensions for text objects
     * Text typically sits on the baseline with standard height */
    ebase[yyval] = 0; /* Baseline at bottom of object */
    eht[yyval] = VERT(2); /* Standard text height */

    /* Process token based on type */
    switch (t) {
    case 'q':
        /* Quoted text - use literally without translation */
        j = p1;
        break;

    case '~':
        /* Non-breaking space character */
        j = space_str;
        break;

    case '^':
        /* Empty/null content */
        j = empty_str;
        break;

    case '\t':
        /* Tab character - convert to troff tab */
        j = tab_str;
        break;

    default:
        /* Regular text - attempt symbol lookup first */
        i = lookup(p1, (void *)restab);
        if (i >= 0) {
            /* Found in symbol table - use predefined translation */
            j = restab[i].resval;
        } else {
            /* Not in symbol table - perform character-by-character translation */
            csp = 0; /* Reset output buffer position */
            psp = 0; /* Reset input parsing position */

            /* Process each character in the input string */
            while ((c = p1[psp]) != '\0') {
                psp++; /* Advance input position before processing */
                trans(c, p1); /* Translate character with context */

                /* Check for buffer overflow */
                if (csp > CSSIZE) {
                    /* Truncate input string for error message */
                    if (strlen(p1) > 25) {
                        p1[25] = '\0';
                    }
                    error(FATAL, "converted token %s... too long", p1);
                }
            }

            /* Null-terminate the converted string */
            cs[csp] = '\0';
            j = cs;
        }
        break;
    }

    /* Generate debug output if enabled */
    if (dbg) {
        printf(".\t%ctext: S%d <- %s; b=%d,h=%d\n",
               t, yyval, j ? j : "(null)", ebase[yyval], eht[yyval]);
    }

    /* Generate troff string definition command */
    printf(".ds %d \"%s\n", yyval, j ? j : "");
}

/**
 * @brief Translate individual characters into appropriate troff sequences.
 *
 * This function handles the detailed character-by-character translation
 * of input text, with special handling for mathematical operators,
 * punctuation, and troff escape sequences. It maintains context awareness
 * for multi-character sequences and applies appropriate spacing.
 *
 * Character Categories:
 * - Digits and basic punctuation: Handled in roman font
 * - Parentheses and brackets: Roman with spacing
 * - Mathematical operators: Special spacing and handling
 * - Comparison operators: Lookahead for compound operators
 * - Minus signs: Context-sensitive handling
 * - Backslash escapes: Pass-through with proper parsing
 * - Quotes: Convert to troff prime symbols
 * - Default: Pass through unchanged
 *
 * Spacing Strategy:
 * - shim() adds appropriate spacing around operators
 * - Roman characters for readability in math context
 * - Special handling for multi-character operators
 *
 * @param c Character to be translated
 * @param p1 Complete input string (for lookahead operations)
 */
void trans(int c, char *p1) {
    /* Input validation */
    if (!p1) {
        if (dbg)
            printf(".\tERROR: trans: null input string\n");
        return;
    }

    /* Process character based on its type and mathematical context */
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case ':':
    case ';':
    case '!':
        /* Digits and basic punctuation - use roman font for clarity */
        roman(c);
        break;

    case '(':
    case '[':
    case ')':
    case ']':
        /* Parentheses and brackets - add spacing for readability */
        shim(); /* Space before delimiter */
        roman(c); /* Delimiter in roman font */
        break;

    case '+':
    case '|':
        /* Binary operators - need spacing on both sides */
        shim(); /* Space before operator */
        roman(c); /* Operator character */
        shim(); /* Space after operator */
        break;

    case '=':
    case '>':
    case '<':
        /* Comparison operators - check for compound forms */
        if (psp < strlen(p1) && p1[psp] == '=') {
            /* Compound operator: ==, >=, <= */
            name4(c, '='); /* Generate two-character troff name */
            psp++; /* Skip the '=' we just consumed */
        } else {
            /* Single operator */
            shim(); /* Space before */
            safe_append_char(c); /* The operator itself */
            shim(); /* Space after */
        }
        break;

    case '-':
        /* Minus sign or arrow - context sensitive */
        if (psp < strlen(p1) && p1[psp] == '>') {
            /* Arrow operator: -> */
            shim(); /* Space before arrow */
            name4('-', '>'); /* Generate arrow symbol */
            psp++; /* Skip the '>' we just consumed */
        } else {
            /* Regular minus sign */
            shim(); /* Space before */
            name4('m', 'i'); /* Minus symbol (proper mathematical minus) */
            shim(); /* Space after */
        }
        break;

    case '/':
        /* Division slash - use proper mathematical slash */
        name4('s', 'l'); /* Troff slash symbol */
        break;

    case '~':
    case ' ':
        /* Spaces and tildes - convert to spacing */
        shim(); /* Add appropriate mathematical spacing */
        shim(); /* Double spacing for emphasis */
        break;

    case '^':
        /* Caret - used for spacing adjustment */
        shim(); /* Add spacing */
        break;

    case '\\':
        /* Troff escape sequence - pass through with proper parsing */
        safe_append_char(c); /* The backslash itself */

        if (psp < strlen(p1)) {
            c = p1[psp++]; /* Get next character */
            safe_append_char(c); /* Add it to output */

            if (psp < strlen(p1)) {
                safe_append_char(p1[psp++]); /* Third character */
            }

            /* Special handling for troff character names \(xx */
            if (c == '(' && psp < strlen(p1)) {
                safe_append_char(p1[psp++]); /* Fourth character for \(xx */
            }

            /* Special handling for \*( sequences (font changes) */
            if (c == '*' && psp < strlen(p1) && p1[psp - 1] == '(' && psp + 1 < strlen(p1)) {
                safe_append_char(p1[psp++]); /* Fifth character */
                safe_append_char(p1[psp++]); /* Sixth character */
            }
        }
        break;

    case '\'':
        /* Single quote - convert to mathematical prime */
        name4('f', 'm'); /* Prime symbol for derivatives */
        break;

    default:
        /* Regular characters - pass through unchanged */
        safe_append_char(c);
        break;
    }
}

/**
 * @brief Insert appropriate mathematical spacing.
 *
 * This function is responsible for adding proper spacing in mathematical
 * expressions. In the current implementation, it serves as a placeholder
 * for spacing logic that could be enhanced based on context and
 * surrounding characters.
 *
 * Design Note:
 * The original implementation is intentionally minimal, allowing troff's
 * built-in spacing mechanisms to handle most cases. This function could
 * be enhanced to:
 * - Add thin spaces (\|) around operators
 * - Insert medium spaces (\^) for grouping
 * - Apply thick spaces (\&) for major separations
 * - Use negative spacing (\!) for tight positioning
 *
 * Current Behavior:
 * - No explicit spacing is added
 * - Relies on troff's default spacing rules
 * - Maintains compatibility with existing expressions
 */
void shim(void) {
    char prev;

    if (csp > 0)
        prev = cs[csp - 1];
    else
        prev = '\0';

    if (prev == '|')
        safe_append_string("\\^");
    else if (prev == '^')
        safe_append_string("\\&");
    else if (prev == '&')
        safe_append_string("\\!");
    else
        safe_append_string("\\|");
}

/**
 * @brief Append a character in roman font to the output buffer.
 *
 * Adds a single character to the character conversion buffer. This
 * function is used for characters that should appear in roman font
 * within mathematical expressions, such as digits, punctuation, and
 * certain operators.
 *
 * Font Context:
 * In mathematical typesetting, certain characters (particularly digits
 * and some punctuation) are traditionally set in roman (upright) font
 * even within italic mathematical expressions. This function handles
 * such characters.
 *
 * @param c Character to append in roman font
 */
void roman(int c) {
    safe_append_char(c);
}

/**
 * @brief Generate a two-character troff name escape sequence.
 *
 * Creates troff character name sequences of the form \(xy, where x and y
 * are the two character codes. This is used for accessing special
 * mathematical symbols and operators that are not directly available
 * on the keyboard.
 *
 * Troff Character Names:
 * - \(>= : Greater than or equal
 * - \(<= : Less than or equal  
 * - \(-> : Right arrow
 * - \(mi : Minus sign (mathematical)
 * - \(sl : Slash (division)
 * - \(fm : Prime symbol
 *
 * Output Format:
 * The function generates: \(c1c2
 * Where c1 and c2 are the two input characters.
 *
 * @param c1 First character of the two-character name
 * @param c2 Second character of the two-character name
 */
void name4(int c1, int c2) {
    safe_append_char('\\'); /* Escape character */
    safe_append_char('('); /* Name introducer */
    safe_append_char(c1); /* First character */
    safe_append_char(c2); /* Second character */
}
