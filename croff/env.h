#ifndef CROFF_ENV_H
#define CROFF_ENV_H

/* Environment diversion structure used by troff. */
struct env {
    int op;     /* output position */
    int dnl;    /* current vertical position */
    int dimac;  /* diversion macro */
    int ditrap; /* diversion trap */
    int ditf;   /* diversion trap flag */
    int alss;   /* amount of extra leading after line */
    int blss;   /* amount of extra leading before line */
    int nls;    /* indicates a line was output */
    int mkline; /* line that should trigger a trap */
    int maxl;   /* maximum line number seen */
    int hnl;    /* line number of next trap */
    int curd;   /* current diversion name */
};

#endif /* CROFF_ENV_H */
