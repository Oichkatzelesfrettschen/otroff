/**
 * @file roff_c.h
 * @brief C-compatible declarations for ROFF system
 *
 * This header provides C-compatible declarations extracted from roff.hpp
 * for use by pure C source files (.c files) in the ROFF system.
 *
 * Design: Minimal subset of declarations needed by C files, with no C++ syntax.
 */

#ifndef ROFF_C_H
#define ROFF_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler compatibility macros */
#if defined(__GNUC__) || defined(__clang__)
    #define ROFF_UNUSED __attribute__((unused))
#else
    #define ROFF_UNUSED
#endif

/* Character classification functions */
int alph(int c);
int alph2(int c);

/* Hyphenation state variables */
extern int hypedf;      /* Hyphenation processed flag */
extern int hyf;         /* Hyphenation enabled flag */
extern int nhyph;       /* Number of hyphens inserted */
extern int thresh;      /* Hyphenation threshold */
extern int maxdig;      /* Maximum digram score */
extern int suff;        /* Suffix file descriptor */
extern int old;         /* Old character value */
extern int nfile;       /* File number */

/* Pointers for hyphenation analysis */
extern char* wordp;     /* Word pointer */
extern char* hstart;    /* Hyphenation start */
extern char* nhstart;   /* Next hyphenation start */
extern char* maxloc;    /* Maximum score location */

/* Buffers */
extern char sufbuf[];   /* Suffix buffer */

/* File operations */
int rdsufb(int offset, int file_desc);

#ifdef __cplusplus
}
#endif

#endif /* ROFF_C_H */
