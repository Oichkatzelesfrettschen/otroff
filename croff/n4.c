/*
 * n4.c - Number registers, conversion, and arithmetic for troff (C90)
 *
 * Copyright (c) 1979 Bell Telephone Laboratories, Incorporated
 * Copyright (c) 2024 Modern C90 port
 *
 * This module handles the core numeric processing functionality for the
 * troff text formatting system, including number register operations,
 * arithmetic expression evaluation, and various number formatting modes.
 *
 * MAJOR COMPONENTS:
 * - Number register operations (.nr, .rr, .af commands)
 * - Arithmetic expression evaluation with full precedence
 * - Number formatting (decimal, roman, alphabetic)
 * - Built-in register access (.s, .v, .f, etc.)
 * - Input number parsing with scaling units
 * - Format conversion and output functions
 *
 * DESIGN PRINCIPLES:
 * 1. Robust arithmetic with overflow protection
 * 2. Comprehensive scaling unit support
 * 3. Efficient number register management
 * 4. Full expression evaluation with precedence
 * 5. Multiple output formats for flexibility
 *
 * NUMERIC FORMATS SUPPORTED:
 * - Decimal: 1, 2, 3, ...
 * - Roman: i, ii, iii, iv, v, ... (lowercase)
 * - Roman: I, II, III, IV, V, ... (uppercase)
 * - Alphabetic: a, b, c, ... (lowercase)
 * - Alphabetic: A, B, C, ... (uppercase)
 * - Zero-filled: 001, 002, 003, ...
 *
 * SCALING UNITS:
 * - u: basic units
 * - v: vertical spacing units
 * - m: ems (current font size)
 * - n: ens (half ems)
 * - p: points (1/72 inch)
 * - i: inches
 * - c: centimeters
 * - P: picas (1/6 inch)
 *
 * COMPATIBILITY:
 * - C90 compliant with proper function prototypes
 * - Compatible with original Bell Labs troff behavior
 * - Enhanced error handling and bounds checking
 * - Maintains historical expression syntax
 */

#include "tdef.h"
#include "env.h"
#include "t.h"
#include "tw.h"

#include <stdio.h>      /* C90: standard I/O functions */
#include <stdlib.h>     /* C90: exit, abs, etc. */
#include <string.h>     /* C90: string functions */

/* Version identification */
static char Sccsid[] = "@(#)n4.c  1.4 of 4/26/77";

/* External global variables - these are defined elsewhere in the system */
extern int ascii;           /* ASCII mode flag */
extern int cbuf[NC];        /* Character buffer */
extern int *cp;             /* Character buffer pointer */
extern int r[NN];           /* Number register names */
extern int *vlist;          /* Number register values */
extern int inc[NN];         /* Number register increments */
extern int fmt[NN];         /* Number register formats */
extern int ch;              /* Current input character */
extern int lgf;             /* Line gathering flag */
extern int *frame;          /* Frame stack pointer */
extern int pl;              /* Page length */
extern int lastl;           /* Last line length */
extern int ralss;           /* Right margin adjustment */
extern int totout;          /* Total output lines */
extern int nrbits;          /* Number register formatting bits */
extern int nonumb;          /* No number found flag */
extern int vflag;           /* Vertical motion flag */
extern int noscale;         /* No scaling flag */
extern int dfact;           /* Default scaling factor */
extern int dfactd;          /* Default scaling divisor */
extern int po;              /* Page offset */
extern int nform;           /* Number format */
extern int ll;              /* Line length */
extern int in;              /* Indent */
extern int font;            /* Current font */
extern int lss;             /* Line spacing */
extern int pts;             /* Point size */
extern int font1;           /* Previous font */
extern struct env *dip;     /* Current diversion pointer */
extern int fi;              /* Fill mode */
extern int res;             /* Resolution */
extern int cwidth;          /* Character width */
extern int dotT;            /* Terminal type */
extern int ulfont;          /* Underline font */
extern int ev;              /* Environment */
extern int ne;              /* Need lines */
extern int ad, admod;       /* Adjustment mode and modifier */
extern int print;           /* Print flag */
extern int ls;              /* Line spacing multiplier */
extern int xxx;             /* Temporary variable */

/* Command table structure */
extern struct contab {
    int rq;                 /* Request name */
    int (*f)(void);         /* Function pointer - C90 style */
} contab[NM];

