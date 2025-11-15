/*
 * troff_processor.h - Troff Processor Global Buffers (Pure C17)
 *
 * Global buffers and pointers used by troff processing
 * Converted from C++ class to pure C globals
 */

#ifndef TROFF_PROCESSOR_H
#define TROFF_PROCESSOR_H

#include <string.h>
#include "tdef.h"

/* Buffer sizes */
#ifndef IBUFSZ
#define IBUFSZ 1024
#endif

#ifndef OBUFSZ
#define OBUFSZ 1024
#endif

/*
 * Troff processor global buffers
 */
typedef struct {
    /* Input buffers and pointers */
    char inputBuffer[IBUFSZ];   /* Primary input buffer */
    char extraBuffer[IBUFSZ];   /* Secondary input buffer */
    char *inputPtr;             /* Pointer into inputBuffer */
    char *extraPtr;             /* Pointer into extraBuffer */
    char *endInput;             /* End pointer for inputBuffer */
    char *endExtra;             /* End pointer for extraBuffer */

    /* Output buffer and pointer */
    char outputBuffer[OBUFSZ];  /* Device output buffer */
    char *outputPtr;            /* Pointer into outputBuffer */
} TroffProcessor;

/* Global processor instance */
extern TroffProcessor g_processor;

/* Initialize processor buffers */
void troff_processor_init(TroffProcessor *proc);

#endif /* TROFF_PROCESSOR_H */
