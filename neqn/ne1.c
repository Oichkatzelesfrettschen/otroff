/* C17 - no scaffold needed */
/**
 * @file ne1.c
 * @brief NEQN equation typesetting - Part 1: Box manipulation and basic constructs.
 *
 * This file contains functions for constructing and manipulating various
 * equation elements in the NEQN typesetting system. These functions typically
 * generate troff commands to render fractions, subscripts, superscripts,
 * square roots, piles (stacks of expressions), and other mathematical notations.
 * They operate on abstract "box" representations of equation parts, identified
 * by integer handles, and manage their height (`eht`) and baseline (`ebase`)
 * properties. The global variable `yyval` often stores the handle to the
 * resulting constructed element.
 *
 * Mathematical Typesetting Concepts:
 * - Objects have height (eht) and baseline (ebase) properties
 * - Baseline is measured from the bottom of the object
 * - Height is total vertical extent of the object
 * - Objects are referenced by integer handles for memory management
 * - Troff commands are generated to position and render objects
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Robust error handling and bounds checking
 * - Clear separation of concerns between layout and rendering
 * - Comprehensive documentation for maintainability
 */

#include "ne.h" /* NEQN type definitions and global declarations */
#include <stdio.h> /* Standard I/O for printf (troff command generation) */
#include <stdlib.h> /* Standard library functions */
#include <string.h> /* String manipulation functions */

/* SCCS version identifier */
static const char sccs_id[] = "@(#)ne1.c 1.3 25/05/29";

/* External variables - these should be defined in ne.h or other modules */
extern int yyval; /* Result of parsing rule, often an object handle */
extern int eht[]; /* Effective height of objects */
extern int ebase[]; /* Effective baseline of objects */
extern int lp[]; /* List of piled objects */
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
void boverb(int p1, int p2);
void bshiftb(int p1, int dir, int p2);
void eqnbox(int p1, int p2);
void size(int p1, int p2);
int numb(char *p1);
void font(int p1, int p2);
void shift(int p1);
void sqrt_neqn(int p2);
void lpile(int type, int p1, int p2);
void shift2(int p1, int p2, int p3);

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
 * @brief Build a stacked fraction (numerator over denominator).
 *
 * Generates troff commands to typeset expression p1 over expression p2,
 * separated by a horizontal fraction line. The function calculates proper
 * vertical positioning, centering, and baseline alignment for mathematical
 * fractions. Updates `yyval` to the handle of the new fraction object.
 *
 * Mathematical Layout:
 * - Numerator is centered above the fraction line
 * - Denominator is centered below the fraction line  
 * - Both are aligned to the maximum width of the two
 * - Baseline is positioned relative to the denominator
 *
 * Troff Commands Generated:
 * - .ds defines the fraction as a string macro
 * - \v commands handle vertical positioning
 * - \h commands handle horizontal centering
 * - \l command draws the fraction line
 *
 * @param p1 Handle to the numerator object
 * @param p2 Handle to the denominator object
 */
void boverb(int p1, int p2) {
    int h; /* Total height of the fraction */
    int b; /* Baseline of the fraction */
    int treg; /* Temporary troff register for width calculation */

    /* Validate input parameters */
    if (!validate_object_handle(p1) || !validate_object_handle(p2)) {
        if (dbg)
            printf(".\tERROR: boverb: invalid handles p1=%d, p2=%d\n", p1, p2);
        return;
    }

    treg = oalloc(); /* Allocate a temporary register for width calculations */
    if (!validate_object_handle(treg)) {
        if (dbg)
            printf(".\tERROR: boverb: failed to allocate temporary register\n");
        return;
    }

    yyval = p1; /* Use p1's handle for the new combined object initially */

    /* Calculate dimensions for the fraction box */
    h = eht[p1] + eht[p2] + VERT(2); /* Add extra space for fraction bar */
    b = eht[p2] - VERT(1); /* Position baseline relative to denominator */

    if (dbg) {
        printf(".\tDEBUG: boverb: S%d <- S%d over S%d; ebase=%d, eht=%d\n",
               yyval, p1, p2, b, h);
    }

    /* Ensure width registers are properly initialized for both components */
    nrwid(p1, ps, p1);
    nrwid(p2, ps, p2);

    /* Calculate maximum width for proper centering */
    printf(".nr %d \\n(%d\n", treg, p1);
    printf(".if \\n(%d>\\n(%d .nr %d \\n(%d\n", p2, treg, treg, p2);

    /* Generate troff commands for fraction layout */
    printf(".ds %d \\v'%du'\\h'(\\n(%du-\\n(%du)/2u'\\*(%d\\\n",
           yyval, eht[p2] - ebase[p2] - VERT(1), treg, p2, p2);

    printf("\\h'(-(\\n(%du-\\n(%du))/2u'\\v'%du'\\*(%d\\\n",
           treg, p1, -eht[p2] + ebase[p2] - ebase[p1], p1);

    printf("\\h'(-(\\n(%du-\\n(%du))/2u'\\v'%du'\\l'\\n(%du'\\v'%du'\n",
           treg, p1, ebase[p1], treg, VERT(1));

    /* Update object properties for the completed fraction */
    ebase[yyval] = b;
    eht[yyval] = h;

    /* Clean up temporary resources */
    ofree(p2);
    ofree(treg);

    generate_debug_output("boverb", yyval);
}

