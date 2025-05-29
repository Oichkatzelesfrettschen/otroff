#include "tdef.h"
#include "t.h"
#include "tw.h"
#include "proto.h"

/*
troff6.c

width functions, sizes and fonts
*/

extern int eschar;
extern int widthp;
extern int ohc;
extern int xfont;
extern int smnt;
extern int setwdf;
extern char trtab[];
extern int chbits;
extern int nonumb;
extern int noscale;
extern int font;
extern int font1;
extern int pts;
extern int sps;
extern int nlflg;
extern int nform;
extern int dfact;
extern int dfactd;
extern int lss;
extern int lss1;
extern int vflag;
extern int ch0;
extern int level;
extern int ch;
extern int res;
extern int xxx;
/* Default font labels. */
int fontlab[] = {'R', 'I', 'B', 'S', 0};

/*
 * Return the width of character `c`.
 */
int width(int c) {
    int i, j, k;

    j = c;
    k = 0;
    if (j & MOT) {
        if (j & VMOT)
            goto rtn;
        k = j & ~MOTV;
        if (j & NMOT)
            k = -k;
        goto rtn;
    }
    if ((i = (j & CMASK)) == 010) {
        k = -widthp;
        goto rtn;
    }
    if (i == PRESC)
        i = eschar;
    if ((i == ohc) ||
        (i >= 0370))
        goto rtn;
    if (j & ZBIT)
        goto rtn;
    i = trtab[i] & BMASK;
    if (i < 040)
        goto rtn;
    k = (*(t.codetab[i - 32]) & 0177) * t.Char;
    widthp = k;
rtn:
    return (k);
}
/*
 * Return the numeric code for the next special character request.
 */
int setch(void) {
    int i, *j, k;
    extern int chtab[];

    if ((i = getrq()) == 0)
        return (0);
    for (j = chtab; *j != i; j++)
        if (*(j++) == 0)
            return (0);
    k = *(++j) | chbits;
    return (k);
}
/*
 * Search `j` for font number matching `i`.
 */
int find(int i, int j[]) {
    int k;

    if (((k = i - '0') >= 1) && (k <= 4) && (k != smnt))
        return (--k);
    for (k = 0; j[k] != i; k++)
        if (j[k] == 0)
            return (-1);
    return (k);
}
/*
 * Update `chbits` to reflect the current point size and font.
 */
void mchbits(void) {
    int i, j, k;

    chbits = (((pts) << 2) | font) << (BYTE + 1);
    sps = width(' ' | chbits);
}
/*
 * Parse and apply a point-size change request.
 */
void setps(void) {
    int i, j;

    if ((((i = getch() & CMASK) == '+') || (i == '-')) &&
        (((j = (ch = getch() & CMASK) - '0') >= 0) && (j <= 9))) {
        ch = 0;
        return;
    }
    if ((i = -'0') == 0) {
        return;
    }
    if ((i > 0) && (i <= 9)) {
        if ((i <= 3) &&
            ((j = (ch = getch() & CMASK) - '0') >= 0) && (j <= 9)) {
            i = 10 * i + j;
            ch = 0;
        }
    }
}
/*
 * Handle the \f request to change fonts.
 */
void caseft(void) {
    skip();
    setfont(1);
}

/*
 * Switch to font position `a`.
 */
void setfont(int a) {
    int i, j;

    if (a)
        i = getrq();
    else
        i = getsn();
    if (!i || (i == 'P')) {
        j = font1;
        goto s0;
    }
    if (i == 'S')
        return;
    if ((j = find(i, fontlab)) == -1)
        return;
s0:
    font1 = font;
    font = j;
    mchbits();
}
/*
 * Compute width information for a string.
 */
void setwd(void) {
    int i, base, wid;
    int delim, em, k;
    int savlevel, savhp, savfont, savfont1;

    base = v.st = v.sb = wid = v.ct = 0;
    if ((delim = getch() & CMASK) & MOT)
        return;
    savhp = v.hp;
    savlevel = level;
    v.hp = level = 0;
    savfont = font;
    savfont1 = font1;
    setwdf++;
    while ((((i = getch()) & CMASK) != delim) && !nlflg) {
        wid = +width(i);
        if (!(i & MOT)) {
            em = 2 * t.Halfline;
        } else if (i & VMOT) {
            k = i & ~MOTV;
            if (i & NMOT)
                k = -k;
            base = -k;
            em = 0;
        } else
            continue;
        if (base < v.sb)
            v.sb = base;
        if ((k = base + em) > v.st)
            v.st = k;
    }
    nform = 0;
    setn1(wid);
    v.hp = savhp;
    level = savlevel;
    font = savfont;
    font1 = savfont1;
    mchbits();
    setwdf = 0;
}
/* Vertical motion. */
int vmot(void) {
    dfact = lss;
    vflag++;
    return mot();
}

/* Horizontal motion. */
int hmot(void) {
    dfact = EM;
    return mot();
}

/*
 * Parse a motion request and return a motion encoded value.
 */
int mot(void) {
    int i, j;

    j = HOR;
    getch(); /*eat delim*/
    if (i = atoi()) {
        if (vflag)
            j = VERT;
        i = makem(quant(i, j));
    }
    getch();
    vflag = 0;
    dfact = 1;
    return (i);
}
/*
 * Return a half-line motion for underline or reverse operations.
 */
int sethl(int k) {
    int i;

    i = t.Halfline;
    if (k == 'u')
        i = -i;
    else if (k == 'r')
        i = -2 * i;
    vflag++;
    i = makem(i);
    vflag = 0;
    return (i);
}
/*
 * Encode motion `i` for output.
 */
int makem(int i) {
    int j;

    if ((j = i) < 0)
        j = -j;
    j = (j & ~MOTV) | MOT;
    if (i < 0)
        j |= NMOT;
    if (vflag)
        j |= VMOT;
    return (j);
}
/* Assign a font name to a position. */
void casefp(void) {
    int i, j, k;

    skip();
    if (((i = (getch() & CMASK) - '0' - 1) < 0) || (i > 3))
        return;
    if (skip() || !(j = getrq()))
        return;
    fontlab[i] = j;
}
/* Set vertical spacing. */
void casevs(void) {
    int i;

    skip();
    vflag++;
    dfact = INCH; /*default scaling is points!*/
    dfactd = 72;
    res = VERT;
    i = inumb(&lss);
    if (nonumb)
        i = lss1;
    if (i < VERT)
        i = VERT;
    lss1 = lss;
    lss = i;
}
/*
 * Process extra leading size specification.
 */
int xlss(void) {
    int i, j;

    getch();
    dfact = lss;
    i = quant(atoi(), VERT);
    dfact = 1;
    getch();
    if ((j = i) < 0)
        j = -j;
    ch0 = ((j & 03700) << 3) | HX;
    if (i < 0)
        ch0 |= 040000;
    return (((j & 077) << 9) | LX);
}
void caseps(void) {}
void caselg(void) {}
void casecs(void) {}
void casebd(void) {}
void casess(void) {}
/* No-op ligature handler used by NROFF. */
int getlg(int i) {
    return i;
}
