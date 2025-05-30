/*
 * Runtime helper prototypes used across the roff utilities.
 */
#ifndef RUNTIME_H
#define RUNTIME_H
#include "cxx23_scaffold.hpp"

#include <stddef.h>

/* Enable or disable write permission on the controlling terminal. */
void mesg(int enable);

/* Return distance to next 8-column tab stop from a column position. */
int dsp(int column);

/* Write buffer contents to stdout and reset the index pointer. */
void flush_output(char *buf, size_t *p);

#endif /* RUNTIME_H */
