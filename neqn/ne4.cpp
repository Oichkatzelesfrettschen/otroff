#include "cxx23_scaffold.hpp"
/**
 * @file ne4.c
 * @brief NEQN equation typesetting - Part 4: Main driver and I/O management.
 *
 * This file contains the main program driver for the NEQN typesetting system.
 * It handles command-line argument processing, input/output file management,
 * equation detection and processing, and overall program flow control.
 * The module coordinates between the parser, typesetting engine, and troff
 * output generation.
 *
 * Key Responsibilities:
 * - Command-line argument parsing and validation
 * - Input file processing and line-by-line parsing
 * - Equation boundary detection (.EQ/.EN and inline delimiters)
 * - Integration with yacc-generated parser
 * - Troff output generation and formatting
 * - Error handling and reporting
 * - Resource management (file descriptors, memory allocation)
 *
 * Program Flow:
 * 1. Initialize system and parse command-line arguments
 * 2. Open input/output files as specified
 * 3. Process input line by line, detecting equation boundaries
 * 4. For equations: invoke parser and generate troff output
 * 5. For regular text: pass through unchanged
 * 6. Handle cleanup and exit
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Robust error handling and recovery
 * - Clean separation between parsing and output generation
 * - Efficient memory and file descriptor management
 * - Comprehensive debug and error reporting
 */

#include "ne.h" /* NEQN type definitions and global declarations */
#include <stdio.h> /* Standard I/O operations */
#include <stdlib.h> /* Standard library functions */
#include <signal.h> /* Signal handling for broken pipes */
#include <unistd.h> /* UNIX standard functions */
#include <fcntl.h> /* File control operations */
#include <string.h> /* String manipulation functions */

/* SCCS version identifier */
static const char sccs_id[] = "@(#)ne4.c 1.3 25/05/29";

/* Use custom getc from parser to maintain parsing state */
#define getc ngetc

/* Signal definitions for portability */
#ifndef SIGPIPE
#define SIGPIPE 13 /* Broken pipe signal - troff has stopped reading */
#endif

/* Buffer size constants */
#define INPUT_BUFFER_SIZE 600 /**< Maximum size for input line buffer */
#define MAX_REGISTERS 100 /**< Maximum number of troff registers */
#define FIRST_REGISTER 11 /**< First available register for allocation */

/* Error severity levels */
#define FATAL 1 /**< Fatal error level */
#define WARNING 0 /**< Warning level */

/* Global configuration variables */
int gsize = 10; /**< Default point size for equations */
int gfont = 'R'; /**< Default font (Roman) for equations */
char in[INPUT_BUFFER_SIZE]; /**< Input line buffer */
int noeqn = 0; /**< Flag: suppress equation output if non-zero */

/* External variables defined in other modules */
extern int first; /**< First equation flag */
extern int lefteq, righteq; /**< Inline equation delimiter characters */
extern int eqline, linect; /**< Line number tracking */
extern int eqnreg, eqnht; /**< Equation register and height */
extern int lastchar; /**< Last character read */
extern int used[]; /**< Register allocation tracking array */
extern int eht[], ebase[]; /**< Object height and baseline arrays */
extern int ct, ps, ft; /**< Global typesetting state */
extern int fout, fin; /**< Output and input file descriptors */
extern int svargc; /**< Saved argument count */
extern char **svargv; /**< Saved argument vector */
extern int ptr, ifile; /**< Parser state variables */
extern int dbg; /**< Debug flag */

/* External function declarations */
extern int yyparse(void); /* Yacc-generated parser entry point */
extern int ngetc(void); /* Custom character input function */
extern int numb(char *str); /* Parse number from string */
extern void error(int level, char *fmt, char *arg); /* Error reporting */
extern int VERT(int n); /* Convert to vertical units */

/* Local function prototypes for C90 compliance */
static void signal_handler(int sig);
static int validate_arguments(int argc, char *argv[]);
static int process_equation_block(void);
static int process_inline_equation(void);
static void cleanup_and_exit(int status);

/* Function prototypes for external interface */
int main(int argc, char *argv[]);
int ne_getline(char *s);
int inline_equation(void);
int putout(int p1);
int max(int i, int j);
int oalloc(void);
int ofree(int n);
int setps(int p);
int nrwid(int n1, int p, int n2);
int setfile(int argc, char *argv[]);
int init(void);
int flush(int fd);

