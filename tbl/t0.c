/* t0.c: global variable definitions for tbl */
/* Pure C17 version - converted from C++ class */

#include "tbl.h"
#include <string.h>

/* Global variable definitions */
int nlin = 0, ncol = 0, iline = 1, nclin = 0, nslin = 0;
int style[MAXHEAD][MAXCOL];
int ctop[MAXHEAD][MAXCOL];
char font[MAXHEAD][MAXCOL][2];
char csize[MAXHEAD][MAXCOL][4];
int lefline[MAXHEAD][MAXCOL];
char cll[MAXCOL][CLLEN];
int stynum[MAXLIN + 1];
int F1 = 0, F2 = 0;
struct colstr *table[MAXLIN];
int evenup[MAXCOL];
int evenflg = 0;
int expflg = 0;
int ctrflg = 0;
int boxflg = 0;
int dboxflg = 0;
int tab = '\t';
int allflg = 0;
int leftover = 0;
int textflg = 0;
int left1flg = 0;
int rightl = 0;
char *cstore = NULL;
char *cspace = NULL;
char *last = NULL;
int sep[MAXCOL];
int fullbot[MAXLIN];
int instead[MAXLIN];
int used[MAXCOL];
int lused[MAXCOL];
int rused[MAXCOL];
int linestop[MAXLIN];
const char *ifile = "Input";
int texname = 'a';
int texct = 0;
char texstr[64] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWYXZ0123456789";
int texmax = 0;
int linstart = 0;
char *exstore = NULL;
char *exlim = NULL;
FILE *tabin = NULL;
FILE *tabout = NULL;

/* Initialize function - called at program start */
void tbl_init(void) {
    texmax = (int)strlen(texstr) - 1;
}
