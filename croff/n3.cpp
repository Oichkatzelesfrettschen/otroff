#include "cxx23_scaffold.hpp"
/*
 * n3.c - NROFF/TROFF Macro and String Processing Module
 * 
 * C90 compliant implementation with proper type handling
 */

#include "tdef.hpp"  // primary troff definitions
#include "env.hpp"   // environment structure definitions
#include "t.hpp"     // common troff header
#include "proto.hpp" // function prototypes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

/* Memory and allocation constants */
#define NBLIST 256
#define BLK 128
#define NEV_COUNT 10
#define EVS_SIZE 100
#define HASH_TABLE_SIZE 1024

/* Hash table for fast macro lookup */
struct hash_entry {
    int key;
    int value;
    struct hash_entry *next;
};

/* Global variables */
int pagech = '%';
int strflg;
int blist[NBLIST];
int wbuf[BLK];
int rbuf[BLK];
static struct hash_entry *hash_table[HASH_TABLE_SIZE];

/* External declarations with proper types */
extern int ch, ibf, nextb, lgf, copyf, ch0, ip;
extern int app, ds, nlflg, nchar, pendt, rchar, dilev;
extern int nonumb, lt, nrbits, nform, oldmn, newmn, macerr;
extern int apptr, offset, aplnk, diflg, woff, roff, wbfi, po, xxx;
extern char *enda;
extern int *nxf, *argtop, *ap, *frame, *stk, *cp;
extern struct env *dip;
extern int fmt[];

extern struct contab {
    int rq;
    union {
        int (*func)(void);
        int offset;
    } f;
} contab[NM];

/* External structure references from headers - use proper extern declarations */

/* C90 Function prototypes */
static int hash_function(int key);
static void init_hash_table(void);
static void caseig(void);
static void casern(void);
static void caserm(void);

void caseas(void);
void caseds(void);
void caseam(void);
void casede(void);
int findmn(int i);
void clrmn(int i);
int finds(int mn);
void copys(void);
int copyb(void);
int skip(void);
int alloc(void);
void blk_free(int i);
int boff(int i);
int blisti(int i);
char *setbrk(int x);
void wbt(int i);
void wbf(int i);
void wbfl(void);
int rbf(void);
int rbf0(int p);
int incoff(int p);
int popi(void);
int pushi(int newip);
int getsn(void);
int setstr(void);
void collect(void);
void seta(void);
void caseda(void);
void casedi(void);
void casedt(void);
void casetl(void);
void casepc(void);
int hseg(int (*f)(int), int *p);
void casepm(void);
void kvt(int k, char *p);

/* External function prototypes */
extern int getch(void);
extern int getach(void);
extern int getrq(void);
extern int chget(int c);
extern void control(int c, int flag);
extern void flushi(void);
extern void done2(int code);
extern void edone(int code);
extern int vnumb(int val);
extern void newline(int flag);
extern void horiz(int val);
extern int quant(int val, int unit);
extern int findr(int c);
extern int fnumb(int val, int (*f)(int));
extern int width(int c);
extern void pchar(int c);
extern void Wolf(void);
extern int seek(int fd, long offset, int whence);
extern void prstr(char *s);
extern void prstrfl(char *s);

/* System call redefinitions */
extern ssize_t sys_read(int fd, void *buf, size_t count);
extern ssize_t sys_write(int fd, const void *buf, size_t count);
#define read sys_read
#define write sys_write

/* Hash table functions */
static int hash_function(int key) {
    return key % HASH_TABLE_SIZE;
}

static void init_hash_table(void) {
    int j, key, hash_index;
    struct hash_entry *entry;

    memset(hash_table, 0, sizeof(hash_table));

    for (j = 0; j < NM; j++) {
        key = contab[j].rq & ~MMASK;
        hash_index = hash_function(key);
        entry = (struct hash_entry *)malloc(sizeof(struct hash_entry));
        if (entry != NULL) {
            entry->key = key;
            entry->value = j;
            entry->next = hash_table[hash_index];
            hash_table[hash_index] = entry;
        }
    }
}

