#ifndef B31C5E03_C992_4129_9DF6_E5F9A4A1B850
#define B31C5E03_C992_4129_9DF6_E5F9A4A1B850

/*
 * Generic helper macros used across the ROFF modules.
 *
 * ROFF_UNUSED   - mark a variable or function as intentionally unused
 *                 to silence compiler warnings.
 */

#if defined(__clang__) || defined(__GNUC__)
#define ROFF_UNUSED __attribute__((unused))
#else
#define ROFF_UNUSED
#endif

#endif /* B31C5E03_C992_4129_9DF6_E5F9A4A1B850 */
