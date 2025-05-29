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
extern char bxh[];
extern char hxx[];
extern char bxxh[];
extern char xhx[];
extern char xxh[];

/* table of punctuation characters used by the original driver */
static const char ptab[] = " .,()\"'`";

/* placeholder functions for the heavy lifting performed by the
 * original suffix() and digram() routines.  These simply reference
 * the digram tables so the compiler doesn't optimise them away.
 */
static void suffix(const char *start, const char *end) {
  (void)start;
  (void)end;
  /* TODO: implement suffix handling using suftab. */
}

static void digram(const char *start, const char *end) {
  /* touch the arrays to silence unused variable warnings */
  int tmp = bxh[0] + hxx[0] + bxxh[0] + xhx[0] + xxh[0];
  (void)tmp;
  (void)start;
  (void)end;
  /* TODO: implement digram based scoring. */
}

/*
 * Simplified hyphenation check: return 1 if the word contains a
 * vowel that isn't the first or last character.
 */
int hyphenate(const char *w) {
  size_t len = strlen(w);
  size_t i;

  if (len <= 2)
    return 0;

  for (i = 1; i + 1 < len; ++i)
    if (vowel((unsigned char)w[i]))
      return 1;
  return 0;
}

/*
 * Translate the initial section of the PDP-11 hyphenation driver.
 * This scans over punctuation, identifies the word boundaries and
 * invokes the (stubbed) suffix and digram routines.
 */
void hyphen_driver(const char *word) {
  const char *p = word;
  const char *wdstart;
  const char *hyend;

  (void)ptab; /* silence unused variable warning; table kept for reference */

  /* skip leading punctuation */
  while (*p && punct((unsigned char)*p))
    ++p;

  if (!*p || !alph((unsigned char)*p))
    return; /* no alphabetic characters */

  wdstart = p++;

  /* find end of the alphabetic span */
  while (*p && alph((unsigned char)*p))
    ++p;
  hyend = p - 1;

  /* trailing punctuation must be the end of the string */
  while (*p && punct((unsigned char)*p))
    ++p;
  if (*p)
    return; /* more characters follow */

  /* minimum length requirement from the original code */
  if ((hyend - wdstart) < 4)
    return;

  suffix(wdstart, hyend);
  digram(wdstart, hyend);
}

void do_hyphen(const char *w) {
  if (hyphenate(w))
    printf("%s-\n", w);
  else
    puts(w);
}
