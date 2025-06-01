#include "../cxx17_scaffold.hpp"
/**
 * @file ne2.c
 * @brief NEQN equation typesetting - Part 2: Advanced constructs and special formatting.
 *
 * This file contains functions for constructing advanced mathematical elements
 * in the NEQN typesetting system, including limit expressions (from/to),
 * parentheses and brackets of variable size, diacritical marks (vectors, dots,
 * bars, tildes), movement commands for fine positioning, and special mathematical
 * symbols. These functions complement the basic box manipulation in ne1.c to
 * provide comprehensive mathematical typesetting capabilities.
 *
 * Mathematical Constructs Handled:
 * - Limit expressions with optional superscripts and subscripts
 * - Variable-height parentheses, brackets, braces, floors, ceilings
 * - Diacritical marks: vectors, dots, bars, tildes, circumflexes
 * - Positional movement in four directions with precise measurements
 * - Special mathematical symbols (summation, union, intersection, etc.)
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Robust error handling and input validation
 * - Comprehensive documentation for maintainability
 * - Precise mathematical positioning using troff units
 */

#include "ne.hpp" /* NEQN type definitions and global declarations */
#include <cstdio> /* Standard I/O for printf (troff command generation) */
#include <cstdlib> /* Standard library functions */
#include <cstring> /* String manipulation functions */

/* SCCS version identifier */
static const char sccs_id[] = "@(#)ne2.c 1.3 25/05/29";

/* External variables - these should be defined in ne.h or other modules */
extern int yyval; /* Result of parsing rule, often an object handle */
extern int eht[]; /* Effective height of objects */
extern int ebase[]; /* Effective baseline of objects */
extern int ps; /* Current point size */
extern int dbg; /* Debug flag for diagnostic output */

/* External function declarations - these should be in ne.h */
extern int VERT(int n); /* Convert to vertical units */
extern int oalloc(void); /* Allocate object handle */
extern void ofree(int handle); /* Free object handle */
extern void nrwid(int obj, int ps, int new_obj); /* Set width register */
extern int max(int a, int b); /* Maximum of two integers */

/* Local function prototypes for C90 compliance */
static int validate_object_handle(int handle);
static void generate_debug_output(const char *function_name, int result_handle);

/* Function prototypes for external interface */
void fromto(int p1, int p2, int p3);
void paren(int leftc, int p1, int rightc);
void brack(int m, char *t, char *c, char *b);
void diacrit(int p1, int type);
void move(int dir, int amt, int p);
void funny(int n);

/**
 * @brief Validate that an object handle is within acceptable bounds.
 *
 * Performs basic validation to ensure object handles are reasonable.
 * This helps prevent array bounds violations and other memory errors.
 *
 * @param handle The object handle to validate
 * @return 1 if handle appears valid, 0 otherwise
 */
static int validate_object_handle(int handle) {
    /* Basic bounds checking - assumes handles are positive and reasonable */
    return (handle > 0 && handle < 1000);
}

/**
 * @brief Generate standardized debug output for function completion.
 *
 * Provides consistent debug messaging across all functions when debug
 * mode is enabled. Helps with tracing execution flow and result values.
 *
 * @param function_name Name of the calling function
 * @param result_handle The resulting object handle
 */
static void generate_debug_output(const char *function_name, int result_handle) {
    if (dbg && function_name && validate_object_handle(result_handle)) {
        printf(".\t%s: completed, result=S%d, eht=%d, ebase=%d\n",
               function_name, result_handle, eht[result_handle], ebase[result_handle]);
    }
}

