/**
 * @file main.c
 * @brief Pure C17 entry point for ROFF text formatter
 *
 * Simple main function for the C17 ROFF implementation.
 */

#include <stdio.h>
#include <stdlib.h>

/* ROFF function declarations */
extern void init_globals(void);
extern int process_roff(int argc, char **argv);

int main(int argc, char **argv) {
    /* Initialize ROFF global variables */
    init_globals();

    /* Print startup message */
    fprintf(stderr, "ROFF Text Formatter (C17 Build)\n");
    fprintf(stderr, "Processing %d argument(s)\n", argc - 1);

    /* Process input files */
    if (argc > 1) {
        /* Would call ROFF processing here */
        fprintf(stderr, "Processing files...\n");
    } else {
        fprintf(stderr, "Usage: %s [input_files...]\n", argv[0]);
        return 1;
    }

    return 0;
}