/* Utility functions */
static void caseig(void) {
    register int i;
    offset = 0;
    if ((i = copyb()) != '.')
        control(i, 1);
}

static void casern(void) {
    register int i, j;

    j = getrq();
    if (j == 0)
        return;

    clrmn(findmn(j));
    lgf++;
    skip();

    if (((i = getrq()) == 0) || ((oldmn = findmn(i)) < 0))
        return;

    skip();
    clrmn(findmn(j = getrq()));

    if (j)
        contab[oldmn].rq = (contab[oldmn].rq & MMASK) | j;
}

static void caserm(void) {
    lgf++;
    skip();
    clrmn(findmn(getrq()));
}

/* Main API functions */
void caseas(void) {
    app++;
    caseds();
}

void caseds(void) {
    ds++;
    casede();
}

void caseam(void) {
    app++;
    casede();
}

void casede(void) {
    register int i, savoff, req;

    if (dip->op)
        Wolf();

    req = '.';
    lgf++;
    skip();

    if ((i = getrq()) == 0)
        goto de1;

    if ((offset = finds(i)) == 0)
        goto de1;

    if (ds)
        copys();
    else
        req = copyb();

    wbfl();
    clrmn(oldmn);

    if (newmn)
        contab[newmn].rq = i | MMASK;

    if (apptr) {
        savoff = offset;
        offset = apptr;
        wbt(IMP);
        offset = savoff;
    }

    offset = dip->op;

    if (req != '.')
        control(req, 1);

de1:
    ds = app = 0;
}

int findmn(int i) {
    int hash_index;
    struct hash_entry *entry;

    hash_index = hash_function(i);
    entry = hash_table[hash_index];

    while (entry) {
        if (entry->key == i) {
            return entry->value;
        }
        entry = entry->next;
    }

    return -1;
}

void clrmn(int i) {
    if (i >= 0) {
        if (contab[i].rq & MMASK)
            blk_free(contab[i].f.offset);
        contab[i].rq = 0;
        contab[i].f.offset = 0;
    }
}

int finds(int mn) {
    register int i, savip;

    oldmn = findmn(mn);
    newmn = apptr = aplnk = 0;

    if (app && (oldmn >= 0) && (contab[oldmn].rq & MMASK)) {
        savip = ip;
        ip = contab[oldmn].f.offset;
        oldmn = -1;

        while ((i = rbf()) != 0)
            ;

        apptr = ip;
        if (!diflg)
            ip = incoff(ip);
        nextb = ip;
        ip = savip;
    } else {
        for (i = 0; i < NM; i++) {
            if (contab[i].rq == 0)
                break;
        }

        if ((i == NM) || (nextb = alloc()) == 0) {
            app = 0;
            if (macerr++ > 1)
                done2(02);
            edone(04);
            return (offset = 0);
        }

        contab[i].f.offset = nextb;

        if (!diflg) {
            newmn = i;
            if (oldmn == -1)
                contab[i].rq = -1;
        } else {
            contab[i].rq = mn | MMASK;
        }
    }

    app = 0;
    return (offset = nextb);
}

int skip(void) {
    register int i;

    while (((i = getch()) & CMASK) == ' ')
        ;

    ch = i;
    return (nlflg);
}