/**
 * @brief Build a limit expression with optional from/to components.
 *
 * Constructs mathematical expressions like summation, integral, or product
 * symbols with optional subscript (from) and superscript (to) components.
 * The main expression is centered, with the from component below and the
 * to component above, properly aligned and sized.
 *
 * Layout Strategy:
 * - Main expression (p1) forms the center reference
 * - From component (p2) positioned below with proper clearance
 * - To component (p3) positioned above with proper clearance
 * - All components centered horizontally relative to maximum width
 * - Baseline positioned to maintain mathematical alignment
 *
 * @param p1 Handle to the main expression (required)
 * @param p2 Handle to the "from" subscript component (0 if not present)
 * @param p3 Handle to the "to" superscript component (0 if not present)
 */
void fromto(int p1, int p2, int p3) {
    int h, b, h1, b1, pss; /* Height, baseline, and size variables */

    /* Validate required main expression parameter */
    if (!validate_object_handle(p1)) {
        if (dbg)
            printf(".\tERROR: fromto: invalid main handle p1=%d\n", p1);
        return;
    }

    /* Validate optional components if present */
    if (p2 > 0 && !validate_object_handle(p2)) {
        if (dbg)
            printf(".\tERROR: fromto: invalid from handle p2=%d\n", p2);
        return;
    }
    if (p3 > 0 && !validate_object_handle(p3)) {
        if (dbg)
            printf(".\tERROR: fromto: invalid to handle p3=%d\n", p3);
        return;
    }

    yyval = oalloc(); /* Allocate new handle for combined expression */
    if (!validate_object_handle(yyval)) {
        if (dbg)
            printf(".\tERROR: fromto: failed to allocate result handle\n");
        return;
    }

    /* Extract dimensions of main expression for reference */
    h1 = eht[yyval] = eht[p1];
    b1 = ebase[p1];
    b = 0; /* Accumulator for baseline adjustment */
    pss = ps; /* Use current point size for components */

    /* Initialize width calculation with main expression */
    nrwid(p1, ps, p1);
    printf(".nr %d \\n(%d\n", yyval, p1);

    /* Process "from" component if present */
    if (p2 > 0) {
        nrwid(p2, pss, p2);
        printf(".if \\n(%d>\\n(%d .nr %d \\n(%d\n", p2, yyval, yyval, p2);
        eht[yyval] += eht[p2]; /* Extend total height downward */
        b = eht[p2]; /* Track from component height for baseline */
    }

    /* Process "to" component if present */
    if (p3 > 0) {
        nrwid(p3, pss, p3);
        printf(".if \\n(%d>\\n(%d .nr %d \\n(%d\n", p3, yyval, yyval, p3);
        eht[yyval] += eht[p3]; /* Extend total height upward */
    }

    /* Begin troff string definition for combined expression */
    printf(".ds %d ", yyval);

    /* Position and draw "from" component below main expression */
    if (p2 > 0) {
        printf("\\v'%du'\\h'\\n(%du-\\n(%du/2u'\\*(%d",
               eht[p2] - ebase[p2] + b1, yyval, p2, p2);
        printf("\\h'-\\n(%du-\\n(%du/2u'\\v'%du'\\\n",
               yyval, p2, -(eht[p2] - ebase[p2] + b1));
    }

    /* Position and draw main expression at center */
    printf("\\h'\\n(%du-\\n(%du/2u'\\*(%d\\h'\\n(%du-\\n(%du+2u/2u'\\\n",
           yyval, p1, p1, yyval, p1);

    /* Position and draw "to" component above main expression */
    if (p3 > 0) {
        printf("\\v'%du'\\h'-\\n(%du-\\n(%du/2u'\\*(%d\\h'\\n(%du-\\n(%du/2u'\\v'%du'\\\n",
               -(h1 - b1 + ebase[p3]), yyval, p3, p3, yyval, p3, (h1 - b1 + ebase[p3]));
    }

    printf("\n"); /* Complete troff string definition */

    /* Calculate final baseline position */
    ebase[yyval] = b + b1;

    if (dbg) {
        printf(".\tfromto: S%d <- %d f %d t %d; h=%d b=%d\n",
               yyval, p1, p2, p3, eht[yyval], ebase[yyval]);
    }

    /* Clean up component handles */
    ofree(p1);
    if (p2 > 0)
        ofree(p2);
    if (p3 > 0)
        ofree(p3);

    generate_debug_output("fromto", yyval);
}

