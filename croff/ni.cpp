#include "../cxx17_scaffold.hpp"
/*
 * ni.c - nroff/troff initialization and global variables
 * 
 * This file contains global variable declarations, initialization values,
 * and the command table for the nroff/troff text formatting system.
 * 
 * Original: @(#)ni.c  1.3 of 4/26/77
 * Modernized for C90 compliance and maintainability
 */

#include "tdef.hpp" // updated header extension
#include <cstddef>

/* Version identification */
static char Sccsid[] = "@(#)ni.c  1.3 of 4/26/77";

/*
 * Output buffer management
 */
char obuf[OBUFSZ]; /* Output buffer */
char *obufp = obuf; /* Output buffer pointer */

/*
 * Built-in number registers
 * These registers store system state and user-accessible values
 */
int r[NN] = {
    '%', /* Page number register */
    'nl', /* Current vertical position */
    'yr', /* Year (last 2 digits) */
    'hp', /* Current horizontal position */
    'ct', /* Character type */
    'dn', /* Width/height of last diversion */
    'mo', /* Month (1-12) */
    'dy', /* Day of month (1-31) */
    'dw', /* Day of week (1-7) */
    'ln', /* Output line number */
    'dl', /* Width of last completed diversion */
    'st', /* Depth of string/macro stack */
    'sb', /* Depth of input stack */
    'c.' /* Current input character */
};

/*
 * Page range control
 */
int pto = 10000; /* Page range upper limit */
int pfrom = 1; /* Page range lower limit */
int print = 1; /* Print flag */

/*
 * File system paths and indices
 */
char nextf[NS] = "/usr/lib/tmac.xxxxx"; /* Next file template */
int nfi = 14; /* Next file index */

#ifdef NROFF
char termtab[NS] = "/usr/lib/term/37"; /* Terminal table path */
int tti = 14; /* Terminal table index */
#endif

char suftab[] = "/usr/lib/suftab"; /* Suffix table path */

/*
 * System initialization and control
 */
int init = 1; /* Initialization flag */
int fc = IMP; /* Field character */
int eschar = '\\'; /* Escape character */

/*
 * Page layout parameters
 */
int pl = 11 * INCH; /* Page length */
int po = PO; /* Page offset */

/*
 * Scaling and resolution
 */
int dfact = 1; /* Default scaling factor */
int dfactd = 1; /* Default scaling divisor */
int res = 1; /* Device resolution */
int smnt = 4; /* Mounting position */

/*
 * Character set and device parameters
 */
int ascii = ASCII; /* ASCII character set flag */
int ptid = PTID; /* Phototypesetter ID */
char ptname[] = "/dev/cat"; /* Phototypesetter device name */
int lg = LG; /* Ligature flag */

/*
 * Page number list management
 */
int pnlist[NPN] = {-1}; /* Page number list */
int *pnp = pnlist; /* Page number pointer */
int npn = 1; /* Number of page numbers */
int npnflg = 1; /* Page number flag */

/*
 * Output control
 */
int oldbits = -1; /* Previous character bits */
int xflg = 1; /* Cross-reference flag */
int dpn = -1; /* Dry run page number */
int totout = 1; /* Total output flag */

/*
 * Font and character formatting
 */
int ulfont = 1; /* Underline font */
int ulbit = 1 << 9; /* Underline bit pattern */
int tabch = TAB; /* Tab character */
int ldrch = LEADER; /* Leader character */

int xxx; /* Scratch variable */

/*
 * Function prototypes for command handlers
 * These functions implement the various nroff/troff commands
 */
extern int caseds(void), caseas(void), casesp(void), caseft(void);
extern int caseps(void), casevs(void), casenr(void), caseif(void);
extern int casepo(void), casetl(void), casetm(void), casebp(void);
extern int casech(void), casepn(void), tbreak(void), caseti(void);
extern int casene(void), casenf(void), casece(void), casefi(void);
extern int casein(void), caseli(void), casell(void), casens(void);
extern int casemk(void), casert(void), caseam(void), casede(void);
extern int casedi(void), caseda(void), casewh(void), casedt(void);
extern int caseit(void), caserm(void), casern(void), casead(void);
extern int casers(void), casena(void), casepl(void), caseta(void);
extern int casetr(void), caseul(void), caselt(void), casenx(void);
extern int caseso(void), caseig(void), casetc(void), casefc(void);
extern int caseec(void), caseeo(void), caselc(void), caseev(void);
extern int caserd(void), caseab(void), casefl(void), done(void);
extern int casess(void), casefp(void), casecs(void), casebd(void);
extern int caselg(void), casehc(void), casehy(void), casenh(void);
extern int casenm(void), casenn(void), casesv(void), caseos(void);
extern int casels(void), casecc(void), casec2(void), caseem(void);
extern int caseaf(void), casehw(void), casemc(void), casepm(void);
extern int casecu(void), casepi(void), caserr(void), caseuf(void);
extern int caseie(void), caseel(void), casepc(void), caseht(void);

