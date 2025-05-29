#ifdef USE_SSE
#include <stddef.h>
void *fast_memcpy(void *dst, const void *src, size_t n);
int fast_memcmp(const void *s1, const void *s2, size_t n);
#else
#include <string.h>
static inline void *fast_memcpy(void *dst, const void *src, size_t n)
{
    return memcpy(dst, src, n);
}
static inline int fast_memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}
#endif
