/*
 * t.c - Portable time manipulation functions
 *
 * This module provides portable implementations of time value manipulation
 * routines, originally designed to replace PDP-11 assembly code with
 * equivalent C functions that maintain the same functionality across
 * different architectures.
 *
 * The time values are stored as arrays of two integers representing
 * a 32-bit value split into high and low 16-bit words, matching the
 * historic PDP-11 memory layout for compatibility.
 */

#include <stdint.h>

/* Define NDI if not already defined */
#ifndef NDI
#define NDI 100  /* or appropriate value based on your application */
#endif

/* Define NN if not already defined */
#ifndef NN
#define NN 100  /* or appropriate value based on your application */
#endif

/* Define NNAMES if not already defined */
#ifndef NNAMES
#define NNAMES 100  /* or appropriate value based on your application */
#endif

#include "t.h"

/*
 * sub1 - Subtract a delta value from a time value
 *
 * This function subtracts a 32-bit delta from a time value stored as
 * two 16-bit words. The time array stores the value in {high, low} order
 * to maintain compatibility with the original PDP-11 implementation.
 *
 * Parameters:
 *   tt    - Pointer to array of 2 integers representing time value
 *           tt[0] contains high 16 bits, tt[1] contains low 16 bits
 *   delta - 32-bit signed integer value to subtract from time
 *
 * The function performs the subtraction on the full 32-bit value and
 * stores the result back into the two-word format, handling underflow
 * correctly through unsigned arithmetic.
 *
 * Note: Uses explicit casts to ensure portable behavior across different
 * architectures and compilers, particularly for sign extension handling.
 */
void
sub1(int tt[2], int delta)
{
    uint32_t time_value;    /* Combined 32-bit time value */
    uint32_t delta_u;       /* Delta as unsigned for safe arithmetic */
    
    /* Combine the two 16-bit words into a single 32-bit value
     * Cast to uint16_t first to ensure only lower 16 bits are used,
     * preventing sign extension issues on negative values */
    time_value = ((uint32_t)(uint16_t)tt[0] << 16) | (uint16_t)tt[1];
    
    /* Convert delta to unsigned for well-defined underflow behavior */
    delta_u = (uint32_t)delta;
    
    /* Perform subtraction using unsigned arithmetic
     * This ensures proper wrap-around behavior on underflow */
    time_value -= delta_u;
    
    /* Split the result back into high and low 16-bit words
     * Cast back to int to match the array element type */
    tt[0] = (int)(time_value >> 16);           /* High 16 bits */
    tt[1] = (int)(time_value & 0xFFFFu);       /* Low 16 bits */
}