int copyb(void) {
    register int i, j, k;
    int ii, req, state, savoff;

    if (skip() || !(j = getrq()))
        j = '.';

    req = j;
    k = j >> BYTE;
    j &= BMASK;

    copyf++;
    flushi();
    nlflg = 0;
    state = 1;

    while (1) {
        i = (ii = getch()) & CMASK;

        if (state == 3) {
            if (i == k)
                break;
            if (!k) {
                ch = ii;
                i = getach();
                ch = ii;
                if (!i)
                    break;
            }
            state = 0;
            goto c0;
        }

        if (i == '\n') {
            state = 1;
            nlflg = 0;
            goto c0;
        }

        if ((state == 1) && (i == '.')) {
            state++;
            savoff = offset;
            goto c0;
        }

        if ((state == 2) && (i == j)) {
            state++;
            goto c0;
        }

        state = 0;

    c0:
        if (offset)
            wbf(ii);
    }

    if (offset) {
        wbfl();
        offset = savoff;
        wbt(0);
    }

    copyf--;
    return (req);
}

void copys(void) {
    register int i;

    copyf++;

    if (skip())
        goto c0;

    if (((i = getch()) & CMASK) != '"')
        wbf(i);

    while (((i = getch()) & CMASK) != '\n')
        wbf(i);

c0:
    wbt(0);
    copyf--;
}

/* Memory management */
int alloc(void) {
    register int i;
    int j;

    for (i = 0; i < NBLIST; i++) {
        if (blist[i] == 0)
            break;
    }

    if (i == NBLIST) {
        return (nextb = 0);
    } else {
        blist[i] = -1;
        if ((j = boff(i)) < NEV_COUNT * EVS_SIZE)
            return (nextb = 0);
        return (nextb = j);
    }
}

void blk_free(int i) {
    register int j;

    while ((blist[j = blisti(i)]) != -1) {
        i = blist[j];
        blist[j] = 0;
    }
    blist[j] = 0;
}

int boff(int i) {
    return (NEV_COUNT * EVS_SIZE + i * BLK);
}

int blisti(int i) {
    return ((i - NEV_COUNT * EVS_SIZE) / (BLK));
}

/* Buffer I/O */
void wbt(int i) {
    wbf(i);
    wbfl();
}

void wbf(int i) {
    register int j;

    if (!offset)
        return;

    if (!woff) {
        woff = offset;
        wbfi = 0;
    }

    wbuf[wbfi++] = i;

    if (!((++offset) & (BLK - 1))) {
        wbfl();

        if (blist[j = blisti(--offset)] == -1) {
            if (alloc() == 0) {
                prstr("Out of temp file space.\n");
                done2(01);
            }
            blist[j] = nextb;
        }
        offset = blist[j];
    }

    if (wbfi >= BLK)
        wbfl();
}

void wbfl(void) {
    if (woff == 0)
        return;

    seek(ibf, woff << 1, 0);
    write(ibf, (char *)wbuf, wbfi << 1);

    if ((woff & (~(BLK - 1))) == (roff & (~(BLK - 1))))
        roff = -1;

    woff = 0;
}

int rbf(void) {
    register int i;

    if ((i = rbf0(ip)) == 0) {
        if (!app)
            i = popi();
    } else {
        ip = incoff(ip);
    }

    return (i);
}

int rbf0(int p) {
    register int i;

    if ((i = (p & (~(BLK - 1)))) != roff) {
        roff = i;
        seek(ibf, roff << 1, 0);
        if (read(ibf, (char *)rbuf, BLK << 1) == 0)
            return (0);
    }

    return (rbuf[p & (BLK - 1)]);
}

int incoff(int p) {
    register int j;

    if (!((j = (++p)) & (BLK - 1))) {
        if ((j = blist[blisti(--p)]) == -1) {
            prstr("Bad storage allocation.\n");
            blk_free(p);
            done2(-5);
        }
    }

    return (j);
}

/* Stack management with proper type handling */
int popi(void) {
    register int *p;

    if (frame == stk)
        return (0);

    if (strflg)
        strflg--;

    p = nxf = frame;
    *p++ = 0;
    frame = (int *)(intptr_t)*p++;
    ip = *p++;
    nchar = *p++;
    rchar = *p++;
    pendt = *p++;
    ap = (int *)(intptr_t)*p++;
    cp = (int *)(intptr_t)*p++;
    ch0 = *p++;

    return (*p);
}

