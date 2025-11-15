/*
 * Typewriter Driving Table Structure and Device Driver Interface
 * 
 * This header defines the typewriter device table structure used by nroff
 * for character output formatting and device-specific control sequences.
 * The structure contains formatting parameters and control strings for
 * typewriter-style output devices.
 */


/* C17 - no scaffold needed */ // utilities

/* Standard C library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // POSIX header, leave as is
#include <signal.h>

#ifdef NROFF

/* The struct typewriter_table is defined in tdef.h */
/* Just declare extern t here */
extern struct typewriter_table t;

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
