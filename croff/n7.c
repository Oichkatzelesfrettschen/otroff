/*
 * n7.c - Text Processing and Line Breaking Module
 * 
 * This module handles the core text processing functionality of troff including:
 * - Line breaking and justification
 * - Text flow and filling
 * - Character and word processing
 * - Page and line management
 * - Hyphenation and spacing
 * 
 * Part of the troff text formatting system originally developed at Bell Labs.
 * Modernized for C90 compliance while maintaining original functionality.
 * 
 * Key Functions:
 * - tbreak(): Break current line and output accumulated text
 * - text(): Process incoming text characters with filling
 * - nofill(): Output text without filling
 * - newline(): Handle newline processing and page management
 * - movword(): Move words from input to output buffer
 * - getword(): Extract next word from input stream
 * 
 * Design Principles:
 * - Maintains character-level precision for typographic control
 * - Handles both fill and no-fill modes
 * - Supports line numbering and adjustment
 * - Integrates with diversion and macro systems
 * - Provides robust error handling and overflow protection
 */

#include "tdef.h"
#include "env.h"
#include "t.h"
#include "tw.h"
#include "proto.h"

#ifdef NROFF
#define GETCH gettch
#endif
#ifndef NROFF
#define GETCH getch
#endif

/* Forward declarations for C90 compliance */
extern int getch(void);
extern int width(int c);
extern void pchar(int c);
extern void pchar1(int c);
extern int fnumb(int n, void (*func)(int));
extern void mchbits(void);
extern void flushi(void);
extern int control(int a, int b);
extern void flusho(void);
extern void done1(int status);
extern void done2(int status);
extern void hyphen(int *wp);
extern void casesp(int n);
extern int quant(int n, int m);
extern void hsend(void);
extern int makem(int i);

/* External variables from other modules */
extern struct env *dip;
extern struct v;
extern int pl;
extern int trap;
extern int flss;
extern int npnflg;
extern int npn;
extern int stop;
extern int nflush;
extern int *ejl;
extern int ejf;
extern int ascii;
extern int donef;
extern int nc;
extern int wch;
extern int dpn;
extern int ndone;
extern int lss;
extern int pto;
extern int pfrom;
extern int print;
extern int nlist[NTRAP];
extern int mlist[NTRAP];
extern int *frame;
extern int *stk;
extern int *pnp;
extern int nb;
extern int ic;
extern int icf;
extern int ics;
extern int ne;
extern int ll;
extern int un;
extern int un1;
extern int in;
extern int ls;
extern int spread;
extern int totout;
extern int nwd;
extern int *pendw;
extern int *linep;
extern int line[];
extern int lastl;
extern int ch;
extern int ce;
extern int fi;
extern int nlflg;
extern int pendt;
extern int sps;
extern int adsp;
extern int pendnf;
extern int over;
extern int adrem;
extern int nel;
extern int ad;
extern int totout;
extern int ohc;
extern int hyoff;
extern int nhyp;
extern int spflg;
extern int word[];
extern int *wordp;
extern int wne;
extern int chbits;
extern int cwidth;
extern int widthp;
extern int hyf;
extern int xbitf;
extern int vflag;
extern int ul;
extern int cu;
extern int font;
extern int sfont;
extern int it;
extern int itmac;
extern int *hyptr[NHYP];
extern int **hyp;
extern int *wdstart, *wdend;
extern int lnmod;
extern int admod;
extern int nn;
extern int nms;
extern int ndf;
extern int ni;
extern int nform;
extern int lnsize;
extern int po;
extern int ulbit;
extern int *vlist;
extern int nrbits;
extern int nmbits;
extern int xxx;

/* File-local variables and static data */
int brflg;
static int hys;
static int swp;
static char Sccsid[] = "@(#)n7.c  1.2 of 3/4/77";

/* Function prototypes for this module */
void tbreak(void);
void donum(void);
void text(void);
void nofill(void);
void callsp(void);
void ckul(void);
void storeline(int c, int w);
void newline(int a);
int findn1(int a);
void chkpn(void);
int findt(int a);
int findt1(void);
void eject(int *a);
int movword(void);
void horiz(int i);
void setnel(void);
int getword(int x);
void storeword(int c, int w);
int gettch(void);