/*
 * Command table structure
 * Maps two-character command names to their handler functions
 */
struct contab {
    int rq; /* Request code (two characters) */
    int (*f)(void); /* Function pointer to handler */
};

/*
 * Main command dispatch table
 * Contains all nroff/troff commands and their corresponding functions
 */
struct contab contab[NM] = {
    {'ds', caseds}, /* Define string */
    {'as', caseas}, /* Append to string */
    {'sp', casesp}, /* Space vertically */
    {'ft', caseft}, /* Font change */
    {'ps', caseps}, /* Point size */
    {'vs', casevs}, /* Vertical spacing */
    {'nr', casenr}, /* Number register */
    {'if', caseif}, /* Conditional */
    {'ie', caseie}, /* If-else */
    {'el', caseel}, /* Else */
    {'po', casepo}, /* Page offset */
    {'tl', casetl}, /* Title line */
    {'tm', casetm}, /* Terminal message */
    {'bp', casebp}, /* Begin page */
    {'ch', casech}, /* Change trap position */
    {'pn', casepn}, /* Page number */
    {'br', tbreak}, /* Break */
    {'ti', caseti}, /* Temporary indent */
    {'ne', casene}, /* Need space */
    {'nf', casenf}, /* No fill */
    {'ce', casece}, /* Center */
    {'fi', casefi}, /* Fill */
    {'in', casein}, /* Indent */
    {'li', caseli}, /* Literal */
    {'ll', casell}, /* Line length */
    {'ns', casens}, /* No space */
    {'mk', casemk}, /* Mark position */
    {'rt', casert}, /* Return to mark */
    {'am', caseam}, /* Append to macro */
    {'de', casede}, /* Define macro */
    {'di', casedi}, /* Divert */
    {'da', caseda}, /* Divert append */
    {'wh', casewh}, /* When */
    {'dt', casedt}, /* Diversion trap */
    {'it', caseit}, /* Input trap */
    {'rm', caserm}, /* Remove macro */
    {'rr', caserr}, /* Remove register */
    {'rn', casern}, /* Rename */
    {'ad', casead}, /* Adjust */
    {'rs', casers}, /* Restore spacing */
    {'na', casena}, /* No adjust */
    {'pl', casepl}, /* Page length */
    {'ta', caseta}, /* Tab stops */
    {'tr', casetr}, /* Translate */
    {'ul', caseul}, /* Underline */
    {'cu', casecu}, /* Continuous underline */
    {'lt', caselt}, /* Length of title */
    {'nx', casenx}, /* Next file */
    {'so', caseso}, /* Source file */
    {'ig', caseig}, /* Ignore */
    {'tc', casetc}, /* Tab character */
    {'fc', casefc}, /* Field character */
    {'ec', caseec}, /* Escape character */
    {'eo', caseeo}, /* Escape off */
    {'lc', caselc}, /* Leader character */
    {'ev', caseev}, /* Environment */
    {'rd', caserd}, /* Read */
    {'ab', caseab}, /* Abort */
    {'fl', casefl}, /* Flush */
    {'ex', done}, /* Exit */
    {'ss', casess}, /* Space character size */
    {'fp', casefp}, /* Font position */
    {'cs', casecs}, /* Constant spacing */
    {'bd', casebd}, /* Bold */
    {'lg', caselg}, /* Ligature */
    {'hc', casehc}, /* Hyphenation character */
    {'hy', casehy}, /* Hyphenate */
    {'nh', casenh}, /* No hyphenate */
    {'nm', casenm}, /* Number lines */
    {'nn', casenn}, /* No number */
    {'sv', casesv}, /* Save space */
    {'os', caseos}, /* Output saved space */
    {'ls', casels}, /* Line spacing */
    {'cc', casecc}, /* Control character */
    {'c2', casec2}, /* No-break control character */
    {'em', caseem}, /* End macro */
    {'af', caseaf}, /* Assign format */
    {'hw', casehw}, /* Hyphenation exception words */
    {'mc', casemc}, /* Margin character */
    {'pm', casepm}, /* Print macros */
#ifdef NROFF
    {'pi', casepi}, /* Pipe to program */
#endif
    {'uf', caseuf}, /* Underline font */
    {'pc', casepc}, /* Page character */
    {'ht', caseht}, /* Horizontal tab */
};

