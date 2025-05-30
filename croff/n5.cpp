#include "cxx23_scaffold.hpp"
/*
 * n5.c - Miscellaneous processing requests for troff/nroff
 *
 * This file contains various text formatting commands and their implementations
 * including adjustment, indentation, line length, page layout, conditional
 * processing, and other text formatting utilities.
 */

#include "tdef.hpp" // core definitions
#include "env.hpp"  // environment structure
#include "t.hpp"    // troff header
#include <stdio.h>

/* External variable declarations */
extern int ascii;
extern int nonumb;
extern int admod;
extern int ad;
extern int fi;
extern int cc;
extern int c2;
extern int ohc;
extern int tabc;
extern int dotc;
extern int pendnf;
extern int hyf;
extern int ce;
extern int po;
extern int po1;
extern int nc;
extern int in;
extern int un;
extern int un1;
extern int in1;
extern int ll;
extern int ll1;
extern int lt;
extern int lt1;
extern int nlist[NTRAP];
extern int mlist[NTRAP];
extern int lgf;
extern int pl;
extern int npn;
extern int npnflg;
extern int *frame;
extern struct env *dip;
extern int copyf;
extern char nextf[];
extern int trap;
extern int lss;
extern int em;
extern int evlist[EVLSZ];
extern int evi;
extern int ibf;
extern int ev;
extern int ch;
extern int nflush;
extern int tty;
extern int ttys[3];
extern int quiet;
extern int iflg;
extern int eschar;
extern int lit;
extern int *litlev;
extern int ls;
extern int ls1;
extern int tabtab[];
extern char trtab[];
extern int ul;
extern int cu;
extern int sfont;
extern int font;
extern int fontlab[];
extern int it;
extern int itmac;
extern int noscale;
extern int ic;
extern int icf;
extern int ics;
extern int *vlist;
extern int sv;
extern int esc;
extern int nn;
extern int nms;
extern int ndf;
extern int lnmod;
extern int ni;
extern int lnsize;
extern int nb;
extern int offset;
extern int nlflg;
extern int apts, apts1, pts, pts1, font1;
extern int ulfont;
extern int ulbit;
extern int error;
extern int nmbits;
extern int chbits;
extern int tdelim;
extern int xxx;

/* Global arrays */
int iflist[NIF];
int ifx;

/* External control table structure */
extern struct contab {
    int rq;
    int (*f)(void);
} contab[NM];

/* Function prototypes - static (internal) functions */
static int max(int aa, int bb);
static int eatblk(int right, int left);
static int cmpstr(int delim);
static void getnm(int *p, int min);
static void casead(void);
static void casena(void);
static void casefi(void);
static void casenf(void);

/* Function prototypes - external functions */
int chget(int c);
int findn(int i);
void tbreak(void);
int tatoi(void);
int hnumb(int *ptr);
int inumb(int *ptr);
int vnumb(int *ptr);
void setnel(void);
int getrq(void);
void prstrfl(char *s);
int skip(void);
int getch(void);
void newline(int x);
int findt1(void);
void flusho(void);
void flushi(void);
void done2(int x);
void edone(int x);
int eat(int c);
void wbfl(void);
int alloc(void);
void wbf(int i);
void wbt(int x);
void mchbits(void);
int rbf0(int p);
int incoff(int p);
void troff_free(int x);
void getname(void);
void collect(void);
void pushi(int x);
void popi(void);
void stty(int fd, int *args);
void seek(int fd, int offset, int whence);
void prstr(char *s);
int find(int x, int *table);
int findr(int x);
void eject(int x);

/* Additional function prototypes for this file */
void casers(void);
void casens(void);
void casecc(void);
void casec2(void);
void casehc(void);
void casetc(void);
void caselc(void);
void casehy(void);
void casenh(void);
void casece(void);
void casein(void);
void casell(void);
void caselt(void);
void caseti(void);
void casels(void);
void casepo(void);
void casepl(void);
void casewh(void);
void casech(void);
void casepn(void);
void casebp(void);
void casetm(int x);
void casesp(int a);
void casert(void);
void caseem(void);
void casefl(void);
void caseev(void);
void caseie(void);
void caseif(int x);
void caserd(void);
int rdtty(void);
void caseec(void);
void caseeo(void);
void caseli(void);
void caseta(void);
void casene(void);
void casetr(void);
void casecu(void);
void caseul(void);
void casemc(void);
void casemk(void);
void casesv(void);
void casenn(void);
void caseab(void);

