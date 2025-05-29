#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

/* when non-zero the output should be filled and justified */
static int fill_enabled = 1;
static int adjust_enabled = 1;

/* forward declaration */
static void flush_line(void);

/*
 * Accept a chunk of text, break it into words and add them to the
 * current line.  Once the line width would be exceeded, flush the line
 * with justification applied.
 */
void text_line(const char *s)
{
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
    }
}

/*
 * Output the current line followed by a newline.  When justification is
 * enabled, the line is padded with extra spaces so that it occupies the
 * full line width.
 */
static void flush_line(void)
{
    if (!line_len)
        return;

    if (!adjust_enabled || line_len >= ROFF_LINE_WIDTH) {
        fwrite(line_buf, 1, line_len, stdout);
        putchar('\n');
        line_len = 0;
        return;
    }

    /* count words to determine number of space slots */
    char tmp[256];
    memcpy(tmp, line_buf, line_len + 1);
    char *words[64];
    int count = 0;
    char *token = strtok(tmp, " ");
    while (token && count < 64) {
        words[count++] = token;
        token = strtok(NULL, " ");
    }

    /* compute total characters without spaces */
    size_t letters = 0;
    {
        int i;
        for (i = 0; i < count; ++i)
            letters += strlen(words[i]);
    }

    size_t spaces = ROFF_LINE_WIDTH - letters;
    size_t gaps = count > 1 ? (size_t)(count - 1) : 1;
    size_t base = spaces / gaps;
    size_t extra = spaces % gaps;

    {
        int i;
        for (i = 0; i < count; ++i) {
            size_t j;
            fputs(words[i], stdout);
            if (i < count - 1) {
                size_t n = base + (extra ? 1 : 0);
                for (j = 0; j < n; ++j)
                    putchar(' ');
                if (extra)
                    --extra;
            }
        }
    }
    putchar('\n');

    line_len = 0;
}

/* force a newline in the output */
void nline(void)
{
    flush_line();
}
