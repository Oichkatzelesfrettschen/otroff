#include "cxx23_scaffold.hpp"
#include "ne.hpp"
#include "y.tab.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void error(int level, const char *fmt, ...);

#define getc ngetc
#define FATAL 1
#define SSIZE 256

/* Forward declarations */
typedef struct {
    char *key;
    int keyval;
} keytab_t;

typedef struct {
    char *name;
    int keyval;
} lookup_tab;

typedef struct {
    char *nptr;
    char *sptr;
} deftab_t;

/* Function prototypes */
int ngetc(void);
int yylex(void);
void getstr(char *s, int c);
int lookup(char *str, lookup_tab tbl[]);
void define(int type);
void delim(void);
void globsize(void);
void globfont(void);
char *cstr(char *s, int quote);
char *alloc(int n);
int numb(char *s);
void setps(int size);

/* External variables */
extern int yylval;
extern int yyval;
extern int gsize;
extern int gfont;

/* Global variables */
static int lastchar;
static int linect = 1;
static int ifile = 0;
static int svargc;
static char **svargv;
static int fin;
static char lefteq, righteq;
static lookup_tab deftab[100];
static int ptr = 0;
static int eqnreg;
static int dbg = 0;
static char *swt[10];
static int sw = -1;
static int peek = -1;
static int speek[10];
static char token[256];
static int sp = 0;

static lookup_tab keytab[] = {
    {"", 0}};

/*
 * Read next character, handling includes.
 */
int ngetc(void) {
    while (1) {
        if (sw >= 0) {
            lastchar = (peek < 0) ? *swt[sw]++ : peek;
            peek = -1;
            if (lastchar != '\0')
                return lastchar;
            peek = speek[sw--];
            return ' ';
        }
        lastchar = (peek < 0) ? getchar() : peek;
        if (lastchar == '\n')
            linect++;
        peek = -1;
        if (lastchar != '\0')
            return lastchar;
        if (++ifile > svargc) {
            peek = '\0';
            return '\0';
        }
    }
}

/*
 * Lexical analyzer for neqn.
 */
int yylex(void) {
    int c, type;

beg:
    while ((c = getc()) == ' ' || c == '\n')
        ;
    yylval = c;
    switch (c) {
    case '\0':
        return '\0';
    case '\t':
        return TAB;
    case '"':
        for (sp = 0; (c = getc()) != '"';) {
            if (c != '\\')
                token[sp++] = c;
            else {
                if ((c = getc()) != '"')
                    token[sp++] = '\\';
                token[sp++] = c;
            }
            if (sp >= SSIZE)
                error(FATAL, "quoted string %.20s... too long", token);
        }
        token[sp] = '\0';
        yylval = (int)(uintptr_t)&token[0];
        return QTEXT;
    }
    if (c == righteq)
        return '\0';

    getstr(token, c);
    if ((type = lookup(token, deftab)) >= 0) {
        if (sw >= 9)
            error(FATAL, "definitions nested > 9", sw);
        swt[++sw] = deftab[type].name;
        speek[sw] = peek;
        peek = -1;
        goto beg;
    }
    type = lookup(token, keytab);
    if (type < 0)
        return CONTIG;
    if (keytab[type].keyval == DEFINE || keytab[type].keyval == TDEFINE) {
        define(keytab[type].keyval);
        goto beg;
    } else if (keytab[type].keyval == DELIM) {
        delim();
        goto beg;
    } else if (keytab[type].keyval == GSIZE) {
        globsize();
        goto beg;
    } else if (keytab[type].keyval == GFONT) {
        globfont();
        goto beg;
    }
    return keytab[type].keyval;
}

/*
 * Read a string token terminated by c.
 */
void getstr(char *s, int c) {
    for (sp = 0; c != ' ' && c != '\t' && c != '\n' && c != '{' && c != '}' &&
                 c != '"' && c != '~' && c != '^' && c != righteq;) {
        if (c == '\\')
            if ((c = getc()) != '"')
                s[sp++] = '\\';
        s[sp++] = c;
        if (sp >= SSIZE)
            error(FATAL, "token %.20s... too long", s);
        c = getc();
    }
    if (c == '{' || c == '}' || c == '"' || c == '~' || c == '^' || c == '\t' ||
        c == righteq)
        peek = c;
    s[sp] = '\0';
    yylval = (int)(uintptr_t)s;
}

/*
 * Lookup function for tables.
 */
int lookup(char *str, lookup_tab tbl[]) {
    int i;
    for (i = 0; tbl[i].name != NULL; i++) {
        if (strcmp(str, tbl[i].name) == 0)
            return i;
    }
    return -1;
}

/*
 * Collect a delimited string.
 */
char *cstr(char *s, int quote) {
    int del, c, i;
    while ((del = getc()) == ' ' || del == '\t' || del == '\n')
        ;
    if (quote)
        for (i = 0; (c = getc()) != del;)
            s[i++] = c;
    else {
        s[0] = del;
        for (i = 1; (c = getc()) != ' ' && c != '\t' && c != '\n';)
            s[i++] = c;
    }
    s[i] = '\0';
    return s;
}

/*
 * Process a define statement.
 */
void define(int type) {
    int i, c;
    while ((c = getc()) == ' ' || c == '\n')
        ;
    getstr(token, c);
    if (type == DEFINE) {
        if ((i = lookup(token, deftab)) >= 0) {
            yyval = i;
        } else {
            yyval = ptr++;
            for (i = 0; token[i] != '\0'; i++)
                ;
            deftab[yyval].name = alloc(i + 1);
            for (i = 0; (deftab[yyval].name[i] = token[i]); i++)
                ;
        }
        if (dbg)
            printf(".\tdefine %s\n", deftab[yyval].name);
    }
    cstr(token, 1);
    if (type != DEFINE)
        return;
    for (i = 0; token[i] != '\0'; i++)
        ;
    if (dbg)
        printf(".\tname %s defined as %s\n", deftab[yyval].name, token);
}

/*
 * Process delim statement.
 */
void delim(void) {
    int c;
    while ((c = getc()) == ' ' || c == '\n')
        ;
    lefteq = c;
    righteq = getc();
    if (lefteq == 'o' && righteq == 'f')
        lefteq = righteq = '\0';
}

/*
 * Set global size parameter.
 */
void globsize(void) {
    extern int gsize;
    int c;
    while ((c = getc()) == ' ' || c == '\n')
        ;
    getstr(token, c);
    gsize = numb(token);
    yyval = eqnreg = 0;
    setps(gsize);
}

/*
 * Set global font.
 */
void globfont(void) {
    extern int gfont;
    while ((gfont = getc()) == ' ' || gfont == '\n')
        ;
    yyval = eqnreg = 0;
    printf(".ft %c\n", gfont);
}