/*
 * Troff environment block
 * These variables maintain the current formatting state
 */

/* Environment control */
int block = 0; /* Environment block number */
int ics = ICS; /* Input character size */
int ic = 0; /* Input character */
int icf = 0; /* Input character flag */
int chbits = 0; /* Character bits */
int nmbits = 0; /* Number bits */

/* Point size control */
int apts = PS; /* Actual point size */
int apts1 = PS; /* Previous actual point size */
int pts = PS; /* Point size */
int pts1 = PS; /* Previous point size */

/* Font control */
int font = FT; /* Current font */
int font1 = FT; /* Previous font */

/* Spacing control */
int sps = SPS; /* Space size */
int spacesz = SS; /* Space character size */
int lss = VS; /* Line space size */
int lss1 = VS; /* Previous line space size */
int ls = 1; /* Line spacing */
int ls1 = 1; /* Previous line spacing */

/* Line length control */
int ll = LL; /* Line length */
int ll1 = LL; /* Previous line length */
int lt = LL; /* Title length */
int lt1 = LL; /* Previous title length */

/* Text formatting control */
int ad = 1; /* Adjust mode */
int nms = 1; /* Number mode */
int ndf = 1; /* Number default */
int fi = 1; /* Fill mode */

/* Control characters */
int cc = '.'; /* Control character */
int c2 = '\''; /* No-break control character */
int ohc = OHC; /* Output hyphenation character */
int tdelim = IMP; /* Title delimiter */

/* Hyphenation control */
int hyf = 1; /* Hyphenation flag */
int hyoff = 0; /* Hyphenation offset */

/* Miscellaneous formatting state */
int un1 = -1; /* Underline count */
int tabc = 0; /* Tab character count */
int dotc = '.'; /* Dot character */
int adsp = 0; /* Adjust space */
int adrem = 0; /* Adjust remainder */
int lastl = 0; /* Last line flag */
int nel = 0; /* Number of empty lines */
int admod = 0; /* Adjust mode */

/* Word and line processing */
int *wordp = 0; /* Word pointer */
int spflg = 0; /* Space flag */
int *linep = 0; /* Line pointer */
int *wdend = 0; /* Word end pointer */
int *wdstart = 0; /* Word start pointer */
int wne = 0; /* Word need */
int ne = 0; /* Need space */
int nc = 0; /* Number of characters */
int nb = 0; /* Number of blanks */
int lnmod = 0; /* Line modification */
int nwd = 0; /* Number of words */
int nn = 0; /* No number */
int ni = 0; /* Number indent */

/* Special formatting modes */
int ul = 0; /* Underline count */
int cu = 0; /* Continuous underline count */
int ce = 0; /* Center count */
int in = 0; /* Indent */
int in1 = 0; /* Previous indent */
int un = 0; /* Underline */
int wch = 0; /* Width character */
int pendt = 0; /* Pending tab */
int *pendw = 0; /* Pending width */
int pendnf = 0; /* Pending no-fill */
int spread = 0; /* Spread flag */
int it = 0; /* Input trap count */
int itmac = 0; /* Input trap macro */

/* Buffer size control */
int lnsize = LNSIZE; /* Line size */

/* Hyphenation exception table */
int *hyptr[NHYP] = {0}; /* Hyphenation pointers */

/* Tab stop table */
int tabtab[NTAB] = {
    DTAB, DTAB * 2, DTAB * 3, DTAB * 4, /* Tab stops 1-4 */
    DTAB * 5, DTAB * 6, DTAB * 7, DTAB * 8, /* Tab stops 5-8 */
    DTAB * 9, DTAB * 10, DTAB * 11, DTAB * 12, /* Tab stops 9-12 */
    DTAB * 13, DTAB * 14, DTAB * 15, 0 /* Tab stops 13-16 */
};

/* Working buffers */
int line[LNSIZE] = {0}; /* Current line buffer */
int word[WDSIZE] = {0}; /* Current word buffer */

/* Environment block padding */
int blockxxx[EVS - 64 - NHYP - NTAB - WDSIZE - LNSIZE] = {0};

/* Output line buffer */
int oline[LNSIZE + 1]; /* Output line buffer */
