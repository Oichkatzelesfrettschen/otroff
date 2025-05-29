/* tbl.h: external declarations and prototypes */

#ifndef TBL_TBL_H
#define TBL_TBL_H

#include <stdio.h>

#define MAXLIN 200
#define MAXHEAD 30
#define MAXCOL 20
#define MAXCHS 2000
#define CLLEN 10
extern int nlin, ncol, iline, nclin, nslin;
extern int style[MAXHEAD][MAXCOL];
extern int ctop[MAXHEAD][MAXCOL];
extern char font[MAXHEAD][MAXCOL][2];
extern char csize[MAXHEAD][MAXCOL][4];
extern char cll[MAXCOL][CLLEN];
extern int stynum[];
extern int F1, F2;
extern int lefline[MAXHEAD][MAXCOL];
extern int fullbot[];
extern int instead[];
extern int expflg;
extern int ctrflg;
extern int evenflg;
extern int evenup[];
extern int boxflg;
extern int dboxflg;
extern int tab;
extern int allflg;
extern int textflg;
extern int left1flg;
extern int rightl;
struct colstr {
    char *col, *rcol;
};
extern struct colstr *table[];
extern char *cspace, *cstore;
extern char *exstore, *exlim;
extern int sep[];
extern int used[], lused[], rused[];
extern int linestop[];
extern int leftover;
extern char *last, *ifile;
extern int texname;
extern int texct, texmax;
extern char texstr[];
extern int linstart;

extern FILE *tabin, *tabout;
#define CRIGHT 80
#define CLEFT 40
#define CMID 60
#define S1 31
#define S2 32
#define TMP 38
#define SF 35
#define SL 34
#define LSIZE 33
#define SIND 37
/* this refers to the relative position of lines */
#define LEFT 1
#define RIGHT 2
#define THRU 3
#define TOP 1
#define BOT 2

/* Function prototypes */
void gettbl(void);
void drawvert(int start, int end, int c, int lwid);
int nodata(int il);
int oneh(int lin);
void permute(void);
int vspand(int ir, int ij);
int vspen(char *s);
void maktab(void);
void wide(char *s, char *fn, char *size);
void runout(void);
void runtabs(int i);
int ifline(char *s);
void need(void);
void deftail(void);
void putline(int i, int nl);
void puttext(char *s, char *fn, char *size);
void funnies(int stl, int lin);
void putfont(char *fn);
void putsize(char *s);
void yetmore(void);
void checkuse(void);
int real(char *s);
char *chspace(void);
struct colstr *alocv(int n);
void release(void);
void choochar(void);
int point(char *s);
void error(char *s);
char *gets1(char *s);
void un1getc(int c);
int get1char(void);
void savefill(void);
void rstofill(void);
void endoff(void);
void ifdivert(void);
void saveline(void);
void restline(void);
void cleanfc(void);
int gettext(char *sp, int ilin, int icol, char *fn, char *sz);
void untext(void);
int interv(int i, int c);
int interh(int i, int c);
int up1(int i);
char *maknew(char *str);
int match(char *s1, char *s2);
int prefix(char *small, char *big);
int letter(int ch);
int numb(char *str);
int digit(int x);
int max(int a, int b);
int index(char *s, int c);
void tcopy(char *s, char *t);
#endif /* TBL_TBL_H */
