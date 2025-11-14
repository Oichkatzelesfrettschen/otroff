/* C17 - no scaffold needed */
/* tb.c: check which entries exist, also storage allocation */
#include "tbl.h"

/* Analyze table usage for each column. */
void checkuse(void) {
    int i, c, k;
    for (c = 0; c < ncol; c++) {
        used[c] = lused[c] = rused[c] = 0;
        for (i = 0; i < nlin; i++) {
            if (instead[i] || fullbot[i])
                continue;
            k = ctype(i, c);
            if (k == '-' || k == '=')
                continue;
            if ((k == 'n' || k == 'a')) {
                lused[c] |= real(table[i][c].col);
                rused[c] |= real(table[i][c].rcol);
                if (!real(table[i][c].rcol))
                    used[c] |= real(table[i][c].col);
            } else
                used[c] |= real(table[i][c].col);
        }
    }
}
/* Determine whether a data pointer is actual text. */
int real(char *s) {
    if (s == NULL)
        return (0);
    if (!point(s))
        return (1);
    if (*s == 0)
        return (0);
    return (1);
}
int spcount = 0; /* number of allocated char spaces */
#define MAXVEC 20
char *spvecs[MAXVEC];
/* Allocate character storage. */
char *chspace(void) {
    char *pp;
    if (spvecs[spcount] != NULL) // Explicitly check against NULL
        return (spvecs[spcount++]);
    if (spcount >= MAXVEC)
        error("Too many characters in table");
    pp = ((char*)calloc(MAXCHS + 200, 1));
    spvecs[spcount++] = pp; // Store before check for clarity, though original was combined
    if (pp == NULL)
        error("no space for characters");
    return (pp);
}
#define MAXPC 50
char *thisvec;
int tpcount = -1; /* current vector index */
char *tpvecs[MAXPC];
/* Allocate vector storage. */
struct colstr *alocv(int n) {
    char *tp, *q;
    if (tpcount < 0 || thisvec + n > tpvecs[tpcount] + MAXCHS) {
        tpcount++;
        if (tpvecs[tpcount] == NULL) { // Check against NULL
            tpvecs[tpcount] = ((char*)calloc(MAXCHS, 1));
        }
        thisvec = tpvecs[tpcount];
        if (thisvec == NULL) // Check against NULL
            error("no space for vectors");
    }
    tp = thisvec;
    thisvec += n; /* advance allocation pointer */
    for (q = tp; q < thisvec; q++)
        *q = 0; // Initialize allocated memory
    return reinterpret_cast<struct colstr *>(tp);
}
/* Release storage vectors. */
void release(void) {
    extern char *exstore;
    /* give back unwanted space in some vectors */
    spcount = 0;
    tpcount = -1;
    exstore = NULL;
}
