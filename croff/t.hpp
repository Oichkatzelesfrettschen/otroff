/*
 * t.h - Standard header file for troff implementation
 * 
 * This header contains common includes, global state structures,
 * and function prototypes used throughout the troff typesetting system.
 * 
 * Conforms to C90 standard for maximum portability.
 */

#ifndef T_H
#define T_H
#include "cxx23_scaffold.hpp"

/* 
 * Standard C90 library includes for troff implementation
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
 * Configuration constants - these should be defined elsewhere
 * or given reasonable default values for C90 compliance
 */
#ifndef NDI
#define NDI 10 /* Default number of device interfaces */
#endif

#ifndef NN
#define NN 200 /* Default total number of variables */
#endif

#ifndef NNAMES
#define NNAMES 100 /* Default number of named variables */
#endif

/*
 * Global device state structure array
 * 
 * Contains device-specific parameters used across the troff implementation:
 * - op: output position
 * - dnl: device new line
 * - dimac: device initial macro
 * - ditrap: device trap
 * - ditf: device trap flag
 * - alss: additional line space start
 * - blss: baseline skip start
 * - nls: new line start
 * - mkline: make line
 * - maxl: maximum line
 * - hnl: horizontal new line
 * - curd: current device
 */
extern struct device_state {
    int op; /* Output position */
    int dnl; /* Device new line */
    int dimac; /* Device initial macro */
    int ditrap; /* Device trap */
    int ditf; /* Device trap flag */
    int alss; /* Additional line space start */
    int blss; /* Baseline skip start */
    int nls; /* New line start */
    int mkline; /* Make line */
    int maxl; /* Maximum line */
    int hnl; /* Horizontal new line */
    int curd; /* Current device */
} d[NDI];

/*
 * Global variable state structure
 * 
 * Contains system-wide variables and counters:
 * - pn: page number
 * - nl: line number
 * - yr: year
 * - hp: horizontal position
 * - ct: character count
 * - dn: device number
 * - mo: month
 * - dy: day
 * - dw: day of week
 * - ln: line
 * - dl: line length
 * - st: string
 * - sb: string buffer
 * - cd: current directory
 * - vxx: extended variable array
 */
extern struct variable_state {
    int pn; /* Page number */
    int nl; /* Line number */
    int yr; /* Year */
    int hp; /* Horizontal position */
    int ct; /* Character count */
    int dn; /* Device number */
    int mo; /* Month */
    int dy; /* Day */
    int dw; /* Day of week */
    int ln; /* Line */
    int dl; /* Line length */
    int st; /* String */
    int sb; /* String buffer */
    int cd; /* Current directory */
    int vxx[NN - NNAMES]; /* Extended variable array */
} v;

/*
 * Function prototypes
 */

/*
 * sub1 - Subtract a 32-bit delta from a historic 32-bit timestamp
 * 
 * This function performs timestamp arithmetic, subtracting a delta value
 * from a two-element timestamp array representing a 32-bit time value.
 * The implementation handles potential overflow/underflow conditions.
 * 
 * Parameters:
 *   tt[2]  - Two-element array representing 32-bit timestamp (modified)
 *   delta  - Delta value to subtract from timestamp
 * 
 * Returns: void
 * 
 * Note: Implementation is in t.c
 */
void sub1(int tt[2], int delta);

/* 
 * Version identification (commented out for C90 compliance)
 * Original: static char Sccsid[] "@(#)t.h 1.3 of 4/26/77";
 */

#endif /* T_H */
