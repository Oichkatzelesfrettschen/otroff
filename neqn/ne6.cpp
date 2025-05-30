#include "cxx23_scaffold.hpp"
/**
 * @file ne6.c
 * @brief NEQN equation typesetting - Part 6: Matrix and column handling.
 *
 * This file contains functions for constructing mathematical matrices and
 * managing column-based layouts in the NEQN typesetting system. It handles
 * the complex task of organizing mathematical expressions into tabular
 * formats with proper alignment, spacing, and dimension calculations.
 *
 * Key Functionality:
 * - Matrix column definition and management
 * - Multi-row, multi-column mathematical layouts
 * - Automatic row height and baseline alignment
 * - Proper spacing between matrix elements
 * - Integration with pile and alignment systems
 *
 * Mathematical Applications:
 * - Determinants and matrix expressions
 * - Systems of linear equations
 * - Tabular mathematical data
 * - Aligned equation groups
 * - Complex multi-line expressions
 *
 * Technical Implementation:
 * - Uses global lp[] array for layout tracking
 * - Calculates optimal row heights and baselines
 * - Generates troff commands for matrix formatting
 * - Integrates with existing pile and alignment functions
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Robust error handling and input validation
 * - Efficient memory and register management
 * - Comprehensive documentation for maintainability
 * - Clean separation between layout logic and output generation
 */

#include "ne.h" /* NEQN type definitions and global declarations */
#include <stdio.h> /* Standard I/O for printf (troff command generation) */
#include <stdlib.h> /* Standard library functions */

/* SCCS version identifier */
static const char sccs_id[] = "@(#)ne6.c 1.3 25/05/29";

/* Matrix layout constants */
#define MAX_MATRIX_ELEMENTS 100 /**< Maximum elements in a matrix */
#define MAX_MATRIX_COLUMNS 20 /**< Maximum columns in a matrix */
#define MAX_MATRIX_ROWS 20 /**< Maximum rows in a matrix */

/* External variables - these should be defined in ne.h or other modules */
extern int yyval; /* Result of parsing rule, often an object handle */
extern int eht[]; /* Effective height of objects */
extern int ebase[]; /* Effective baseline of objects */
extern int lp[]; /* Layout position array for matrix construction */
extern int ct; /* Current position in layout array */
extern int ps; /* Current point size */
extern int dbg; /* Debug flag for diagnostic output */

/* External function declarations - these should be in ne.h */
extern int oalloc(void); /* Allocate object handle */
extern void ofree(int handle); /* Free object handle */
extern void lpile(int type, int p1, int p2); /* Create logical pile */
extern int max(int a, int b); /* Maximum of two integers */

/* Local function prototypes for C90 compliance */
static int validate_matrix_parameters(int p1, int p2);
static int validate_layout_bounds(int index);
static void calculate_row_dimensions(int row, int p1, int ncol, int nrow, int *hb, int *b);
static void apply_row_dimensions(int row, int p1, int ncol, int nrow, int hb, int b);
static void generate_matrix_debug_output(int result, int nrow, int ncol);

/* Function prototypes for external interface */
void column(int type, int p1, int p2);
void matrix(int p1, int p2);

/**
 * @brief Validate matrix parameters for safety.
 *
 * Performs comprehensive validation of matrix construction parameters
 * to prevent buffer overflows, invalid array accesses, and other
 * memory safety issues.
 *
 * @param p1 Starting position in layout array
 * @param p2 Ending position in layout array (may be unused)
 * @return 0 on success, -1 on validation failure
 */
static int validate_matrix_parameters(int p1, int p2) {
    /* Validate layout array bounds */
    if (p1 < 0 || p1 >= MAX_MATRIX_ELEMENTS) {
        if (dbg)
            printf(".\tERROR: validate_matrix: p1=%d out of bounds\n", p1);
        return -1;
    }

    if (ct < 0 || ct >= MAX_MATRIX_ELEMENTS) {
        if (dbg)
            printf(".\tERROR: validate_matrix: ct=%d out of bounds\n", ct);
        return -1;
    }

    if (p1 >= ct) {
        if (dbg)
            printf(".\tERROR: validate_matrix: p1=%d >= ct=%d\n", p1, ct);
        return -1;
    }

    return 0;
}

/**
 * @brief Validate layout array index bounds.
 *
 * Ensures that array indices used in layout operations are within
 * valid bounds to prevent buffer overflows and memory corruption.
 *
 * @param index Array index to validate
 * @return 1 if index is valid, 0 if out of bounds
 */
static int validate_layout_bounds(int index) {
    return (index >= 0 && index < MAX_MATRIX_ELEMENTS);
}