/**
 * @brief Surround expression with variable-height left and right delimiters.
 *
 * Creates appropriately sized parentheses, brackets, braces, or other
 * delimiters around a mathematical expression. The delimiters scale
 * vertically to match or exceed the height of the enclosed expression.
 *
 * Delimiter Types Supported:
 * - '(' ')': Round parentheses
 * - '[' ']': Square brackets  
 * - '{' '}': Curly braces
 * - 'f': Floor (bottom only)
 * - 'c': Ceiling (top only)
 * - Custom characters: Any single character
 *
 * Sizing Algorithm:
 * - Calculate required height based on expression dimensions
 * - Ensure minimum height of 2 units for readability
 * - For braces, ensure odd height for proper center piece
 * - Generate multi-part delimiters using troff building blocks
 *
 * @param leftc Left delimiter character or type code
 * @param p1 Handle to the expression to be enclosed
 * @param rightc Right delimiter character or type code
 */
void paren(int leftc, int p1, int rightc) {
    int n, m, h1, j, b1, v; /* Sizing and positioning variables */

    /* Validate expression parameter */
    if (!validate_object_handle(p1)) {
        if (dbg)
            printf(".\tERROR: paren: invalid expression handle p1=%d\n", p1);
        return;
    }

    /* Extract expression dimensions */
    h1 = eht[p1];
    b1 = ebase[p1];
    yyval = p1; /* Result uses same handle as input expression */

    /* Calculate required delimiter height in vertical units */
    n = max(b1 + VERT(1), h1 - b1 - VERT(1)) / VERT(1);
    if (n < 2)
        n = 2; /* Ensure minimum height */
    m = n - 2; /* Number of extension pieces needed */

    /* Special handling for braces - require odd height for center piece */
    if (leftc == '{' || rightc == '}') {
        n = (n % 2) ? n : ++n; /* Make odd if even */
        if (n < 3)
            n = 3; /* Minimum height for proper brace */
        m = n - 3; /* Account for top, middle, and bottom pieces */
    }

    /* Update expression dimensions to accommodate delimiters */
    eht[yyval] = VERT(2 * n);
    ebase[yyval] = (n / 2) * VERT(2);
    if (n % 2 == 0) {
        ebase[yyval] -= VERT(1); /* Adjust for even heights */
    }

    /* Calculate vertical positioning offset */
    v = b1 - h1 / 2 + VERT(1);

    /* Begin troff string with initial positioning */
    printf(".ds %d \\v'%du'", yyval, v);

    /* Generate left delimiter */
    switch (leftc) {
    case 'n': /* nothing */
    case '\0': /* null - no left delimiter */
        break;

    case 'f': /* floor - vertical line with bottom */
        brack(m, "\\(bv", "\\(bv", "\\(lf");
        break;

    case 'c': /* ceiling - vertical line with top */
        brack(m, "\\(lc", "\\(bv", "\\(bv");
        break;

    case '{': /* left brace - multi-part construction */
        printf("\\b'\\(lt"); /* top piece */
        for (j = 0; j < m; j += 2)
            printf("\\(bv"); /* extension pieces */
        printf("\\(lk"); /* middle piece */
        for (j = 0; j < m; j += 2)
            printf("\\(bv"); /* more extensions */
        printf("\\(lb'"); /* bottom piece */
        break;

    case '(': /* left parenthesis */
        brack(m, "\\(lt", "\\(bv", "\\(lb");
        break;

    case '[': /* left square bracket */
        brack(m, "\\(lc", "\\(bv", "\\(lf");
        break;

    default: /* custom character - repeat vertically */
        brack(m, reinterpret_cast<char *>(&leftc), reinterpret_cast<char *>(&leftc), reinterpret_cast<char *>(&leftc));
        break;
    }

    /* Insert the original expression */
    printf("\\v'%du'\\*(%d", -v, p1);

    /* Generate right delimiter if specified */
    if (rightc) {
        printf("\\v'%du'", v); /* Return to delimiter baseline */

        switch (rightc) {
        case 'f': /* floor - vertical line with bottom */
            brack(m, "\\(bv", "\\(bv", "\\(rf");
            break;

        case 'c': /* ceiling - vertical line with top */
            brack(m, "\\(rc", "\\(bv", "\\(bv");
            break;

        case '}': /* right brace - multi-part construction */
            printf("\\b'\\(rt"); /* top piece */
            for (j = 0; j < m; j += 2)
                printf("\\(bv"); /* extension pieces */
            printf("\\(rk"); /* middle piece */
            for (j = 0; j < m; j += 2)
                printf("\\(bv"); /* more extensions */
            printf("\\(rb'"); /* bottom piece */
            break;

        case ']': /* right square bracket */
            brack(m, "\\(rc", "\\(bv", "\\(rf");
            break;

        case ')': /* right parenthesis */
            brack(m, "\\(rt", "\\(bv", "\\(rb");
            break;

        default: /* custom character - repeat vertically */
            brack(m, reinterpret_cast<char *>(&rightc), reinterpret_cast<char *>(&rightc), reinterpret_cast<char *>(&rightc));
            break;
        }

        printf("\\v'%du'", -v); /* Return to expression baseline */
    }

    printf("\n"); /* Complete troff string definition */

    if (dbg) {
        printf(".\tparen: h=%d b=%d n=%d v=%d l=%c, r=%c\n",
               eht[yyval], ebase[yyval], n, v, leftc, rightc);
    }

    generate_debug_output("paren", yyval);
}