/*
 * Return the larger of two integers
 */
static int max(int aa, int bb) {
    if (aa > bb)
        return aa;
    else
        return bb;
}

/*
 * Adjust line justification
 * Sets adjustment mode based on input character
 */
static void casead(void) {
    register int i;

    ad = 1;
    /* leave admod alone */
    if (skip())
        return;

    switch (i = getch() & CMASK) {
    case 'r': /* right adj, left ragged */
        admod = 2;
        break;
    case 'l': /* left adj, right ragged */
        admod = ad = 0; /* same as casena */
        break;
    case 'c': /* centered adj */
        admod = 1;
        break;
    case 'b':
    case 'n':
        admod = 0;
        break;
    case '0':
    case '2':
    case '4':
        ad = 0;
        /* fall through */
    case '1':
    case '3':
    case '5':
        admod = (i - '0') / 2;
        break;
    }
}

/*
 * Disable line adjustment
 */
static void casena(void) {
    ad = 0;
}

/*
 * Enable fill mode
 */
static void casefi(void) {
    tbreak();
    fi++;
    pendnf = 0;
    lnsize = LNSIZE;
}

/*
 * Disable fill mode
 */
static void casenf(void) {
    tbreak();
    fi = 0;
    /* Increase line size when not filling */
    lnsize = LNSIZE + WDSIZE;
}

/*
 * Reset number of blank lines
 */
void casers(void) {
    dip->nls = 0;
}

/*
 * Increment number of blank lines
 */
void casens(void) {
    dip->nls++;
}

/*
 * Fetch a character argument or return the default c
 */
int chget(int c) {
    register int i;

    if (skip() ||
        ((i = getch()) & MOT) ||
        ((i & CMASK) == ' ') ||
        ((i & CMASK) == '\n')) {
        ch = i;
        return c;
    } else {
        return i & BMASK;
    }
}

/*
 * Set control character (default '.')
 */
void casecc(void) {
    cc = chget('.');
}

/*
 * Set no-break control character (default apostrophe)
 */
void casec2(void) {
    c2 = chget('\'');
}

/*
 * Set hyphenation character
 */
void casehc(void) {
    ohc = chget(OHC);
}

/*
 * Set tab character
 */
void casetc(void) {
    tabc = chget(0);
}

/*
 * Set leader character
 */
void caselc(void) {
    dotc = chget(0);
}

/*
 * Set hyphenation mode
 */
void casehy(void) {
    register int i;

    hyf = 1;
    if (skip())
        return;

    noscale++;
    i = tatoi();
    noscale = 0;
    if (nonumb)
        return;
    hyf = max(i, 0);
}

/*
 * Disable hyphenation
 */
void casenh(void) {
    hyf = 0;
}

/*
 * Center lines
 */
void casece(void) {
    register int i;

    noscale++;
    skip();
    i = max(tatoi(), 0);
    if (nonumb)
        i = 1;
    tbreak();
    ce = i;
    noscale = 0;
}

/*
 * Set indentation
 */
void casein(void) {
    register int i;

    if (skip())
        i = in1;
    else
        i = max(hnumb(&in), 0);
    tbreak();
    in1 = in;
    in = i;
    if (!nc) {
        un = in;
        setnel();
    }
}

/*
 * Set line length
 */
void casell(void) {
    register int i;

    if (skip())
        i = ll1;
    else
        i = max(hnumb(&ll), INCH / 10);
    ll1 = ll;
    ll = i;
    setnel();
}

/*
 * Set title length
 */
void caselt(void) {
    register int i;

    if (skip())
        i = lt1;
    else
        i = max(hnumb(&lt), 0);
    lt1 = lt;
    lt = i;
}

/*
 * Temporary indent
 */
void caseti(void) {
    register int i;

    if (skip())
        return;
    i = max(hnumb(&in), 0);
    tbreak();
    un1 = i;
    setnel();
}

/*
 * Set line spacing
 */
void casels(void) {
    register int i;

    noscale++;
    if (skip())
        i = ls1;
    else
        i = max(inumb(&ls), 1);
    ls1 = ls;
    ls = i;
    noscale = 0;
}

/*
 * Set page offset
 */
void casepo(void) {
    register int i;

    if (skip())
        i = po1;
    else
        i = max(hnumb(&po), 0);
    po1 = po;
    po = i;
#ifndef NROFF
    if (!ascii)
        esc = po - po1;
#endif
}