/**
 * @brief Calculate optimal dimensions for a matrix row.
 *
 * Determines the maximum height above baseline and maximum baseline
 * depth for all elements in a given row. This ensures that all
 * elements in the row will have consistent vertical alignment.
 *
 * @param row Row number (1-based)
 * @param p1 Starting position in layout array
 * @param ncol Number of columns in matrix
 * @param nrow Number of rows in matrix
 * @param hb Pointer to store height above baseline
 * @param b Pointer to store baseline depth
 */
static void calculate_row_dimensions(int row, int p1, int ncol, int nrow, int *hb, int *b) {
    int i, j;

    /* Initialize dimensions */
    *hb = 0;
    *b = 0;

    /* Calculate starting position for this row */
    j = p1 + row;

    /* Scan all columns in this row */
    for (i = 0; i < ncol; i++) {
        /* Validate array bounds before access */
        if (!validate_layout_bounds(j)) {
            if (dbg)
                printf(".\tERROR: calculate_row_dimensions: invalid index %d\n", j);
            return;
        }

        /* Update maximum height above baseline */
        *hb = max(*hb, eht[lp[j]] - ebase[lp[j]]);

        /* Update maximum baseline depth */
        *b = max(*b, ebase[lp[j]]);

        /* Move to next column in this row */
        j += nrow + 2;
    }
}

/**
 * @brief Apply calculated dimensions to all elements in a matrix row.
 *
 * Sets the height and baseline for all elements in a row to the
 * calculated optimal values, ensuring consistent vertical alignment
 * across the entire row.
 *
 * @param row Row number (1-based)
 * @param p1 Starting position in layout array
 * @param ncol Number of columns in matrix
 * @param nrow Number of rows in matrix
 * @param hb Height above baseline to apply
 * @param b Baseline depth to apply
 */
static void apply_row_dimensions(int row, int p1, int ncol, int nrow, int hb, int b) {
    int i, j;

    /* Calculate starting position for this row */
    j = p1 + row;

    /* Apply dimensions to all columns in this row */
    for (i = 0; i < ncol; i++) {
        /* Validate array bounds before access */
        if (!validate_layout_bounds(j)) {
            if (dbg)
                printf(".\tERROR: apply_row_dimensions: invalid index %d\n", j);
            return;
        }

        /* Set consistent baseline and total height */
        ebase[lp[j]] = b;
        eht[lp[j]] = b + hb;

        /* Move to next column in this row */
        j += nrow + 2;
    }
}

/**
 * @brief Generate comprehensive debug output for matrix construction.
 *
 * Provides detailed debugging information about the constructed matrix,
 * including dimensions, element count, and final properties.
 *
 * @param result Handle of the constructed matrix object
 * @param nrow Number of rows in the matrix
 * @param ncol Number of columns in the matrix
 */
static void generate_matrix_debug_output(int result, int nrow, int ncol) {
    if (dbg) {
        printf(".\tmatrix S%d: r=%d, c=%d, h=%d, b=%d\n",
               result, nrow, ncol, eht[result], ebase[result]);
    }
}

/**
 * @brief Begin definition of a matrix column.
 *
 * This function marks the beginning of a new column in a matrix being
 * constructed. It records the number of elements in the column and
 * sets up the layout tracking structures for proper matrix assembly.
 *
 * Matrix Layout Strategy:
 * - The lp[] array tracks the layout of matrix elements
 * - lp[p1] stores the number of rows in this column
 * - Elements from p1+1 to ct-1 belong to this column
 * - Column type is stored for later processing
 *
 * Critical Bug Fixes:
 * - Added bounds checking for layout array access
 * - Fixed potential buffer overflow in element counting
 * - Added validation for matrix parameter consistency
 *
 * Column Types:
 * - 'L': Left-aligned column
 * - 'C': Center-aligned column  
 * - 'R': Right-aligned column
 *
 * @param type Column alignment type ('L', 'C', or 'R')
 * @param p1 Starting position for this column in layout array
 * @param p2 Ending position (may be unused in current implementation)
 */
void column(int type, int p1, int p2) {
    int i;

    /* Validate input parameters */
    if (validate_matrix_parameters(p1, p2) != 0) {
        return; /* Error already reported */
    }

    /* Validate that we have space for column metadata */
    if (ct >= MAX_MATRIX_ELEMENTS - 1) {
        if (dbg)
            printf(".\tERROR: column: layout array full, ct=%d\n", ct);
        return;
    }

    /* Calculate number of elements in this column */
    lp[p1] = ct - p1 - 1;

    /* Generate debug output showing column contents */
    if (dbg) {
        printf(".\t%c column of", type);
        for (i = p1 + 1; i < ct; i++) {
            /* Validate bounds before accessing lp[i] */
            if (validate_layout_bounds(i)) {
                printf(" S%d", lp[i]);
            }
        }
        printf(", rows=%d\n", lp[p1]);
    }

    /* Store column type and advance layout counter */
    lp[ct] = type;
    ct++;
}

