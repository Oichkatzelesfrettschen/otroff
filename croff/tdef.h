/*
 * tdef.h - Type definitions and constants for troff/nroff typesetting system
 * 
 * This header defines fundamental constants and macros used throughout the
 * troff (typesetter) and nroff (line printer) formatting systems.
 * The file uses conditional compilation to provide different values
 * for TROFF (high-resolution typesetter) vs NROFF (line printer) modes.
 */

#ifndef TDEF_H
#define TDEF_H

/* C17 - no scaffold needed */ // utilities

/* Standard C library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // POSIX header, leave as is
#include <signal.h>

/*
 * Conditional compilation for NROFF (line printer output)
 * NROFF produces output suitable for line printers and terminals
 */
#ifdef NROFF

/* Include typewriter table definition */
/* Note: tw.h defines struct typewriter_table and extern t */
/* We only need the struct definition here for the macros below */
struct typewriter_table {
    int bset;
    int breset;
    int Hor;
    int Vert;
    int Newline;
    int Char;
    int Em;
    int Halfline;
    int Adj;
    char *twinit;
    char *twrest;
    char *twnl;
    char *hlr;
    char *hlf;
    char *flr;
    char *bdon;
    char *bdoff;
    char *ploton;
    char *plotoff;
    char *up;
    char *down;
    char *right;
    char *left;
    char *codetab[256 - 32];
    int zzz;
};

/* External declaration of terminal table instance (defined in nii.c) */
extern struct typewriter_table t;

extern int pipeflg;

/* Basic measurement units for NROFF */
#define EM (t.Em) /* Em space (width of 'M' character) */
#define HOR (t.Hor) /* Horizontal resolution */
#define VERT (t.Vert) /* Vertical resolution */
#define INCH 240 /* Increments per inch for NROFF */
#define SPS (INCH / 10) /* Standard space size */
#define SS (INCH / 10) /* Space size (same as SPS) */

/* Page layout constants for NROFF */
#define TRAILER 0 /* Trailer space (none for NROFF) */
#define UNPAD 0227 /* Unpadded character code (octal) */
#define PO 0 /* Page offset (left margin) */

/* Character set and formatting flags for NROFF */
#define ASCII 1 /* ASCII character set flag */
#define PTID 1 /* Point size identifier */
#define LG 0 /* Ligature flag (disabled) */
#define DTAB 0 /* Default tab spacing (set at init) */
#define ICS (2 * SPS) /* Inter-character spacing */

#else /* TROFF - high-resolution typesetter output */

/* Basic measurement units for TROFF */
#define INCH 432 /* TROFF resolution (points per inch) */
#define SPS 20 /* Space size at 10pt (1/3 Em) */
#define SS 12 /* Space size in 36ths of an Em */

/* Page layout constants for TROFF */
#define TRAILER 4968 /* Trailer: 144*11.5*3 = 11.5 inches */
#define UNPAD 027 /* Unpadded character code (octal) */
#define PO 416 /* Page offset: 26/27ths inch */

/* Resolution and spacing for TROFF */
#define HOR 1 /* Horizontal resolution unit */
#define VERT 3 /* Vertical resolution unit */
#define EM (6 * (pts & 077)) /* Em space based on point size */

/* Character set and formatting flags for TROFF */
#define ASCII 0 /* Non-ASCII character set */
#define PTID 0 /* Point size identifier */
#define LG 1 /* Ligature flag (enabled) */
#define DTAB (INCH / 2) /* Default tab spacing (half inch) */
#define ICS (3 * SPS) /* Inter-character spacing */

#endif /* NROFF/TROFF conditional compilation */

/*
 * Signal definitions (for older systems lacking signal.h)
 * These may be redundant on modern systems but ensure portability
 */
#ifndef SIGHUP
#define SIGHUP 1 /* Hangup signal */
#endif
#ifndef SIGINT
#define SIGINT 2 /* Interrupt signal */
#endif
#ifndef SIGQUIT
#define SIGQUIT 3 /* Quit signal */
#endif
#ifndef SIGFPE
#define SIGFPE 8 /* Floating point exception */
#endif
#ifndef SIGKILL
#define SIGKILL 15 /* Kill signal */
#endif
#ifndef SIGPIPE
#define SIGPIPE 13 /* Broken pipe signal */
#endif

/*
 * Terminal and character constants
 */
