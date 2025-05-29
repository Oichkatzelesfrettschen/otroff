/*
 * Global variables used across the troff translation.  These mirror the
 * data segment of the original roff8.s module while providing the default
 * values expected by the C sources.  Each value is documented with the
 * default used by the historic implementation.
 */

/* basic run-time flags */
int slow = 1;   /* throttle output when non-zero          */
int pto = 9999; /* last page number to process            */
int po = 0;     /* page offset                            */
int ls = 1;     /* line spacing                           */
int ls1 = 1;    /* line spacing for the first line        */
int pn = 1;     /* current page number                    */

/* request settings */
int ad = 1;      /* enable text adjustment                 */
int fi = 1;      /* fill mode                              */
int ce = 0;      /* centring count                         */
int in = 0;      /* current indent                         */
int un = 0;      /* saved indent                           */
int ll = 65;     /* line length                            */
int pl = 66;     /* page length                            */
int skip = 0;    /* lines to vertically skip               */
int ul = 0;      /* underline counter                      */
int hx = 1;      /* enable headers and footers             */
int hyf = 1;     /* allow hyphenation                      */
int ohc = 200;   /* overstrike hold character              */
int tabc = '\''; /* tab repetition character               */
int cc = '.';    /* command character                      */

/* trap margins */
int ma1 = 2; /* top margin trap                        */
int ma2 = 2; /* bottom margin trap                     */
int ma3 = 1; /* header trap                            */
int ma4 = 3; /* footer trap                            */

/* additional layout parameters */
int llh = 65;   /* saved line length                      */
int hypedf = 0; /* non-zero once hyphenation used         */

/* output buffering */
char obuf[128];     /* main output buffer                     */
char *obufp = obuf; /* pointer into obuf                      */

/* hyphenation control */
int thresh = 240; /* hyphenation threshold                  */

/* default tab stops (character columns) */
int tabtab[] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 0};

/* file and string constants */
char suffil[] = "/usr/lib/suftab"; /* suffix table file path           */
char ones[] = "ixcm";              /* Roman numeral ones table         */
char fives[] = "vld";              /* Roman numeral fives table        */
char ttyx[] = "/dev/tty0";         /* controlling terminal device      */
char bfn[] = "/tmp/rtma";          /* temporary buffer name            */

/* output file bookkeeping */
int ofile = -1; /* descriptor for current output file     */
int nextb = 4;  /* next available buffer                   */

/* input file list used by the original code */
int ilist[4];
int *ilistp = ilist; /* pointer into ilist                     */

/* digram tables provided by roff7.c */
extern char bxh[];
extern char hxx[];
extern char bxxh[];
extern char xhx[];
extern char xxh[];
