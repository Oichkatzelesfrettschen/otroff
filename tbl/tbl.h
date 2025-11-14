/* tbl.h: external declarations and prototypes */


/* C17 - no scaffold needed */ // utilities

/* array not needed in C */
#include <stdio.h>
#include <string.h>


const int MAXLIN = 200;
const int MAXHEAD = 30;
const int MAXCOL = 20;
const int MAXCHS = 2000;
const int CLLEN = 10;

struct colstr {
    char *col{}; /* left column text */
    char *rcol{}; /* right column text */
};

class Parser {
  public:
    int nlin = 0, ncol = 0, iline = 1, nclin = 0, nslin = 0;
    std::array<std::array<int, MAXCOL>, MAXHEAD> style{};
    std::array<std::array<int, MAXCOL>, MAXHEAD> ctop{};
    std::array<std::array<std::array<char, 2>, MAXCOL>, MAXHEAD> font{};
    std::array<std::array<std::array<char, 4>, MAXCOL>, MAXHEAD> csize{};
    std::array<std::array<int, MAXCOL>, MAXHEAD> lefline{};
    std::array<std::array<char, CLLEN>, MAXCOL> cll{};
    std::array<int, MAXLIN + 1> stynum{};
    int F1 = 0;
    int F2 = 0;
    std::array<colstr *, MAXLIN> table{};
    std::array<int, MAXCOL> evenup{};
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
    std::array<int, MAXCOL> sep{};
    std::array<int, MAXLIN> fullbot{};
    std::array<int, MAXLIN> instead{};
    std::array<int, MAXCOL> used{};
    std::array<int, MAXCOL> lused{};
    std::array<int, MAXCOL> rused{};
    std::array<int, MAXLIN> linestop{};
    const char *ifile = "Input"; // Changed type and removed cast
    int texname = 'a';
    int texct = 0;
    std::string texstr =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWYXZ0123456789";
    int texmax = 0;
    int linstart = 0;
    char *exstore = NULL;
    char *exlim = NULL;
    FILE *tabin = NULL;
    FILE *tabout = NULL;

    Parser();
};

inline Parser parser;

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

/* Compatibility macros mapping old globals to parser members */
#define nlin parser.nlin
#define ncol parser.ncol
#define iline parser.iline
#define nclin parser.nclin
#define nslin parser.nslin
#define style parser.style
#define ctop parser.ctop
#define font parser.font
#define csize parser.csize
#define cll parser.cll
#define stynum parser.stynum
#define F1 parser.F1
#define F2 parser.F2
#define lefline parser.lefline
#define fullbot parser.fullbot
#define instead parser.instead
#define expflg parser.expflg
#define ctrflg parser.ctrflg
#define evenflg parser.evenflg
#define evenup parser.evenup
#define boxflg parser.boxflg
#define dboxflg parser.dboxflg
#define tab parser.tab
#define allflg parser.allflg
#define textflg parser.textflg
#define left1flg parser.left1flg
#define rightl parser.rightl
#define table parser.table
#define cspace parser.cspace
#define cstore parser.cstore
#define exstore parser.exstore
#define exlim parser.exlim
#define sep parser.sep
#define used parser.used
#define lused parser.lused
#define rused parser.rused
#define linestop parser.linestop
#define leftover parser.leftover
#define last parser.last
#define ifile parser.ifile
#define texname parser.texname
#define texct parser.texct
#define texstr parser.texstr
#define texmax parser.texmax
#define linstart parser.linstart
#define tabin parser.tabin
#define tabout parser.tabout

#ifdef __cplusplus
extern "C" {
#endif
/* Function prototypes */
void tableput();
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
/* from tt.c */
int ctype(int il, int ic);
int min(int a, int b);
int fspan(int i, int c);
int lspan(int i, int c);
int ctspan(int i, int c);
void tohcol(int ic);
int allh(int i);
int thish(int i, int c);
/* from tu.c */
void makeline(int i, int c, int lintype);
void fullwide(int i, int lintype);
void drawline(int i, int cl, int cr, int lintype, int noheight);
void getstop(void);
int left(int i, int c, int *lwidp);
int lefdata(int i, int c);
int next(int i);
int prev(int i);
#ifdef __cplusplus
} // extern "C"
#endif