/**
 * @brief Attach a subscript or superscript to an expression.
 *
 * Generates troff commands to attach p2 as either a subscript (dir > 0) or
 * superscript (dir <= 0) to base expression p1. Handles proper vertical
 * positioning, baseline calculations, and size adjustments according to
 * mathematical typesetting conventions.
 *
 * @param p1 Handle to the base expression object
 * @param dir Direction indicator: > 0 for subscript, <= 0 for superscript  
 * @param p2 Handle to the subscript/superscript object
 */
void bshiftb(int p1, int dir, int p2) {
    int shval; /* Vertical shift amount for positioning p2 */
    int h1, b1, h2, b2; /* Heights and baselines of both objects */
    int d1; /* Standard vertical displacement unit */

    /* Validate input parameters */
    if (!validate_object_handle(p1) || !validate_object_handle(p2)) {
        if (dbg)
            printf(".\tERROR: bshiftb: invalid handles p1=%d, p2=%d\n", p1, p2);
        return;
    }

    yyval = p1; /* Result object uses p1's handle */

    /* Extract dimensional properties for calculations */
    h1 = eht[p1];
    b1 = ebase[p1];
    h2 = eht[p2];
    b2 = ebase[p2];

    if (dir > 0) {
        /* Subscript positioning logic */
        d1 = VERT(1);
        shval = -d1 + h2 - b2;

        if (d1 + b1 > h2) {
            shval = b1 - b2;
        }

        ebase[yyval] = b1 + max(0, h2 - b1 - d1);
        eht[yyval] = h1 + max(0, h2 - b1 - d1);

    } else {
        /* Superscript positioning logic */
        d1 = VERT(1);
        ebase[yyval] = b1;
        shval = -VERT(1) - b2;

        if (VERT(1) + h2 < h1 - b1) {
            shval = -(h1 - b1) + (h2 - b2) - d1;
        }

        eht[yyval] = h1 + max(0, h2 - VERT(1));
    }

    if (dbg) {
        printf(".\tDEBUG: bshiftb: S%d %s S%d; shift=%d, ebase=%d, eht=%d\n",
               p1, (dir > 0) ? "sub" : "sup", p2, shval, ebase[yyval], eht[yyval]);
    }

    /* Generate troff commands for script attachment */
    printf(".as %d \\v'%du'\\*(%d\\v'%du'\n",
           yyval, shval, p2, -shval);

    ofree(p2);
    generate_debug_output("bshiftb", yyval);
}

/**
 * @brief Concatenate two expression boxes horizontally.
 *
 * Combines two mathematical expressions side-by-side, calculating proper
 * baseline alignment and total dimensions. This is fundamental for building
 * complex mathematical expressions from simpler components.
 *
 * @param p1 Handle to the first (left) object
 * @param p2 Handle to the second (right) object  
 */
void eqnbox(int p1, int p2) {
    int b; /* New baseline for combined object */
    int h; /* New height for combined object */

    /* Validate input parameters */
    if (!validate_object_handle(p1) || !validate_object_handle(p2)) {
        if (dbg)
            printf(".\tERROR: eqnbox: invalid handles p1=%d, p2=%d\n", p1, p2);
        return;
    }

    yyval = p1; /* Result object uses p1's handle */

    /* Calculate dimensions for horizontal combination */
    b = max(ebase[p1], ebase[p2]);
    h = b + max(eht[p1] - ebase[p1], eht[p2] - ebase[p2]);

    /* Update result object properties */
    eht[yyval] = h;
    ebase[yyval] = b;

    if (dbg) {
        printf(".\tDEBUG: eqnbox: S%d <- S%d S%d; ebase=%d, eht=%d\n",
               yyval, p1, p2, b, h);
    }

    /* Generate troff command to append second object to first */
    printf(".as %d \"\\*(%d\n", yyval, p2);

    ofree(p2);
    generate_debug_output("eqnbox", yyval);
}

