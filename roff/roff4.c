#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*
 * Minimal C translation of several low level text handling routines
 * that originated in roff4.s.  The original assembly performed line
 * filling and justification.  This simplified version implements
 * similar behaviour using portable C code.
 */

/* default formatted line width */
#define ROFF_LINE_WIDTH 65

/* accumulated output for the current line */
static char line_buf[256];
static size_t line_len;

/* working copy used while tokenising the current line */
static char work_buf[256];

/* list of individual words extracted from the current line */
static char *word_list[64];
static int word_count;

/* spacing parameters computed by adjust_line() */
static int fac;
static int fmq;

/* when non-zero the output should be filled and justified */
static int fill_enabled = 1;
static int adjust_enabled = 1;

/* forward declaration */
static void flush_line(void);
static void adjust_line(void);
static void output_filled(void);

/*
 * Accept a chunk of text, break it into words and add them to the
 * current line.  Once the line width would be exceeded, flush the line
 * with justification applied.
 */
void text_line(const char *s) {
    while (*s) {
        /* skip leading whitespace */
        while (*s && isspace((unsigned char)*s)) {
            if (*s == '\n')
                flush_line();
            ++s;
        }

        if (!*s)
            break;

        /* copy the next word */
        char word[128];
        size_t wlen = 0;
        while (s[wlen] && !isspace((unsigned char)s[wlen]) &&
               wlen < sizeof(word) - 1) {
            word[wlen] = s[wlen];
            wlen++;
        }
        word[wlen] = '\0';
        s += wlen;

        /* flush line if the word would overflow */
        if (line_len && line_len + 1 + wlen > ROFF_LINE_WIDTH)
            flush_line();

        if (line_len && fill_enabled)
            line_buf[line_len++] = ' ';

        memcpy(line_buf + line_len, word, wlen);
        line_len += wlen;
        line_buf[line_len] = '\0';

        /* track the number of words currently stored */
        word_count++;
    }
}

/*
 * Output the current line followed by a newline.  When justification is
 * enabled, the line is padded with extra spaces so that it occupies the
 * full line width.
 */
static void flush_line(void) {
    if (!line_len)
        return;

    /* break the current line into words */
    memcpy(work_buf, line_buf, line_len + 1);
    word_count = 0;
    char *t = strtok(work_buf, " ");
    while (t && word_count < 64) {
        word_list[word_count++] = t;
        t = strtok(NULL, " ");
    }

    adjust_line();
    output_filled();

    line_len = 0;
    word_count = 0;
}

/*
 * Compute spacing parameters for the current line.  ``fac`` holds the
 * minimum number of spaces that should be inserted between words while
 * ``fmq`` tracks the remaining extra spaces that need to be spread
 * across the line.  This mirrors the purpose of the original adjust
 * routine in roff4.s.
 */
static void adjust_line(void) {
    fac = 0;
    fmq = 0;

    if (!adjust_enabled || word_count <= 1)
        return;

    size_t letters = 0;
    int i;
    for (i = 0; i < word_count; ++i)
        letters += strlen(word_list[i]);

    int spaces = ROFF_LINE_WIDTH - (int)letters;
    if (spaces <= 0)
        return;

    int gaps = word_count - 1;
    fac = spaces / gaps;
    fmq = spaces % gaps;
}

/*
 * Output words from ``word_list`` separated by the spacing calculated in
 * adjust_line().  Behaviour is similar to the original fill routine.
 */
static void output_filled(void) {
    if (!adjust_enabled || !fill_enabled || word_count == 0 ||
        line_len >= ROFF_LINE_WIDTH) {
        fwrite(line_buf, 1, line_len, stdout);
        putchar('\n');
        return;
    }

    int i;
    for (i = 0; i < word_count; ++i) {
        int j;
        fputs(word_list[i], stdout);
        if (i < word_count - 1) {
            int n = fac;
            if (fmq > 0) {
                n++;
                fmq--;
            }
            for (j = 0; j < n; ++j)
                putchar(' ');
        }
    }
    putchar('\n');
}

/* force a newline in the output */
void nline(void) { flush_line(); }

/* ---------------------------------------------------------------------- */
/* width -- simplified form of label ``width`` in roff4.s.  The original
 * returned the printing width of character ``c``.  Here we treat all
 * printable characters as width one and control characters as zero.
 */
int width(int c) { return (c > ' ' && c != 0177) ? 1 : 0; }

/* space -- helper corresponding to label ``space``.  It prints ``n``
 * spaces using the function pointer provided.
 */
void space(int n, int (*put)(int)) {
    while (n-- > 0)
        put(' ');
}