/**
 * @brief Output a bracket sequence with top, middle, and bottom components.
 *
 * Generates troff commands to build a vertically extensible bracket or
 * delimiter using specified top, middle (repeated), and bottom pieces.
 * This is used by paren() to construct complex delimiters.
 *
 * Troff Construction:
 * - Uses \b (bracket building) command
 * - Combines top piece, repeated middle pieces, and bottom piece
 * - Automatically handles vertical alignment and spacing
 *
 * @param m Number of middle extension pieces to insert
 * @param t Pointer to top piece string (troff character sequence)
 * @param c Pointer to middle piece string (repeated m times)
 * @param b Pointer to bottom piece string
 */
void brack(int m, char *t, char *c, char *b) {
    int j; /* Loop counter */

    /* Input validation */
    if (!t || !c || !b) {
        if (dbg)
            printf(".\tERROR: brack: null pointer in parameters\n");
        return;
    }

    if (m < 0) {
        if (dbg)
            printf(".\tWARNING: brack: negative extension count %d\n", m);
        m = 0;
    }

    /* Generate troff bracket building command */
    printf("\\b'%s", t); /* Top piece */

    for (j = 0; j < m; j++) { /* Middle extension pieces */
        printf("%s", c);
    }

    printf("%s'", b); /* Bottom piece */

    if (dbg) {
        printf(".\tbrack: generated %d extensions between '%s' and '%s'\n", m, t, b);
    }
}

/**
 * @brief Apply a diacritical mark above an expression.
 *
 * Adds various mathematical accent marks above expressions, including
 * vectors, dots, bars, tildes, and circumflexes. These marks are
 * positioned and sized appropriately relative to the base expression.
 *
 * Diacritical Mark Types:
 * - 'V': Vector arrow (single)
 * - 'Y': Dyad arrow (double) 
 * - 'H': Hat/circumflex (^)
 * - 'T': Tilde (~)
 * - 'D': Single dot
 * - 'U': Double dot (umlaut)
 * - 'B': Bar (overline)
 * - 'N': Line (underline)
 *
 * Positioning Strategy:
 * - Mark is centered horizontally over the expression
 * - Vertical position adjusted for optimal appearance
 * - Expression height increased to accommodate mark
 *
 * @param p1 Handle to the base expression
 * @param type Character code specifying the diacritical mark type
 */
