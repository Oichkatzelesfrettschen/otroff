/* t2.c:  subroutine sequencing for one table */
#include "tbl.h"
#include <stdio.h>
#include <stdlib.h>
/* Execute all stages needed for a single table. */
void tableput(void) {
    saveline();
    savefill();
    ifdivert();
    cleanfc();
    getcomm();
    getspec();
    gettbl();
    getstop();
    checkuse();
    choochar();
    maktab();
    runout();
    release();
    rstofill();
    endoff();
    restline();
}