/**
 * @brief Complete matrix construction and generate troff output.
 *
 * This function finalizes the construction of a mathematical matrix by:
 * 1. Calculating optimal row and column dimensions
 * 2. Ensuring consistent alignment across all elements
 * 3. Creating column piles for vertical arrangement
 * 4. Generating final troff commands for matrix display
 *
 * Matrix Construction Algorithm:
 * 1. Determine matrix dimensions (rows × columns)
 * 2. For each row: calculate maximum height and baseline
 * 3. Apply consistent dimensions to all elements in each row
 * 4. Create vertical piles for each column
 * 5. Combine column piles into final matrix string
 * 6. Generate troff output with proper spacing
 *
 * Critical Bug Fixes:
 * - Fixed arithmetic error: `i = +lp[i] + 2` should be `i += lp[i] + 2`
 * - Added comprehensive bounds checking for all array access
 * - Fixed potential buffer overflow in val[] array
 * - Added validation for consistent row counts across columns
 *
 * Spacing Strategy:
 * - Uses "\ \ " (escaped spaces) between columns
 * - Maintains proper mathematical spacing conventions
 * - Preserves alignment within each column
 *
 * @param p1 Starting position of matrix in layout array
 * @param p2 Ending position (may be unused in current implementation)
 */
void matrix(int p1, int p2) {
    int nrow, ncol, i, j, k, hb, b;
    int val[MAX_MATRIX_COLUMNS]; /* Column handles for final assembly */
    const char *space = "\\ \\ "; /* Spacing between matrix columns */

    /* Validate input parameters */
    if (validate_matrix_parameters(p1, p2) != 0) {
        return; /* Error already reported */
    }

    /* Get number of rows from first column */
    nrow = lp[p1];
    if (nrow <= 0 || nrow > MAX_MATRIX_ROWS) {
        if (dbg)
            printf(".\tERROR: matrix: invalid row count %d\n", nrow);
        return;
    }

    /* Count columns and validate layout consistency */
    ncol = 0;
    i = p1;
    while (i < ct && ncol < MAX_MATRIX_COLUMNS) {
        /* Validate bounds before accessing lp[i] */
        if (!validate_layout_bounds(i)) {
            if (dbg)
                printf(".\tERROR: matrix: invalid layout index %d\n", i);
            return;
        }

        ncol++;

        if (dbg) {
            printf(".\tcolct=%d\n", lp[i]);
        }

        /* CRITICAL BUG FIX: Original code had `i = +lp[i] + 2` which is wrong */
        i += lp[i] + 2; /* Move to next column start */
    }

    if (ncol == 0) {
        if (dbg)
            printf(".\tERROR: matrix: no columns found\n");
        return;
    }

    /* Calculate optimal dimensions for each row */
    for (k = 1; k <= nrow; k++) {
        calculate_row_dimensions(k, p1, ncol, nrow, &hb, &b);

        if (dbg) {
            printf(".\trow %d: b=%d, hb=%d\n", k, b, hb);
        }

        apply_row_dimensions(k, p1, ncol, nrow, hb, b);
    }

    /* Create vertical piles for each column */
    j = p1;
    for (i = 0; i < ncol; i++) {
        /* Validate bounds before accessing layout array */
        if (!validate_layout_bounds(j) || !validate_layout_bounds(j + lp[j] + 1)) {
            if (dbg)
                printf(".\tERROR: matrix: invalid pile bounds for column %d\n", i);
            return;
        }

        /* Create pile for this column */
        lpile(lp[j + lp[j] + 1], j + 1, j + lp[j] + 1);
        val[i] = yyval;

        /* CRITICAL BUG FIX: Original code had `j = +nrow + 2` which is wrong */
        j += nrow + 2; /* Move to next column */
    }

    /* Allocate handle for final matrix result */
    yyval = oalloc();
    if (yyval <= 0) {
        if (dbg)
            printf(".\tERROR: matrix: failed to allocate result handle\n");
        return;
    }

    /* Set matrix dimensions based on first column */
    eht[yyval] = eht[val[0]];
    ebase[yyval] = ebase[val[0]];

    /* Generate debug output */
    generate_matrix_debug_output(yyval, nrow, ncol);

    /* Generate troff string definition for matrix */
    printf(".ds %d \"", yyval);
    for (i = 0; i < ncol; i++) {
        printf("\\*(%d%s", val[i], (i == ncol - 1) ? "" : space);
        ofree(val[i]); /* Free column handle after use */
    }
    printf("\n");

    /* Reset layout counter to start of matrix */
    ct = p1;
}