/**
 * @brief Signal handler for broken pipe conditions.
 *
 * Handles SIGPIPE signals that occur when the output pipe is broken,
 * typically when troff terminates before neqn finishes processing.
 * This provides clean termination rather than abnormal program abort.
 *
 * @param sig Signal number (should be SIGPIPE)
 */
static void signal_handler(int sig) {
    if (sig == SIGPIPE) {
        if (dbg) {
            fprintf(stderr, "neqn: received SIGPIPE, troff terminated\n");
        }
        cleanup_and_exit(0); /* Clean exit when pipe is broken */
    }
    exit(1); /* For other unexpected signals */
}

/**
 * @brief Validate command-line arguments for correctness.
 *
 * Performs basic validation of command-line arguments to ensure
 * they are reasonable and won't cause buffer overflows or other
 * security issues.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, non-zero on validation failure
 */
static int validate_arguments(int argc, char *argv[]) {
    int i;

    if (argc < 0 || argc > 1000) {
        error(FATAL, "invalid argument count: %d", "");
        return -1;
    }

    if (!argv) {
        error(FATAL, "null argument vector", "");
        return -1;
    }

    /* Validate each argument string */
    for (i = 0; i < argc; i++) {
        if (!argv[i]) {
            error(FATAL, "null argument at position %d", "");
            return -1;
        }
        if (strlen(argv[i]) > 1000) {
            error(FATAL, "argument too long at position %d", "");
            return -1;
        }
    }

    return 0;
}

/**
 * @brief Process a block equation (.EQ/.EN delimited).
 *
 * Handles the processing of equations delimited by .EQ and .EN
 * troff macros. This includes parser invocation, register management,
 * and output generation.
 *
 * @return 0 on success, non-zero on error
 */
static int process_equation_block(void) {
    int i;

    /* Clear register allocation table */
    for (i = FIRST_REGISTER; i < MAX_REGISTERS; i++) {
        used[i] = 0;
    }

    /* Output the .EQ line and save current troff state */
    printf("%s", in);
    printf(".nr 99 \\n(.s\n.nr 98 \\n(.f\n");

    /* Initialize parser state and invoke parser */
    init();
    yyparse();

    /* Generate output if equation was successfully parsed */
    if (eqnreg > 0) {
        printf(".nr %d \\w'\\*(%d'\n", eqnreg, eqnreg);
        printf(".if %d>\\n(.v .ne %du\n", eqnht, eqnht);
        printf(".rn %d 10\n", eqnreg);

        if (!noeqn) {
            printf("\\*(10\n");
        }
    }

    /* Restore troff state and output .EN */
    printf(".ps \\n(99\n.ft \\n(98\n");
    printf(".EN");

    return 0;
}

/**
 * @brief Process an inline equation.
 *
 * Handles inline equations that are delimited by user-specified
 * characters rather than .EQ/.EN macros. Multiple consecutive
 * inline equations are concatenated.
 *
 * @return 0 on success, non-zero on error
 */
static int process_inline_equation(void) {
    return inline_equation(); /* Delegate to existing function */
}

/**
 * @brief Perform cleanup operations and exit.
 *
 * Ensures all output is flushed and resources are properly
 * released before program termination.
 *
 * @param status Exit status code
 */
static void cleanup_and_exit(int status) {
    flush(fout);
    flush(fout); /* Double flush for reliability */
    exit(status);
}

/**
 * @brief Main program driver for NEQN equation typesetting.
 *
 * Coordinates the overall program execution, including initialization,
 * command-line processing, input file handling, and equation processing.
 * The main loop reads input line by line and dispatches to appropriate
 * handlers based on content type.
 *
 * Program Flow:
 * 1. Initialize signal handling and global state
 * 2. Parse command-line arguments and open files
 * 3. Enter main processing loop:
 *    - Read input lines one at a time
 *    - Detect equation boundaries (.EQ or inline delimiters)
 *    - Process equations through parser and generate output
 *    - Pass regular text through unchanged
 * 4. Handle end-of-file and cleanup
 *
 * Signal Handling:
 * - SIGPIPE: Clean termination when troff stops reading
 *
 * Error Handling:
 * - Invalid arguments: Fatal error with message
 * - File open failures: Fatal error with filename
 * - Parser errors: Reported but processing continues
 *
 * @param argc Command-line argument count
 * @param argv Command-line argument vector
 * @return 0 on successful completion, 1 on fatal error
 */
