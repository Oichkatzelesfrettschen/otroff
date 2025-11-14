/* C17 - no scaffold needed */
/*
 * pti.c - Portable Typesetter Interpreter
 *
 * A portable translation of the historic phototypesetter interpreter
 * originally written in assembly (pti.s). This interpreter processes
 * typesetter control codes and outputs human-readable descriptions
 * of typesetting operations.
 *
 * The program reads a stream of control bytes and interprets them as:
 * - Character codes (printable characters)
 * - Size control commands
 * - Lead (vertical spacing) commands
 * - Case conversion commands
 * - Escape sequence commands
 * - Rail and magazine positioning commands
 *
 * Entry point corresponds to the 'start' label in the original assembly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Global state variables (originally in .bss section)
 * These maintain the current state of the typesetter
 */

/* leadtot: Accumulated lead (vertical spacing) distance */
static int leadtot = 0;

/* leadmode: Lead direction flag (0=forward, 1=backward) */
static int leadmode = 0;

/* caseflag: Case conversion offset (0=lower, 0100=upper) */
static int caseflag = 0;

/* esc: Pending escape distance to be output */
static int esc = 0;

/* esct: Total accumulated escape distance */
static int esct = 0;

/* escd: Reverse escape mode flag (0=forward, 1=backward) */
static int escd = 0;

/* pts: Current point size (default 12pt) */
static int pts = 12;

/*
 * stab: Type size table mapping point sizes to internal codes
 * Format: {point_size, internal_code}
 * Terminated by {0, 0} entry
 */
static const unsigned char stab[][2] = {
    {6, 10}, {7, 0}, {8, 1}, {9, 7}, {10, 2}, {11, 3}, {12, 4}, {14, 5}, {16, 11}, {18, 6}, {20, 12}, {22, 13}, {24, 14}, {28, 15}, {36, 16}, {0, 0}};

/*
 * Width table entry structure
 * w: character width (in 1/6 point units)
 * c: internal character code
 */
struct wentry {
    unsigned char w; /* Character width */
    unsigned char c; /* Character code */
};

/*
 * wtab: Width/code table from original pti.s
 * Maps ASCII positions to character widths and internal codes
 * Used for character width calculations and code mapping
 */
static const struct wentry wtab[] = {
    /* Space and punctuation */
    {15, 0},
    {9, 145},
    {0, 0},
    {27, 153},
    {18, 155},
    {27, 53},
    {28, 50},
    {9, 150},
    {9, 132},
    {9, 133},
    {16, 122},
    {27, 143},
    {9, 47},
    {27, 123},
    {9, 44},
    {12, 43},

    /* Digits 0-9 */
    {18, 110},
    {18, 111},
    {18, 112},
    {18, 113},
    {18, 114},
    {18, 115},
    {18, 116},
    {18, 117},
    {18, 120},
    {18, 121},

    /* More punctuation */
    {9, 142},
    {9, 23},
    {0, 0},
    {27, 140},
    {0, 0},
    {16, 147},
    {36, 131},

    /* Uppercase letters A-Z */
    {29, 103},
    {23, 75},
    {26, 70},
    {29, 74},
    {25, 72},
    {24, 101},
    {30, 65},
    {29, 60},
    {13, 66},
    {16, 105},
    {29, 107},
    {24, 63},
    {35, 62},
    {30, 61},
    {27, 57},
    {22, 67},
    {27, 55},
    {28, 64},
    {18, 76},
    {24, 56},
    {29, 106},
    {28, 71},
    {36, 104},
    {28, 102},
    {28, 77},
    {24, 73},

    /* Brackets and more punctuation */
    {9, 134},
    {0, 0},
    {9, 135},
    {0, 0},
    {0, 0},
    {0, 0},

    /* Lowercase letters a-z */
    {17, 25},
    {19, 12},
    {16, 27},
    {20, 11},
    {17, 31},
    {13, 14},
    {17, 45},
    {21, 1},
    {10, 6},
    {10, 15},
    {20, 17},
    {10, 5},
    {32, 4},
    {21, 3},
    {19, 33},
    {20, 21},
    {19, 42},
    {14, 35},
    {15, 10},
    {12, 2},
    {20, 16},
    {20, 37},
    {27, 41},
    {20, 13},
    {19, 51},
    {16, 7},

    /* Final entries */
    {0, 0},
    {5, 151},
    {0, 0},
    {0, 0},
    {0, 0}};

