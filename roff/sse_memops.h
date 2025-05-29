/* Header guard to prevent multiple inclusions. */
#ifndef SSE_MEMOPS_H
#define SSE_MEMOPS_H

#include <stddef.h>

/* Prototypes for portable memory operations. */
void *fast_memcpy(void *dst, const void *src, size_t n);
int fast_memcmp(const void *s1, const void *s2, size_t n);

#endif /* SSE_MEMOPS_H */
