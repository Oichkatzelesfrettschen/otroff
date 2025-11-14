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

/* Constants - from roff.hpp and roff8.c */
#define WORD_SIZE 200           /* Maximum word buffer size */
#define LINE_SIZE 500           /* Maximum line buffer size */
#define SUFFIX_BUF_SIZE 512     /* Suffix buffer size */
#define MAX_NAME_LEN 18         /* Maximum filename length */
#define BLOCK_MASK 077          /* Block alignment mask (octal 77) */

/* Character classification functions */
int alph(int c);
int alph2(int c);

/* Core ROFF global variables - from roff.hpp */
extern int ch;          /* Current character from input */
extern int nc;          /* Number of characters in current line */
extern int totout;      /* Total output lines counter */
extern int ls;          /* Line spacing value */
extern int ll;          /* Line length setting */
extern int pl;          /* Page length in lines */
extern int nl;          /* Current line number on page */
extern int bl;          /* Bottom line number (computed) */
extern int ma1, ma2, ma3, ma4;  /* Margins: top, header, footer, bottom */
extern int pn;          /* Current page number */
extern int skip;        /* Lines to skip at next page break */
extern int po;          /* Page offset (left margin) */
extern int un;          /* Temporary indent value */
extern int in;          /* Permanent indent value */
extern int ohc;         /* Output hyphenation character */
extern int wne;         /* Word character count for width calculation */
extern int wch;         /* Word character count */
extern int spaceflg;    /* Extra space flag for sentence spacing */
extern int skp;         /* Skip processing flag for .ig command */
extern int ip;          /* Include processing pointer */
extern int nlflg;       /* Newline flag for input processing */

/* Justification and formatting */
extern int jfomod;      /* Justification mode setting */
extern int fac, fmq;    /* Justification factors for spacing */
extern int nel;         /* Number of characters left in current line */
extern int nwd;         /* Number of words in current line */
extern int ne;          /* Number of characters in current line */

/* Line numbering */
extern int numbmod;     /* Line numbering mode (0=off, 1=mode1, 2=mode2) */
extern int nn;          /* Line number skip count */
extern int ni;          /* Line number indent spacing */
extern int lnumber;     /* Current line number for numbering */

/* Formatting control */
extern int ulstate;     /* Underline state */
extern int ce;          /* Center line count */
extern int fi;          /* Fill mode flag */
extern int ul;          /* Underline count */
extern int ad;          /* Adjust mode flag */
extern int ro;          /* Roman numeral flag for page numbers */
extern int llh;         /* Line length for headers/footers */

/* Page control */
extern int stop;        /* Interactive stop mode flag */
extern int pfrom, pto;  /* Page range limits (from, to) */
extern int hx;          /* Header/footer processing flag */

/* Buffer management */
extern int nextb;       /* Next buffer position for writing */
extern int ibf, ibf1;   /* Buffer file descriptors */
extern int ofile;       /* Output file descriptor */
extern int sufoff;      /* Suffix buffer offset for caching */

/* Buffers and pointers */
extern char line[];     /* Main line accumulation buffer */
extern char* linep;     /* Current position in line buffer */
extern char word[];     /* Word accumulation buffer */
extern char* ehead;     /* Even page header string */
extern char* efoot;     /* Even page footer string */
extern char* ohead;     /* Odd page header string */
extern char* ofoot;     /* Odd page footer string */

/* Include list management */
extern int* ilistp;     /* Include list stack pointer */
extern int ilist[];     /* Include list stack array */

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

/* Core ROFF functions */
int getchar_roff();
int gettchar();
void putchar_roff(int c);
void eject();
void headout(char** header_ptr);
void space(int count);
void jfo();
void donum();
void newline();
void nline();
void fill();
void flush();
void flushi();
void istop();
int width(int c);
void decimal(int num, void (*output_func)(int));
int number1(int default_val);
void setnel();
void storeword(int c);
void storeline(int c);
void need2(int lines);
void wbf(int character, int position);
void rbreak();
void getword();
void hyphen();
void skipcont();

/* File operations */
int rdsufb(int offset, int file_desc);

#ifdef __cplusplus
}
#endif

#endif /* ROFF_C_H */