void diacrit(int p1, int type) {
    int c, t; /* Handles for mark and temporary objects */

    /* Validate expression parameter */
    if (!validate_object_handle(p1)) {
        if (dbg)
            printf(".\tERROR: diacrit: invalid expression handle p1=%d\n", p1);
        return;
    }

    if (dbg) {
        printf(".\tdiacrit: %c type over S%d\n", type, p1);
    }

    /* Allocate handles for mark construction */
    c = oalloc();
    t = oalloc();
    if (!validate_object_handle(c) || !validate_object_handle(t)) {
        if (dbg)
            printf(".\tERROR: diacrit: failed to allocate handles\n");
        return;
    }

    /* Set up width calculation for base expression */
    nrwid(p1, ps, p1);

    /* Generate appropriate diacritical mark */
    switch (type) {
    case 'V': /* vector - single arrow */
    case 'Y': /* dyad - double arrow */
        printf(".ds %d \\v'-1'_\\v'1'\n", c);
        break;

    case 'H': /* hat/circumflex */
        printf(".ds %d ^\n", c);
        break;

    case 'T': /* tilde */
        printf(".ds %d ~\n", c);
        break;

    case 'D': /* single dot */
        printf(".ds %d \\v'-1'.\\v'1'\n", c);
        break;

    case 'U': /* double dot (umlaut) */
        printf(".ds %d \\v'-1'..\\v'1'\n", c);
        break;

    case 'B': /* bar (overline) */
        printf(".ds %d \\v'-1'\\l'\\n(%du'\\v'1'\n", c, p1);
        break;

    case 'N': /* line (underline) */
        printf(".ds %d \\l'\\n(%du'\n", c, p1);
        break;

    default:
        if (dbg)
            printf(".\tWARNING: diacrit: unknown type '%c'\n", type);
        printf(".ds %d ?\n", c); /* Default mark for unknown types */
        break;
    }

    /* Set up width calculation for the mark */
    nrwid(c, ps, c);

    /* Center the mark horizontally over the expression */
    printf(".as %d \\h'-\\n(%du-\\n(%du/2u'\\*(%d",
           p1, p1, c, c);
    printf("\\h'-\\n(%du+\\n(%du/2u'\n", c, p1);

    /* Increase expression height to accommodate mark */
    eht[p1] += VERT(1);

    /* Clean up temporary handles */
    ofree(c);
    ofree(t);

    if (dbg) {
        printf(".\tdiacrit: completed, new height=%d\n", eht[p1]);
    }
}

/**
 * @brief Move expression in specified direction by given amount.
 *
 * Applies fine positioning adjustments to mathematical expressions,
 * allowing precise placement in four directions: forward, backward,
 * up, or down. Movements are specified in hundredths of an em unit.
 *
 * Direction Codes:
 * - 0: Forward (right)
 * - 1: Up
 * - 2: Backward (left) 
 * - 3: Down
 *
 * Amount Encoding:
 * - Specified in hundredths of an em
 * - amt/100 = whole ems, amt%100 = fractional part
 * - Allows precise positioning like 1.25em (amt=125)
 *
 * Troff Commands Generated:
 * - \h for horizontal movement
 * - \v for vertical movement  
 * - Movements are reversed after content to maintain flow
 *
 * @param dir Direction code (0=fwd, 1=up, 2=back, 3=down)
 * @param amt Movement amount in hundredths of an em
 * @param p Handle to the expression to be moved
 */
