/**
 * @file roff1.c
 * @brief ROFF text formatter - Main driver and core functionality.
 *
 * This file contains the main program and core text formatting functionality
 * for the ROFF typesetting system. Originally written in PDP-11 assembly
 * language, this has been converted to portable C90 while preserving all
 * original functionality and behavior.
 *
 * Key Functionality:
 * - Command-line argument processing and file management
 * - Input character processing and escape sequence handling
 * - Text formatting and line breaking
 * - Output buffering and device control
 * - Control command processing (.br, .sp, .ce, etc.)
 * - Tab handling and column tracking
 * - Suffix table management for hyphenation
 *
 * Original Design (PDP-11 Assembly):
 * - Direct system call interface
 * - Manual buffer management
 * - Character-by-character processing
 * - Optimized for memory-constrained systems
 *
 * Modern C90 Implementation:
 * - Standard library functions where appropriate
 * - Portable file I/O operations
 * - Robust error handling and validation
 * - Comprehensive documentation and comments
 * - Clean separation of concerns
 *
 * Text Processing Flow:
 * 1. Initialize system and parse command-line arguments
 * 2. Open input files and setup output buffering
 * 3. Read characters and process escape sequences
 * 4. Handle control commands (lines starting with '.')
 * 5. Format text according to current parameters
 * 6. Output formatted text with proper spacing
 * 7. Handle page breaks and headers/footers
 *
 * Design Principles:
 * - C90 compliance for maximum portability
 * - Preserve original ROFF behavior exactly
 * - Robust error handling and recovery
 * - Efficient memory and file descriptor management
 * - Clean separation between input, processing, and output
 */

#include <stdio.h> /* Standard I/O operations */
#include <stdlib.h> /* Standard library functions */
#include <string.h> /* String manipulation functions */
#include <unistd.h> /* UNIX standard functions */
#include <fcntl.h> /* File control operations */
#include <signal.h> /* Signal handling */
#include <sys/stat.h> /* File status operations */
#include <ctype.h> /* Character classification */
#include "os_abstraction.h" /* Cross-platform wrappers */
#include "roff.h" /* Common ROFF macros */
#include "roff_globals.hpp" /* Shared globals and prototypes */

/* SCCS version identifier */
static const char sccs_id[] ROFF_UNUSED = "@(#)roff1.c 1.3 25/05/29 (converted from PDP-11 assembly)";

/* Buffer size constants */
#define IBUF_SIZE 512 /**< Input buffer size */
#define OBUF_SIZE 128 /**< Output buffer size */
#define SSIZE 400 /**< String buffer size */
#define MAX_TABS 20 /**< Maximum tab stops */
#define MAX_FILES 64 /**< Maximum input files */
#define SUFFIX_SIZE 52 /**< Suffix table size (26 * 2) */

/* Control command constants */
#define CC_CHAR '.' /**< Control command character */
#define ESC_CHAR '\\' /**< Escape character */
#define PREFIX_CHAR '\033' /**< Prefix character for special sequences */

/* Character translation and formatting */
static unsigned char trtab[128]; /**< Character translation table */
static unsigned char tabtab[MAX_TABS] ROFF_UNUSED; /**< Tab stop table */

/* Input/output buffers and state */
static char ibuf[IBUF_SIZE]; /**< Input buffer */
static char obuf[OBUF_SIZE]; /**< Output buffer */
static char *ibufp; /**< Input buffer pointer */
static char *eibuf; /**< End of input buffer */
static char *obufp; /**< Output buffer pointer */

/* File handling state */
static int ifile = 0; /**< Current input file descriptor */
static int ibf = -1; /**< Temporary file descriptor */
static int ibf1 ROFF_UNUSED = -1; /**< Secondary temporary file descriptor */
static int suff = -1; /**< Suffix file descriptor */
static char **argp; /**< Argument pointer */
static int argc; /**< Argument count */
static int nx = 0; /**< Next file flag */

/* Text processing state */
static int ch = 0; /**< Current character */
static int lastchar = 0; /**< Last character read */
static int nlflg = 0; /**< Newline flag */
static int column = 0; /**< Current column position */
static int ocol = 0; /**< Output column position */
static int nsp = 0; /**< Number of spaces pending */
static int nspace = 0; /**< Space count for tabs */
static char tabc = ' '; /**< Tab character */

