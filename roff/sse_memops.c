#include "sse_memops.h"
#include <string.h>

/*
 * Portable C versions of the fast memory routines that were
 * previously implemented in assembly.  These wrappers simply
 * forward to the standard library implementations but are
 * provided as normal functions so the rest of the codebase
 * does not rely on assembly sources.
 */
void *fast_memcpy(void *dst, const void *src, size_t n)
{
    return memcpy(dst, src, n);
}

int fast_memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}
