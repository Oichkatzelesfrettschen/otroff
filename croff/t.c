#include <stdint.h>
#include "t.h"

/*
 * Portable version of the small PDP-11 routine found in t.s.
 * Subtract a 32-bit delta from the time value pointed to by
 * 'tt'.  The time is stored as two 16-bit words in the order
 * {high, low} to match the historic layout.
 */
void sub1(int tt[2], int delta) {
    uint32_t value = ((uint32_t)(uint16_t)tt[0] << 16) |
                     (uint16_t)tt[1];

    value -= (uint32_t)delta;

    tt[0] = (int)(value >> 16);
    tt[1] = (int)(value & 0xFFFF);
}