int main(int argc, char *argv[]) {
    int i, type;

    /* Validate command-line arguments */
    if (validate_arguments(argc, argv) != 0) {
        return 1;
    }

    /* Initialize global state */
    first = 0;
    lefteq = righteq = '\0';

    /* Set up signal handling for broken pipes */
    signal(SIGPIPE, signal_handler);

    /* Initialize optional modules */
    neqn_module_init();

    /* Process command-line arguments and open files */
    setfile(argc, argv);

    /* Main processing loop - read and process input line by line */
    while ((type = ne_getline(in)) != '\0') {
        eqline = linect;

        /* Check for block equation start (.EQ) */
        if (in[0] == '.' && in[1] == 'E' && in[2] == 'Q') {
            process_equation_block();

            /* Handle continuation after .EN */
            if (lastchar == '\0') {
                putchar('\n');
                break;
            }

            if (putchar(lastchar) != '\n') {
                while (putchar(ngetc()) != '\n') {
                    /* Continue reading to end of line */
                }
            }

            flush(fout);
            flush(fout);

        } else if (type == lefteq) {
            /* Process inline equation */
            process_inline_equation();

        } else {
            /* Regular text - pass through unchanged */
            printf("%s", in);
        }
    }

    /* Final cleanup and exit */
    cleanup_and_exit(0);
    return 0; /* Never reached, but satisfies compiler */
}

/**
 * @brief Read a line from input into the supplied buffer.
 *
 * Reads characters from the input stream until a newline, null character,
 * or left equation delimiter is encountered. The function handles both
 * regular text lines and the detection of inline equation boundaries.
 *
 * Input Termination Conditions:
 * - '\n': Normal end of line
 * - '\0': End of file or null character
 * - lefteq: Start of inline equation (if configured)
 *
 * Buffer Management:
 * - Input is read into the provided buffer
 * - Buffer is null-terminated for safe string operations
 * - Left equation delimiter is not included in the buffer
 *
 * @param s Pointer to buffer for storing the input line
 * @return Character that terminated the input (newline, null, or lefteq)
 */
int ne_getline(char *s) {
    int c;
    char *start = s; /* Remember start position for overflow checking */

    /* Input validation */
    if (!s) {
        error(FATAL, "ne_getline: null buffer pointer", "");
        return '\0';
    }

    /* Read characters until termination condition */
    while ((c = ngetc()) != '\n' && c != '\0' && c != lefteq) {
        /* Check for buffer overflow */
        if (s - start >= INPUT_BUFFER_SIZE - 1) {
            error(FATAL, "input line too long (>%d characters)", "");
            break;
        }
        *s++ = c;
    }

    /* Handle left equation delimiter */
    if (c == lefteq) {
        --s; /* Don't include delimiter in buffer */
    }

    /* Null-terminate the string */
    *s = '\0';

    if (dbg) {
        fprintf(stderr, "ne_getline: read line, terminated by %c\n",
                c == '\n' ? 'n' : c == '\0' ? '0'
                                            : c);
    }

    return c;
}

/**
 * @brief Process inline equations within text.
 *
 * Handles equations that are embedded within regular text lines using
 * user-specified delimiter characters. Multiple consecutive inline
 * equations are concatenated into a single troff string for output.
 *
 * Processing Flow:
 * 1. Save current troff state (point size and font)
 * 2. Allocate a troff string register for accumulating output
 * 3. Process equations until a non-equation line is encountered
 * 4. For each equation: parse and append result to string register
 * 5. Output the accumulated string and restore troff state
 *
 * Troff State Management:
 * - Point size and font are saved and restored
 * - String registers are allocated and freed properly
 * - Output is generated as a single troff string reference
 *
 * @return 0 on success, non-zero on error
 */
int inline_equation(void) {
    int ds, t;

    /* Save current troff state */
    printf(".nr 99 \\n(.s\n.nr 98 \\n(.f\n");

    /* Allocate string register for accumulating output */
    ds = oalloc();
    if (ds <= 0) {
        error(FATAL, "failed to allocate string register for inline equation", "");
        return -1;
    }

    /* Initialize the accumulator string */
    printf(".ds %d \"\n", ds);

    /* Process consecutive inline equations */
    do {
        /* Add current line content to accumulator */
        printf(".as %d \"%s\n", ds, in);

        /* Initialize parser and process equation */
        init();
        yyparse();

        /* Append equation result if parsing succeeded */
        if (eqnreg > 0) {
            printf(".as %d \\*(%d\n", ds, eqnreg);
            ofree(eqnreg);
        }

        /* Restore troff state after equation */
        printf(".ps \\n(99\n.ft \\n(98\n");

    } while ((t = ne_getline(in)) == lefteq);

    /* Append final line content and output result */
    printf(".as %d \"%s", ds, in);
    printf(".ps \\n(99\n.ft \\n(98\n");
    printf("\\*(%d\n", ds);

    /* Clean up allocated register */
    ofree(ds);

    /* Flush output */
    flush(fout);
    flush(fout);

    return 0;
}

