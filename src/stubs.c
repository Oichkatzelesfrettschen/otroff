#include "os/os_abstraction.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file stubs.c
 * @brief Portable C replacements for small PDP-11 assembly routines.
 *
 * These implementations provide minimal behaviour compatible with the
 * original assembler versions so that the converted ROFF sources build and
 * run.  Only the most essential logic is reproduced here.
 */

/* External state shared across the ROFF modules */
extern int ch; /* Current buffered character */
extern int nlflg; /* Newline flag from input routine */
extern int column; /* Current input column */
extern int ocol; /* Current output column */
extern int nsp; /* Pending space count */
extern int pfrom; /* Starting page number */
extern int pto; /* Ending page number */
extern int pn; /* Current page number */
extern int slow; /* Slow output mode flag */
extern int ifile; /* Current input file descriptor */
extern int argc; /* Remaining argument count */
extern char **argp; /* Pointer to next file name */
extern int nx; /* .nx request flag */
extern int ma1, ma2, ma3, ma4; /* Page margins */
extern int pl; /* Page length */
extern int hx; /* Header/footer spacing */
extern int bl; /* Bottom line of page */
extern int nl; /* Current line on page */
extern int ul; /* Underline count */
extern int ulstate; /* Underline processing state */
extern int ulc; /* Remaining underline characters */
extern int bsc; /* Backspace count */
extern int llh; /* Header line length */
extern int ll; /* Normal line length */
extern char *ehead; /* Even page header string */
extern char *ohead; /* Odd  page header string */
extern char *efoot; /* Even page footer string */
extern char *ofoot; /* Odd  page footer string */
extern char *obufp; /* Output buffer pointer */
extern void skipcont(void);

/* Output buffer base lives in roff8.c as a static array */
/** Return pointer to start of output buffer (defined in roff8.c). */
static char *obuf_base(void) {
    static char *base_ptr = NULL;

    if (base_ptr == NULL) {
        base_ptr = obufp;
    }

    return base_ptr;
}

/**
 * Read a character from the current input stream.
 *
 * This is a greatly simplified version of the original routine.  It only
 * implements the basic buffering logic required by the rest of the C
 * translation.
 *
 * @return Next character or 0 on end of input.
 */
int getchar_roff(void) {
    int c;

    if (ch != 0) {
        c = ch;
        ch = 0;
        return c;
    }

    if (nlflg) {
        nlflg = 0;
        return '\n';
    }

    c = getchar();
    if (c == EOF) {
        return 0;
    }

    if (c == '\n') {
        nlflg = 1;
        column = 0;
    } else {
        column++;
    }

    return c & 0x7f;
}

/**
 * Output a character using the ROFF buffering scheme.
 *
 * @param c Character to output
 */
void putchar_roff(int c) {
    char *base = obuf_base();

    if (pn < pfrom || pn > pto) {
        return;
    }

    c &= 0x7f;
    if (c == 0) {
        return;
    }

    if (c == ' ') {
        nsp++;
        return;
    }

    if (c == '\n') {
        nsp = 0;
        ocol = 0;
        *obufp++ = (char)c;
        return;
    }

    while (nsp > 0) {
        if (!slow) {
            int tab_stop = ((ocol + 8) / 8) * 8;
            if (tab_stop - ocol <= nsp) {
                *obufp++ = '\t';
                nsp -= tab_stop - ocol;
                ocol = tab_stop;
                continue;
            }
        }
        *obufp++ = ' ';
        ocol++;
        nsp--;
    }

    *obufp++ = (char)c;
    ocol++;

    if (obufp - base >= 128) {
        flush();
    }
}

/** Flush characters up to a newline without processing. */
void flushi(void) {
    ch = 0;
    while (!nlflg) {
        getchar_roff();
    }
}

/** Recompute page top and bottom based on margins. */
void topbot(void) {
    enum { DEFAULT_PAGE_LENGTH = 66,
           MIN_MARGIN = 2 };

    if (pl == 0) {
        bl = 0;
        return;
    }

    bl = pl - ma3 - ma4 - hx;
    if (ma1 + ma2 + hx >= bl) {
        ma1 = ma2 = ma3 = ma4 = MIN_MARGIN;
        pl = DEFAULT_PAGE_LENGTH;
        topbot();
        return;
    }

    if (nl > bl) {
        nl = bl;
    }
}

/**
 * Read a header/footer definition from input.
 *
 * @param[out] p  Location to store allocated string pointer
 */
void headin(char **p) {
    int delim;
    int c;
    char buf[256];
    size_t len = 0;

    if (p == NULL) {
        return;
    }

    skipcont();
    delim = gettchar();

    if (delim == '\n') {
        buf[0] = '\0';
    } else {
        while ((c = gettchar()) != '\n' && c != delim && len < sizeof(buf) - 1) {
            buf[len++] = (char)c;
        }
        buf[len] = '\0';
    }

    free(*p);
    *p = malloc(len + 1);
    if (*p != NULL) {
        memcpy(*p, buf, len + 1);
    }

    llh = ll;
}

/**
 * Output a stored header/footer string with simple page number substitution.
 *
 * @param[in,out] p Pointer to header/footer string pointer
 */
void headout(char **p) {
    const char *s;

    if (p == NULL || hx == 0) {
        return;
    }

    s = *p;
    if (s == NULL) {
        return;
    }

    while (*s) {
        if (*s == '%') {
            char numbuf[16];
            size_t i;
            snprintf(numbuf, sizeof(numbuf), "%d", pn);
            for (i = 0; numbuf[i] != '\0'; i++) {
                putchar_roff(numbuf[i]);
            }
        } else {
            putchar_roff((unsigned char)*s);
        }
        s++;
    }

    putchar_roff('\n');
}

/**
 * Output a number of newline characters.
 *
 * @param count    Number of lines to output
 * @param spacing  Unused in this simplified version
 */
void nlines(int count, int spacing) {
    (void)spacing;
    while (count-- > 0) {
        putchar_roff('\n');
        nl++;
    }
}

/**
 * Switch to the next input file in the argument list.
 *
 * @return 0 on success, -1 if no more files are available
 */
int nextfile(void) {
    if (ifile > 0) {
        os_close(ifile);
        ifile = -1;
    }

    if (nx) {
        return -1;
    }

    if (argc <= 0) {
        return -1;
    }

    ifile = os_open(*argp, O_RDONLY, 0);
    if (ifile < 0) {
        return -1;
    }

    argp++;
    argc--;
    return 0;
}

/**
 * Return next character with underline processing.
 *
 * @return Next character or 0 on end of input
 */
int gettchar(void) {
    int c;

    if (ul <= 0) {
        return getchar_roff();
    }

    for (;;) {
        if (ulstate) {
            if (bsc > 0) {
                bsc--;
                return '\b';
            }
            if (ulc > 0) {
                ulc--;
                return '_';
            }
            ulstate = 0;
        }

        c = getchar_roff();

        if (('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') ||
            ('a' <= c && c <= 'z')) {
            bsc++;
            ulc++;
            return c;
        }

        if (ulc > 0) {
            ulstate = 1;
            ch = c;
            continue;
        }

        return c;
    }
}

/** Flush the output buffer to standard output. */
void flush(void) {
    char *base = obuf_base();
    size_t len = (size_t)(obufp - base);

    if (len > 0) {
        os_write(STDOUT_FILENO, base, len);
        obufp = base;
    }
}

/** Determine if a character is alphabetic. */
int alph(int c) { return isalpha(c) != 0; }

/** Extended alphabetic test used by skipcont. */
int alph2(int c) { return alph(c); }
