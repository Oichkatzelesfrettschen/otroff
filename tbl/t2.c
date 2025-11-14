/* C17 - no scaffold needed */
#include "tbl.h"

#include <stdlib.h>
#include <stdio.h>

// Execute all stages needed for a single table.
void tableput() {
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