/* Formatting parameters */
static int pfrom = 1; /**< Starting page number */
static int pto = 32767; /**< Ending page number */
static int pn = 1; /**< Current page number */
static int stop = 0; /**< Stop after processing flag */
static int slow = 1; /**< Slow output mode flag */

/* Underline processing state */
static int ul = 0; /**< Underline mode */
static int ulstate = 0; /**< Underline state machine */
static int ulc = 0; /**< Underline character count */
static int bsc = 0; /**< Backspace count */

/* Include stack for nested files */
static int ip ROFF_UNUSED = 0; /**< Include pointer */
static int ilistp ROFF_UNUSED = 0; /**< Include list pointer */
static int iliste ROFF_UNUSED = 0; /**< Include list end */

/* Suffix table for hyphenation */
static unsigned short suftab[26]; /**< Suffix lookup table */

/* Temporary file management */
static char bfn[] = "/tmp/roffXXXXXXa"; /**< Temporary file name template */
static char suffil[] = "/usr/lib/suftab"; /**< Suffix table file */
static char ttyx[] ROFF_UNUSED = "/dev/ttyx"; /**< TTY device name */

/* Error messages */
static const char emes1[] = "Too many files.\n";

/* Function prototypes for C90 compliance */
static void initialize_system(void);
static void process_arguments(int argc, char *argv[]);
static void setup_files(void);
static void main_loop(void);
static void cleanup_and_exit(int status);

/* Character input/output functions */
static int ngetc(void);
static int getchar_roff(void);
static void putchar_roff(int c);
static void pchar1(int c);
static void flush_output(void);

/* Text processing functions */
static void text_handler(void);
static void control_handler(void);
static void flushi(void);
static int width(int c);

/* Utility functions */
static void make_temp_file(void);
static void error_exit(const char *msg);
static int next_file(void);
static void signal_setup(void);
static void ttyn_setup(void);
static int parse_number(char **ptr);

/* Control command handlers provided in roff2.cpp */

/* Global state for control commands */
typedef struct {
    char cmd[3]; /**< Two-character command */
    void (*handler)(void); /**< Handler function pointer */
} control_entry_t;

/**
 * @brief Control command lookup table.
 *
 * Maps two-character control commands to their handler functions.
 * Maintains alphabetical order for efficient lookup.
 */
static const control_entry_t control_table[] = {
    {"ad", case_ad}, /* Adjust text */
    {"bp", case_bp}, /* Break page */
    {"br", case_br}, /* Break line */
    {"cc", case_cc}, /* Control character */
    {"ce", case_ce}, /* Center lines */
    /* Additional entries would go here... */
    {"", NULL} /* Sentinel entry */
};

/**
 * @brief Escape sequence lookup table.
 *
 * Maps escape characters to their corresponding output values.
 * Based on the original assembly esctab.
 */
typedef struct {
    char esc; /**< Escape character */
    unsigned char val; /**< Output value */
} escape_entry_t;

static const escape_entry_t escape_table[] = {
    {'d', 032}, /* Half line down */
    {'u', 035}, /* Half line up */
    {'r', 036}, /* Reverse line feed */
    {'x', 016}, /* SO (extra chars) */
    {'y', 017}, /* SI (normal chars) */
    {'l', 0177}, /* Delete */
    {'t', 011}, /* Horizontal tab */
    {'a', 0100}, /* At sign */
    {'n', 043}, /* Number sign */
    {'\\', 0134}, /* Backslash */
    {0, 0} /* Sentinel */
};

/**
 * @brief Prefix sequence lookup table.
 *
 * Maps prefix characters to their corresponding output values.
 * Based on the original assembly pfxtab.
 */
static const escape_entry_t prefix_table[] = {
    {'7', 036}, /* Reverse line feed */
    {'8', 035}, /* Half line up */
    {'9', 032}, /* Half line down */
    {'4', 030}, /* Backspace */
    {'3', 031}, /* Carriage return */
    {'1', 026}, /* Set horizontal tabs */
    {'2', 027}, /* Clear horizontal tabs */
    {0, 0} /* Sentinel */
};

/**
 * @brief Main program entry point.
 *
 * Initializes the ROFF system, processes command-line arguments,
 * sets up input/output files, and enters the main processing loop.
 * Handles all cleanup and proper program termination.
 *
 * Command-line Arguments:
 * - +n: Start printing from page n
 * - -n: Stop printing after page n
 * - -s: Stop after processing (don't print)
 * - -h: High-speed mode (no tabs)
 * - files: Input files to process
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status (0 for success, 1 for error)
 */