int pushi(int newip) {
    register int *p;

    if ((enda - (STKSIZE << 1)) < (char *)nxf)
        setbrk(DELTA);

    p = nxf;
    p++;
    *p++ = (int)(intptr_t)frame;
    *p++ = ip;
    *p++ = nchar;
    *p++ = rchar;
    *p++ = pendt;
    *p++ = (int)(intptr_t)ap;
    *p++ = (int)(intptr_t)cp;
    *p++ = ch0;
    *p++ = ch;

    cp = 0;
    nchar = rchar = pendt = 0;
    ap = 0;
    ch0 = ch = 0;

    frame = nxf;

    if (*nxf == 0)
        nxf = (int *)((char *)nxf + STKSIZE);
    else
        nxf = (int *)argtop;

    return (ip = newip);
}

char *setbrk(int x) {
    register char *i;

    if ((i = (char *)malloc((size_t)x)) == NULL) {
        prstrfl("Core limit reached.\n");
        edone(0100);
    } else {
        enda = i + x;
    }

    return i;
}
/* String handling */
int getsn(void) {
    register int i;

    if ((i = getach()) == 0)
        return (0);

    if (i == '(')
        return (getrq());
    else
        return (i);
}

int setstr(void) {
    register int i;

    lgf++;

    if (((i = getsn()) == 0) ||
        ((i = findmn(i)) == -1) ||
        !(contab[i].rq & MMASK)) {
        lgf--;
        return (0);
    } else {
        if ((enda - 2) < (char *)nxf)
            setbrk(DELTA);

        *nxf = 0;
        strflg++;
        lgf--;
        return (pushi(contab[i].f.offset));
    }
}

/* Argument handling */
void collect(void) {
    register int i;
    register int *strp;
    int *argpp, *argppend;
    int quote, *savnxf, *lim;

    copyf++;
    *nxf = 0;

    if (skip())
        goto rtn;

    savnxf = nxf;
    lim = nxf = (int *)((char *)nxf + 20 * STKSIZE);
    strflg = 0;

    if ((argppend = strp = (argpp = savnxf + STKSIZE) + 9) > (int *)enda)
        setbrk(DELTA);

    for (i = 8; i >= 0; i--)
        argpp[i] = 0;

    while ((argpp != argppend) && (!skip())) {
        *argpp++ = (int)(intptr_t)strp;
        quote = 0;

        if (((i = getch()) & CMASK) == '"')
            quote++;
        else
            ch = i;

        while (1) {
            i = getch();

            if (nlflg || ((!quote) && ((i & CMASK) == ' ')))
                break;

            if (quote && ((i & CMASK) == '"') &&
                (((i = getch()) & CMASK) != '"')) {
                ch = i;
                break;
            }

            *strp++ = i;

            if (strflg && (strp >= lim)) {
                prstrfl("Macro argument too long.\n");
                copyf--;
                edone(004);
            }

            if ((enda - 4) <= (char *)strp)
                setbrk(DELTA);
        }

        *strp++ = 0;
    }

    nxf = savnxf;
    *nxf = (int)(argpp - nxf - STKSIZE);
    argtop = strp;

rtn:
    copyf--;
}

void seta(void) {
    register int i;

    if (((i = (getch() & CMASK) - '0') > 0) &&
        (i <= 9) && (i <= *frame))
        ap = (int *)(intptr_t)(*(i + frame + STKSIZE - 1));
}

/* Diversion functions */
void caseda(void) {
    app++;
    casedi();
}

void casedi(void) {
    register int i, j;

    lgf++;

    if (skip() || ((i = getrq()) == 0)) {
        if (dip->op > 0)
            wbt(0);

        if (dilev > 0) {
            v.dn = dip->dnl;
            v.dl = dip->maxl;
            dip = (struct env *)&d[--dilev];
            offset = dip->op;
        }
        goto rtn;
    }

    if (++dilev == NDI) {
        --dilev;
        prstr("Cannot divert.\n");
        edone(02);
    }

    if (dip->op)
        wbt(0);

    diflg++;
    dip = (struct env *)&d[dilev];
    dip->op = finds(i);
    dip->curd = i;
    clrmn(oldmn);

    for (j = 1; j <= 10; j++)
        ((int *)dip)[j] = 0;

rtn:
    app = 0;
    diflg = 0;
}