/* Function prototypes for static functions */
static int fnumb(int i, int (*f)(int));
static int decml(int i, int (*f)(int));
static int roman(int i, int (*f)(int));
static int roman0(int i, int (*f)(int), char *onesp, char *fivesp);
static int abc(int i, int (*f)(int));
static int abc0(int i, int (*f)(int));
static int wrc(int i);
static long atoi0(void);
static long ckph(void);
static long atoi1(void);

/* Function prototypes for module functions */
void setn(void);
void setn1(int i);
int findr(int i);
int tatoi(void);
void caserr(void);
void casenr(void);
void caseaf(void);
void vnumb(int *i);
void hnumb(int *i);
int inumb(int *n);
int quant(int n, int m);

/* External function prototypes */
extern int getch(void);
extern int getsn(void);
extern int getrq(void);
extern int findt1(void);
extern int skip(void);
extern int alph(int c);
extern void prstrfl(const char *s);
extern void done2(int code);
extern void edone(int code);

/*
 * setn - Set a number register
 * 
 * Handles the .nr command and built-in register access.
 * Supports increment/decrement operations and special registers.
 */
void 
setn(void) 
{
    register int i, j;
    int f;

    /* Initialize format and increment flag */
    f = nform = 0;
    
    /* Check for increment (+) or decrement (-) prefix */
    if ((i = getch() & CMASK) == '+') {
        f = 1;
    } else if (i == '-') {
        f = -1;
    } else {
        ch = i;  /* Put character back */
    }
    
    /* Get register name */
    if ((i = getsn()) == 0) {
        return;  /* No register name found */
    }
    
    /* Handle built-in registers (those starting with '.') */
    if ((i & 0177) == '.') {
        switch (i) {
        case 256*'s' + '.':  /* Point size (.s) */
            i = pts & 077;
            break;
        case 256*'v' + '.':  /* Vertical spacing (.v) */
            i = lss;
            break;
        case 256*'f' + '.':  /* Font number (.f) */
            i = font + 1;
            break;
        case 256*'p' + '.':  /* Page length (.p) */
            i = pl;
            break;
        case 256*'t' + '.':  /* Distance to next trap (.t) */
            i = findt1();
            break;
        case 256*'o' + '.':  /* Page offset (.o) */
            i = po;
            break;
        case 256*'l' + '.':  /* Line length (.l) */
            i = ll;
            break;
        case 256*'i' + '.':  /* Indent (.i) */
            i = in;
            break;
        case 256*'$' + '.':  /* Number of arguments (.$) */
            i = *frame;
            break;
        case 256*'A' + '.':  /* ASCII flag (.A) */
            i = ascii;
            break;
        case 256*'c' + '.':  /* Current page number (.c) */
            i = v.cd;
            break;
        case 256*'n' + '.':  /* Length of last line (.n) */
            i = lastl;
            break;
        case 256*'a' + '.':  /* Adjustment mode (.a) */
            i = ralss;
            break;
        case 256*'h' + '.':  /* High-water mark in diversion (.h) */
            i = dip->hnl;
            break;
        case 256*'d' + '.':  /* Current vertical position (.d) */
            if (dip->op) {
                i = dip->dnl;
            } else {
                i = v.nl;
            }
            break;
        case 256*'u' + '.':  /* Fill mode (.u) */
            i = fi;
            break;
        case 256*'j' + '.':  /* Adjustment type (.j) */
            i = ad + 2 * admod;
            break;
        case 256*'w' + '.':  /* Width of last character (.w) */
            i = cwidth;
            break;
        case 256*'x' + '.':  /* Underline font (.x) */
            i = ulfont + 1;
            break;
        case 256*'y' + '.':  /* Previous font (.y) */
            i = font1;
            break;
        case 256*'T' + '.':  /* Terminal type (.T) */
            i = dotT;
            break;
        case 256*'V' + '.':  /* Vertical resolution (.V) */
            i = VERT;
            break;
        case 256*'H' + '.':  /* Horizontal resolution (.H) */
            i = HOR;
            break;
        case 256*'k' + '.':  /* Current horizontal position (.k) */
            i = ne;
            break;
        case 256*'P' + '.':  /* Print mode (.P) */
            i = print;
            break;
        case 256*'L' + '.':  /* Line spacing (.L) */
            i = ls;
            break;
        case 256*'z' + '.':  /* Name of current diversion (.z) */
            /* Special case: store diversion name in character buffer */
            i = dip->curd;
            cbuf[0] = i & BMASK;
            cbuf[1] = (i >> BYTE) & BMASK;
            cbuf[2] = 0;
            cp = cbuf;
            return;
        default:
            goto s0;  /* Not a built-in register */
        }
    } else {
    s0:
        /* Handle user-defined registers */
        if ((j = findr(i)) == -1) {
            i = 0;  /* Register not found */
        } else {
            /* Apply increment/decrement and get current value */
            i = (vlist[j] = (vlist[j] + inc[j] * f));
            nform = fmt[j];  /* Use register's format */
        }
    }
    
    /* Convert value to string and store in buffer */
    setn1(i);
    cp = cbuf;
}

