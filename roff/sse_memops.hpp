/* Header guard to prevent multiple inclusions. */
#ifndef SSE_MEMOPS_H
#define SSE_MEMOPS_H
#include "cxx23_scaffold.hpp"

#include <cstddef>

/* Prototypes for portable memory operations. */
[[nodiscard]] void *fast_memcpy(void *dst, const void *src, size_t n);
[[nodiscard]] int fast_memcmp(const void *s1, const void *s2, size_t n);

#endif /* SSE_MEMOPS_H */