/**
 * @brief Generate final troff output for a completed equation.
 *
 * Converts the parsed equation into final troff commands with proper
 * vertical spacing, font selection, and size adjustments. This function
 * handles the complex positioning calculations needed for mathematical
 * typesetting integration with regular text.
 *
 * Output Generation:
 * - Vertical spacing adjustments for proper baseline alignment
 * - Font and size changes for mathematical content
 * - Restoration of original troff state after equation
 *
 * Spacing Calculations:
 * - Before: Space above baseline to equation top
 * - After: Space below baseline to equation bottom
 * - Adjustments ensure proper line spacing integration
 *
 * @param p1 Handle to the equation object for output
 * @return 0 on success
 */
int putout(int p1) {
    int before, after;

    /* Validate input parameter */
    if (p1 <= 0 || p1 >= MAX_REGISTERS) {
        error(WARNING, "putout: invalid register %d", "");
        return -1;
    }

    /* Calculate equation height for troff */
    eqnht = eht[p1];

    /* Begin troff string definition with baseline marker */
    printf(".ds %d \\x'0'", p1);

    /* Calculate space needed above baseline */
    before = eht[p1] - ebase[p1] - VERT(3);
    if (before > 0) {
        printf("\\x'0-%du'", before);
    }

    /* Generate equation content with font and size */
    printf("\\f%c\\s%d\\*(%d\\s\\n(99\\f\\n(98", gfont, gsize, p1);

    /* Calculate space needed below baseline */
    after = ebase[p1] - VERT(1);
    if (after > 0) {
        printf("\\x'%du'", after);
    }

    putchar('\n');

    /* Store register number for later reference */
    eqnreg = p1;

    /* Generate debug output if enabled */
    if (dbg) {
        printf(".\tanswer <- S%d: h=%d, b=%d, before=%d, after=%d\n",
               p1, eht[p1], ebase[p1], before, after);
    }

    return 0;
}

/**
 * @brief Return the maximum of two integers.
 *
 * Simple utility function for calculating maximum values, commonly
 * used in typesetting calculations for determining spacing and
 * positioning requirements.
 *
 * @param i First integer value
 * @param j Second integer value
 * @return The larger of the two input values
 */
int max(int i, int j) {
    return (i > j) ? i : j;
}

/**
 * @brief Allocate a troff string register.
 *
 * Finds and allocates an unused troff string register from the
 * available pool. Registers are tracked in the global 'used' array
 * to prevent conflicts and double allocation.
 *
 * Register Management:
 * - Registers 11-99 are available for allocation
 * - Register 0-10 are reserved for system use
 * - used[] array tracks allocation status
 *
 * @return Register number on success, 0 on failure
 */
int oalloc(void) {
    int i;

    /* Search for available register */
    for (i = FIRST_REGISTER; i < MAX_REGISTERS; i++) {
        if (used[i] == 0) {
            used[i] = 1; /* Mark as allocated */

            if (dbg) {
                fprintf(stderr, "oalloc: allocated register %d\n", i);
            }

            return i;
        }
    }

    /* No registers available */
    error(FATAL, "no string registers available", "");
    return 0;
}

/**
 * @brief Free a previously allocated troff string register.
 *
 * Marks a register as available for reuse by clearing its allocation
 * flag in the used[] array. This enables efficient register recycling
 * during equation processing.
 *
 * @param n Register number to free
 * @return 0 on success
 */
int ofree(int n) {
    /* Validate register number */
    if (n < FIRST_REGISTER || n >= MAX_REGISTERS) {
        error(WARNING, "ofree: invalid register %d", "");
        return -1;
    }

    if (used[n] == 0) {
        error(WARNING, "ofree: register %d not allocated", "");
        return -1;
    }

    /* Mark register as available */
    used[n] = 0;

    if (dbg) {
        fprintf(stderr, "ofree: freed register %d\n", n);
    }

    return 0;
}

/**
 * @brief Set the current point size in troff.
 *
 * Generates a troff command to change the point size for subsequent
 * text output. This is used to implement size changes within
 * mathematical expressions.
 *
 * @param p Point size value
 * @return 0 on success
 */
