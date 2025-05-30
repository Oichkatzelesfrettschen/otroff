/*
 * proto.h - Function prototypes for cross troff utilities
 * 
 * This header file contains function declarations for the core troff
 * text formatting system utilities. These functions handle memory
 * management, initialization, number processing, case handling,
 * and various troff-specific operations.
 *
 * Compatible with C90 standard and portable across platforms.
 */

#ifndef CROFF_PROTO_H
#define CROFF_PROTO_H
#include "cxx23_scaffold.hpp"

/* Memory management */
/*
 * setbrk - Set break point for memory allocation
 * @size: Size in bytes to allocate
 * Returns: Pointer to allocated memory block, NULL on failure
 */
char *setbrk(int size);

/* Initialization functions */
/*
 * init1 - First stage initialization with character parameter
 * @a: Character parameter for initialization
 */
void init1(char a);

#endif /* CROFF_PROTO_H */