/**
 * @brief Set point size for mathematical expressions (parser action).
 *
 * This function serves as a semantic action in the NEQN parser for size
 * change commands. It updates the result value but does not directly
 * modify global state - that's handled by the parser using this result.
 *
 * @param p1 Previous context object (unused in this implementation)
 * @param p2 New size value
 */
void size(int p1, int p2) {
    /* Size validation - ensure reasonable point size values */
    if (p2 < 4 || p2 > 72) {
        if (dbg)
            printf(".\tWARNING: size: unusual size value %d\n", p2);
    }

    yyval = p2; /* Parser result is the size value itself */

    if (dbg) {
        printf(".\tDEBUG: size: context=S%d, new_size=%d, result=S%d\n",
               p1, p2, yyval);
    }
}

/**
 * @brief Parse a numeric string to integer value.
 *
 * Converts a string representation of a number to its integer value,
 * ignoring any non-digit characters. This provides flexible parsing
 * for mathematical expressions that may contain mixed content.
 *
 * @param p1 Pointer to null-terminated string containing number
 * @return Parsed integer value, or 0 if no digits found
 */
int numb(char *p1) {
    int i; /* String index for character iteration */
    int n; /* Accumulated numeric value */
    int c; /* Current character being processed */

    /* Input validation */
    if (p1 == NULL) {
        if (dbg)
            printf(".\tERROR: numb: null string pointer\n");
        return 0;
    }

    /* Parse digits from string, accumulating value */
    n = 0;
    for (i = 0; (c = p1[i]) != '\0'; i++) {
        if (c >= '0' && c <= '9') {
            n = n * 10 + (c - '0');
        }
        /* Non-digit characters are silently ignored */
    }

    if (dbg) {
        printf(".\tDEBUG: numb: input='%s', result=%d\n", p1, n);
    }

    return n;
}

/**
 * @brief Set font for mathematical expressions (parser action).
 *
 * Similar to size(), this serves as a semantic action for font change
 * commands in the NEQN parser. The actual font change is applied by
 * parser rules that use this result value.
 *
 * @param p1 Previous context object (unused in this implementation)  
 * @param p2 New font identifier/value
 */
void font(int p1, int p2) {
    yyval = p2; /* Parser result is the font identifier */

    if (dbg) {
        printf(".\tDEBUG: font: context=S%d, new_font=S%d, result=S%d\n",
               p1, p2, yyval);
    }
}

/**
 * @brief Apply horizontal shift to an expression (parser action).
 *
 * Passes through an expression object, potentially with shift properties
 * encoded in its troff commands. The actual shifting is handled by the
 * troff formatter based on commands generated during object construction.
 *
 * @param p1 Handle to the object to be shifted
 */
void shift(int p1) {
    /* Validate input parameter */
    if (!validate_object_handle(p1)) {
        if (dbg)
            printf(".\tERROR: shift: invalid handle p1=%d\n", p1);
        return;
    }

    yyval = p1; /* Object passes through unchanged */

    if (dbg) {
        printf(".\tDEBUG: shift: S%d (ps=%d)\n", yyval, ps);
    }
}

/**
 * @brief Render a square root over an expression.
 *
 * Generates troff commands to draw a square root symbol (radical) over
 * the specified expression. The radical consists of a vertical line,
 * horizontal overbar, and proper positioning commands.
 *
 * @param p2 Handle to the expression under the square root
 */
void sqrt_neqn(int p2) {
    /* Validate input parameter */
    if (!validate_object_handle(p2)) {
        if (dbg)
            printf(".\tERROR: sqrt_neqn: invalid handle p2=%d\n", p2);
        return;
    }

    yyval = p2; /* Result uses same handle as input expression */

    /* Ensure width register is available for positioning calculations */
    nrwid(p2, ps, p2);

    /* Generate troff commands for square root construction */
    printf(".ds %d \\v'%du'\\e\\L'%du'\\l'\\n(%du'",
           p2, ebase[p2], -eht[p2], p2);
    printf("\\v'%du'\\h'-\\n(%du'\\*(%d\n",
           eht[p2] - ebase[p2], p2, p2);

    /* Adjust height to include radical symbol overhead */
    eht[p2] = eht[p2] + VERT(1);

    if (dbg) {
        printf(".\tDEBUG: sqrt_neqn: S%d; new_eht=%d, ebase=%d\n",
               p2, eht[p2], ebase[p2]);
    }

    generate_debug_output("sqrt_neqn", yyval);
}