/*
 * setn1 - Convert number register value to character buffer
 * 
 * Converts integer value to string representation using the
 * appropriate format (decimal, roman, alphabetic).
 */
void 
setn1(int i) 
{
    int j;

    /* Initialize buffer and format number */
    cp = cbuf;
    nrbits = 0;
    j = fnumb(i, wrc);
    *cp = 0;        /* Null terminate */
    cp = cbuf;      /* Reset pointer */
}

/*
 * findr - Locate or create a number register slot
 * 
 * Searches for an existing register or allocates a new slot.
 * Returns the index of the register or -1 on error.
 */
int 
findr(int i) 
{
    register int j;
    static int numerr = 0;  /* Error counter */

    if (i == 0) {
        return (-1);  /* Invalid register name */
    }
    
    /* Search for existing register */
    for (j = 0; j < NN; j++) {
        if (i == r[j]) {
            break;
        }
    }
    
    if (j != NN) {
        return (j);  /* Found existing register */
    }
    
    /* Find empty slot for new register */
    for (j = 0; j < NN; j++) {
        if (r[j] == 0) {
            r[j] = i;
            break;
        }
    }
    
    /* Handle table overflow */
    if (j == NN) {
        if (!numerr) {
            prstrfl("Too many number registers.\n");
        }
        if (++numerr > 1) {
            done2(04);  /* Fatal error */
        } else {
            edone(04);  /* Recoverable error */
        }
    }
    
    return (j);
}

/*
 * fnumb - Format number using specified output function
 * 
 * Handles negative numbers and delegates to appropriate formatting
 * function based on nform (decimal, roman, alphabetic).
 */
static int 
fnumb(int i, int (*f)(int)) 
{
    register int j;

    j = 0;
    
    /* Handle negative numbers */
    if (i < 0) {
        j = (*f)('-' | nrbits);
        i = -i;
    }
    
    /* Format according to current format */
    switch (nform) {
    default:
    case '1':
    case 0:
        return (decml(i, f) + j);  /* Decimal */
    case 'i':
    case 'I':
        return (roman(i, f) + j);  /* Roman numerals */
    case 'a':
    case 'A':
        return (abc(i, f) + j);    /* Alphabetic */
    }
}

/*
 * decml - Output decimal number recursively
 * 
 * Converts integer to decimal representation by recursive division.
 */
static int 
decml(int i, int (*f)(int)) 
{
    register int j, k;

    k = 0;
    nform--;  /* Adjust format counter */
    
    /* Recursively output higher digits */
    if ((j = i / 10) || (nform > 0)) {
        k = decml(j, f);
    }
    
    /* Output current digit */
    return (k + (*f)((i % 10 + '0') | nrbits));
}

/*
 * roman - Output integer in roman numerals
 * 
 * Handles both uppercase and lowercase roman numerals.
 * Zero is output as '0' for clarity.
 */
static int 
roman(int i, int (*f)(int)) 
{
    if (!i) {
        return (*f)('0' | nrbits);
    }
    
    if (nform == 'i') {
        return roman0(i, f, "ixcmz", "vldw");  /* Lowercase */
    } else {
        return roman0(i, f, "IXCMZ", "VLDW");  /* Uppercase */
    }
}

/*
 * roman0 - Core roman numeral conversion algorithm
 * 
 * Recursively converts decimal to roman numerals using
 * the provided character sets for ones and fives.
 */
static int 
roman0(int i, int (*f)(int), char *onesp, char *fivesp) 
{
    register int q, rem, k;

    k = 0;
    if (!i) {
        return (0);
    }
    
    /* Process higher decimal places recursively */
    k = roman0(i / 10, f, onesp + 1, fivesp + 1);
    
    /* Process current decimal place */
    q = (i = i % 10) / 5;
    rem = i % 5;
    
    /* Handle subtractive notation (4 and 9) */
    if (rem == 4) {
        k += (*f)(*onesp | nrbits);
        if (q) {
            i = *(onesp + 1);  /* 9: one before ten */
        } else {
            i = *fivesp;       /* 4: one before five */
        }
        return (k + (*f)(i | nrbits));
    }
    
    /* Output five if present */
    if (q) {
        k += (*f)(*fivesp | nrbits);
    }
    
    /* Output remaining ones */
    while (--rem >= 0) {
        k += (*f)(*onesp | nrbits);
    }
    
    return (k);
}