/*
 * Set page length
 */
void casepl(void) {
    register int i;

    skip();
    if ((i = vnumb(&pl)) == 0)
        pl = 11 * INCH; /* 11 inches default */
    else
        pl = i;
    if (v.nl > pl)
        v.nl = pl;
}

/*
 * Set trap
 */
void casewh(void) {
    register int i, j, k;

    lgf++;
    skip();
    i = vnumb(0);
    if (nonumb)
        return;
    skip();
    j = getrq();
    if ((k = findn(i)) != NTRAP) {
        mlist[k] = j;
        return;
    }
    for (k = 0; k < NTRAP; k++)
        if (mlist[k] == 0)
            break;
    if (k == NTRAP) {
        prstrfl("Cannot plant trap.\n");
        return;
    }
    mlist[k] = j;
    nlist[k] = i;
}

/*
 * Change trap
 */
void casech(void) {
    register int i, j, k;

    lgf++;
    skip();
    if (!(j = getrq()))
        return;
    else
        for (k = 0; k < NTRAP; k++)
            if (mlist[k] == j)
                break;
    if (k == NTRAP)
        return;
    skip();
    i = vnumb(0);
    if (nonumb)
        mlist[k] = 0;
    nlist[k] = i;
}

/*
 * Find trap number associated with page position i
 */
int findn(int i) {
    register int k;

    for (k = 0; k < NTRAP; k++)
        if ((nlist[k] == i) && (mlist[k] != 0))
            break;
    return k;
}

/*
 * Set page number
 */
void casepn(void) {
    register int i;

    skip();
    noscale++;
    i = max(inumb(&v.pn), 0);
    noscale = 0;
    if (!nonumb) {
        npn = i;
        npnflg++;
    }
}

/*
 * Begin page
 */
void casebp(void) {
    register int i, savframe;

    if (dip->op)
        return;
    savframe = *frame;
    skip();
    if ((i = vnumb(&v.pn)) < 0)
        i = 0;
    tbreak();
    if (!nonumb) {
        npn = i;
        npnflg++;
    } else if (dip->nls) {
        return;
    }
    eject(savframe);
}

/*
 * Print a message string; x indicates user abort when non-zero
 */
void casetm(int x) {
    register int i;
    char tmbuf[NTM];

    lgf++;
    copyf++;
    if (skip() && x)
        prstrfl("User Abort.");
    for (i = 0; i < NTM - 2;)
        if ((tmbuf[i++] = getch()) == '\n')
            break;
    if (i == NTM - 2)
        tmbuf[i++] = '\n';
    tmbuf[i] = 0;
    prstrfl(tmbuf);
    copyf--;
}

/*
 * Space the requested distance
 */
void casesp(int a) {
    register int i, j, savlss;

    tbreak();
    if (dip->nls || trap)
        return;
    i = findt1();
    if (!a) {
        skip();
        j = vnumb(0);
        if (nonumb)
            j = lss;
    } else {
        j = a;
    }
    if (j == 0)
        return;
    if (i < j)
        j = i;
    savlss = lss;
    if (dip->op)
        i = dip->dnl;
    else
        i = v.nl;
    if ((i + j) < 0)
        j = -i;
    lss = j;
    newline(0);
    lss = savlss;
}

/*
 * Reverse line feed
 */
void casert(void) {
    register int a, *p;

    skip();
    if (dip->op)
        p = &dip->dnl;
    else
        p = &v.nl;
    a = vnumb(p);
    if (nonumb)
        a = dip->mkline;
    if ((a < 0) || (a >= *p))
        return;
    nb++;
    casesp(a - *p);
}

/*
 * Set end macro
 */
void caseem(void) {
    lgf++;
    skip();
    em = getrq();
}

/*
 * Flush output
 */
void casefl(void) {
    tbreak();
    flusho();
}

/*
 * Environment switch
 */
void caseev(void) {
    register int nxev;
    extern int block;

    if (skip()) {
        if (evi == 0)
            return;
        nxev = evlist[--evi];
    } else {
        noscale++;
        nxev = tatoi();
        noscale = 0;
        if (nonumb) {
            if (evi == 0)
                return;
            nxev = evlist[--evi];
        } else {
            flushi();
            if ((nxev >= NEV) || (nxev < 0) || (evi >= EVLSZ)) {
                prstrfl("Cannot do ev.\n");
                if (error)
                    done2(040);
                else
                    edone(040);
                return;
            }
            evlist[evi++] = ev;
        }
    }

    if (ev == nxev)
        return;

    seek(ibf, ev * EVS * 2, 0);
    write(ibf, &block, EVS * 2);
    seek(ibf, nxev * EVS * 2, 0);
    read(ibf, &block, EVS * 2);
    ev = nxev;
}