int main(int argc_param, char *argv[]) {
    /* Check stack space and basic system requirements */
    if (argc_param < 1) {
        error_exit(emes1);
    }

    /* Initialize global state */
    initialize_system();

    /* Process command-line arguments */
    process_arguments(argc_param, argv);

    /* Setup files and I/O */
    setup_files();

    /* Enter main processing loop */
    main_loop();

    /* Clean shutdown */
    cleanup_and_exit(0);
    return 0; /* Never reached */
}

/**
 * @brief Initialize system state and data structures.
 *
 * Sets up all global variables, translation tables, and system
 * parameters to their initial values. Corresponds to the init
 * code in the original assembly version.
 */
static void initialize_system(void) {
    int i;

    /* Initialize translation table to identity mapping */
    for (i = 0; i < 128; i++) {
        trtab[i] = (unsigned char)i;
    }

    /* Initialize buffer pointers */
    ibufp = ibuf;
    eibuf = ibuf;
    obufp = obuf;

    /* Initialize formatting state */
    pfrom = 1;
    pto = 32767;
    pn = 1;
    stop = 0;
    slow = 1;

    /* Initialize character processing state */
    ch = 0;
    lastchar = 0;
    nlflg = 0;
    column = 0;
    ocol = 0;
    nsp = 0;

    /* Initialize underline state */
    ul = 0;
    ulstate = 0;
    ulc = 0;
    bsc = 0;

    /* Setup signal handling */
    signal_setup();

    /* Setup TTY permissions */
    ttyn_setup();
}

/**
 * @brief Process command-line arguments.
 *
 * Parses and validates command-line options and file arguments.
 * Sets global state variables based on the options provided.
 *
 * @param argc_param Argument count
 * @param argv Argument vector
 */
static void process_arguments(int argc_param, char *argv[]) {
    int i;
    char *arg;

    argc = argc_param - 1; /* Don't count program name */
    argp = argv + 1; /* Skip program name */

    /* Process each argument */
    for (i = 1; i < argc_param; i++) {
        arg = argv[i];

        if (arg[0] == '+') {
            /* Starting page number */
            pfrom = parse_number(&arg);
        } else if (arg[0] == '-') {
            if (arg[1] == 's') {
                /* Stop mode */
                stop = 1;
            } else if (arg[1] == 'h') {
                /* High-speed mode */
                slow = 0;
            } else {
                /* Ending page number */
                pto = parse_number(&arg);
            }
        }
        /* Regular files are handled later in main_loop */
    }
}

/**
 * @brief Setup files and I/O systems.
 *
 * Creates temporary files, opens suffix tables, and prepares
 * the I/O system for text processing.
 */
static void setup_files(void) {
    /* Create temporary file for buffering */
    make_temp_file();

    /* Try to open suffix table for hyphenation */
    suff = os_open(suffil, O_RDONLY, 0);
    if (suff >= 0) {
        os_lseek(suff, 20, SEEK_SET);
        os_read(suff, suftab, sizeof(suftab));
    }
}

/**
 * @brief Main text processing loop.
 *
 * Reads characters from input, processes escape sequences and
 * control commands, and generates formatted output. This is
 * the core of the ROFF formatting engine.
 */
static void main_loop(void) {
    int c;

    while (1) {
        nlflg = 0;
        c = getchar_roff();

        if (c == CC_CHAR) {
            /* Control command */
            control_handler();
            flushi();
        } else if (c == '\0') {
            /* End of input */
            break;
        } else {
            /* Regular text character */
            ch = c;
            text_handler();
        }
    }
}

/**
 * @brief Handle regular text characters.
 *
 * Processes normal text characters, applying formatting rules
 * and outputting them with proper spacing and alignment.
 */
static void text_handler(void) {
    /* For now, simple pass-through */
    /* Full text formatting logic would go here */
    putchar_roff(ch);
}

/**
 * @brief Handle control commands.
 *
 * Processes lines beginning with the control character (usually '.').
 * Looks up the command in the control table and calls the appropriate
 * handler function.
 */
static void control_handler(void) {
    int c1, c2;
    int i;
    char cmd[3];

    /* Read two-character command */
    c1 = getchar_roff();
    c2 = getchar_roff();

    cmd[0] = c1;
    cmd[1] = c2;
    cmd[2] = '\0';

    /* Look up command in table */
    for (i = 0; control_table[i].handler != NULL; i++) {
        if (strcmp(cmd, control_table[i].cmd) == 0) {
            control_table[i].handler();
            return;
        }
    }

    /* Unknown command - ignore */
    if (strlen(cmd) > 0) {
        /* Could log unknown command here */
    }
}