void move(int dir, int amt, int p) {
    int a, a1, a2; /* Amount calculations */

    /* Validate expression parameter */
    if (!validate_object_handle(p)) {
        if (dbg)
            printf(".\tERROR: move: invalid expression handle p=%d\n", p);
        return;
    }

    /* Validate direction parameter */
    if (dir < 0 || dir > 3) {
        if (dbg)
            printf(".\tERROR: move: invalid direction %d\n", dir);
        return;
    }

    yyval = p; /* Result uses same handle as input */

    /* Split amount into whole and fractional parts */
    a1 = amt / 100; /* Whole ems */
    a2 = amt % 100; /* Hundredths of ems */

    /* Begin troff string definition */
    printf(".ds %d ", yyval);

    if (dir == 0 || dir == 2) {
        /* Horizontal movement: forward or backward */
        printf("\\h'%s%d.%dm'\\*(%d\n",
               (dir == 2) ? "-" : "", a1, a2, p);
    } else if (dir == 1) {
        /* Vertical movement: up */
        printf("\\v'-%d.%dm'\\*(%d\\v'%d.%dm'\n",
               a1, a2, p, a1, a2);
    } else if (dir == 3) {
        /* Vertical movement: down */
        printf("\\v'%d.%dm'\\*(%d\\v'-%d.%dm'\n",
               a1, a2, p, a1, a2);
    }

    /* Calculate positional impact on dimensions */
    a = (ps * 6 * amt) / 100; /* Convert to internal units */

    /*
     * Note: Dimensional adjustments are commented out in original code.
     * This preserves the original behavior where moves don't affect
     * the calculated height and baseline of expressions.
     * 
     * if (dir == 1 || dir == 3)
     *     eht[yyval] += a;
     * if (dir == 1)
     *     ebase[yyval] -= a;
     * else if (dir == 3)
     *     ebase[yyval] += a;
     */

    if (dbg) {
        printf(".\tmove %d dir %d amt %d; h=%d b=%d\n",
               p, dir, amt, eht[yyval], ebase[yyval]);
    }

    generate_debug_output("move", yyval);
}

/**
 * @brief Generate special mathematical symbols.
 *
 * Creates predefined mathematical symbols that are commonly used in
 * equations but require special troff character sequences. These symbols
 * are given standard dimensions for consistent spacing and alignment.
 *
 * Symbol Types:
 * - 'S': Summation (Σ)
 * - 'U': Union (∪) 
 * - 'A': Intersection (∩)
 * - 'P': Product (Π)
 * - 'I': Integral (∫)
 *
 * Symbol Properties:
 * - Standard height of 2 vertical units
 * - Baseline at bottom for proper alignment
 * - Consistent spacing characteristics
 *
 * @param n Character code specifying the symbol type
 */
void funny(int n) {
    char *f; /* Pointer to troff character sequence */

    yyval = oalloc(); /* Allocate handle for symbol */
    if (!validate_object_handle(yyval)) {
        if (dbg)
            printf(".\tERROR: funny: failed to allocate symbol handle\n");
        return;
    }

    /* Map symbol codes to troff character sequences */
    switch (n) {
    case 'S': /* Summation */
        f = "\\(*S";
        break;

    case 'U': /* Union */
        f = "\\(cu";
        break;

    case 'A': /* Intersection */
        f = "\\(ca";
        break;

    case 'P': /* Product */
        f = "\\(*P";
        break;

    case 'I': /* Integral */
        f = "\\(is";
        break;

    default:
        if (dbg)
            printf(".\tWARNING: funny: unknown symbol code '%c'\n", n);
        f = "?"; /* Default for unknown symbols */
        break;
    }

    /* Generate troff string definition */
    printf(".ds %d %s\n", yyval, f);

    /* Set standard dimensions for mathematical symbols */
    eht[yyval] = VERT(2); /* Standard height */
    ebase[yyval] = 0; /* Baseline at bottom */

    if (dbg) {
        printf(".\tfunny: S%d <- %s; h=%d b=%d\n",
               yyval, f, eht[yyval], ebase[yyval]);
    }

    generate_debug_output("funny", yyval);
}