/* Function prototypes */
static void prn(int c);
static void str(const char *s);
static void numb(long n, int base);

/*
 * prn: Print control byte in octal format followed by a space
 * Used for debugging output to show the raw control codes
 *
 * Args: c - control byte to print
 */
static void prn(int c) {
    printf("%o ", ((unsigned char)c));
}

/*
 * str: Write literal string to stdout
 * Simple wrapper for fputs to maintain consistency with original
 *
 * Args: s - null-terminated string to output
 */
static void str(const char *s) {
    if (s != NULL) {
        fputs(s, stdout);
    }
}

/*
 * numb: Output number in specified base
 * Converts number to string representation and outputs it
 *
 * Args: n    - number to convert and output
 *       base - numeric base for conversion (typically 8, 10, or 16)
 */
static void numb(long n, int base) {
    char buf[32]; /* Buffer for converted digits */
    char *p; /* Pointer for building string backwards */
    int neg = 0; /* Negative number flag */

    /* Validate base parameter */
    if (base < 2 || base > 36) {
        str("0"); /* Output zero for invalid base */
        return;
    }

    p = &buf[31]; /* Start at end of buffer */
    *p = '\0'; /* Null terminate */

    /* Handle negative numbers */
    if (n < 0) {
        neg = 1;
        n = -n;
    }

    /* Convert digits (at least one digit for zero) */
    do {
        int digit = ((int)n % base);
        *--p = ((char)(digit < 10) ? ('0' + digit) : ('A' + digit - 10));
        n /= base;
    } while (n > 0);

    /* Add negative sign if needed */
    if (neg) {
        *--p = '-';
    }

    fputs(p, stdout);
}

/*
 * main: Program entry point
 * Parses command line arguments and runs the main interpreter loop
 *
 * Command line format:
 *   pti [-offset] [filename]
 *   
 * Where:
 *   -offset: Optional octal offset to seek to in file
 *   filename: Input file (stdin if omitted)
 *
 * Returns: 0 on success, 1 on error
 */