/* Additional external function declarations needed for C90 compliance */
extern int findr(int r);
extern int findn(int a);
extern int prstrfl(char *s);
extern void dostop(void);

/* Missing function prototypes */
void tbreak1(void);

/*
 * Break the current line and output accumulated text.
 * 
 * This function handles the actual line breaking and justification:
 * - Processes pending words and spaces
 * - Applies justification and adjustment
 * - Outputs characters with proper spacing
 * - Handles line numbering if enabled
 * - Manages inter-character spacing
 */
void tbreak(void) {
    register int *i, j, pad;

    trap = 0;
    if (nb)
        return;
    if ((!dip->op) && (v.nl == -1)) {
        newline(1);
        return;
    }
    if (!nc) {
        setnel();
        if (!wch)
            return;
        if (pendw)
            getword(1);
        movword();
    } else if (pendw && !brflg) {
        getword(1);
        movword();
    }
    *linep = dip->nls = 0;
#ifdef NROFF
    if (!dip->op)
        horiz(po);
#endif
    if (lnmod)
        donum();
    lastl = ne;
    if (brflg != 1) {
        totout = 0;
    } else if (ad) {
        if ((lastl = (ll - un)) < ne)
            lastl = ne;
    }
    if (admod && ad && (brflg != 2)) {
        lastl = ne;
        adsp = adrem = 0;
#ifdef NROFF
        if (admod == 1)
            un += quant(nel / 2, t.Adj);
#endif
#ifndef NROFF
        if (admod == 1)
            un += nel / 2;
#endif
        else if (admod == 2)
            un += nel;
    }
    totout++;
    brflg = 0;
    if (lastl > dip->maxl)
        dip->maxl = lastl;
    horiz(un);
    for (i = line; nc > 0;) {
        if (((j = *i++) & CMASK) == ' ') {
            pad = 0;
            do {
                pad += width(j);
                nc--;
            } while (((j = *i++) & CMASK) == ' ');
            i--;
            pad += adsp;
            if (adrem) {
                if (adrem < 0) {
#ifdef NROFF
                    pad -= t.Adj;
                    adrem += t.Adj;
                } else if ((totout & 01) ||
                           ((adrem / t.Adj) >= (--nwd))) {
                    pad += t.Adj;
                    adrem -= t.Adj;
#endif
#ifndef NROFF
                    pad--;
                    adrem++;
                } else {
                    pad++;
                    adrem--;
#endif
                }
            }
            horiz(pad);
        } else {
            pchar(j);
            nc--;
        }
    }
    if (ic) {
        if ((j = ll - un - lastl + ics) > 0)
            horiz(j);
        pchar(ic);
    }
    if (icf)
        icf++;
    else
        ic = 0;
    ne = nwd = 0;
    un = in;
    setnel();
    newline(0);
    if (dip->op) {
        if (dip->dnl > dip->hnl)
            dip->hnl = dip->dnl;
    } else {
        if (v.nl > dip->hnl)
            dip->hnl = v.nl;
    }
    for (j = ls - 1; (j > 0) && !trap; j--)
        newline(0);
    spread = 0;
}
/*
 * Output line numbering.
 * 
 * This function generates line numbers for each text line:
 * - Formats line numbers with proper spacing
 * - Handles line number increment and display
 * - Manages numbering frequency control
 * - Applies proper fonts and character formatting
 */
