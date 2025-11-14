/* C17 - no scaffold needed */
/* tf.c: save and restore fill mode around table */
#include "tbl.h"

/* Save current fill mode. */
void savefill(void) {
    /* remembers the status of fill mode in nr 45 (SF) */
    /* nr SF = 0 means no fill, 1 means fill */
    fprintf(tabout, ".nr %d \\n(.u\n", SF);
    fprintf(tabout, ".nf\n");
}
/* Restore saved fill mode. */
void rstofill(void) {
    fprintf(tabout, ".if \\n(%d>0 .fi\n", SF);
}
/* Clean up at end of table. */
void endoff(void) {
    int i;
    for (i = 0; i < MAXHEAD; i++)
        if (linestop[i])
            fprintf(tabout, ".nr #%c 0\n", 'a' + i);
    for (i = 0; i < texct; i++)
        fprintf(tabout, ".rm %c+\n", texstr[i]);
    fprintf(tabout, "%s\n", last);
}
/* Set up diversions if needed. */
void ifdivert(void) {
    fprintf(tabout, ".ds #d .d\n");
    fprintf(tabout, ".if \\(ts\\n(.z\\(ts\\(ts .ds #d nl\n");
}
/* Save current line number. */
void saveline(void) {
    fprintf(tabout, ".nr %d \\n(c.\n", SL);
    linstart = iline;
}
/* Restore line number from saved value. */
void restline(void) {
    fprintf(tabout, ".nr c. \\n(%d+%d\n", SL, iline - linstart);
}
/* Clear field and character delimiters. */
void cleanfc(void) {
    fprintf(tabout, ".fc\n");
}