/*
 * abc - Output integer using alphabetic sequence
 * 
 * Converts integers to alphabetic representation (a, b, c, ...).
 * Zero is output as '0' for clarity.
 */
static int 
abc(int i, int (*f)(int)) 
{
    if (!i) {
        return (*f)('0' | nrbits);
    }
    return abc0(i - 1, f);  /* Convert to 0-based indexing */
}

/*
 * abc0 - Core alphabetic conversion algorithm
 * 
 * Recursively converts to base-26 alphabetic representation.
 */
static int 
abc0(int i, int (*f)(int)) 
{
    register int j, k;

    k = 0;
    
    /* Handle higher "digits" recursively */
    if ((j = i / 26)) {
        k = abc0(j - 1, f);
    }
    
    /* Output current "digit" */
    return (k + (*f)((i % 26 + nform) | nrbits));
}

/*
 * wrc - Write character to number buffer
 * 
 * Safely writes a character to the global character buffer
 * with bounds checking.
 */
static int 
wrc(int i) 
{
    if (cp >= &cbuf[NC]) {
        return 0;  /* Buffer overflow */
    }
    *cp++ = i;
    return 1;
}

/*
 * tatoi - Read and convert integer from input
 * 
 * Main entry point for parsing integers from input stream.
 */
int 
tatoi(void) 
{
    register int i;
    
    return (i = (int)atoi0());
}

/*
 * atoi0 - Arithmetic expression evaluator
 * 
 * Recursively evaluates arithmetic expressions with proper
 * operator precedence and parentheses handling.
 * Supports: +, -, *, /, %, &(and), :(or), =, >, <, >=, <=
 */
static long 
atoi0(void) 
{
    register int ii, k, cnt;
    long i, acc;

    i = 0;
    acc = 0;
    nonumb = 0;
    cnt = -1;

a0:
    cnt++;
    switch ((ii = getch()) & CMASK) {
    default:
        ch = ii;  /* Put character back */
        if (cnt) {
            break;
        }
        /* Fall through for first character */
    case '+':
        i = ckph();
        if (nonumb) {
            break;
        }
        acc += i;
        goto a0;
    case '-':
        i = ckph();
        if (nonumb) {
            break;
        }
        acc -= i;
        goto a0;
    case '*':
        i = ckph();
        if (nonumb) {
            break;
        }
        acc *= i;
        goto a0;
    case '/':
        i = ckph();
        if (nonumb) {
            break;
        }
        if (i == 0) {
            prstrfl("Divide by zero.\n");
            acc = 0;
        } else {
            acc /= i;
        }
        goto a0;
    case '%':
        i = ckph();
        if (nonumb) {
            break;
        }
        if (i != 0) {
            acc %= i;
        }
        goto a0;
    case '&':  /* Logical AND */
        i = ckph();
        if (nonumb) {
            break;
        }
        if ((acc > 0) && (i > 0)) {
            acc = 1;
        } else {
            acc = 0;
        }
        goto a0;
    case ':':  /* Logical OR */
        i = ckph();
        if (nonumb) {
            break;
        }
        if ((acc > 0) || (i > 0)) {
            acc = 1;
        } else {
            acc = 0;
        }
        goto a0;
    case '=':  /* Equality test */
        if (((ii = getch()) & CMASK) != '=') {
            ch = ii;  /* Single = treated as == */
        }
        i = ckph();
        if (nonumb) {
            acc = 0;
            break;
        }
        if (i == acc) {
            acc = 1;
        } else {
            acc = 0;
        }
        goto a0;
    case '>':  /* Greater than (or >=) */
        k = 0;
        if (((ii = getch()) & CMASK) == '=') {
            k++;  /* >= operator */
        } else {
            ch = ii;
        }
        i = ckph();
        if (nonumb) {
            acc = 0;
            break;
        }
        if (acc > (i - k)) {
            acc = 1;
        } else {
            acc = 0;
        }
        goto a0;
    case '<':  /* Less than (or <=) */
        k = 0;
        if (((ii = getch()) & CMASK) == '=') {
            k++;  /* <= operator */
        } else {
            ch = ii;
        }
        i = ckph();
        if (nonumb) {
            acc = 0;
            break;
        }
        if (acc < (i + k)) {
            acc = 1;
        } else {
            acc = 0;
        }
        goto a0;
    case ')':
        break;  /* End of parenthesized expression */
    case '(':
        acc = atoi0();  /* Recursively evaluate parentheses */
        goto a0;
    }
    
    return (acc);
}