void casedt(void) {
    lgf++;

    dip->dimac = dip->ditrap = dip->ditf = 0;

    skip();
    dip->ditrap = vnumb(0);

    if (nonumb)
        return;

    skip();
    dip->dimac = getrq();
}

/* Title processing */
void casetl(void) {
    register int i, j;
    int w1, w2, w3, begin, delim;

    dip->nls = 0;
    skip();

    if (dip->op)
        wbfl();

    if ((offset = begin = alloc()) == 0)
        return;

    if ((delim = getch()) & MOT) {
        ch = delim;
        delim = '\'';
    } else
        delim &= CMASK;

    if (!nlflg)
        while (((i = getch()) & CMASK) != '\n') {
            if ((i & CMASK) == delim)
                i = IMP;
            wbf(i);
        }

    wbf(IMP);
    wbf(IMP);
    wbt(0);

    w1 = hseg(width, (int *)(intptr_t)begin);
    w2 = hseg(width, (int *)0);
    w3 = hseg(width, (int *)0);

    offset = dip->op;

#ifdef NROFF
    if (!offset)
        horiz(po);
#endif

    hseg((int (*)(int))pchar, (int *)(intptr_t)begin);

    if (w2 || w3)
        horiz(j = quant((lt - w2) / 2 - w1, HOR));
    hseg((int (*)(int))pchar, (int *)0);

    if (w3) {
        horiz(lt - w1 - w2 - w3 - j);
        hseg((int (*)(int))pchar, (int *)0);
    }

    newline(0);

    if (dip->op) {
        if (dip->dnl > dip->hnl)
            dip->hnl = dip->dnl;
    } else {
        if (v.nl > dip->hnl)
            dip->hnl = v.nl;
    }

    blk_free(begin);
}

void casepc(void) {
    pagech = chget(IMP);
}

int hseg(int (*f)(int), int *p) {
    register int acc, i;
    static int *q;

    acc = 0;

    if (p)
        q = p;

    while (1) {
        i = rbf0((int)(intptr_t)q);
        q = (int *)(intptr_t)incoff((int)(intptr_t)q);

        if (!i || (i == IMP))
            return (acc);

        if ((i & CMASK) == pagech) {
            nrbits = i & ~CMASK;
            nform = fmt[findr('%')];
            acc += fnumb(v.pn, f);
        } else
            acc += (*f)(i);
    }
}

/* Debugging */
void casepm(void) {
    register int i, k;
    register char *p;
    int j, xx, cnt, kk, tot;
    char pmline[10];

    kk = cnt = 0;
    tot = !skip();

    for (i = 0; i < NM; i++) {
        if (!((xx = contab[i].rq) & MMASK))
            continue;

        p = pmline;
        j = contab[i].f.offset;
        k = 1;

        while ((j = blist[blisti(j)]) != -1)
            k++;

        cnt++;
        kk += k;

        if (!tot) {
            *p++ = xx & 0177;
            if (!(*p++ = (xx >> BYTE) & 0177))
                *(p - 1) = ' ';
            *p++ = ' ';
            kvt(k, p);
            prstr(pmline);
        }
    }

    if (tot || (cnt > 1)) {
        kvt(kk, pmline);
        prstr(pmline);
    }
}

void kvt(int k, char *p) {
    if (k >= 100)
        *p++ = k / 100 + '0';

    if (k >= 10)
        *p++ = (k % 100) / 10 + '0';

    *p++ = k % 10 + '0';
    *p++ = '\n';
    *p = 0;
}