/**
 * @brief Build a vertical pile (stack) of mathematical expressions.
 *
 * Creates a vertical arrangement of multiple expressions with specified
 * alignment and spacing. This is used for constructs like matrices,
 * cases, and multi-line expressions in mathematical typesetting.
 *
 * @param type Character indicating pile alignment type ('L', 'R', 'C', '-')
 * @param p1 Start index in global lp[] array
 * @param p2 End index (exclusive) in global lp[] array
 */
void lpile(int type, int p1, int p2) {
    int i; /* Loop counter */
    int gap; /* Vertical gap between elements */
    int h_sum; /* Sum of individual element heights */
    int b_accum; /* Accumulated height for baseline calculation */
    int nlist; /* Number of elements in the pile */
    int nlist2; /* Midpoint calculation helper */
    int mid_idx; /* Index of middle element */
    int current_hi; /* Current element height */
    int current_bi; /* Current element baseline */
    int max_width_reg; /* Register for maximum element width */

    /* Validate pile range parameters */
    if (p1 < 0 || p2 < p1 || p2 - p1 > 100) {
        if (dbg)
            printf(".\tERROR: lpile: invalid range p1=%d, p2=%d\n", p1, p2);
        return;
    }

    yyval = oalloc(); /* Allocate new handle for pile object */
    if (!validate_object_handle(yyval)) {
        if (dbg)
            printf(".\tERROR: lpile: failed to allocate pile handle\n");
        return;
    }

    max_width_reg = yyval;

    /* Determine vertical spacing based on pile type */
    gap = (type == '-') ? 0 : VERT(1);

    nlist = p2 - p1;
    if (nlist <= 0) {
        if (dbg)
            printf(".\tDEBUG: lpile: empty pile range %d to %d\n", p1, p2);
        eht[yyval] = 0;
        ebase[yyval] = 0;
        printf(".ds %d \"\"\n", yyval);
        return;
    }

    /* Calculate middle element index for baseline reference */
    nlist2 = (nlist + 1) / 2;
    mid_idx = p1 + nlist2 - 1;

    /* Calculate total pile height including gaps */
    h_sum = 0;
    for (i = p1; i < p2; i++) {
        if (!validate_object_handle(lp[i])) {
            if (dbg)
                printf(".\tERROR: lpile: invalid element handle lp[%d]=%d\n", i, lp[i]);
            continue;
        }
        h_sum += eht[lp[i]];
    }
    eht[yyval] = h_sum + (nlist - 1) * gap;

    /* Calculate pile baseline position */
    b_accum = 0;

    if (nlist % 2 == 1) {
        /* Odd count: sum heights below middle element */
        for (i = p1; i < mid_idx; i++) {
            b_accum += (eht[lp[i]] + gap);
        }
        ebase[yyval] = b_accum + ebase[lp[mid_idx]];
    } else {
        /* Even count: position between middle elements */
        for (i = p1; i < mid_idx; i++) {
            b_accum += (eht[lp[i]] + gap);
        }
        ebase[yyval] = b_accum + eht[lp[mid_idx]] + gap / 2;
    }

    if (dbg) {
        printf(".\tDEBUG: lpile: S%d <- %c pile of %d elements:", yyval, type, nlist);
        for (i = p1; i < p2; i++) {
            printf(" S%d", lp[i]);
        }
        printf("; eht=%d, ebase=%d\n", eht[yyval], ebase[yyval]);
    }

    /* Calculate maximum width for alignment purposes */
    if (nlist > 0 && validate_object_handle(lp[p1])) {
        nrwid(lp[p1], ps, lp[p1]);
        printf(".nr %d \\n(%d\n", max_width_reg, lp[p1]);

        for (i = p1 + 1; i < p2; i++) {
            if (validate_object_handle(lp[i])) {
                nrwid(lp[i], ps, lp[i]);
                printf(".if \\n(%d>\\n(%d .nr %d \\n(%d\n",
                       lp[i], max_width_reg, max_width_reg, lp[i]);
            }
        }
    }

    /* Generate pile definition starting at pile baseline */
    printf(".ds %d \\v'%du'", yyval, ebase[yyval]);
    printf("\\\n");

    /* Generate positioning and drawing commands for each element */
    for (i = p2 - 1; i >= p1; i--) {
        if (!validate_object_handle(lp[i]))
            continue;

        current_hi = eht[lp[i]];
        current_bi = ebase[lp[i]];

        /* Move to element's baseline position */
        printf("\\v'-%du'", current_bi);

        /* Apply horizontal alignment based on pile type */
        switch (type) {
        case 'L':
            printf("\\*(%d", lp[i]);
            break;

        case 'R':
            printf("\\h'(\\n(%du-\\n(%du)'\\*(%d\\h'(-\\n(%du+\\n(%du))'",
                   max_width_reg, lp[i], lp[i], max_width_reg, lp[i]);
            break;

        case 'C':
        case '-':
            printf("\\h'((\\n(%du-\\n(%du))/2u)'\\*(%d\\h'(-(\\n(%du-\\n(%du))/2u)'",
                   max_width_reg, lp[i], lp[i], max_width_reg, lp[i]);
            break;

        default:
            if (dbg)
                printf(".\tWARNING: lpile: unknown type '%c'\n", type);
            printf("\\*(%d", lp[i]);
            break;
        }

        /* Position for next element above (if not at top) */
        if (i > p1) {
            printf("\\v'-%du'", (current_hi - current_bi + gap));
        }

        printf("\\");
        if (i > p1)
            printf("\n");
    }

    /* Final positioning adjustment after pile completion */
    printf("\\v'%du'\n", (eht[yyval] - ebase[yyval]) + gap);

    /* Clean up individual element handles */
    for (i = p1; i < p2; i++) {
        if (validate_object_handle(lp[i])) {
            ofree(lp[i]);
        }
    }

    generate_debug_output("lpile", yyval);
}