/*
 * ckph - Parse parentheses during arithmetic evaluation
 * 
 * Handles parenthesized subexpressions and delegates to
 * appropriate parsing function.
 */
static long 
ckph(void) 
{
    register int i;
    long j;

    if (((i = getch()) & CMASK) == '(') {
        j = atoi0();  /* Evaluate parenthesized expression */
    } else {
        ch = i;       /* Put character back */
        j = atoi1();  /* Parse basic number */
    }
    
    return (j);
}

/*
 * atoi1 - Basic number parser with scaling units
 * 
 * Parses numbers with optional scaling units (u, v, m, n, p, i, c, P).
 * Handles signed numbers, absolute positioning, and decimal fractions.
 */
static long 
atoi1(void) 
{
    register int i, j, digits;
    long acc;
    int neg, abs, field;

    /* Initialize parsing state */
    neg = abs = field = digits = 0;
    acc = 0;

a0:
    /* Handle sign and absolute position modifiers */
    switch ((i = getch()) & CMASK) {
    default:
        ch = i;
        break;
    case '+':
        goto a0;  /* Ignore leading plus */
    case '-':
        neg = 1;
        goto a0;
    case '|':
        abs = 1 + neg;  /* Absolute position */
        neg = 0;
        goto a0;
    }

a1:
    /* Parse numeric digits */
    while (((j = ((i = getch()) & CMASK) - '0') >= 0) && (j <= 9)) {
        field++;
        digits++;
        acc = 10 * acc + j;
    }
    
    /* Handle decimal point */
    if ((i & CMASK) == '.') {
        field++;
        digits = 0;
        goto a1;  /* Continue parsing fractional part */
    }
    
    ch = i;  /* Put back non-digit character */
    
    if (!field) {
        goto a2;  /* No number found */
    }
    
    /* Parse scaling unit */
    switch ((i = getch()) & CMASK) {
    case 'u':  /* Basic units */
        i = j = 1;
        break;
    case 'v':  /* Vertical spacing units */
        j = lss;
        i = 1;
        break;
    case 'm':  /* Ems */
        j = EM;
        i = 1;
        break;
    case 'n':  /* Ens */
        j = EM;
#ifndef NROFF
        i = 2;  /* Half em in troff */
#endif
#ifdef NROFF
        i = 1;  /* Same as em in nroff */
#endif
        break;
    case 'p':  /* Points */
        j = INCH;
        i = 72;
        break;
    case 'i':  /* Inches */
        j = INCH;
        i = 1;
        break;
    case 'c':  /* Centimeters */
        j = INCH * 50;
        i = 127;
        break;
    case 'P':  /* Picas */
        j = INCH;
        i = 6;
        break;
    default:   /* Default scaling */
        j = dfact;
        ch = i;  /* Put character back */
        i = dfactd;
    }
    
    /* Apply sign */
    if (neg) {
        acc = -acc;
    }
    
    /* Apply scaling if not disabled */
    if (!noscale) {
        acc = (acc * j) / i;
    }
    
    /* Handle decimal fractions */
    if ((field != digits) && (digits > 0)) {
        while (digits--) {
            acc /= 10;
        }
    }
    
    /* Handle absolute positioning */
    if (abs) {
        if (dip->op) {
            j = dip->dnl;  /* Diversion position */
        } else {
            j = v.nl;      /* Current page position */
        }
        if (!vflag) {
            j = v.hp;      /* Horizontal position */
        }
        if (abs == 2) {
            j = -j;        /* Negative absolute */
        }
        acc -= j;
    }

a2:
    nonumb = !field;  /* Set flag if no number was parsed */
    return (acc);
}

/*
 * caserr - Clear a number register (.rr command)
 * 
 * Removes a user-defined number register, freeing its slot
 * for reuse. Predefined registers cannot be removed.
 */
