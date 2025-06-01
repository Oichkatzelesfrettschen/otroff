/*
 * Typewriter Driving Table Structure and Device Driver Interface
 * 
 * This header defines the typewriter device table structure used by nroff
 * for character output formatting and device-specific control sequences.
 * The structure contains formatting parameters and control strings for
 * typewriter-style output devices.
 */

#pragma once
#include "../cxx17_scaffold.hpp" // utilities

/* Standard C library includes */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h> // POSIX header, leave as is
#include <csignal>

#ifdef NROFF

/*
 * Typewriter device table structure
 * Contains device-specific parameters and control sequences
 */
extern struct typewriter_table {
    /* Basic device parameters */
    int bset; /* Basic character set flag */
    int breset; /* Basic reset flag */
    int Hor; /* Horizontal resolution */
    int Vert; /* Vertical resolution */
    int Newline; /* Newline handling parameter */
    int Char; /* Character width parameter */
    int Em; /* Em width in device units */
    int Halfline; /* Half-line spacing in device units */
    int Adj; /* Adjustment parameter */

    /* Device control strings */
    char *twinit; /* Typewriter initialization string */
    char *twrest; /* Typewriter reset string */
    char *twnl; /* Typewriter newline string */
    char *hlr; /* Half-line reverse string */
    char *hlf; /* Half-line forward string */
    char *flr; /* Full-line reverse string */
    char *bdon; /* Bold on string */
    char *bdoff; /* Bold off string */
    char *ploton; /* Plot mode on string */
    char *plotoff; /* Plot mode off string */
    char *up; /* Cursor up string */
    char *down; /* Cursor down string */
    char *right; /* Cursor right string */
    char *left; /* Cursor left string */

    /* Character code translation table */
    /* Maps ASCII codes 32-255 to device-specific sequences */
    char *codetab[256 - 32];

    /* Termination marker */
    int zzz; /* End-of-table marker */
} t;

#endif /* NROFF */

/*
 * Device Driver Interface Function Prototypes
 * These functions provide the interface between the formatter
 * and the specific typewriter device driver.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ptinit - Initialize the printer/typewriter device
 * Called once at program startup to set up device state
 */
void ptinit(void);

/*
 * twdone - Finalize typewriter operations
 * Called at program termination to clean up device state
 */
void twdone(void);

/*
 * ptout - Output a character to the device
 * @param i: Character code to output
 * Handles device-specific character translation and output
 */
void ptout(int i);

/*
 * ptlead - Handle vertical spacing (leading)
 * Performs vertical positioning operations on the device
 */
void ptlead(void);

/*
 * dostop - Handle stop/pause operations
 * Implements device-specific stop or synchronization operations
 */
void dostop(void);

#ifdef __cplusplus
} // extern "C"
#endif
