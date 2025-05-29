#include "tdef.h"
#include <stddef.h>
#include "proto.h"

/*
troff8.c

hyphenation
*/

char hbuf[NHEX];
char *nexth hbuf;
int *hyend;
extern int *wdstart, *wdend;
extern int *hyptr[];
extern int **hyp;
extern int hyoff;
extern int suffid;
extern int sufind[26];
/* Accessor for suffix table bytes. */
unsigned char suftab_get_byte(size_t);
extern int noscale;
extern int xxx;
#define THRESH 160 /*digram goodness threshold*/
int thresh THRESH;

/* Hyphenate the word pointed to by wp. */
void hyphen(int *wp) {
    int *i, j;

    i = wp;
    while (punct(*i++))
        ;
    if (!alph(*--i))
        return;
    wdstart = i++;
    while (alph(*i++))
        ;
    hyend = wdend = --i - 1;
    while (punct(*i++))
        ;
    if (*--i)
        return;
    if ((wdend - wdstart - 4) < 0)
        return;
    hyp = hyptr;
    *hyp = 0;
    hyoff = 2;
    if (exword() || suffix())
        goto rtn;
    digram();
rtn:
    *hyp++ = 0;
    if (*hyptr)
        for (j = 1; j;) {
            j = 0;
            for (hyp = hyptr + 1; *hyp != 0; hyp++) {
                if (*(hyp - 1) > *hyp) {
                    j++;
                    i = *hyp;
                    *hyp = *(hyp - 1);
                    *(hyp - 1) = i;
                }
            }
        }
}
int punct(int i) {
    if (!i || alph(i))
        return 0;
    else
        return 1;
}

int alph(int i) {
    int j;

    j = i & CMASK;
    if (((j >= 'A') && (j <= 'Z')) || ((j >= 'a') && (j <= 'z')))
        return (1);
    else
        return (0);
}
void caseht(void) {
    int i;

    thresh = THRESH;
    if (skip())
        return;
    noscale++;
    thresh = tatoi();
    noscale = 0;
}
void casehw(void) {
    int i, k;
    char *j;

    k = 0;
    while (!skip()) {
        if ((j = nexth) >= (hbuf + NHEX - 2)) {
        full:
            prstr("Exception word list full.\n");
            *nexth = 0;
            return;
        }
        while (1) {
            if ((i = getch()) & MOT)
                continue;
            if (((i = &CMASK) == ' ') || (i == '\n')) {
                *j++ = 0;
                nexth = j;
                *j = 0;
                if (i == ' ')
                    break;
                else
                    return;
            }
            if (i == '-') {
                k = 0200;
                continue;
            }
            *j++ = maplow(i) | k;
            k = 0;
            if (j >= (hbuf + NHEX - 2))
                goto full;
        }
    }
}
/* Check exception word list for current word. */
int exword(void) {
    int *w, i;
    char *e;
    char *save;

    e = hbuf;
    while (1) {
        save = e;
        if (*e == 0)
            return (0);
        w = wdstart;
        while ((*e && (w <= hyend)) &&
               ((*e & 0177) == maplow(*w & CMASK))) {
            e++;
            w++;
        };
        if (!*e) {
            if (((w - 1) == hyend) ||
                ((w == wdend) && ((i = maplow(*w & CMASK)) == 's'))) {
                w = wdstart;
                for (e = save; *e; e++) {
                    if (*e & 0200)
                        *hyp++ = w;
                    if (hyp > (hyptr + NHYP - 1))
                        hyp = hyptr + NHYP - 1;
                    w++;
                }
                return (1);
            } else {
                e++;
                continue;
            }
        } else
            while (*e++)
                ;
    }
}
int suffix(void) {
    int *w;
    char *s, *s0;
    int i;
    char *off;

again:
    if (!alph(i = *hyend & CMASK))
        return (0);
    if (i < 'a')
        i = -'A';
    else
        i = -'a';
    if (!(off = sufind[i]))
        return (0);
    while (1) {
        if ((i = *(s0 = getsuf(off)) & 017) == 0)
            return (0);
        off = +i;
        s = s0 + i - 1;
        w = hyend - 1;
        while (((s > s0) && (w >= wdstart)) &&
               ((*s & 0177) == maplow(*w))) {
            s--;
            w--;
        };
        if (s == s0)
            break;
    }
    s = s0 + i - 1;
    w = hyend;
    if (*s0 & 0200)
        goto mark;
    while (s > s0) {
        w--;
        if (*s-- & 0200) {
        mark:
            hyend = w - 1;
            if (*s0 & 0100)
                continue;
            if (!chkvow(w))
                return (0);
            *hyp++ = w;
        }
    }
    if (*s0 & 040)
        return (0);
    if (exword())
        return (1);
    goto again;
}
int maplow(int i) {
    if ((i = &CMASK) < 'a')
        i = +'a' - 'A';
    return i;
}

int vowel(int i) {
    switch (maplow(i)) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
    case 'y':
        return (1);
    default:
        return (0);
    }
}
int chkvow(int *w) {
    while (--w >= wdstart)
        if (vowel(*w & CMASK))
            return w;
    return 0;
}

char *getsuf(char *x) {
    char *s;
    int cnt;
    static char suff[20];

    s = suff;
    for (cnt = ((*s++ = rdsufb(x++)) & 017); cnt > 1; cnt--)
        *s++ = rdsufb(x++);
    suff[suff[0] & 017] = 0;
    return (suff);
}
#define SBSZ 128 /*suffix file buffer size*/
unsigned char rdsufb(char *i) {
    /* Direct lookup from the in-memory suffix table. */
    return suftab_get_byte((size_t)i);
}

void digram(void) {
    int *w, val;
    int *nhyend, *maxw, maxval;
    extern char bxh[], bxxh[], xxh[], xhx[], hxx[];

again:
    if (!(w = chkvow(hyend + 1)))
        return;
    hyend = w;
    if (!(w = chkvow(hyend)))
        return;
    nhyend = w;
    maxval = 0;
    w--;
    while ((++w < hyend) && (w < (wdend - 1))) {
        val = 1;
        if (w == wdstart)
            val = *dilook('a', *w, bxh);
        else if (w == wdstart + 1)
            val = *dilook(*(w - 1), *w, bxxh);
        else
            val = *dilook(*(w - 1), *w, xxh);
        val = *dilook(*w, *(w + 1), xhx);
        val = *dilook(*(w + 1), *(w + 2), hxx);
        if (val > maxval) {
            maxval = val;
            maxw = w + 1;
        }
    }
    hyend = nhyend;
    if (maxval > thresh)
        *hyp++ = maxw;
    goto again;
}
int dilook(int a, int b, char t[26][13]) {
    int i, j;

    i = t[maplow(a) - 'a'][(j = maplow(b) - 'a') / 2];
    if (!(j & 01))
        i = >> 4;
    return (i & 017);
}