int setps(int p) {
    /* Validate point size range */
    if (p < 4 || p > 72) {
        error(WARNING, "setps: unusual point size %d", "");
    }

    printf(".ps %d\n", p);
    return 0;
}

/**
 * @brief Set width register for an object.
 *
 * Generates troff commands to measure the width of a string register
 * and store the result in a numeric register. This is essential for
 * proper spacing and alignment calculations in mathematical typesetting.
 *
 * @param n1 Numeric register to store width
 * @param p Point size for measurement (unused in current implementation)
 * @param n2 String register to measure
 * @return 0 on success
 */
int nrwid(int n1, int p, int n2) {
    /* Validate register numbers */
    if (n1 < 0 || n1 >= MAX_REGISTERS || n2 < 0 || n2 >= MAX_REGISTERS) {
        error(WARNING, "nrwid: invalid register numbers %d, %d", "");
        return -1;
    }

    printf(".nr %d \\w'\\*(%d'\n", n1, n2);
    return 0;
}

/**
 * @brief Setup input and output files based on command-line arguments.
 *
 * Processes command-line arguments to configure program behavior and
 * open appropriate input/output files. Handles various options for
 * controlling equation processing and formatting.
 *
 * Command-Line Options:
 * - -d<chars>: Set inline equation delimiters
 * - -s<size>: Set default point size
 * - -f<font>: Set default font
 * - -e: Suppress equation output (noeqn mode)
 * - Other: Enable debug mode
 *
 * File Handling:
 * - No arguments: Read from stdin
 * - Filename argument: Open specified file
 * - Error handling for file open failures
 *
 * @param argc Command-line argument count
 * @param argv Command-line argument vector
 * @return 0 on success, exits on fatal error
 */
int setfile(int argc, char *argv[]) {
    /* Save argument information for error reporting */
    svargc = --argc;
    svargv = argv;

    /* Process command-line options */
    while (svargc > 0 && svargv[1][0] == '-') {
        switch (svargv[1][1]) {
        case 'd':
            /* Set inline equation delimiters */
            lefteq = svargv[1][2];
            righteq = svargv[1][3];
            if (dbg) {
                fprintf(stderr, "setfile: delimiters set to '%c' '%c'\n",
                        lefteq, righteq);
            }
            break;

        case 's':
            /* Set default point size */
            gsize = numb(&svargv[1][2]);
            if (gsize < 4 || gsize > 72) {
                error(WARNING, "unusual point size %d", "");
            }
            break;

        case 'f':
            /* Set default font */
            gfont = svargv[1][2];
            break;

        case 'e':
            /* Enable noeqn mode (suppress equation output) */
            noeqn++;
            break;

        default:
            /* Unknown option - enable debug mode */
            dbg = 1;
            if (dbg) {
                fprintf(stderr, "setfile: debug mode enabled\n");
            }
            break;
        }

        svargc--;
        svargv++;
    }

    /* Initialize parser state */
    ptr = 0;
    fout = dup(1); /* Duplicate stdout for output */
    ifile = 1; /* Current file index */
    linect = 1; /* Line counter */

    /* Setup input file */
    if (svargc <= 0) {
        /* No input file specified - use stdin */
        fin = dup(0);
    } else {
        /* Open specified input file */
        if ((fin = open(svargv[1], O_RDONLY)) < 0) {
            error(FATAL, "can't open file %s", svargv[1]);
        }
    }

    return 0;
}

/**
 * @brief Initialize global state for equation processing.
 *
 * Resets global variables to their initial state before processing
 * each equation. This ensures clean state between equations and
 * prevents interference between separate equation blocks.
 *
 * State Initialization:
 * - Clear object counter
 * - Set point size to global default
 * - Set font to global default  
 * - Increment equation counter
 *
 * @return 0 on success
 */
int init(void) {
    ct = 0; /* Reset object counter */
    ps = gsize; /* Set current point size */
    ft = gfont; /* Set current font */
    first++; /* Increment equation counter */

    if (dbg) {
        fprintf(stderr, "init: equation %d, ps=%d, ft=%c\n", first, ps, ft);
    }

    return 0;
}

/**
 * @brief Flush output for the specified file descriptor.
 *
 * Ensures that all buffered output is written to the specified file
 * descriptor. Handles both stderr (fd=2) and stdout (default) cases
 * with appropriate flush operations.
 *
 * @param fd File descriptor to flush
 * @return Result of fflush operation
 */
int flush(int fd) {
    if (fd == 2) {
        return fflush(stderr);
    }
    return fflush(stdout);
}
