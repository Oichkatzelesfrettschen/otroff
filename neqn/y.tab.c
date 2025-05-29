/* Stub parser for neqn */
#include <stdio.h>

/* Token definitions */
enum {
    CONTIG = 257,
    QTEXT,
    SPACE,
    THIN,
    TAB,
    MATRIX,
    LCOL,
    CCOL,
    RCOL,
    COL,
    MARK,
    LINEUP,
    SUM,
    INT,
    PROD,
    UNION,
    INTER,
    LPILE,
    PILE,
    CPILE,
    RPILE,
    ABOVE,
    DEFINE,
    TDEFINE,
    NDEFINE,
    DELIM,
    GSIZE,
    GFONT,
    FROM,
    TO,
    OVER,
    SQRT,
    SUP,
    SUB,
    SIZE,
    FONT,
    ROMAN,
    ITALIC,
    BOLD,
    FAT,
    UP,
    DOWN,
    BACK,
    FWD,
    MQ,
    MQ1,
    LEFT,
    RIGHT,
    DOT,
    DOTDOT,
    HAT,
    TILDE,
    BAR,
    UNDER,
    VEC,
    DYAD
};

/* Parser state */
typedef int YYSTYPE;
int yyval;
int yylval;
int *yypv;

/* Dummy parser always succeeds */
int yyparse(void) { return 0; }
