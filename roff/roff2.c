#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* Helper routines provided by other translated modules */
void rbreak(void);
void skipcont(void);

/* PDP-11 global state approximations */
extern int ad, fi, ce, in, un, ll, ls, ls1, pn, pl, skip, ul, hx, hyf;
extern int ohc, tabc, cc;
extern int jfomod, ro, nx, nn, numbmod, lnumber, po, ma1, ma2, ma3, ma4, ni;
extern int ulstate;
extern int nlflg;
extern int *ilistp, ilist[];
extern int skp;
extern int nextb;
extern int ip;
extern int bname;

/* character fetched by skipcont() */
extern int ch;

/* Stub helpers for not yet translated routines */
static void stub(const char *name) { printf("[stub] %s\n", name); }
static int number(int d) {
  stub("number");
  return d;
}
static int number1(int d) {
  stub("number1");
  return d;
}
static int min(int v) { return v; }
static void need(void) { stub("need"); }
static void need2(void) { stub("need2"); }
static void nlines(int n) {
  (void)n;
  stub("nlines");
}
static void flushi(void) { stub("flushi"); }
static void text(void) { stub("text"); }
static void storeline(int c) {
  (void)c;
  stub("storeline");
}
static void eject(void) { stub("eject"); }
static void topbot(void) { stub("topbot"); }
static void headin(int *p) {
  (void)p;
  stub("headin");
}
static void getname(int *p) {
  (void)p;
  stub("getname");
}
static void nextfile(void) { stub("nextfile"); }
static void copyb(void) { stub("copyb"); }

/* ---------------------------------------------------------------------- */
/* PDP-11 request handlers from roff2.s                                  */

/* casead: .ad - enable adjustment */
void casead(void) {
  rbreak();
  ad++;
}

/* casebr: .br - line break */
void casebr(void) { rbreak(); }

/* casecc: .cc - change control character */
void casecc(void) {
  skipcont();
  int c = getchar();
  if (c != '\n' && c != EOF)
    cc = c;
  ch = c;
}

/* casece: .ce - centre next N lines */
void casece(void) {
  rbreak();
  int n = number(0);
  n = min(n);
  ce = n;
  need();
}

/* caseds: .ds - double spacing */
void caseds(void) {
  rbreak();
  ls = 2;
}

/* casefi: .fi - enable filling */
void casefi(void) {
  rbreak();
  fi++;
}

/* casein: .in - set indent */
void casein(void) {
  rbreak();
  int n = min(number(in));
  in = n;
  un = n;
}

/* caseix: .ix - set indent without break */
void caseix(void) {
  int n = min(number(in));
  in = n;
}

/* caseli: .li - literal input */
void caseli(void) {
  int n = min(number(0));
  while (n-- > 0) {
    flushi();
    nlflg = 0;
    text();
  }
}

/* casell: .ll - line length */
void casell(void) {
  int n = min(number(ll));
  ll = n;
}

/* casels: .ls - line spacing */
void casels(void) {
  rbreak();
  skipcont();
  int c = getchar();
  if (c == '\n') {
    ls = ls1;
    return;
  }
  ch = c;
  int n = number1(ls);
  n = min(--n);
  n++;
  ls = n;
  ls1 = n;
}

/* casena: .na - disable adjustment */
void casena(void) {
  rbreak();
  ad = 0;
}

/* casene: .ne - need vertical space */
void casene(void) {
  int n = min(number(0));
  need();
}

/* casenf: .nf - no fill */
void casenf(void) {
  rbreak();
  fi = 0;
}

/* casepa: .pa - new page */
void casepa(void) {
  rbreak();
  eject();
  skipcont();
  if (!nlflg) {
    int n = min(number(pn));
    pn = n;
  }
}

/* casebp: .bp - new page */
void casebp(void) { casepa(); }

/* casebl: .bl - blank line */
void casebl(void) {
  rbreak();
  int n = min(number(0));
  need2();
  while (n-- > 0) {
    storeline(' ');
    rbreak();
  }
}

/* casepl: .pl - page length */
void casepl(void) {
  pl = number(pl);
  topbot();
}

/* casesk: .sk - skip */
void casesk(void) { skip = min(number(0)); }

/* casesp: .sp - space vertically */
void casesp(void) {
  rbreak();
  nlines(number(0));
}

