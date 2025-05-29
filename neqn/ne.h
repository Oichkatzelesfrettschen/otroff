#define FATAL 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Function prototypes */
int getline(char *s);
int inline(void);
int putout(int p1);
int max(int i, int j);
int oalloc(void);
int ofree(int n);
int setps(int p);
int nrwid(int n1, int p, int n2);
int setfile(int argc, char *argv[]);
int yyerror(void);
int init(void);
int error(int fatal, char *s1, char *s2);
#ifdef gcos
#define fout cout
#define fin cin
#define close cclose
#define exit cexit
#define signal wdleng
#define flush wdleng
#define dup(n) n
#define alloc(n) calloc(n, 1)
#define free(p) cfree(p, 0, 0)
#define open(f, m) copen(f, m == 1 ? 'w' : 'r')
#endif

#define VERT(n) (20 * n)

extern int dbg;
extern int ct;
extern int lp[40];
extern int used[100]; /* available registers */
extern int ps; /* dflt init pt size */
extern int ft; /* dflt font */
extern int first;
extern int fout, fin;
extern int ifile;
extern int linect; /* line number in file */
extern int eqline; /* line where eqn started */
extern int svargc;
extern char **svargv;
extern int eht[100];
extern int ebase[100];
struct {
    char c1;
    char c2;
};
extern int yyval;
extern int *yypv;
extern int yylval;
extern int tht[30];
extern int tbase[30];
extern int ptr;
extern struct {
    char *nptr;
    char *sptr;
} deftab[100];
extern int eqnreg, eqnht;
extern int lefteq, righteq;
extern int lastchar; /* last character read by lex */