int main(int argc, char **argv) {
    long offset = 0; /* Optional seek offset */
    const char *name = NULL; /* Input filename */
    FILE *fp = NULL; /* Input file pointer */
    int ch; /* Current input character */

    /*
     * Argument parsing (corresponds to start of original pti.s)
     * Handle optional octal offset parameter
     */
    if (argc > 1 && argv[1] != NULL && argv[1][0] == '-') {
        /* Parse octal offset from command line */
        offset = strtol(argv[1] + 1, NULL, 8);
        --argc;
        ++argv;
    }

    /* Get input filename if provided */
    if (argc > 1 && argv[1] != NULL) {
        name = argv[1];
    }

    /* Open input file or use stdin */
    fp = name ? fopen(name, "rb") : stdin;
    if (fp == NULL) {
        if (name != NULL) {
            perror(name);
        } else {
            perror("stdin");
        }
        return 1;
    }

    /* Seek to specified offset if provided */
    if (offset > 0) {
        if (fseek(fp, offset, SEEK_SET) != 0) {
            perror("fseek");
            if (fp != stdin) {
                fclose(fp);
            }
            return 1;
        }
    }

    /*
     * Main interpreter loop
     * Process each input byte according to typesetter control protocol
     */
    for (;;) {
        ch = getc(fp);
        if (ch == EOF) {
            break; /* End of input - exit loop */
        }

        /*
         * Escape sequence processing
         * High bit set indicates escape distance byte
         */
        if (ch & 0200) {
            prn(ch);
            esc += (~ch) & 0177; /* Accumulate escape distance */
            continue;
        }

        /*
         * Output pending escape sequence if any
         * Must be done before processing regular commands
         */
        if (esc != 0) {
            str(escd ? "\\< " : "\\> ");
            numb((long)esc, 10);
            str("\n");

            /* Apply escape direction */
            if (escd) {
                esc = -esc;
            }
            esct += esc;
            esc = 0;
        }

        /* Output the control byte in octal */
        prn(ch);

        /*
         * Process specific command bytes (0100-0114 octal)
         * These are the main typesetter control commands
         */
        switch (ch) {
        case 0100: /* minit - Initialize typesetter */
            str("Initialize\n");
            continue;

        case 0101: /* mlr - Move to Lower Rail */
            str("Lower Rail\n");
            continue;

        case 0102: /* mur - Move to Upper Rail */
            str("Upper Rail\n");
            continue;

        case 0103: /* mum - Move to Upper Magazine */
            str("Upper Mag\n");
            continue;

        case 0104: /* mlm - Move to Lower Magazine */
            str("Lower Mag\n");
            continue;

        case 0105: /* mlc - Set Lower Case mode */
            str("Lower Case\n");
            caseflag = 0;
            continue;

        case 0106: /* muc - Set Upper Case mode */
            str("Upper Case\n");
            caseflag = 0100;
            continue;

        case 0107: /* mef - Set Forward Escape mode */
            str("\\> mode, ");
            numb((long)esct, 10);
            str("\n");
            escd = 0;
            continue;

        case 0110: /* meb - Set Backward Escape mode */
            str("\\< mode, ");
            numb((long)esct, 10);
            str("\n");
            escd = 1;
            continue;

        case 0111: /* mstop - Stop command */
            str("*****Stop*****\n");
            continue;

        case 0112: /* mlf - Set Lead Forward mode */
            str("Lead forward, ");
            numb((long)leadtot, 10);
            str("\n");
            leadmode = 0;
            continue;

        case 0114: /* mlb - Set Lead Backward mode */
            str("Lead backward, ");
            numb((long)leadtot, 10);
            str("\n");
            leadmode = 1;
            continue;

        default:
            break; /* Not a recognized specific command */
        }

        /*
         * Process command categories by bit patterns
         */

        /* Illegal control codes (pattern 01xx except specific ones above) */
        if ((ch & 0360) == 0100) {
            str("Illegal control\n");
            continue;
        }

        /* Lead commands (pattern 011x xxxx) */
        if ((ch & 0340) == 0140) {
            int n = (~ch) & 037; /* Extract 5-bit distance */
            str("Lead ");
            numb((long)n, 10);
            str("\n");

            /* Apply lead direction */
            if (leadmode) {
                n = -n;
            }
            leadtot += n;
            continue;
        }

        /* Size commands (pattern 0010 xxxx) */
        if ((ch & 0360) == 0120) {
            int n = ch & 017; /* Extract 4-bit size code */
            size_t i;

            str("Size ");

            /* Look up point size in size table */
            for (i = 0; stab[i][0] != 0; ++i) {
                if (stab[i][1] == (unsigned char)n) {
                    pts = (int)stab[i][0];
                    numb((long)pts, 10);
                    break;
                }
            }

            /* Handle unknown size codes */
            if (stab[i][0] == 0) {
                str("unknown(");
                numb((long)n, 10);
                str(")");
            }

            str("\n");
            continue;
        }

        /*
         * Printable character processing (pattern 00xx xxxx)
         * These are actual characters to be typeset
         */
        if ((ch & 0300) == 0) {
            unsigned char c = ((unsigned char)ch + caseflag);
            const struct wentry *w = NULL;
            size_t i;
            int width = 0;

            /* Look up character in width table */
            for (i = 0; i < sizeof(wtab) / sizeof(wtab[0]); ++i) {
                if (wtab[i].c == c) {
                    w = &wtab[i];
                    c = ((unsigned char)040 + i); /* Map back to ASCII */
                    break;
                }
            }

            /* Calculate character width in current point size */
            if (w != NULL) {
                width = (((int)w->w) * pts) / 6;
            }

            /* Output the character */
            if (c >= 32 && c < 127) {
                putchar((int)c);
            } else {
                printf("\\%03o", c); /* Non-printable as octal */
            }

            str("\n");

            /* Width calculated but not used in this translation */
            (void)width;
            continue;
        }

        /* Unknown command - just output newline */
        str("\n");
    }

    /*
     * End of input processing
     * Output final lead total and cleanup
     */
    str("Lead total ");
    numb((long)leadtot, 10);
    str("\n");

    /* Close input file if not stdin */
    if (fp != stdin) {
        fclose(fp);
    }

    return 0;
}
