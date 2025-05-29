/*
 * Global data translated from the original ``roff8.s`` module.  These
 * variables provided default settings and state used throughout the
 * formatter.  Only a subset of the original assembler data is actually
 * utilised by the modernised sources but all initialised items are
 * reproduced here for completeness.
 */

/* Basic runtime flags and counters */
int slow = 1;   /* non-zero if output should be throttled        */
int pto = 9999; /* last page to process                          */
int po = 0;     /* page offset                                   */
int ls = 1;     /* line spacing                                  */
int ls1 = 1;    /* line spacing for the first line               */
int pn = 1;     /* current page number                           */

/* Trap related margins */
int ma1 = 2;
int ma2 = 2;
int ma3 = 1;
int ma4 = 3;

/* Layout parameters */
int ll = 65;  /* line length                                   */
int llh = 65; /* line length hold area                         */
int hx = 1;   /* hyphenation mode                              */
int pl = 66;  /* page length                                   */
int ad = 1;   /* text adjustment flag                          */
int fi = 1;   /* fill mode flag                                */

char cc = '.';  /* command character                             */
int ohc = 200;  /* overstrike hold character                     */
int hyf = 1;    /* enable hyphenation                            */
int hypedf = 0; /* hyphenation has been used                     */

char obuf[128];     /* main output buffer                            */
char *obufp = obuf; /* pointer into ``obuf``                         */

int thresh = 240; /* hyphenation threshold                         */
char tabc = '\''; /* tab repetition character                      */

/* Default tab stop positions (in character columns) */
int tabtab[] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 0};

/* Files and strings referenced during startup */
char suffil[] = "/usr/lib/suftab"; /* suffix table file path        */
char ones[] = "ixcm";              /* Roman numeral 1s table        */
char fives[] = "vld";              /* Roman numeral 5s table        */
char ttyx[] = "/dev/tty0";         /* controlling terminal device   */
char bfn[] = "/tmp/rtma";          /* temporary buffer file name    */

int ofile = -1; /* output file descriptor                        */
int nextb = 4;  /* next available buffer                         */

/*
 * The original code kept a pointer to a list of input files in ``ilist``.
 * The list itself is declared here purely to satisfy that dependency.
 */
int ilist[4];
int *ilistp = ilist; /* pointer into ``ilist``                        */

/* Hyphenation digram tables (externals provided by roff7.c) */
extern const unsigned char bxh[];
extern const unsigned char hxx[];
extern const unsigned char bxxh[];
extern const unsigned char xhx[];
extern const unsigned char xxh[];