/*
 * If-else conditional
 */
void caseie(void) {
    if (ifx >= NIF) {
        prstr("if-else overflow.\n");
        ifx = 0;
        edone(040);
    }
    caseif(1);
    ifx++;
}

/*
 * Handle conditional requests
 */
void caseif(int x) {
    register int i, notflag, true_cond;

    if (x == 2) {
        notflag = 0;
        true_cond = iflist[ifx];
        goto i1;
    }
    true_cond = 0;
    skip();
    if (((i = getch()) & CMASK) == '!') {
        notflag = 1;
    } else {
        notflag = 0;
        ch = i;
    }
    i = tatoi();
    if (!nonumb) {
        if (i > 0)
            true_cond++;
        goto i1;
    }
    switch ((i = getch()) & CMASK) {
    case 'e':
        if (!(v.pn & 01))
            true_cond++;
        break;
    case 'o':
        if (v.pn & 01)
            true_cond++;
        break;
#ifdef NROFF
    case 'n':
        true_cond++;
        /* fall through */
    case 't':
#endif
#ifndef NROFF
    case 't':
        true_cond++;
        /* fall through */
    case 'n':
#endif
    case ' ':
        break;
    default:
        true_cond = cmpstr(i);
    }
i1:
    if (notflag)
        true_cond = !true_cond;
    if (x == 1)
        iflist[ifx] = !true_cond;
    if (true_cond) {
    i2:
        do {
            v.hp = 0;
        } while (((i = getch()) & CMASK) == ' ');
        if ((i & CMASK) == LEFT)
            goto i2;
        ch = i;
        nflush++;
    } else {
        copyf++;
        if (eat(LEFT) == LEFT) {
            while (eatblk(RIGHT, LEFT) != RIGHT)
                nlflg = 0;
        }
        copyf--;
    }
}

/*
 * Consume input up to matching delimiter
 */
static int eatblk(int right, int left) {
    register int i;

e0:
    while (((i = getch() & CMASK) != right) &&
           (i != left) &&
           (i != '\n'))
        ;
    if (i == left) {
        while ((i = eatblk(right, left)) != right)
            nlflg = 0;
        goto e0;
    }
    return i;
}

/*
 * Compare a delimited string with data on input
 */
static int cmpstr(int delim) {
    register int i, j, p;
    int begin, cnt, k;
    int savapts, savapts1, savfont, savfont1,
        savpts, savpts1;

    if (delim & MOT)
        return 0;
    delim &= CMASK;
    if (dip->op)
        wbfl();
    if ((offset = begin = alloc()) == 0)
        return 0;
    cnt = 0;
    v.hp = 0;
    savapts = apts;
    savapts1 = apts1;
    savfont = font;
    savfont1 = font1;
    savpts = pts;
    savpts1 = pts1;
    while (((j = (i = getch()) & CMASK) != delim) && (j != '\n')) {
        wbf(i);
        cnt++;
    }
    wbt(0);
    k = !cnt;
    if (nlflg)
        goto rtn;
    p = begin;
    apts = savapts;
    apts1 = savapts1;
    font = savfont;
    font1 = savfont1;
    pts = savpts;
    pts1 = savpts1;
    mchbits();
    v.hp = 0;
    while (((j = (i = getch()) & CMASK) != delim) && (j != '\n')) {
        if (rbf0(p) != i) {
            eat(delim);
            k = 0;
            break;
        }
        p = incoff(p);
        k = !(--cnt);
    }
rtn:
    apts = savapts;
    apts1 = savapts1;
    font = savfont;
    font1 = savfont1;
    pts = savpts;
    pts1 = savpts1;
    mchbits();
    offset = dip->op;
    troff_free(begin);
    return k;
}

/*
 * Read from terminal
 */
void caserd(void) {
    lgf++;
    skip();
    getname();
    if (!iflg) {
        if (quiet) {
            ttys[2] &= ~ECHO;
            stty(0, ttys);
            prstrfl(""); /* bell */
        } else {
            if (nextf[0]) {
                prstr(nextf);
                prstr(":");
            } else {
                prstr(""); /* bell */
            }
        }
    }
    collect();
    tty++;
    pushi(-1);
}