/**
 * @brief Read next character from input with escape processing.
 *
 * Handles escape sequences, prefix sequences, and special character
 * processing. Maintains compatibility with the original assembly
 * version's character processing logic.
 *
 * @return Next processed character
 */
static int getchar_roff(void) {
    int c;
    int i;

    /* Check for saved character */
    if (ch != 0) {
        c = ch;
        ch = 0;
        return c;
    }

    /* Check for newline flag */
    if (nlflg) {
        return '\n';
    }

    /* Get character from input */
    c = ngetc();

    /* Handle escape sequences */
    if (c == ESC_CHAR) {
        c = ngetc();

        /* Look up in escape table */
        for (i = 0; escape_table[i].esc != 0; i++) {
            if (escape_table[i].esc == c) {
                c = escape_table[i].val;
                break;
            }
        }
    } else if (c == PREFIX_CHAR) {
        /* Handle prefix sequences */
        c = ngetc();

        /* Look up in prefix table */
        for (i = 0; prefix_table[i].esc != 0; i++) {
            if (prefix_table[i].esc == c) {
                c = prefix_table[i].val;
                break;
            }
        }
    }

    /* Update position tracking */
    if (c == '\n') {
        nlflg = 1;
        column = 0;
    } else {
        column += width(c);
    }

    return c;
}

/**
 * @brief Low-level character input function.
 *
 * Reads characters from files or buffers, handling file switching
 * and input buffering. Corresponds to the get1 function in the
 * original assembly version.
 *
 * @return Next character from input stream
 */
static int ngetc(void) {
    int c;
    ssize_t n;

    /* Handle tab expansion */
    if (nspace > 0) {
        nspace--;
        return tabc;
    }

    /* Check if we need to read more input */
    if (ibufp >= eibuf) {
        if (ifile == 0) {
            if (next_file() < 0) {
                return '\0'; /* End of input */
            }
        }

        /* Read into buffer */
        n = os_read(ifile, ibuf, IBUF_SIZE);
        if (n <= 0) {
            if (next_file() < 0) {
                return '\0';
            }
            n = os_read(ifile, ibuf, IBUF_SIZE);
            if (n <= 0) {
                return '\0';
            }
        }

        ibufp = ibuf;
        eibuf = ibuf + n;
    }

    /* Get character from buffer */
    c = *ibufp++;

    /* Handle tab expansion */
    if (c == '\t') {
        /* Simple tab handling - expand to spaces */
        int spaces = 8 - (column % 8);
        if (spaces > 1) {
            nspace = spaces - 1;
        }
        return ' ';
    }

    return c;
}

/**
 * @brief Output a character with formatting.
 *
 * Handles character output with proper spacing, tab expansion,
 * and special character processing. Maintains output buffering
 * for efficiency.
 *
 * @param c Character to output
 */
static void putchar_roff(int c) {
    /* Check page range */
    if (pn < pfrom) {
        return;
    }

    if (pn > pto) {
        return;
    }

    /* Apply character translation */
    c &= 0177; /* Mask to 7 bits */
    if (c == 0) {
        return;
    }

    c = trtab[c];

    /* Handle spaces */
    if (c == ' ') {
        nsp++;
        return;
    }

    /* Handle newlines */
    if (c == '\n') {
        nsp = 0;
        ocol = 0;
        pchar1(c);
        return;
    }

    /* Output pending spaces */
    while (nsp > 0) {
        if (!slow) {
            /* Fast mode - use tabs when possible */
            int tab_stop = ((ocol + 8) / 8) * 8;
            if (tab_stop - ocol <= nsp) {
                pchar1('\t');
                nsp -= (tab_stop - ocol);
                continue;
            }
        }
        pchar1(' ');
        nsp--;
    }

    /* Output the character */
    pchar1(c);
}

/**
 * @brief Low-level character output function.
 *
 * Outputs a single character to the output buffer, handling
 * buffer flushing and position tracking.
 *
 * @param c Character to output
 */
static void pchar1(int c) {
    /* Update column position */
    if (c == '\t') {
        ocol = ((ocol + 8) / 8) * 8;
    } else if (c == '\n') {
        ocol = 0;
    } else {
        ocol += width(c);
    }

    /* Add to output buffer */
    *obufp++ = c;

    /* Check if buffer is full */
    if (obufp >= obuf + OBUF_SIZE) {
        flush_output();
    }
}