void donum(void) {
    register int i, nw;

    nrbits = nmbits;
    nw = width('1' | nrbits);
    if (nn) {
        nn--;
        goto d1;
    }
    if (v.ln % ndf) {
        v.ln++;
    d1:
        un += nw * (3 + nms + ni);
        return;
    }
    i = 0;
    if (v.ln < 100)
        i++;
    if (v.ln < 10)
        i++;
    horiz(nw * (ni + i));
    nform = 0;
    fnumb(v.ln, pchar);
    un += nw * nms;
    v.ln++;
}
/* Process incoming text characters. */
void text(void) {
    int i;
    static int spcnt;

    nflush++;
    if ((!dip->op) && (v.nl == -1)) {
        newline(1);
        return;
    }
    setnel();
    if (ce || !fi) {
        nofill();
        return;
    }
    if (pendw)
        goto t4;
    if (pendt)
        if (spcnt)
            goto t2;
        else
            goto t3;
    pendt++;
    if (spcnt)
        goto t2;
    while (((i = GETCH()) & CMASK) == ' ')
        spcnt++;
    if (nlflg) {
    t1:
        nflush = pendt = ch = spcnt = 0;
        callsp();
        return;
    }
    ch = i;
    if (spcnt) {
    t2:
        tbreak();
        if (nc || wch)
            goto rtn;
        un = +spcnt * sps;
        spcnt = 0;
        setnel();
        if (trap)
            goto rtn;
        if (nlflg)
            goto t1;
    }
t3:
    if (spread)
        goto t5;
    if (pendw || !wch) {
    t4:
        if (getword(0))
            goto t6;
    }
    if (!movword())
        goto t3;
t5:
    if (nlflg)
        pendt = 0;
    adsp = adrem = 0;
    if (ad) {
        adsp = nel / (nwd - 1);
#ifdef NROFF
        adsp = (adsp / t.Adj) * t.Adj;
#endif
        adrem = nel - adsp * (nwd - 1);
    }
    brflg = 1;
    tbreak();
    spread = 0;
    if (!trap)
        goto t3;
    if (!nlflg)
        goto rtn;
t6:
    pendt = 0;
    ckul();
rtn:
    nflush = 0;
}
/* Output a line without filling. */
void nofill(void) {
    int i, j;

    if (!pendnf) {
        over = 0;
        tbreak();
        if (trap)
            goto rtn;
        if (nlflg) {
            ch = nflush = 0;
            callsp();
            return;
        }
        adsp = adrem = 0;
        nwd = 10000;
    }
    while ((j = ((i = GETCH()) & CMASK)) != '\n') {
        if (j == ohc)
            continue;
        if (j == CONT) {
            pendnf++;
            nflush = 0;
            flushi();
            ckul();
            return;
        }
        storeline(i, -1);
    }
    if (ce) {
        ce--;
        if ((i = quant(nel / 2, HOR)) > 0)
            un = +i;
    }
    if (!nc)
        storeline(FILLER, 0);
    brflg = 2;
    tbreak();
    ckul();
rtn:
    pendnf = nflush = 0;
}
/* Call the space-handling routine. */
void callsp(void) {
    int i;

    if (flss)
        i = flss;
    else
        i = lss;
    flss = 0;
    casesp(i);
}
/* Handle cleanup of underline and italic state. */
void ckul(void) {
    if (ul && (--ul == 0)) {
        cu = 0;
        font = sfont;
        mchbits();
    }
    if (it && (--it == 0) && itmac)
        control(itmac, 0);
}
/* Store a character and its width into the output line buffer. */
void storeline(int c, int w) {
    int i;

    if ((c & CMASK) == JREG) {
        if ((i = findr(c >> BYTE)) != -1)
            vlist[i] = ne;
        return;
    }
    if (linep >= (line + lnsize - 1)) {
        if (!over) {
            prstrfl("Line overflow.\n");
            over++;
            c = 0343;
            w = -1;
            goto s1;
        }
        return;
    }
s1:
    if (w == -1)
        w = width(c);
    ne = +w;
    nel = -w;
    *linep++ = c;
    nc++;
}
/* Output a newline. */
void newline(int a) {
    int i, j, nlss;
    int opn;

    if (a)
        goto nl1;
    if (dip->op) {
        j = lss;
        pchar1(FLSS);
        if (flss)
            lss = flss;
        i = lss + dip->blss;
        dip->dnl = +i;
        pchar1(i);
        pchar1('\n');
        lss = j;
        dip->blss = flss = 0;
        if (dip->alss) {
            pchar1(FLSS);
            pchar1(dip->alss);
            pchar1('\n');
            dip->dnl = +dip->alss;
            dip->alss = 0;
        }
        if (dip->ditrap && !dip->ditf &&
            (dip->dnl >= dip->ditrap) && dip->dimac) {
            if (control(dip->dimac, 0)) {
                trap++;
                dip->ditf++;
            }
        }
        return;
    }
    j = lss;
    if (flss)
        lss = flss;
    nlss = dip->alss + dip->blss + lss;
    v.nl = +nlss;
#ifndef NROFF
    if (ascii) {
        dip->alss = dip->blss = 0;
    }
#endif
    pchar1('\n');
    flss = 0;
    lss = j;
    if (v.nl < pl)
        goto nl2;
nl1:
    ejf = dip->hnl = v.nl = 0;
    ejl = frame;
    if (donef) {
        if ((!nc && !wch) || ndone)
            done1(0);
        ndone++;
        donef = 0;
        if (frame == stk)
            nflush++;
    }
    opn = v.pn;
    v.pn++;
    if (npnflg) {
        v.pn = npn;
        npn = npnflg = 0;
    }
nlpn:
    if (v.pn == pfrom) {
        print++;
        pfrom = -1;
    } else if (opn == pto) {
        print = 0;
        opn = -1;
        chkpn();
        goto nlpn;
    }
    if (stop && print) {
        dpn++;
        if (dpn >= stop) {
            dpn = 0;
            dostop();
        }
    }
nl2:
    trap = 0;
    if (v.nl == 0) {
        if ((j = findn(0)) != NTRAP) {
            trap = control(mlist[j], 0);
        }
    } else if ((i = findt(v.nl - nlss)) <= nlss) {
        if ((j = findn1(v.nl - nlss + i)) == NTRAP) {
            prstrfl("Trap botch.\n");
            done2(-5);
        }
        trap = control(mlist[j], 0);
    }
}
/* Find trap matching position a. */
int findn1(int a) {
    int i, j;

    for (i = 0; i < NTRAP; i++) {
        if (mlist[i]) {
            if ((j = nlist[i]) < 0)
                j = +pl;
            if (j == a)
                break;
        }
    }
    return (i);
}
/* Check page numbers for printing. */
void chkpn(void) {
    pto = *(pnp++);
    pfrom = pto & ~MOT;
    if (pto == -1) {
        flusho();
        done1(0);
    }
    if (pto & MOT) {
        pto &= ~MOT;
        print++;
        pfrom = 0;
    }
}
/* Find distance to next trap. */
int findt(int a) {
    int i, j, k;

    k = 32767;
    if (dip->op) {
        if (dip->dimac && ((i = dip->ditrap - a) > 0))
            k = i;
        return (k);
    }
    for (i = 0; i < NTRAP; i++) {
        if (mlist[i]) {
            if ((j = nlist[i]) < 0)
                j = +pl;
            if ((j = -a) <= 0)
                continue;
            if (j < k)
                k = j;
        }
    }
    i = pl - a;
    if (k > i)
        k = i;
    return (k);
}
/* Convenience wrapper around findt using current line. */
int findt1(void) {
    int i;

    if (dip->op)
        i = dip->dnl;
    else
        i = v.nl;
    return (findt(i));
}
/* Begin a new page ejecting as needed. */
void eject(int *a) {
    int savlss;

    if (dip->op)
        return;
    ejf++;
    if (a)
        ejl = a;
    else
        ejl = frame;
    if (trap)
        return;
e1:
    savlss = lss;
    lss = findt(v.nl);
    newline(0);
    lss = savlss;
    if (v.nl && !trap)
        goto e1;
}
/* Move the next word into the output line. */
int movword(void) {
    int i, w, *wp;
    int savwch, hys;

    over = 0;
    wp = wordp;
    if (!nwd) {
        while (((i = *wp++) & CMASK) == ' ') {
            wch--;
            wne = -width(i);
        }
        wp--;
    }
    if ((wne > nel) &&
        !hyoff && hyf &&
        (!nwd || (nel > 3 * sps)) &&
        (!(hyf & 02) || (findt1() > lss)))
        hyphen(wp);
    savwch = wch;
    hyp = hyptr;
    nhyp = 0;
    while (*hyp && (*hyp <= wp))
        hyp++;
    while (wch) {
        if ((hyoff != 1) && (*hyp == wp)) {
            hyp++;
            if (!wdstart ||
                ((wp > (wdstart + 1)) &&
                 (wp < wdend) &&
                 (!(hyf & 04) || (wp < (wdend - 1))) &&
                 (!(hyf & 010) || (wp > (wdstart + 2))))) {
                nhyp++;
                storeline(IMP, 0);
            }
        }
        i = *wp++;
        w = width(i);
        wne = -w;
        wch--;
        storeline(i, w);
    }
    if (nel >= 0) {
        nwd++;
        return (0);
    }
    xbitf = 1;
    hys = width(0200); /*hyphen*/
m1:
    if (!nhyp) {
        if (!nwd)
            goto m3;
        if (wch == savwch)
            goto m4;
    }
    if (*--linep != IMP)
        goto m5;
    if (!(--nhyp))
        if (!nwd)
            goto m2;
    if (nel < hys) {
        nc--;
        goto m1;
    }
m2:
    if (((i = *(linep - 1) & CMASK) != '-') &&
        (i != 0203)) {
        *linep = (*(linep - 1) & ~CMASK) | 0200;
        w = width(*linep);
        nel = -w;
        ne = +w;
        linep++;
        /*
	hsend();
*/
    }
m3:
    nwd++;
m4:
    wordp = wp;
    return (1);
m5:
    nc--;
    w = width(*linep);
    ne = -w;
    nel = +w;
    wne = +w;
    wch++;
    wp--;
    goto m1;
}
/* Output horizontal motion. */
void horiz(int i) {
    vflag = 0;
    if (i)
        pchar(makem(i));
}
/* Set nel and related counters for a new line. */
void setnel(void) {
    if (!nc) {
        linep = line;
        if (un1 >= 0) {
            un = un1;
            un1 = -1;
        }
        nel = ll - un;
        ne = adsp = adrem = 0;
    }
}
/* Extract the next word from input. */
int getword(int x) {
    int i, j, swp;
    int noword;

    noword = 0;
    if (x)
        if (pendw) {
            *pendw = 0;
            goto rtn;
        }
    if ((wordp = pendw))
        goto g1;
    hyp = hyptr;
    wordp = word;
    over = wne = wch = 0;
    hyoff = 0;
    while (1) {
        j = (i = GETCH()) & CMASK;
        if (j == '\n') {
            wne = wch = 0;
            noword = 1;
            goto rtn;
        }
        if (j == ohc) {
            hyoff = 1;
            continue;
        }
        if (j == ' ') {
            storeword(i, cwidth);
            continue;
        }
        break;
    }
    swp = widthp;
    storeword(' ' | chbits, -1);
    if (spflg) {
        storeword(' ' | chbits, -1);
        spflg = 0;
    }
    widthp = swp;
g0:
    if (j == CONT) {
        pendw = wordp;
        nflush = 0;
        flushi();
        return (1);
    }
    if (hyoff != 1) {
        if (j == ohc) {
            hyoff = 2;
            *hyp++ = wordp;
            if (hyp > (hyptr + NHYP - 1))
                hyp = hyptr + NHYP - 1;
            goto g1;
        }
        if ((j == '-') ||
            (j == 0203) /*3/4 Em dash*/
        )
            if (wordp > word + 1) {
                hyoff = 2;
                *hyp++ = wordp + 1;
                if (hyp > (hyptr + NHYP - 1))
                    hyp = hyptr + NHYP - 1;
            }
    }
    storeword(i, cwidth);
g1:
    j = (i = GETCH()) & CMASK;
    if (j != ' ') {
        if (j != '\n')
            goto g0;
        j = *(wordp - 1) & CMASK;
        if ((j == '.') ||
            (j == '!') ||
            (j == '?'))
            spflg++;
    }
    *wordp = 0;
rtn:
    wdstart = 0;
    wordp = word;
    pendw = 0;
    *hyp++ = 0;
    setnel();
    return (noword);
}
/* Append character c of width w to the current word buffer. */
void storeword(int c, int w) {
    if (wordp >= &word[WDSIZE - 1]) {
        if (!over) {
            prstrfl("Word overflow.\n");
            over++;
            c = 0343;
            w = -1;
            goto s1;
        }
        return;
    }
s1:
    if (w == -1)
        w = width(c);
    wne = +w;
    *wordp++ = c;
    wch++;
}
#ifdef NROFF
extern char trtab[];
/* Wrapper around getch that handles underline logic. */
int gettch(void) {
    int i, j;

    if (!((i = getch()) & MOT) && (i & ulbit)) {
        j = i & CMASK;
        if (cu && (trtab[j] == ' '))
            i = ((i & ~ulbit) & ~CMASK) | '_';
        if (!cu && (j > 32) && (j < 0370) && !(*t.codetab[j - 32] & 0200))
            i &= ~ulbit;
    }
    return (i);
}
#endif /* NROFF */