/**
 * @brief Handle combined subscript and superscript on an expression.
 *
 * Attaches both a subscript and superscript to a base expression simultaneously.
 * This is more complex than individual scripts because proper kerning and
 * positioning must account for both scripts' dimensions and interactions.
 *
 * @param p1 Handle to the base expression object
 * @param p2 Handle to the subscript object
 * @param p3 Handle to the superscript object
 */
void shift2(int p1, int p2, int p3) {
    int h1, b1, h2, b2, h3, b3; /* Dimensions of all three objects */
    int subsh; /* Vertical shift for subscript */
    int supsh; /* Vertical shift for superscript */
    int d1, d2; /* Displacement units for positioning */
    int treg; /* Temporary register for width calculations */

    /* Validate all input parameters */
    if (!validate_object_handle(p1) || !validate_object_handle(p2) ||
        !validate_object_handle(p3)) {
        if (dbg)
            printf(".\tERROR: shift2: invalid handles p1=%d, p2=%d, p3=%d\n",
                   p1, p2, p3);
        return;
    }

    treg = oalloc();
    if (!validate_object_handle(treg)) {
        if (dbg)
            printf(".\tERROR: shift2: failed to allocate temporary register\n");
        return;
    }

    yyval = p1; /* Result object uses base expression handle */

    if (dbg) {
        printf(".\tDEBUG: shift2: S%d <- base S%d, sub S%d, super S%d\n",
               yyval, p1, p2, p3);
    }

    /* Extract dimensional properties for all objects */
    h1 = eht[p1];
    b1 = ebase[p1];
    h2 = eht[p2];
    b2 = ebase[p2];
    h3 = eht[p3];
    b3 = ebase[p3];

    /* Calculate subscript positioning */
    d1 = VERT(1);
    subsh = -d1 + h2 - b2;
    if (d1 + b1 > h2) {
        subsh = b1 - b2;
    }

    /* Calculate superscript positioning */
    d2 = VERT(1);
    supsh = -d2 - b3;
    if (d2 + h3 < h1 - b1) {
        supsh = -(h1 - b1) + (h3 - b3) - d2;
    }

    /* Calculate combined object dimensions */
    eht[yyval] = h1 + max(0, h3 - d2) + max(0, h2 - b1 - d1);
    ebase[yyval] = b1 + max(0, h2 - b1 - d1);

    /* Determine maximum script width for alignment */
    nrwid(p2, ps, p2);
    nrwid(p3, ps, p3);
    printf(".nr %d \\n(%d\n", treg, p3);
    printf(".if \\n(%d>\\n(%d .nr %d \\n(%d\n", p2, treg, treg, p2);

    /* Generate troff commands for script positioning */
    printf(".as %d \\v'%du'\\*(%d\\h'-\\n(%du'\\v'%du'\\\n",
           p1, subsh, p2, p2, -subsh + supsh);
    printf("\\*(%d\\h'-\\n(%du+\\n(%du'\\v'%du'\n",
           p3, p3, treg, -supsh);

    if (dbg) {
        printf(".\tDEBUG: shift2: S%d final eht=%d, ebase=%d\n",
               yyval, eht[yyval], ebase[yyval]);
    }

    /* Clean up component objects and temporary register */
    ofree(p2);
    ofree(p3);
    ofree(treg);

    generate_debug_output("shift2", yyval);
}
