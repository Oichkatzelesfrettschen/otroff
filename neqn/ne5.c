/* C17 - no scaffold needed */
/**
 * @file ne5.c
 * @brief NEQN equation typesetting - Part 5: Position marking and alignment.
 *
 * This file contains functions for managing position markers and alignment
 * operations in the NEQN typesetting system. These functions enable precise
 * horizontal positioning and alignment of mathematical expressions, allowing
 * for complex multi-line equations with proper alignment points.
 *
 * Key Functionality:
 * - Position marking using troff's \k escape sequence
 * - Horizontal alignment to previously marked positions
 * - Support for both in-place marking and separate mark objects
 * - Proper dimension tracking for aligned expressions
 *
 * Mathematical Applications:
 * - Aligning equals signs in multi-line equations
 * - Creating tabular mathematical layouts
 * - Positioning continuation lines in long expressions
 * - Coordinating alignment across equation groups
 *
 * Troff Integration:
 * - Uses troff register 97 as the standard position marker
 * - Generates \k position marking commands
 * - Creates \h horizontal motion commands for alignment
 * - Maintains proper width calculations for spacing
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Minimal troff register usage (only register 97)
 * - Robust error handling and input validation
 * - Comprehensive documentation for maintainability
 * - Efficient position calculation algorithms
 */

#include "ne.h" /* NEQN type definitions and global declarations */
#include <stdio.h> /* Standard I/O for printf (troff command generation) */
#include <stdlib.h> /* Standard library functions */

/* SCCS version identifier */
static const char sccs_id[] = "@(#)ne5.c 1.3 25/05/29";

/* Troff register constants */
#define MARK_REGISTER 97 /**< Troff register used for position marking */
#define MAX_HANDLE 1000 /**< Maximum valid object handle value */

/* External variables - these should be defined in ne.h or other modules */
extern int yyval; /* Result of parsing rule, often an object handle */
extern int eht[]; /* Effective height of objects */
extern int ebase[]; /* Effective baseline of objects */
extern int ps; /* Current point size */
extern int dbg; /* Debug flag for diagnostic output */

/* External function declarations - these should be in ne.h */
extern int oalloc(void); /* Allocate object handle */
extern void nrwid(int obj, int ps, int new_obj); /* Set width register */

/* Local function prototypes for C90 compliance */
static int validate_object_handle(int handle);
static void generate_debug_output(const char *function_name, int input_handle, int result_handle);

/* Function prototypes for external interface */
void mark(int n);
void lineup(int n);

/**
 * @brief Validate that an object handle is within acceptable bounds.
 *
 * Performs basic validation to ensure object handles are reasonable.
 * This helps prevent array bounds violations and other memory errors.
 * Used consistently across all position marking operations.
 *
 * @param handle The object handle to validate
 * @return 1 if handle appears valid, 0 otherwise
 */
static int validate_object_handle(int handle) {
    /* Basic bounds checking - assumes handles are positive and reasonable */
    return (handle >= 0 && handle < MAX_HANDLE);
}

/**
 * @brief Generate standardized debug output for position operations.
 *
 * Provides consistent debug messaging across all position marking and
 * alignment functions when debug mode is enabled. Helps with tracing
 * execution flow and understanding position calculations.
 *
 * @param function_name Name of the calling function
 * @param input_handle The input object handle (may be 0)
 * @param result_handle The resulting object handle
 */
static void generate_debug_output(const char *function_name, int input_handle, int result_handle) {
    if (dbg && function_name) {
        if (input_handle > 0) {
            printf(".\t%s: input=S%d -> result=S%d\n",
                   function_name, input_handle, result_handle);
        } else {
            printf(".\t%s: created new mark -> result=S%d\n",
                   function_name, result_handle);
        }
    }
}

/**
 * @brief Mark the current horizontal position for later alignment reference.
 *
 * This function establishes a position marker that can be referenced later
 * for horizontal alignment operations. It supports two modes of operation:
 * 
 * 1. In-place marking (n != 0): Adds a position marker to an existing object
 * 2. Standalone marking (n == 0): Creates a new object containing only the marker
 *
 * Technical Implementation:
 * - Uses troff's \k escape sequence to capture current horizontal position
 * - Stores position in troff register 97 (MARK_REGISTER)
 * - For standalone marks, creates minimal object with zero dimensions
 * - For in-place marks, appends marker to existing object string
 *
 * Critical Bug Fix:
 * The original code had a major bug where it referenced eht[n] and ebase[n]
 * when n=0, causing undefined behavior. This has been fixed to use proper
 * zero dimensions for standalone markers.
 *
 * Troff Command Generation:
 * - In-place: ".as <n> \\k(97" (append mark to existing string)
 * - Standalone: ".ds <new> \\k(97" (define new string with mark)
 *
 * Usage Examples:
 * - mark(0): Create standalone position marker
 * - mark(expr): Add position marker to existing expression
 *
 * @param n Object handle to mark (0 for standalone marker)
 */