/* casess: .ss - single spacing */
void casess(void) {
  rbreak();
  ls = 1;
}

/* casetr: .tr - translate characters */
void casetr(void) {
  skipcont();
  int c1;
  while ((c1 = getchar()) != '\n' && c1 != EOF) {
    int c2 = getchar();
    if (c2 == '\n' || c2 == EOF)
      c2 = ' ';
    stub("trtab update");
  }
}

/* caseta: .ta - set tabs */
extern int tabtab[];
void caseta(void) {
  int *tp = &tabtab[0];
  for (;;) {
    int n = min(number(0));
    n--;
    if (n <= 0)
      break;
    *tp++ = n;
  }
  *tp = 0;
}

/* caseti: .ti - temporary indent */
void caseti(void) {
  rbreak();
  un = min(number(in));
}

/* caseul: .ul - underline count */
void caseul(void) { ul = min(number(0)); }

/* caseun: .un - underline control */
void caseun(void) { un = min(in - number(0)); }

/* casehx: .hx - header toggle */
void casehx(void) {
  if (hx)
    hx = 0;
  else
    hx = 1;
  topbot();
}

/* casehe: .he - header even */
extern int ehead, ohead, efoot, ofoot;
void casehe(void) {
  headin(&ehead);
  ohead = ehead;
}

/* casefo: .fo - footer even */
void casefo(void) {
  headin(&efoot);
  ofoot = efoot;
}

/* caseeh: .eh - even header */
void caseeh(void) { headin(&ehead); }

/* caseoh: .oh - odd header */
void caseoh(void) { headin(&ohead); }

/* caseef: .ef - even footer */
void caseef(void) { headin(&efoot); }

/* caseof: .of - odd footer */
void caseof(void) { headin(&ofoot); }

/* casem1: .m1 - trap margin */
void casem1(void) {
  ma1 = min(number(ma1));
  topbot();
}

/* casem2: .m2 - trap margin */
void casem2(void) {
  ma2 = min(number(ma2));
  topbot();
}

/* casem3: .m3 - trap margin */
void casem3(void) {
  ma3 = min(number(ma3));
  topbot();
}

/* casem4: .m4 - trap margin */
void casem4(void) {
  ma4 = min(number(ma4));
  topbot();
}

/* casehc: .hc - hyphenation character */
void casehc(void) {
  skipcont();
  int c = getchar();
  if (c == '\n')
    c = 200;
  ohc = c;
}

/* casetc: .tc - tab character */
void casetc(void) {
  skipcont();
  int c = getchar();
  if (c == '\n')
    c = ' ';
  tabc = c;
}

/* casehy: .hy - hyphenation */
void casehy(void) { hyf = number(0); }

/* casen1: .n1 - number style 1 */
void casen1(void) {
  rbreak();
  numbmod = 1;
  nn = 0;
  int n = number(0);
  if (n > 0) {
    lnumber = n;
    return;
  }
  numbmod = 0;
}

/* casen2: .n2 - number style 2 */
void casen2(void) {
  rbreak();
  numbmod = 2;
  nn = 0;
  int n = number(0);
  if (n > 0) {
    lnumber = n;
    return;
  }
  numbmod = 0;
}

/* casenn: .nn - number lines */
void casenn(void) { nn = min(number(0)); }

/* caseni: .ni - number indent */
void caseni(void) { ni = min(number(ni)); }

/* casejo: .jo - justify output */
void casejo(void) { jfomod = number(0); }

/* casear: .ar - arabic numerals */
void casear(void) { ro = 0; }

/* casero: .ro - roman numerals */
void casero(void) { ro++; }

/* casenx: .nx - next file */
void casenx(void) {
  skipcont();
  getname(&nextb);
  nx++;
  nextfile();
  nlflg++;
  ip = 0;
  ilistp = ilist;
}

/* casepo: .po - page offset */
void casepo(void) {
  rbreak();
  po = min(number(po));
}

/* casede: .de - define macro */
void casede(void) {
  if (ip)
    return;
  skipcont();
  getname(&bname);
  skp = 0;
  copyb();
}

/* caseig: .ig - ignore until .de end */
void caseig(void) {
  skp++;
  copyb();
}

/* casemk: .mk - mark */
void casemk(void) {
  rbreak();
  putchar('\002');
}