/*
 * Read a single character from the terminal
 */
int rdtty(void) {
    int onechar;

    onechar = 0;
    if (read(0, &onechar, 1) == 1) {
        if (onechar == '\n')
            tty++;
        else
            tty = 1;
        if (tty != 3)
            return onechar;
    }
    popi();
    tty = 0;
    if (quiet) {
        ttys[2] |= ECHO;
        stty(0, ttys);
    }
    return 0;
}

/*
 * Set escape character
 */
void caseec(void) {
    eschar = chget('\\');
}

/*
 * Turn off escape character
 */
void caseeo(void) {
    eschar = 0;
}

/*
 * Literal input
 */
void caseli(void) {
    skip();
    lit = max(inumb(0), 1);
    litlev = frame;
    if ((!dip->op) && (v.nl == -1))
        newline(1);
}

/*
 * Set tab stops
 */
void caseta(void) {
    register int i, j;

    tabtab[0] = nonumb = 0;
    for (i = 0; ((i < (NTAB - 1)) && !nonumb); i++) {
        if (skip())
            break;
        tabtab[i] = max(hnumb(&tabtab[max(i - 1, 0)]), 0) & TMASK;
        if (!nonumb)
            switch (j = ch & CMASK) {
            case 'C':
                tabtab[i] |= CTAB;
                break;
            case 'R':
                tabtab[i] |= RTAB;
                break;
            default: /* includes L */
                break;
            }
        nonumb = ch = 0;
    }
    tabtab[i] = 0;
}

/*
 * Need space
 */
void casene(void) {
    register int i, j;

    skip();
    i = vnumb(0);
    if (nonumb)
        i = lss;
    if (i > (j = findt1())) {
        i = lss;
        lss = j;
        dip->nls = 0;
        newline(0);
        lss = i;
    }
}

/*
 * Character translation
 */
void casetr(void) {
    register int i, j;

    lgf++;
    skip();
    while ((i = getch() & CMASK) != '\n') {
        if ((i & MOT) || ((j = getch()) & MOT))
            return;
        if ((j &= CMASK) == '\n')
            j = ' ';
        trtab[i] = j;
    }
}

/*
 * Continuous underline
 */
void casecu(void) {
    cu++;
    caseul();
}

/*
 * Underline
 */
void caseul(void) {
    register int i;

    noscale++;
    if (skip())
        i = 1;
    else
        i = tatoi();
    if (ul && (i == 0)) {
        font = sfont;
        ul = cu = 0;
    }
    if (i) {
        if (!ul) {
            sfont = font;
            font = ulfont;
        }
        ul = i;
    }
    noscale = 0;
    mchbits();
}

/*
 * Margin character
 */
void casemc(void) {
    register int i;

    if (icf > 1)
        ic = 0;
    icf = 0;
    if (skip())
        return;
    ic = getch();
    icf = 1;
    skip();
    i = max(hnumb(0), 0);
    if (!nonumb)
        ics = i;
}

/*
 * Mark vertical position
 */
void casemk(void) {
    register int i, j;

    if (dip->op)
        j = dip->dnl;
    else
        j = v.nl;
    if (skip()) {
        dip->mkline = j;
        return;
    }
    i = getrq();
    if (i == 0 || i == -1) { /* Check for invalid or error return values */
        prstrfl("Error: Invalid request code.\n");
        return;
    }
    vlist[findr(i)] = j;
}

/*
 * Save vertical space
 */
void casesv(void) {
    register int i;

    skip();
    if ((i = vnumb(0)) < 0)
        return;
    if (nonumb)
        i = 1;
    sv += i;
}

/*
 * Read a numeric parameter with a minimum value
 */
static void getnm(int *p, int min) {
    register int i;

    *p = min; /* Initialize *p to the minimum value */
    eat(' ');
    if (skip())
        return;
    i = tatoi();
    if (nonumb)
        return;
    *p = max(i, min);
}

/*
 * Number lines
 */
void casenn(void) {
    noscale++;
    nn = max(tatoi(), 1);
    noscale = 0;
}

/*
 * caseab - abort processing
 *
 * Print the remainder of the input line as an error message and then
 * terminate processing cleanly.
 */
void caseab(void) {
    casetm(1); /* Output the user-supplied abort message */
    done2(0); /* Perform cleanup and exit */
}