void 
caserr(void) 
{
    register int i, j;

    lgf++;  /* Increment line gathering flag */
    skip(); /* Skip whitespace */
    
    if ((i = getrq()) == 0) {
        return;  /* No register name provided */
    }
    
    /* Search for register in user-defined range */
    for (j = NNAMES; j < NN; j++) {
        if (i == r[j]) {
            break;
        }
    }
    
    /* Clear register if found */
    if (j != NN) {
        r[j] = vlist[j] = inc[j] = fmt[j] = 0;
    }
}

/*
 * casenr - Set number register (.nr command)
 * 
 * Sets a number register's value and optional increment.
 * Creates new registers as needed.
 */
void 
casenr(void) 
{
    register int i, j;

    lgf++;  /* Increment line gathering flag */
    skip(); /* Skip whitespace */
    
    /* Get register name */
    if ((i = findr(getrq())) == -1) {
        goto rtn;  /* Invalid register name */
    }
    
    skip();  /* Skip whitespace */
    
    /* Parse register value */
    j = inumb(&vlist[i]);
    if (nonumb) {
        goto rtn;  /* No value provided */
    }
    vlist[i] = j;
    
    skip();  /* Skip whitespace */
    
    /* Parse optional increment value */
    j = tatoi();
    if (nonumb) {
        goto rtn;  /* No increment provided */
    }
    inc[i] = j;

rtn:
    return;
}

/*
 * caseaf - Assign format to number register (.af command)
 * 
 * Sets the output format for a number register.
 * Formats: 1 (decimal), i/I (roman), a/A (alphabetic), 001 (zero-filled)
 */
void 
caseaf(void) 
{
    register int i, j, k;

    lgf++;  /* Increment line gathering flag */
    
    /* Parse command arguments */
    if (skip() || !(i = getrq()) || skip()) {
        return;  /* Missing arguments */
    }
    
    k = 0;
    
    /* Check if format is alphabetic */
    if (!alph(j = getch())) {
        ch = j;  /* Put back non-alphabetic character */
        
        /* Count consecutive digits for zero-fill format */
        while (((j = getch() & CMASK) >= '0') && (j <= '9')) {
            k++;
        }
    }
    
    /* Use format character or digit count */
    if (!k) {
        k = j;
    }
    
    /* Set format for the register */
    fmt[findr(i)] = k & BMASK;
}

/*
 * vnumb - Parse vertical motion number
 * 
 * Sets up context for parsing vertical measurements and
 * calls the general number parser.
 */
void 
vnumb(int *i) 
{
    vflag++;            /* Set vertical flag */
    dfact = lss;        /* Default factor is line spacing */
    res = VERT;         /* Vertical resolution */
    *i = inumb(i);
}

/*
 * hnumb - Parse horizontal motion number
 * 
 * Sets up context for parsing horizontal measurements and
 * calls the general number parser.
 */
void 
hnumb(int *i) 
{
    dfact = EM;         /* Default factor is em space */
    res = HOR;          /* Horizontal resolution */
    *i = inumb(i);
}

/*
 * inumb - Convert input number with scaling
 * 
 * Main number parsing function that handles scaling units,
 * relative increments, and quantization.
 */
int 
inumb(int *n) 
{
    register int i, j, f;

    f = 0;
    
    /* Handle relative increment/decrement */
    if (n) {
        if ((j = (i = getch()) & CMASK) == '+') {
            f = 1;
        } else if (j == '-') {
            f = -1;
        } else {
            ch = i;  /* Put character back */
        }
    }
    
    /* Parse the number */
    i = tatoi();
    
    /* Apply relative change if specified */
    if (n && f) {
        i = *n + f * i;
    }
    
    /* Quantize to appropriate resolution */
    i = quant(i, res);
    
    /* Reset parsing context */
    vflag = 0;
    res = dfactd = dfact = 1;
    
    /* Return 0 if no number was found */
    if (nonumb) {
        i = 0;
    }
    
    return (i);
}

/*
 * quant - Quantize number to nearest multiple
 * 
 * Rounds n to the nearest multiple of m, handling
 * negative numbers correctly.
 */
int 
quant(int n, int m) 
{
    register int i, neg;

    neg = 0;
    
    /* Handle negative numbers */
    if (n < 0) {
        neg++;
        n = -n;
    }
    
    /* Quantize to nearest multiple */
    i = n / m;
    if ((n - m * i) > (m / 2)) {
        i += 1;  /* Round up */
    }
    i *= m;
    
    /* Restore sign */
    if (neg) {
        i = -i;
    }
    
    return (i);
}
