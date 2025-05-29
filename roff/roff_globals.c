/*
 * Global variables derived from the PDP-11 module roff8.s.
 * Only a minimal subset of the original data tables is used by
 * the translated sources but the entire list is provided for
 * completeness.
 */

int slow = 1;
int pto = 9999;
int po = 0;
int ls = 1;
int ls1 = 1;
int pn = 1;
int ma1 = 2;
int ma2 = 2;
int ma3 = 1;
int ma4 = 3;
int ll = 65;
int llh = 65;
int hx = 1;
int pl = 66;
int ad = 1;
int fi = 1;
int cc = '.';
int ohc = 200;
int hyf = 1;
int hypedf = 0;

char obuf[128];
char *obufp = obuf;

int thresh = 240;
int tabc = '\'';
int tabtab[] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 0};

char suffil[] = "/usr/lib/suftab";
char ones[] = "ixcm";
char fives[] = "vld";
char ttyx[] = "/dev/tty0";
char bfn[] = "/tmp/rtma";

int ofile = -1;
int nextb = 4;

int ilist[4];
int *ilistp = ilist;

/* uninitialised variables (BSS in the original) */
int old, stop, garb;
int bname;
char nextf[20];
int nx, ibf, ibf1, skp, ip;
int iliste;
int column, ocol, nspace, fac, fmq, nhl, nel;
int jfomod, wordp, nlflg, spaceflg;
int linep, undflg, wordend, maxdig, maxloc, totout;
int hstart, nhstart, nhyph;
int argc, argp, ibufp, eibuf, wne;
int nl, bl, nc, ne, lnumber, numbmod, skip, nwd;
int ulstate, ulc, bsc, nsp, nn, ro, pfrom, ni;
int onesp, fivesp, ul, ce, in, un, wch;
int suff;
char sufb[20];
char sufbuf[512];
short suftab[26 * 2];
int ifile, charv, nfile;
int ehead, ohead, efoot, ofoot;
unsigned char trtab[128];
char word[200];
char line[500];