void mark(int n) {
    /* Validate input handle if non-zero */
    if (n != 0 && !validate_object_handle(n)) {
        if (dbg)
            printf(".\tERROR: mark: invalid object handle n=%d\n", n);
        return;
    }

    if (n != 0) {
        /* In-place marking: append position marker to existing object */
        printf(".as %d \\k(%d\n", n, MARK_REGISTER);
        yyval = n; /* Result is the same object with marker added */

    } else {
        /* Standalone marking: create new object containing only the marker */
        yyval = oalloc(); /* Allocate new handle for marker object */
        if (!validate_object_handle(yyval)) {
            if (dbg)
                printf(".\tERROR: mark: failed to allocate marker handle\n");
            return;
        }

        /* Generate troff string definition with position marker */
        printf(".ds %d \\k(%d\n", yyval, MARK_REGISTER);

        /* Calculate width of the marker object (essentially zero) */
        nrwid(yyval, ps, yyval);

        /* 
         * CRITICAL BUG FIX: Original code used eht[n] and ebase[n] when n=0,
         * which is undefined behavior. Set proper zero dimensions instead.
         */
        eht[yyval] = 0; /* No height contribution for standalone marker */
        ebase[yyval] = 0; /* Baseline at zero for standalone marker */
    }

    generate_debug_output("mark", n, yyval);
}

/**
 * @brief Align an object to a previously established position marker.
 *
 * This function creates horizontal alignment by positioning an object or
 * cursor relative to a position marker that was previously established by
 * the mark() function. It supports two alignment modes:
 *
 * 1. Object alignment (n != 0): Aligns an existing object to the mark
 * 2. Cursor alignment (n == 0): Moves cursor to the marked position
 *
 * Technical Implementation:
 * - Uses troff's \h horizontal motion command
 * - References position stored in troff register 97 (MARK_REGISTER)
 * - Calculates motion as: |position - object_width
 * - For cursor moves, simply moves to the absolute marked position
 *
 * Critical Bug Fix:
 * The original code had the same bug as mark(), referencing eht[n] and 
 * ebase[n] when n=0. This has been fixed to use proper zero dimensions
 * for cursor positioning objects.
 *
 * Alignment Mathematics:
 * - Current position: wherever we are now
 * - Target position: value stored in register 97
 * - For objects: align left edge to marked position
 * - Motion calculation: target_position - current_position - object_width
 *
 * Troff Command Generation:
 * - Object: ".ds <n> \\h'|\\n(97u-\\n(<n>u)'\\*(<n>)"
 * - Cursor: ".ds <new> \\h'|\\n(97u)'"
 *
 * Usage Examples:
 * - lineup(expr): Align expression to previously marked position
 * - lineup(0): Move cursor to marked position for subsequent content
 *
 * @param n Object handle to align (0 for cursor positioning)
 */
void lineup(int n) {
    /* Validate input handle if non-zero */
    if (n != 0 && !validate_object_handle(n)) {
        if (dbg)
            printf(".\tERROR: lineup: invalid object handle n=%d\n", n);
        return;
    }

    if (n != 0) {
        /* Object alignment: position existing object at marked location */

        /* First, ensure width calculation is current */
        nrwid(n, ps, n);

        /* Generate alignment command: move to mark minus object width, then place object */
        printf(".ds %d \\h'|\\n(%du-\\n(%du'\\*(%d\n",
               n, MARK_REGISTER, n, n);

        yyval = n; /* Result is the same object, now aligned */

    } else {
        /* Cursor alignment: create new object that moves cursor to mark */

        yyval = oalloc(); /* Allocate new handle for cursor movement */
        if (!validate_object_handle(yyval)) {
            if (dbg)
                printf(".\tERROR: lineup: failed to allocate cursor handle\n");
            return;
        }

        /* Generate cursor movement command: move to absolute marked position */
        printf(".ds %d \\h'|\\n(%du'\n", yyval, MARK_REGISTER);

        /* Calculate width of the movement object (the distance moved) */
        nrwid(yyval, ps, yyval);

        /* 
         * CRITICAL BUG FIX: Original code used eht[n] and ebase[n] when n=0,
         * which is undefined behavior. Set proper zero dimensions instead.
         */
        eht[yyval] = 0; /* No height contribution for cursor movement */
        ebase[yyval] = 0; /* Baseline at zero for cursor movement */
    }

    generate_debug_output("lineup", n, yyval);
}