#define ECHO 010 /* TTY echo mode flag (octal) */
#define NARSP 0177 /* Narrow space character (octal) */
#define HNSP 0226 /* Half narrow space character (octal) */

/*
 * Default formatting parameters
 */
#define PS 10 /* Default point size */
#define FT 0 /* Default font position */
#define LL (65 * INCH / 10) /* Line length: 39 picas = 6.5 inches */
#define VS (INCH / 6) /* Vertical spacing: 12 points */

/*
 * System limits and buffer sizes
 */
#define NN 170 /* Number of number registers */
#define NNAMES 14 /* Predefined register names */
#define NIF 5 /* If-else nesting depth */
#define NS 64 /* Name buffer size */
#define NTM 256 /* Terminal message buffer size */
#define NEV 3 /* Number of environments */
#define EVLSZ 10 /* Environment stack size */
#define EVS (3 * 256) /* Environment size in words */
#define NM 252 /* Requests plus macros */
#define DELTA 512 /* Delta core bytes for allocation */
#define STKSIZE 10 /* Stack size in words */

/*
 * Hyphenation and word processing limits
 */
#define NHYP 10 /* Maximum hyphens per word */
#define NHEX 128 /* Exception word list byte size */
#define NTAB 35 /* Number of tab stops */
#define NSO 5 /* "so" (source file) nesting depth */
#define WDSIZE 170 /* Word buffer size */
#define LNSIZE 480 /* Line buffer size */
#define NDI 5 /* Number of diversions */

/*
 * Character and motion encoding bits
 * These constants define bit patterns used for character encoding
 */
#define DBL 0100000 /* Double size indicator bit */
#define MOT 0100000 /* Motion character indicator */
#define MOTV 0160000 /* Motion part clear mask */
#define VMOT 0040000 /* Vertical motion bit */
#define NMOT 0020000 /* Negative motion indicator */
#define MMASK 0100000 /* Macro mask indicator */
#define CMASK 0100377 /* Character mask */
#define ZBIT 0400 /* Zero width character bit */
#define BMASK 0377 /* Byte mask (8 bits) */

/*
 * Miscellaneous constants
 */
#define BYTE 8 /* Bits per byte */
#define IMP 004 /* Impossible character code */
#define FILLER 037 /* Filler character (octal) */
#define PRESC 026 /* Prescaler character */
#define HX 0376 /* High-order part of xlss */
#define LX 0375 /* Low-order part of xlss */
#define CONT 025 /* Continuation character */
#define COLON 013 /* Colon character code */
#define XPAR 030 /* Extra parameter character */
#define ESC 033 /* Escape character */
#define FLSS 031 /* Flush character */
#define RPT 014 /* Repeat character */
#define JREG 0374 /* Jump register character */

/*
 * Trap and pagination constants
 */
#define NTRAP 20 /* Number of traps */
#define NPN 20 /* Numbers in "-o" option */

/*
 * Output device control constants
 */
#define T_PAD 0101 /* CAT (phototypesetter) padding */
#define T_INIT 0100 /* Terminal initialization */
#define T_IESC 16 /* Initial escape offset */
#define T_STOP 0111 /* Terminal stop code */
#define NPP 10 /* Pads per field */

/*
 * Buffer sizes for I/O operations
 */
#define FBUFSZ 256 /* Field buffer size in words */
#define OBUFSZ 512 /* Output buffer size in bytes */
#define IBUFSZ 512 /* Input buffer size in bytes */
#define NC 256 /* Character buffer size in words */
#define NOV 10 /* Number of overstrike characters */

/*
 * System-specific constants
 */
#define LONG0 long /* Long integer type flag */
#define ZONE 5 /* Time zone offset (5 hrs for EST) */

/*
 * Special delimiter and formatting characters
 */
#define TDELIM 032 /* Table delimiter */
#define LEFT 035 /* Left delimiter */
#define RIGHT 036 /* Right delimiter */
#define LEADER 001 /* Leader character (dots) */
#define TAB 011 /* Tab character */
#define OHC 024 /* Optional hyphen character */

/*
 * Tab processing bit masks
 */
#define TMASK 037777 /* Tab mask (15 bits) */
#define RTAB 0100000 /* Right-aligned tab bit */
#define CTAB 0040000 /* Centered tab bit */


#endif /* TDEF_H */
