#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*
 * Partial C translation of the hyphenation driver starting at the
 * `hyphen:` label in roff5.s.  Only the driver logic itself is
 * represented here.  The complex suffix and digram scoring
 * routines are stubbed out.
 */

/* helpers implemented in hyphen_utils.c */
int punct(int c);
int vowel(int c);

/* helpers provided by roff3.c */
int alph(int c);

/* digram scoring tables from roff7.c */
extern const unsigned char bxh[];
extern const unsigned char hxx[];
extern const unsigned char bxxh[];
extern const unsigned char xhx[];
extern const unsigned char xxh[];

/* table of punctuation characters used by the original driver.  The
 * array itself mirrors the one embedded in the assembly routine
 * labelled `punct:` within roff5.s.  It is unused by the simplified
 * C translation but kept for reference. */
static const char ptab[] = " .,()\"'`";

/* -- digram utilities -------------------------------------------------- */

/*
 * Translate the `dilook` subroutine from roff5.s.  The PDP-11 code
 * packed two digram weights into each byte of the tables in roff7.c.
 * The low nibble stores the weight for even-indexed letters and the
 * high nibble contains the odd ones.  The lookup is performed on
 * lower-cased alphabetic input.  Non alphabetic characters yield a
 * weight of zero.
 */
static int digram_lookup(int c1, int c2, const unsigned char *table) {
    if (!alph(c1) || !alph(c2))
        return 0;

    c1 = tolower(c1) - 'a';
    c2 = tolower(c2) - 'a';

    if (c1 < 0 || c1 >= 26 || c2 < 0 || c2 >= 26)
        return 0;

    int index = 13 * c1 + (c2 >> 1);
    unsigned char val = table[index];

    if (c2 & 1)
        val >>= 4;

    return val & 0x0f;
}

/*
 * Rough approximation of the `suffix` routine.  The original code
 * consulted a large suffix table to bias hyphenation away from common
 * endings.  Here we recognise only a few patterns in order to
 * demonstrate the call structure.
 *
 * Returns the suggested hyphen index or -1 if no suffix rule matches.
 */
static int suffix(const char *start, const char *end) {
    size_t len = (size_t)(end - start + 1);

    if (len > 5 && strncmp(end - 2, "ing", 3) == 0)
        return (int)(len - 3);
    if (len > 4 && strncmp(end - 1, "es", 2) == 0)
        return (int)(len - 2);
    if (len > 4 && strncmp(end - 1, "ed", 2) == 0)
        return (int)(len - 2);
    if (len > 4 && strncmp(end - 1, "er", 2) == 0)
        return (int)(len - 2);
    if (len > 4 && strncmp(end - 1, "ly", 2) == 0)
        return (int)(len - 2);

    return -1;
}

/*
 * Simplified form of the `digram` routine.  The PDP-11 version used
 * complex heuristics driven by five digram tables.  This condensed
 * variant scores every interior position of the word and selects the
 * break point with the highest weight.
 *
 * The function returns the index of the best hyphen location relative
 * to `start`, or -1 if no position exceeds a minimal threshold.
 */
static int digram(const char *start, const char *end) {
    size_t len = (size_t)(end - start + 1);
    int best_score = 0;
    int best_pos = -1;

    size_t i;
    for (i = 1; i + 1 < len; ++i) {
        int score = 1;

        score += digram_lookup(start[i - 1], start[i], xxh);
        score += digram_lookup(start[i], start[i + 1], xhx);
        score += digram_lookup(start[i - 1], start[i + 1], bxxh);
        score += digram_lookup(start[i], start[i + 1], hxx);

        if (i == 1)
            score += digram_lookup('a', start[i], bxh);

        if (score > best_score) {
            best_score = score;
            best_pos = (int)i;
        }
    }

    return best_score > 20 ? best_pos : -1;
}

/*
 * Main hyphenation entry.  This mirrors the logic found at the
 * `hyphen:` label in roff5.s.  Punctuation at either end of the word
 * is ignored, then the suffix and digram heuristics are consulted to
 * locate a suitable break point.  A non-zero return indicates that a
 * hyphen position was found.
 */
int hyphenate(const char *w) {
    const char *p = w;

    while (*p && punct((unsigned char)*p))
        ++p;

    if (!*p || !alph((unsigned char)*p))
        return 0;

    const char *start = p++;
    while (*p && alph((unsigned char)*p))
        ++p;
    const char *end = p - 1;

    while (*p && punct((unsigned char)*p))
        ++p;
    if (*p)
        return 0;

    if ((end - start) < 4)
        return 0;

    int pos = suffix(start, end);
    if (pos == -1)
        pos = digram(start, end);

    return pos != -1;
}

/*
 * Translate the initial section of the PDP-11 hyphenation driver.
 * This scans over punctuation, identifies the word boundaries and
 * invokes the (stubbed) suffix and digram routines.
 */
void hyphen_driver(const char *word) {
    /* Wrapper retained for compatibility with the original source. */
    if (hyphenate(word))
        printf("%s-\n", word);
    else
        puts(word);
}

void do_hyphen(const char *w) {
    if (hyphenate(w))
        printf("%s-\n", w);
    else
        puts(w);
}

/* ---------------------------------------------------------------------- */
/* checkvow -- stand in for the ``checkvow`` routine of roff5.s.  It
 * tested for vowels moving backwards from a pointer.  The simplified
 * version always returns false.
 */
int checkvow(const char *p) {
    (void)p;
    return 0;
}

/*
 * rdsuf -- stub representing the `rdsuf` subroutine in roff5.s which read
 * suffix information.  This simplified version merely logs the call.
 */
void rdsuf(const char *name) {
    (void)name;
    puts("[stub] rdsuf");
}