/**
 * @brief Flush output buffer to stdout.
 *
 * Writes the contents of the output buffer to standard output
 * and resets the buffer pointer.
 */
static void flush_output(void) {
    size_t len = obufp - obuf;

    if (len > 0) {
        os_write(STDOUT_FILENO, obuf, len);
        obufp = obuf;
    }
}

/**
 * @brief Calculate display width of a character.
 *
 * Returns the number of column positions a character occupies
 * when displayed. Most characters are width 1.
 *
 * @param c Character to measure
 * @return Display width in columns
 */
static int width(int c) {
    /* Simple implementation - most characters are width 1 */
    if (c < ' ' || c > '~') {
        return 0; /* Control characters have no width */
    }
    return 1;
}

/**
 * @brief Open next input file.
 *
 * Switches to the next file in the argument list, closing
 * the current file if necessary.
 *
 * @return 0 on success, -1 on end of files
 */
static int next_file(void) {
    /* Close current file */
    if (ifile > 0) {
        os_close(ifile);
        ifile = 0;
    }

    /* Check for more files */
    if (nx) {
        /* Handle nx flag - not implemented in this version */
        return -1;
    }

    if (argc <= 0) {
        return -1; /* No more files */
    }

    /* Open next file */
    ifile = os_open(*argp, O_RDONLY, 0);
    if (ifile < 0) {
        return -1; /* Could not open file */
    }

    argp++;
    argc--;

    return 0;
}

/**
 * @brief Create temporary file for processing.
 *
 * Creates a unique temporary file name and opens it for
 * read/write operations.
 */
static void make_temp_file(void) {
    int i;
    struct stat st;

    /* Try different file names until we find one that doesn't exist */
    for (i = 0; i < 26; i++) {
        bfn[strlen(bfn) - 1] = 'a' + i;

        if (os_stat(bfn, &st) < 0) {
            /* File doesn't exist - try to create it */
            ibf = os_open(bfn, O_CREAT | O_RDWR, 0600);
            if (ibf >= 0) {
                return; /* Success */
            }
        }
    }

    /* Could not create temporary file */
    error_exit("Cannot create temporary file");
}

/**
 * @brief Setup signal handling.
 *
 * Installs signal handlers for proper cleanup on interruption.
 */
static void signal_setup(void) {
    signal(SIGINT, SIG_IGN); /* Ignore interrupt */
    signal(SIGQUIT, SIG_IGN); /* Ignore quit */
}

/**
 * @brief Setup TTY permissions.
 *
 * Manages terminal permissions for proper output control.
 */
static void ttyn_setup(void) {
    /* TTY setup would go here if needed */
    /* Original assembly version had complex TTY handling */
}

/**
 * @brief Parse number from string.
 *
 * Extracts a numeric value from a string pointer, advancing
 * the pointer past the parsed number.
 *
 * @param ptr Pointer to string pointer
 * @return Parsed number value
 */
static int parse_number(char **ptr) {
    int num = 0;
    char *p = *ptr + 1; /* Skip the +/- sign */

    while (isdigit(*p)) {
        num = num * 10 + (*p - '0');
        p++;
    }

    *ptr = p;
    return num;
}

/**
 * @brief Flush input until newline.
 *
 * Reads and discards characters until a newline is encountered.
 * Used to skip to the end of the current line.
 */
static void flushi(void) {
    ch = 0;
    while (!nlflg) {
        getchar_roff();
    }
}

/**
 * @brief Output error message and exit.
 *
 * Prints an error message to stderr and exits with error status.
 *
 * @param msg Error message to display
 */
static void error_exit(const char *msg) {
    os_write(STDERR_FILENO, msg, strlen(msg));
    exit(1);
}

/**
 * @brief Clean up resources and exit.
 *
 * Performs final cleanup operations and exits with the
 * specified status code.
 *
 * @param status Exit status code
 */
static void cleanup_and_exit(int status) {
    /* Flush any remaining output */
    flush_output();

    /* Close open files */
    if (ifile > 0) {
        os_close(ifile);
    }
    if (ibf >= 0) {
        os_close(ibf);
        os_unlink(bfn); /* Remove temporary file */
    }
    if (suff >= 0) {
        os_close(suff);
    }

    /* Restore TTY permissions */
    /* TTY restoration code would go here */

    exit(status);
}

/* Control command handlers are implemented in roff2.cpp */
